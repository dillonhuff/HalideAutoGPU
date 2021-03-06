include ../support/Makefile.inc
include ../support/autoscheduler.inc

BIN ?= bin

#HL_TARGET=host-cuda

all: $(BIN)/process

$(BIN)/lens_blur.generator: lens_blur_generator.cpp ../autoscheduler/SimpleAutoSchedule.h ../autoscheduler/SimpleAutoSchedule.cpp ../autoscheduler/AutoSchedule.cpp ../autoscheduler/AutoSchedule.h ../autoscheduler/DerivativeUtils.h ../autoscheduler/DerivativeUtils.cpp $(GENERATOR_DEPS)
	@-mkdir -p $(BIN)
	$(CXX) $(CXXFLAGS) -g $(filter-out %.h,$^) -o $@ $(LDFLAGS) $(HALIDE_SYSTEM_LIBS) $(USE_EXPORT_DYNAMIC)

$(BIN)/lens_blur.a: $(BIN)/lens_blur.generator
	@-mkdir -p $(BIN)
	$< -g lens_blur -o $(BIN) -f lens_blur target=$(HL_TARGET)-large_buffers-no_runtime auto_schedule=false


$(BIN)/lens_blur_auto_schedule.a: $(BIN)/lens_blur.generator $(AUTOSCHED_BIN)/libauto_schedule.so 
	@-mkdir -p $(BIN)
	$< -g lens_blur -o $(BIN) -f lens_blur_auto_schedule target=$(HL_TARGET)-large_buffers auto_schedule=true -p $(AUTOSCHED_BIN)/libauto_schedule.so -e static_library,h,assembly,stmt,schedule 

$(BIN)/lens_blur_simple_auto_schedule.a: $(BIN)/lens_blur.generator
	@mkdir -p $(@D)
	HL_USE_SIMPLE_AUTOSCHEDULER=1 \
	$< -g lens_blur -o $(BIN) -f lens_blur_simple_auto_schedule target=$(HL_TARGET)-no_runtime auto_schedule=false -e static_library,h

$(BIN)/lens_blur_auto_schedule_store.a: $(BIN)/lens_blur.generator $(AUTOSCHED_BIN)/libauto_schedule.so
	@mkdir -p $(@D)
	HL_AUTO_FOLDED_FUSION=1 \
	$<  -g lens_blur -o $(BIN) -f lens_blur_auto_schedule_store target=$(HL_TARGET)-large_buffers auto_schedule=true  -p $(AUTOSCHED_BIN)/libauto_schedule.so -e static_library,h,schedule 


$(BIN)/lens_blur_auto_schedule_no_fus.a: $(BIN)/lens_blur.generator $(AUTOSCHED_BIN)/libauto_schedule.so
	@mkdir -p $(@D)
	HL_GPU_NO_FUS=1 \
	$<  -g lens_blur -o $(BIN) -f lens_blur_auto_schedule_no_fus target=$(HL_TARGET)-large_buffers auto_schedule=true  -p $(AUTOSCHED_BIN)/libauto_schedule.so -e static_library,h,schedule 



$(BIN)/process: process.cpp $(BIN)/lens_blur_auto_schedule.a $(BIN)/lens_blur_auto_schedule_store.a  $(BIN)/lens_blur.a $(BIN)/lens_blur_auto_schedule_no_fus.a
	@-mkdir -p $(BIN)
	$(CXX) $(CXXFLAGS) -I$(BIN) -Wall -O3 $^ -o $@ $(LDFLAGS) $(IMAGE_IO_FLAGS) $(CUDA_LDFLAGS) $(OPENCL_LDFLAGS) $(OPENGL_LDFLAGS)

$(BIN)/out.png: $(BIN)/process
	@-mkdir -p $(BIN)
	$(BIN)/process $(IMAGES)/rgb.png 32 13 0.5 32 3 $(BIN)/out.png

clean:
	rm -rf $(BIN)

test: $(BIN)/out.png

