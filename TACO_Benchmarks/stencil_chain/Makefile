include ../support/Makefile.inc
include ../support/autoscheduler.inc

BIN ?= bin

#HL_TARGET=host-cuda-cuda_capability_35

all: $(BIN)/process

$(BIN)/stencil_chain.generator: stencil_chain_generator.cpp ../autoscheduler/SimpleAutoSchedule.h ../autoscheduler/AutoSchedule.cpp ../autoscheduler/AutoSchedule.h ../autoscheduler/SimpleAutoSchedule.cpp ../autoscheduler/DerivativeUtils.h ../autoscheduler/DerivativeUtils.cpp $(GENERATOR_DEPS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -g $(filter-out %.h,$^) -o $@ $(LDFLAGS) $(HALIDE_SYSTEM_LIBS) $(USE_EXPORT_DYNAMIC)

$(BIN)/stencil_chain.a: $(BIN)/stencil_chain.generator
	@mkdir -p $(@D)
	$^ -g stencil_chain -o $(BIN) -f stencil_chain target=$(HL_TARGET) auto_schedule=false



$(BIN)/stencil_chain_auto_schedule.a: $(BIN)/stencil_chain.generator  $(AUTOSCHED_BIN)/libauto_schedule.so
	@mkdir -p $(@D)
	$< -g stencil_chain -o $(BIN) -f stencil_chain_auto_schedule target=$(HL_TARGET)-no_runtime auto_schedule=true -p $(AUTOSCHED_BIN)/libauto_schedule.so  -e stmt,assembly,static_library,h 2> stderr.txt > stdout.txt

$(BIN)/stencil_chain_simple_auto_schedule.a: $(BIN)/stencil_chain.generator
	@mkdir -p $(@D)
	HL_USE_SIMPLE_AUTOSCHEDULER=1 \
	$< -g stencil_chain -o $(BIN) -f stencil_chain_simple_auto_schedule target=$(HL_TARGET)-no_runtime auto_schedule=false -e static_library,h

$(BIN)/stencil_chain_auto_schedule_store.a: $(BIN)/stencil_chain.generator $(AUTOSCHED_BIN)/libauto_schedule.so
	@mkdir -p $(@D)
	HL_AUTO_FOLDED_FUSION=1 \
	$<  -g stencil_chain -o $(BIN) -f stencil_chain_auto_schedule_store target=$(HL_TARGET) auto_schedule=true  -p $(AUTOSCHED_BIN)/libauto_schedule.so -e static_library,h,schedule 


$(BIN)/stencil_chain_auto_schedule_no_fus.a: $(BIN)/stencil_chain.generator $(AUTOSCHED_BIN)/libauto_schedule.so
	@mkdir -p $(@D)
	HL_GPU_NO_FUS=1 \
	$<  -g stencil_chain -o $(BIN) -f stencil_chain_auto_schedule_no_fus target=$(HL_TARGET) auto_schedule=true  -p $(AUTOSCHED_BIN)/libauto_schedule.so -e static_library,h,schedule 





$(BIN)/process: process.cpp $(BIN)/stencil_chain.a  $(BIN)/stencil_chain_auto_schedule.a $(BIN)/stencil_chain_simple_auto_schedule.a $(BIN)/stencil_chain_auto_schedule_store.a $(BIN)/stencil_chain_auto_schedule_no_fus.a
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -I$(BIN) -Wall -O3 $^ -o $@ $(LDFLAGS) $(IMAGE_IO_FLAGS) $(CUDA_LDFLAGS) $(OPENCL_LDFLAGS) $(OPENGL_LDFLAGS)

$(BIN)/out.png: $(BIN)/process
	@mkdir -p $(@D)
	$(BIN)/process $(IMAGES)/rgb.png 10 $(BIN)/out.png

clean:
	rm -rf $(BIN)

test: $(BIN)/out.png

