#include <cstdio>
#include <chrono>
// Hello this is a comment I am a comment
// Yet another comment
#include "gausspyramid.h"
#ifndef NO_AUTO_SCHEDULE
#include "gausspyramid_auto_schedule_store.h"
#include "gausspyramid_auto_schedule.h"
#include "gausspyramid_simple_auto_schedule.h"
#include "gausspyramid_auto_schedule_no_fus.h"
#endif

#include "benchmark_util.h"
#include "HalideBuffer.h"
#include "halide_image_io.h"

using namespace std;
using namespace Halide::Runtime;
using namespace Halide::Tools;

int main(int argc, char **argv) {
    if (argc < 7) {
        printf("Usage: ./process input.png levels alpha beta timing_iterations output.png\n"
               "e.g.: ./process input.png 8 1 1 10 output.png\n");
        return 0;
    }
#ifdef cuda_alloc
    halide_reuse_device_allocations(nullptr, true);
#endif
    // Input may be a PNG8
    Buffer<uint16_t> input = load_and_convert_image(argv[1]);

    Buffer<uint16_t> output(32, 32);
    //for (int r = 0; r < 1000000; r++) {
    gausspyramid(input, output); 
    output.device_sync(); 
    //}
    cout << "Done with auto schedule" << endl;
    //return 0;

   multi_way_bench({
        {"Manual", [&]() { gausspyramid(input, output); output.device_sync(); }},
    #ifndef NO_AUTO_SCHEDULE
        {"Nested auto-scheduled", [&]() { gausspyramid_auto_schedule_store(input, output); output.device_sync(); }},
       {"Auto-scheduled", [&]() { gausspyramid_auto_schedule(input, output); output.device_sync(); }},
          {"No-fusion auto-scheduled", [&]() { gausspyramid_auto_schedule_no_fus(input, output); output.device_sync(); }},
        {"Simple auto-scheduled", [&]() { gausspyramid_simple_auto_schedule(input, output); output.device_sync(); }}
    #endif
        }
    );

   std::cout << "Done, transferring image" << endl;

    convert_and_save_image(output, argv[6]);

    return 0;
}
