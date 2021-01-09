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
    Input<Buffer<float>>  input{"input", 3};
    Output<Buffer<float>> output{"output", 3};

    Var x, y, c;

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
          cast<float>(max(
                hw_input(2*x, 2*y, c),
                hw_input(2*x + 1, 2*y, c),
                hw_input(2*x, 2*y + 1, c),
                hw_input(2*x + 1, 2*y + 1, c)));

        output(x, y, c) = hw_output(x, y, c);

        input.dim(0).set_bounds_estimate(0, 2048*2);
        input.dim(1).set_bounds_estimate(0, 2048*2);
        input.dim(2).set_bounds_estimate(0, 2048*2);

        output.estimate(x, 0, 128)
              .estimate(y, 0, 128).
              estimate(c, 0, 64);

        if (auto_schedule) {
        } else {
          //clamped.trace_loads();
          clamped.compute_root();
        }

    }
};

}  // namespace

HALIDE_REGISTER_GENERATOR(GaussianBlur, max_pool)

