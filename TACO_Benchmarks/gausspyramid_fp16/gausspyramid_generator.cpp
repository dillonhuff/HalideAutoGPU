/*
 * An application for applying a Gaussian blur.
 * It uses a 3x3 stencil with constant weights.
 */

#include "Halide.h"

namespace {

using namespace Halide;

const int pyramid_levels = 4;

typedef float InPixelType;

class GaussianBlur : public Halide::Generator<GaussianBlur> {
public:
    Input<Buffer<InPixelType>>  input{"input", 2};
    Output<Buffer<InPixelType>> output{"output", 2};

    Var x, y, c;

    Func downsample_fp(Func f) {
      RDom reduce(-1, 2, -1, 2);

      Func ds;
      ds(x, y) = cast(Float(32), (0));
<<<<<<< HEAD
      //ds(x, y) = cast(Int(16), (0));
      ds(x, y) += f(2*x + reduce.x, 2*y + reduce.y);
      Func avg;
      avg(x, y) = ds(x, y) / cast(Float(32), Expr(2));
      //avg(x, y) = ds(x, y) / cast(Int(16), Expr(2));
=======
      ds(x, y) += f(2*x + reduce.x, 2*y + reduce.y);
      Func avg;
      avg(x, y) = ds(x, y) / cast(Float(32), Expr(2));
>>>>>>> c9b0a4cc56594c0ae122e134cb2ab2ad49d25bd9
      return avg;
    }

    void generate() {

        Var xo("xo"), yo("yo"), xi("xi"), yi("yi");

        Func clamped = Halide::BoundaryConditions::repeat_edge(input);

        Func hw_input, input_copy;
        input_copy(x, y) = cast<InPixelType>(clamped(x, y));
        hw_input(x, y) = cast(Float(32), input_copy(x, y));
<<<<<<< HEAD
        //hw_input(x, y) = cast(Int(16), input_copy(x, y));
=======
>>>>>>> c9b0a4cc56594c0ae122e134cb2ab2ad49d25bd9

        Func gPyramid[pyramid_levels];
        gPyramid[0](x, y) =
          hw_input(x, y);

        for (int j = 1; j < pyramid_levels; j++) {
          gPyramid[j](x, y) =
            downsample_fp(gPyramid[j - 1])(x, y);
        }

        Func hw_output;
        hw_output(x, y) =
          cast<InPixelType>( gPyramid[pyramid_levels - 1](x, y) );
        output(x, y) = hw_output(x, y);

        input.dim(0).set_bounds_estimate(0, 2048*2);
        input.dim(1).set_bounds_estimate(0, 2048*2);

        output.estimate(x, 0, 2048)
              .estimate(y, 0, 2048);

        output.bound(x, 0, 256);
        output.bound(y, 0, 256);

        if (auto_schedule) {
        } else {
          for (int i = 0; i < (int) pyramid_levels; i++) {
            gPyramid[i].gpu_single_thread().compute_root();
            //gPyramid[i].compute_root();
          }
        }

    }
};

}  // namespace

HALIDE_REGISTER_GENERATOR(GaussianBlur, gausspyramid)

