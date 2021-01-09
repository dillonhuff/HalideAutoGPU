#include <cstdio>
#include <chrono>
#include <fstream>

// Hello this is a comment I am a comment
// Yet another comment hello
#include "exposurefusion.h"
#ifndef NO_AUTO_SCHEDULE
//#include "exposurefusion_auto_schedule_store.h"
#include "exposurefusion_auto_schedule.h"
#include "exposurefusion_cpu.h"
#include "exposurefusion_simple_auto_schedule.h"
#include "exposurefusion_auto_schedule_no_fus.h"
#endif

#include "benchmark_util.h"
#include "HalideBuffer.h"
#include "halide_image_io.h"

using namespace std;
using namespace Halide::Runtime;
using namespace Halide::Tools;
using namespace std::chrono;

typedef float PixelType;

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
    Buffer<PixelType> input = load_and_convert_image(argv[1]);

    int cols = 2048;
    int rows = 2048;
    Buffer<PixelType> output(cols, rows);

    cout << "Starting CPU..."  << endl;
    //exposurefusion_cpu(input, output);
    //assert(false);

    cout << "Starting auto-schedule runs..."  << endl;
    const int num_runs = 100000;
    __int64_t start_us = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();

    for (int r = 0; r < num_runs; r++) {
      //cout << "r = " << r << endl;
      exposurefusion_auto_schedule(input, output); 
      output.device_sync(); 
    }
    __int64_t end_us = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();

    ofstream times("times.csv");
    times << start_us << endl;
    times << end_us << endl;
    times << num_runs << endl;
    times.close();

    ofstream input_info("input_info.txt");
    input_info << "x," << input.width() << endl;
    input_info << "y," << input.height() << endl;
    input_info << "b," << input.channels() << endl;
    input_info.close();

    cout << "microseconds since epoch: " << end_us << endl;
    auto diff = end_us - start_us;
    cout << "diff = " << diff << endl;
    cout << "per run = " << diff / num_runs << endl;
    cout << "Done with auto schedule" << endl;


    multi_way_bench({
        //{"Manual", [&]() { exposurefusion(input, output); output.device_sync(); }},
    #ifndef NO_AUTO_SCHEDULE
        //{"Nested auto-scheduled", [&]() { exposurefusion_auto_schedule_store(input, output); output.device_sync(); }},
       {"Auto-scheduled", [&]() { exposurefusion_auto_schedule(input, output); output.device_sync(); }},
          {"No-fusion auto-scheduled", [&]() { exposurefusion_auto_schedule_no_fus(input, output); output.device_sync(); }},
        //{"Simple auto-scheduled", [&]() { exposurefusion_simple_auto_schedule(input, output); output.device_sync(); }}
    #endif
        }
    );

   std::cout << "Done, transferring image" << endl;

    convert_and_save_image(output, argv[6]);

    return 0;
}
