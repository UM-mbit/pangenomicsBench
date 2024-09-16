OMP_ENABLED?=0
VTUNE_ENABLED?=0

VTUNE_HOME=/opt/intel/oneapi/vtune/latest

SRC_DIR:=src
BIN_DIR:=bin
OBJ_DIR:=obj

CXX ?= g++
EXE:=cacheUBenchmark


PROF_UTILS=$(CWD)/../ProfileScripts/RoiCppHeaders
CWD:=$(shell pwd)
CXXFLAGS:= -O3 -ggdb -g3 -fopenmp
INCLUDE_FLAGS :=-I$(PROF_UTILS)/include -I$(SRC_DIR)  -I$(VTUNE_HOME)/include 
LD_LIB_DIR_FLAGS := -L$(PROF_UTILS)/lib -L$(VTUNE_HOME)/lib64 
LD_LIB_FLAGS := -lprofileUtils -littnotify -fopenmp -ldl
OBJ_FILES := $(OBJ_DIR)/main.single.o 
OBJ_FILES_PROF := $(OBJ_DIR)/main.prof.o 
OBJ_FILES_OMP := $(OBJ_DIR)/main.omp.o

all:$(OBJ_DIR) $(BIN_DIR) $(BIN_DIR)/$(EXE).omp $(BIN_DIR)/$(EXE).prof $(BIN_DIR)/$(EXE)


$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR)/%single.o: src/main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c -o $@ src/main.cpp

$(OBJ_DIR)/%.omp.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -DTIMER_ENABLED=1 -DTHREADING_ENABLED=1 $(INCLUDE_FLAGS) -c -o $@ src/main.cpp

$(OBJ_DIR)/%.prof.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -DTIMER_ENABLED=1 -DVTUNE_ENABLED=1 -DPIN_ENABLED=1 $(INCLUDE_FLAGS) -c -o $@ src/main.cpp

$(BIN_DIR)/$(EXE): $(OBJ_FILES)
	$(CXX) $(INCLUDE_FLAGS) -o $@ $^ $(LD_LIB_DIR_FLAGS) $(LD_LIB_FLAGS) 

$(BIN_DIR)/$(EXE).prof: $(OBJ_FILES_PROF)
	$(CXX) $(INCLUDE_FLAGS) -o $@ $^ $(LD_LIB_DIR_FLAGS) $(LD_LIB_FLAGS) 

$(BIN_DIR)/$(EXE).omp: $(OBJ_FILES_OMP)
	$(CXX) $(INCLUDE_FLAGS) -o $@ $^ $(LD_LIB_DIR_FLAGS) $(LD_LIB_FLAGS) 

clean:
	rm -f $(BIN_DIR)/*
	rm -f $(OBJ_DIR)/*
