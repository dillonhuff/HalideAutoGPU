/*
 * An application for applying a Gaussian blur.
 * It uses a 3x3 stencil with constant weights.
 */

#include "Halide.h"

namespace {

using namespace Halide;

// Size of blur for gradients.
//const int blockSize = 3;
//int imgSize = 1024 / 2 - 1;

const int pyramid_levels = 6;

class GaussianBlur : public Halide::Generator<GaussianBlur> {
public:
    Input<Buffer<uint16_t>>  input{"input", 2};
    Output<Buffer<uint16_t>> output{"output", 2};

    Var x, y, c;

    Func downsample(Func f) {
      RDom reduce(-1, 1, -1, 1);
      Func ds;
      ds(x, y) = 0;
      ds(x, y) += f(x + reduce.x, y + reduce.y);
      Func avg;
      avg(x, y) = ds(x, y) / Expr(9);
      return avg;
    }

    void generate() {
        /* THE ALGORITHM */

        Var xo("xo"), yo("yo"), xi("xi"), yi("yi");

        Func clamped = Halide::BoundaryConditions::repeat_edge(input);

        Func hw_input, input_copy;
        input_copy(x, y) = cast<uint16_t>(clamped(x, y));
        hw_input(x, y) = input_copy(x, y);

        Func gPyramid[pyramid_levels];
        gPyramid[0](x, y) =
          hw_input(x, y);

        for (int j = 1; j < pyramid_levels; j++) {
          gPyramid[j](x, y) =
            downsample(gPyramid[j - 1])(x, y);
        }

        Func hw_output;
        hw_output(x, y) =
          //cast<uint16_t>( hw_input(x, y) );
          //cast<uint16_t>( gPyramid[0](x, y) );
          cast<uint16_t>( gPyramid[pyramid_levels - 1](x, y) );
        output(x, y) = hw_output(x, y);

        input.dim(0).set_bounds_estimate(0, 2048*2);
        input.dim(1).set_bounds_estimate(0, 2048*2);

        output.estimate(x, 0, 2048)
              .estimate(y, 0, 2048);

        output.bound(x, 0, 32);
        output.bound(y, 0, 32);

        if (auto_schedule) {
        }

    }
};

}  // namespace

HALIDE_REGISTER_GENERATOR(GaussianBlur, gausspyramid)

