#include <fstream>
#include <cstdio>
#include <chrono>
// Hello this is a comment I am a comment
// Yet another comment
#include "max_pool.h"
#ifndef NO_AUTO_SCHEDULE
//#include "max_pool_auto_schedule_store.h"
#include "max_pool_auto_schedule.h"
#include "max_pool_cpu.h"
#include "max_pool_simple_auto_schedule.h"
#include "max_pool_auto_schedule_no_fus.h"
#endif

#include "benchmark_util.h"
#include "HalideBuffer.h"
#include "halide_image_io.h"

#include <cmath>

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
    Buffer<uint16_t> input(128, 128, 64);
    for (int c = 0; c < input.channels(); c++) {
      for (int y = 0; y < input.height(); y++) {
        for (int x = 0; x < input.width(); x++) {
          input(x, y, c) = rand();
        }
      }
    }

    ofstream input_info("input_info.txt");
    input_info << "x," << input.width() << endl;
    input_info << "y," << input.height() << endl;
    input_info << "b," << input.channels() << endl;
    input_info.close();

    Buffer<uint16_t> output(64, 64, 64);
    max_pool_cpu(input, output);

    const long int num_runs = 4000000;
    //const long int num_runs = 1;
    __int64_t start_us = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();

    for (long int r = 0; r < num_runs; r++) {
      //cout << "r = " << r << endl;
      max_pool_auto_schedule(input, output); 
      output.device_sync(); 
    }
    __int64_t end_us = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();

    ofstream times("times.csv");
    times << start_us << endl;
    times << end_us << endl;
    times << num_runs << endl;
    times.close();
    //for (int r = 0; r < 1; r++) {
      //max_pool(input, output); 
      //output.device_sync(); 
    //}
    cout << "Done with auto schedule" << endl;
    //return 0;

   multi_way_bench({
        {"Manual", [&]() { max_pool(input, output); output.device_sync(); }},
    #ifndef NO_AUTO_SCHEDULE
        //{"Nested auto-scheduled", [&]() { max_pool_auto_schedule_store(input, output); output.device_sync(); }},
       {"Auto-scheduled", [&]() { max_pool_auto_schedule(input, output); output.device_sync(); }},
          {"No-fusion auto-scheduled", [&]() { max_pool_auto_schedule_no_fus(input, output); output.device_sync(); }},
        {"Simple auto-scheduled", [&]() { max_pool_simple_auto_schedule(input, output); output.device_sync(); }}
    #endif
        }
    );

   std::cout << "Done, transferring image" << endl;

    convert_and_save_image(output, argv[6]);

    return 0;
}
