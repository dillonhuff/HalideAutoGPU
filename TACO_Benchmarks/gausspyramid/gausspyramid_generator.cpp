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

const int pyramid_levels = 4;


class GaussianBlur : public Halide::Generator<GaussianBlur> {
public:
    Input<Buffer<uint16_t>>  input{"input", 2};
    Output<Buffer<uint16_t>> output{"output", 2};

    Var x, y, c;

    Func downsample(Func f) {
      //Func ds;
      //ds(x, y) = (3*f(2*x + 1, 2*y) + f(2*x, 2*y));
      //return ds;

      RDom reduce(-1, 1, -1, 1);

      Func ds;
      ds(x, y) = 0;
      ds(x, y) += f(2*x + reduce.x, 2*y + reduce.y);
      Func avg;
      avg(x, y) = ds(x, y) / Expr(9);
      return avg;
    }

    Func downsample_fp(Func f) {
      //Func ds;
      //ds(x, y) = (f(2*x + 1, 2*y) + f(2*x, 2*y)) / 2.0f;
      //return ds;
      
      RDom reduce(-1, 1, -1, 1);

      Func ds;
      ds(x, y) = cast(Float(32), (0.0f));
      ds(x, y) += f(2*x + reduce.x, 2*y + reduce.y);
      Func avg;
      avg(x, y) = ds(x, y) / Expr(2.0f);
      return avg;
    }

    void generate() {
        /* THE ALGORITHM */

        Var xo("xo"), yo("yo"), xi("xi"), yi("yi");

        Func clamped = Halide::BoundaryConditions::repeat_edge(input);

        Func hw_input, input_copy;
        input_copy(x, y) = cast<uint16_t>(clamped(x, y));
        hw_input(x, y) = cast<float>(input_copy(x, y));

        Func gPyramid[pyramid_levels];
        gPyramid[0](x, y) =
          hw_input(x, y);

        for (int j = 1; j < pyramid_levels; j++) {
          gPyramid[j](x, y) =
            downsample_fp(gPyramid[j - 1])(x, y);
            //downsample(gPyramid[j - 1])(x, y);
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

        output.bound(x, 0, 256);
        output.bound(y, 0, 256);

        if (auto_schedule) {
        }

    }
};

}  // namespace

HALIDE_REGISTER_GENERATOR(GaussianBlur, gausspyramid)

