VTUNE_HOME=/opt/intel/oneapi/vtune/latest

SRC_DIR:=src
BIN_DIR:=bin

CXX ?= g++
EXE:=pgsgd

PROF_UTILS=$(CWD)/../ProfileScripts/RoiCppHeaders
CWD:=$(shell pwd)
CXXFLAGS:= -Wall -O3 -fopenmp
INCLUDE_FLAGS := -I$(PROF_UTILS)/include -I$(VTUNE_HOME)/include -I$(CWD)/deps/odgi/src -I$(CWD)/deps/odgi/src/algorithms -I$(CWD)/deps/odgi/deps/libhandlegraph/src/include -I$(CWD)/deps/odgi/deps/DYNAMIC/include -I$(CWD)/deps/odgi/deps/hopscotch-map/include -I$(CWD)/deps/odgi/deps/sparsepp/sparsepp -I$(CWD)/deps/odgi/deps/flat_hash_map -I$(CWD)/deps/odgi/deps/atomicbitvector/include -I$(CWD)/deps/odgi/deps/Xoshiro-cpp -I$(CWD)/deps/odgi/deps/dirtyzipf -I$(CWD)/deps/odgi/deps/gfakluge/src -I$(CWD)/deps/odgi/deps/gfakluge/src/tinyFA -I$(CWD)/deps/odgi/deps/atomic_queue/include/atomic_queue -I$(CWD)/deps/odgi/deps/sdsl-lite/include -I$(CWD)/deps/odgi/deps/PicoSHA2 -I$(CWD)/deps/odgi/deps/lodepng
LD_LIB_DIR_FLAGS := -L$(PROF_UTILS)/lib -L$(VTUNE_HOME)/lib64 -L$(CWD)/deps/odgi/lib
LD_LIB_FLAGS := -lprofileUtils -littnotify -lodgi -fopenmp -ldl
OBJ_FILES := $(SRC_DIR)/main.single.o
OBJ_FILES_PROF := $(SRC_DIR)/main.prof.o

all: $(BIN_DIR)/$(EXE) $(BIN_DIR)/$(EXE).prof

$(SRC_DIR)/%.single.o: src/main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

$(SRC_DIR)/%.prof.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -DTIMER_ENABLED=1 -DVTUNE_ENABLED=1 -DPIN_ENABLED=1 $(INCLUDE_FLAGS) -c -o $@ $<

$(BIN_DIR)/$(EXE): $(OBJ_FILES)
	$(CXX) $(INCLUDE_FLAGS) -o $@ $^ $(LD_LIB_DIR_FLAGS) $(LD_LIB_FLAGS)

$(BIN_DIR)/$(EXE).prof: $(OBJ_FILES_PROF)
	$(CXX) $(INCLUDE_FLAGS) -o $@ $^ $(LD_LIB_DIR_FLAGS) $(LD_LIB_FLAGS) 

clean:
	rm -f $(BIN_DIR)/$(EXE) $(BIN_DIR)/$(EXE).prof && rm -f src/*.o
