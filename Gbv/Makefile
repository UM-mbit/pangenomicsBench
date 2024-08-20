OMP_ENABLED?=0
VTUNE_ENABLED?=0

#Note this is tested on vtune 2024.1
VTUNE_HOME=/opt/intel/oneapi/vtune/latest

#this file is mostly canabilized from graphaligner. We're not linking 
#Graphaligner as a library, we're pretty much building this kernal code 
#thing as a part of graphaligner
SRC_DIR:=src
BIN_DIR:=bin
OBJ_DIR:=obj
$(shell mkdir -p bin)
$(shell mkdir -p obj)

CXX ?= g++
EXE:=gbv

CWD:=$(shell pwd)
GA_ROOT:=$(CWD)/deps/GraphAligner
CXXFLAGS:=-Wall -Wextra -std=c++17 -O3 -g3  -I$(SRC_DIR) -I$(GA_ROOT)/src -I$(GA_ROOT)/concurrentqueue -I$(GA_ROOT)/BBHash -I$(GA_ROOT)/zstr/src -I$(GA_ROOT)/parallel-hashmap/parallel_hashmap/ `pkg-config --cflags protobuf` `pkg-config --cflags libsparsehash` -Wno-unknown-pragmas -Wno-unused-parameter -I$(GA_ROOT)/MEMfinder/src -I`jemalloc-config --includedir` -I/opt/intel/oneapi/vtune/latest/include -I/home/kaplannp/miniconda3/envs/GraphAligner/include -I$(GA_ROOT)/include/json/single_include -DOMP_ENABLED=$(OMP_ENABLED) -DVTUNE_ENABLED=$(VTUNE_ENABLED) -I$(CWD)/../SharedDeps/include
LIBS=-lm -lz -lboost_program_options `pkg-config --libs protobuf` -lsdsl -L/usr/local/lib -L$(VTUNE_HOME)/lib64 -littnotify -lboost_serialization -ldl
JEMALLOCFLAGS= -L`jemalloc-config --libdir` -Wl,-rpath,`jemalloc-config --libdir` -Wl,-Bstatic -ljemalloc -Wl,-Bdynamic `jemalloc-config --libs`
_DEPS = vg.pb.h fastqloader.h GraphAlignerWrapper.h vg.pb.h BigraphToDigraph.h stream.hpp Aligner.h ThreadReadAssertion.h AlignmentGraph.h CommonUtils.h GfaGraph.h ReadCorrection.h MinimizerSeeder.h AlignmentSelection.h EValue.h MEMSeeder.h DNAString.h DiploidHeuristic.h
DEPS = $(patsubst %, $(GA_ROOT)/src/%, $(_DEPS)) $(SRC_DIR)/eval.h $(SRC_DIR)/loadParams.h

_OBJ = Aligner.o vg.pb.o fastqloader.o BigraphToDigraph.o ThreadReadAssertion.o AlignmentGraph.o CommonUtils.o GraphAlignerWrapper.o GfaGraph.o ReadCorrection.o MinimizerSeeder.o AlignmentSelection.o EValue.o MEMSeeder.o DNAString.o DiploidHeuristic.o eval.o loadParams.o main.o
OBJ = $(patsubst %, $(OBJ_DIR)/%, $(_OBJ)) $(OBJ_DIR)/eval.o $(OBJ_DIR)/main.o  $(OBJ_DIR)/loadParams.o 

JEMALLOCFLAGS= -L`jemalloc-config --libdir` -Wl,-rpath,`jemalloc-config --libdir` -Wl,-Bstatic -ljemalloc -Wl,-Bdynamic `jemalloc-config --libs`
LINKFLAGS = $(CXXFLAGS) -Wl,-Bstatic $(LIBS) -Wl,-Bdynamic -Wl,--as-needed -lpthread -pthread -static-libstdc++ $(JEMALLOCFLAGS) `pkg-config --libs libdivsufsort` `pkg-config --libs libdivsufsort64`


all: $(BIN_DIR)/$(EXE)

$(BIN_DIR)/$(EXE): $(OBJ) $(GA_ROOT)/MEMfinder/lib/memfinder.a
	$(CXX) -o $@ $^ $(LINKFLAGS)

$(OBJ_DIR)/GraphAlignerWrapper.o: $(GA_ROOT)/src/GraphAlignerWrapper.cpp $(GA_ROOT)/src/GraphAligner.h $(GA_ROOT)/src/NodeSlice.h $(GA_ROOT)/src/WordSlice.h $(GA_ROOT)/src/ArrayPriorityQueue.h $(GA_ROOT)/src/ComponentPriorityQueue.h $(GA_ROOT)/src/GraphAlignerVGAlignment.h $(GA_ROOT)/src/GraphAlignerGAFAlignment.h $(GA_ROOT)/src/GraphAlignerBitvectorBanded.h $(GA_ROOT)/src/GraphAlignerBitvectorCommon.h $(GA_ROOT)/src/GraphAlignerCommon.h $(DEPS)

$(OBJ_DIR)/eval.o: src/eval.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)
$(OBJ_DIR)/loadParams.o: src/loadParams.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)
$(OBJ_DIR)/main.o: src/main.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(OBJ_DIR)/%.o: $(GA_ROOT)/src/%.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<


$(OBJ_DIR)/vg.pb.o: $(GA_ROOT)/src/vg.pb.cc
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(GA_ROOT)/MEMfinder/lib/memfinder.a:
	$(MAKE) -C $(GA_ROOT)/MEMfinder lib DEBUGFLAG="-DNDEBUG"

$(GA_ROOT)/src/%.pb.cc $(GA_ROOT)/src/%.pb.h: $(GA_ROOT)/src/%.proto
	protoc -I=$(GA_ROOT)/src --cpp_out=$(GA_ROOT)/src $<

clean:
	rm -f $(BIN_DIR)/*
	rm -f $(OBJ_DIR)/*
	rm -f $(GA_ROOT)/src/vg.pb.cc
	rm -f $(GA_ROOT)/src/vg.pb.h
