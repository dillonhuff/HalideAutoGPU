include ../support/Makefile.inc
include ../support/autoscheduler.inc

BIN ?= bin

#HL_TARGET=host-cuda

all: $(BIN)/process

$(BIN)/max_pool.generator: max_pool_generator.cpp ../autoscheduler/SimpleAutoSchedule.h ../autoscheduler/SimpleAutoSchedule.cpp ../autoscheduler/AutoSchedule.cpp ../autoscheduler/AutoSchedule.h ../autoscheduler/DerivativeUtils.h ../autoscheduler/DerivativeUtils.cpp $(GENERATOR_DEPS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -g $(filter-out %.h,$^) -o $@ $(LDFLAGS) $(HALIDE_SYSTEM_LIBS) $(USE_EXPORT_DYNAMIC)

$(BIN)/max_pool.a: $(BIN)/max_pool.generator
	@mkdir -p $(@D)
	$^ -g max_pool -o $(BIN) -f max_pool target=$(HL_TARGET)-no_runtime auto_schedule=false


$(BIN)/max_pool_auto_schedule.a: $(BIN)/max_pool.generator $(AUTOSCHED_BIN)/libauto_schedule.so
	@mkdir -p $(@D)
	$< -g max_pool -o $(BIN) -f max_pool_auto_schedule target=$(HL_TARGET) auto_schedule=true -p $(AUTOSCHED_BIN)/libauto_schedule.so  -e static_library,h,schedule

$(BIN)/max_pool_simple_auto_schedule.a: $(BIN)/max_pool.generator
	@mkdir -p $(@D)
	HL_USE_SIMPLE_AUTOSCHEDULER=1 \
	$< -g max_pool -o $(BIN) -f max_pool_simple_auto_schedule target=$(HL_TARGET)-no_runtime auto_schedule=false -e static_library,h

$(BIN)/max_pool_auto_schedule_store.a: $(BIN)/max_pool.generator $(AUTOSCHED_BIN)/libauto_schedule.so
	@mkdir -p $(@D)
	#HL_AUTO_FOLDED_FUSION=1 \
	#$<  -g max_pool -o $(BIN) -f max_pool_auto_schedule_store target=$(HL_TARGET) auto_schedule=true  -p $(AUTOSCHED_BIN)/libauto_schedule.so -e static_library,h,schedule 


$(BIN)/max_pool_auto_schedule_no_fus.a: $(BIN)/max_pool.generator $(AUTOSCHED_BIN)/libauto_schedule.so
	@mkdir -p $(@D)
	HL_GPU_NO_FUS=1 \
	$<  -g max_pool -o $(BIN) -f max_pool_auto_schedule_no_fus target=$(HL_TARGET) auto_schedule=true  -p $(AUTOSCHED_BIN)/libauto_schedule.so -e static_library,h,schedule 





#$(BIN)/process: process.cpp $(BIN)/max_pool.a $(BIN)/max_pool_auto_schedule.a $(BIN)/max_pool_simple_auto_schedule.a $(BIN)/max_pool_auto_schedule_store.a $(BIN)/max_pool_auto_schedule_no_fus.a
$(BIN)/process: process.cpp $(BIN)/max_pool.a $(BIN)/max_pool_auto_schedule.a $(BIN)/max_pool_simple_auto_schedule.a $(BIN)/max_pool_auto_schedule_no_fus.a
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -I$(BIN) -Wall -O3 $^ -o $@ $(LDFLAGS) $(IMAGE_IO_FLAGS) $(CUDA_LDFLAGS) $(OPENCL_LDFLAGS) $(OPENGL_LDFLAGS)

$(BIN)/out.png: $(BIN)/process
	@mkdir -p $(@D)
	$(BIN)/process $(IMAGES)/gray.png 8 1 1 10 $(BIN)/out.png

$(BIN)/out.tiff: $(BIN)/process
	@mkdir -p $(@D)
	$(BIN)/process $(IMAGES)/gray.png 8 1 1 10 $(BIN)/out.tiff


clean:
	rm -rf $(BIN)

test: $(BIN)/out.png $(BIN)/out.tiff

