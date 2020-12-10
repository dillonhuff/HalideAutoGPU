include ../support/Makefile.inc
include ../support/autoscheduler.inc

BIN ?= bin

APP_NAME = gausspyramid

#HL_TARGET +=-debug
#HL_TARGET=host-cuda

all: $(BIN)/process

$(BIN)/$(APP_NAME).generator: $(APP_NAME)_generator.cpp ../autoscheduler/SimpleAutoSchedule.h ../autoscheduler/SimpleAutoSchedule.cpp ../autoscheduler/AutoSchedule.cpp ../autoscheduler/AutoSchedule.h ../autoscheduler/DerivativeUtils.h ../autoscheduler/DerivativeUtils.cpp $(GENERATOR_DEPS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -g $(filter-out %.h,$^) -o $@ $(LDFLAGS) $(HALIDE_SYSTEM_LIBS) $(USE_EXPORT_DYNAMIC)

$(BIN)/$(APP_NAME).a: $(BIN)/$(APP_NAME).generator
	@mkdir -p $(@D)
	$^ -g $(APP_NAME) -o $(BIN) -f $(APP_NAME) target=$(HL_TARGET)-no_runtime auto_schedule=false


$(BIN)/$(APP_NAME)_auto_schedule.a: $(BIN)/$(APP_NAME).generator $(AUTOSCHED_BIN)/libauto_schedule.so
	@mkdir -p $(@D)
	$< -g $(APP_NAME) -o $(BIN) -f $(APP_NAME)_auto_schedule target=$(HL_TARGET) auto_schedule=true -p $(AUTOSCHED_BIN)/libauto_schedule.so  -e static_library,h,schedule

$(BIN)/$(APP_NAME)_simple_auto_schedule.a: $(BIN)/$(APP_NAME).generator
	@mkdir -p $(@D)
	HL_USE_SIMPLE_AUTOSCHEDULER=1 \
	$< -g $(APP_NAME) -o $(BIN) -f $(APP_NAME)_simple_auto_schedule target=$(HL_TARGET)-no_runtime auto_schedule=false -e static_library,h

$(BIN)/$(APP_NAME)_auto_schedule_store.a: $(BIN)/$(APP_NAME).generator $(AUTOSCHED_BIN)/libauto_schedule.so
	@mkdir -p $(@D)
	HL_AUTO_FOLDED_FUSION=1 \
	$<  -g $(APP_NAME) -o $(BIN) -f $(APP_NAME)_auto_schedule_store target=$(HL_TARGET) auto_schedule=true  -p $(AUTOSCHED_BIN)/libauto_schedule.so -e static_library,h,schedule 


$(BIN)/$(APP_NAME)_auto_schedule_no_fus.a: $(BIN)/$(APP_NAME).generator $(AUTOSCHED_BIN)/libauto_schedule.so
	@mkdir -p $(@D)
	HL_GPU_NO_FUS=1 \
	$<  -g $(APP_NAME) -o $(BIN) -f $(APP_NAME)_auto_schedule_no_fus target=$(HL_TARGET) auto_schedule=true  -p $(AUTOSCHED_BIN)/libauto_schedule.so -e static_library,h,schedule 





$(BIN)/process: process.cpp $(BIN)/$(APP_NAME).a $(BIN)/$(APP_NAME)_auto_schedule.a $(BIN)/$(APP_NAME)_simple_auto_schedule.a $(BIN)/$(APP_NAME)_auto_schedule_store.a $(BIN)/$(APP_NAME)_auto_schedule_no_fus.a
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

