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
    Input<Buffer<uint16_t>>  input{"input", 3};
    Output<Buffer<uint16_t>> output{"output", 3};

    Var x, y, c;

    Func downsample(Func f) {
      //Func ds;
      //ds(x, y) = (3*f(2*x + 1, 2*y) + f(2*x, 2*y));
      //return ds;

      RDom reduce(-1, 1, -1, 1);

      Func ds;
      ds(x, y) = 0.0f;
      ds(x, y) += f(2*x + reduce.x, 2*y + reduce.y);
      Func avg;
      //avg(x, y) = ds(x, y) / Expr(9);
      avg(x, y) = ds(x, y) / Expr(9.0f);
      return avg;
    }

    void generate() {
        /* THE ALGORITHM */

        Var xo("xo"), yo("yo"), xi("xi"), yi("yi");
        //input.trace_loads();

        Func clamped = Halide::BoundaryConditions::repeat_edge(input);

        Func hw_input, input_copy;
        input_copy(x, y, c) = cast<float>(clamped(x, y, c));
        hw_input(x, y, c) = input_copy(x, y, c);

        Func hw_output;
        hw_output(x, y, c) =
          cast<uint16_t>( hw_input(x * 2, y * 2, c) );
        output(x, y, c) = hw_output(x, y, c);

        input.dim(0).set_bounds_estimate(0, 2048*2);
        input.dim(1).set_bounds_estimate(0, 2048*2);
        input.dim(2).set_bounds_estimate(0, 2048*2);

        output.estimate(x, 0, 2048)
              .estimate(y, 0, 2048).
              estimate(c, 0, 2048);

        //output.bound(x, 0, 32);
        //output.bound(y, 0, 32);

        if (auto_schedule) {
        } else {
          clamped.trace_loads();
          clamped.compute_root();
        }

    }
};

}  // namespace

HALIDE_REGISTER_GENERATOR(GaussianBlur, max_pool)

