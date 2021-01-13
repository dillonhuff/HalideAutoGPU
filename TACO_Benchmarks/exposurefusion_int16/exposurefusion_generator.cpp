/*
 * An application for applying a Gaussian blur.
 * It uses a 3x3 stencil with constant weights.
 */

#include "Halide.h"
#include <vector>

using namespace std;

namespace {

using namespace Halide;

// Size of blur for gradients.
//const int blockSize = 3;
//int imgSize = 1024 / 2 - 1;

const int pyramid_levels = 4;

class GaussianBlur : public Halide::Generator<GaussianBlur> {
public:
    Input<Buffer<uint16_t>>  input{"input", 2};
    Output<Buffer<uint16_t>> output{"output", 2};

    Var x, y, c;

    Func downsample_fp(Func f) {

      Func ds;
      ////ds(x, y) = (f(2*x + 1, 2*y) + f(2*x, 2*y) + f(2*x, 2*y + 1)) / 3.0f;
      ds(x, y) = (f(2*x + 1, 2*y) + f(2*x, 2*y)) / 9;
      return ds;
    }

    Func random_pointwise_stage(Func f) {
      Func ds;
      ds(x, y) = f(x, y)*f(x, y);
      return ds;
    }

    Func upsample(Func f) {
      Func ds;
      ds(x, y) = cast<uint16_t>(f(x / 2, y / 2));
      return ds;
    }

    vector<Func> laplace_pyramid(Func bright) {
      vector<Func> gPyramid = gauss_pyramid(bright);
      assert(gPyramid.size() == pyramid_levels);

      vector<Func> lPyramid;
      lPyramid.resize(pyramid_levels);
      lPyramid[pyramid_levels - 1](x, y) = gPyramid.back()(x, y);

      for (int j = pyramid_levels - 2; j >= 0; j--) {
        lPyramid[j](x, y) =
          gPyramid[j](x, y) - upsample(gPyramid[j + 1])(x, y);
      }

      return lPyramid;
    }

    vector<Func> gauss_pyramid(Func l0) {
      vector<Func> gPyramid;
      vector<Func> gPyramid_clamped;
      gPyramid.resize(pyramid_levels);
      gPyramid_clamped.resize(pyramid_levels);
      gPyramid[0](x, y) =
        l0(x, y);
      gPyramid_clamped[0](x, y) =
        l0(x, y);
      Expr w = input.dim(0).extent(), h = input.dim(1).extent();
      for (int j = 1; j < pyramid_levels; j++) {
        gPyramid[j](x, y) =
          downsample_fp(gPyramid[j - 1])(x, y);
        w /= 2;
        h /= 2;
        gPyramid_clamped[j] = BoundaryConditions::repeat_edge(gPyramid[j], {{0, w}, {0, h}});
      }

      return gPyramid_clamped;
    }

    void generate() {
        /* THE ALGORITHM */

        Var xo("xo"), yo("yo"), xi("xi"), yi("yi");

        Func bright, dark, bright_weight, dark_weight;

        Func clamped = Halide::BoundaryConditions::repeat_edge(input);

        Func hw_input, input_copy;
        input_copy(x, y) = cast<uint16_t>(clamped(x, y));
        hw_input(x, y) = input_copy(x, y);

        bright(x, y) = 2*hw_input(x, y);
        dark(x, y) = hw_input(x, y);

        bright_weight(x, y) = select(bright(x, y) < 128, 1, 0);
        dark_weight(x, y) = select(dark(x, y) > 128, 1, 0);

        //auto bright_pyramid = gauss_pyramid(bright);
        //auto dark_pyramid = gauss_pyramid(dark);

        auto bright_pyramid = laplace_pyramid(bright);
        auto dark_pyramid = laplace_pyramid(dark);

        auto bright_weight_pyramid = gauss_pyramid(bright);
        auto dark_weight_pyramid = gauss_pyramid(dark);

        Func blend[pyramid_levels];
        for (int j = 0; j < pyramid_levels; j++) {
          blend[j](x, y) = bright_weight_pyramid[j](x, y) * bright_pyramid[j](x, y)
            + dark_weight_pyramid[j](x, y) * dark_pyramid[j](x, y);
        }

        // Collapse pyramid
        Func collapsed[pyramid_levels];
        collapsed[pyramid_levels - 1](x, y) = blend[pyramid_levels - 1](x, y);
        for (int j = pyramid_levels - 2; j >= 0; j--) {
          collapsed[j](x, y) = upsample(collapsed[j + 1])(x, y) + blend[j](x, y);
        }

        Func hw_output;
        hw_output(x, y) =
          cast<uint16_t>( collapsed[0](x, y) );
        output(x, y) = hw_output(x, y);

        input.dim(0).set_bounds_estimate(0, 2048*2);
        input.dim(1).set_bounds_estimate(0, 2048*2);

        output.estimate(x, 0, 2048)
              .estimate(y, 0, 2048);

        //output.bound(x, 0, 2048);
        //output.bound(y, 0, 2048);

        if (auto_schedule) {
        } else {
          //clamped.trace_loads();
          //clamped.compute_root();
        }

    }
};

}  // namespace

HALIDE_REGISTER_GENERATOR(GaussianBlur, exposurefusion)
