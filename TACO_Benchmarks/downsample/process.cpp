#include <cstdio>
#include <chrono>
#include <fstream>

// Hello this is a comment I am a comment
// Yet another comment hello
#include "downsample.h"
#ifndef NO_AUTO_SCHEDULE
#include "downsample_auto_schedule_store.h"
#include "downsample_auto_schedule.h"
#include "downsample_simple_auto_schedule.h"
#include "downsample_auto_schedule_no_fus.h"
#endif

#include "benchmark_util.h"
#include "HalideBuffer.h"
#include "halide_image_io.h"

using namespace std;
using namespace Halide::Runtime;
using namespace Halide::Tools;
using namespace std::chrono;

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

    Buffer<uint16_t> output(1024, 1024);
    const int num_runs = 1000000;
    __int64_t start_us = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();

    for (int r = 0; r < num_runs; r++) {
      //cout << "r = " << r << endl;
      downsample_auto_schedule(input, output); 
      output.device_sync(); 
    }
    __int64_t end_us = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();

    ofstream times("times.csv");
    times << start_us << endl;
    times << end_us << endl;
    times << num_runs << endl;
    times.close();

    cout << "microseconds since epoch: " << end_us << endl;
    auto diff = end_us - start_us;
    cout << "diff = " << diff << endl;
    cout << "per run = " << diff / num_runs << endl;
    cout << "Done with auto schedule" << endl;


    multi_way_bench({
        {"Manual", [&]() { downsample(input, output); output.device_sync(); }},
    #ifndef NO_AUTO_SCHEDULE
        {"Nested auto-scheduled", [&]() { downsample_auto_schedule_store(input, output); output.device_sync(); }},
       {"Auto-scheduled", [&]() { downsample_auto_schedule(input, output); output.device_sync(); }},
          {"No-fusion auto-scheduled", [&]() { downsample_auto_schedule_no_fus(input, output); output.device_sync(); }},
        //{"Simple auto-scheduled", [&]() { downsample_simple_auto_schedule(input, output); output.device_sync(); }}
    #endif
        }
    );

   std::cout << "Done, transferring image" << endl;

    convert_and_save_image(output, argv[6]);

    return 0;
}