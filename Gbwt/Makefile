#Note this is tested on vtune 2024.1
#VTUNE_HOME=/opt/intel/oneapi/vtune/latest

SRC_DIR:=src
BIN_DIR:=bin
OBJ_DIR:=obj

CXX ?= g++
EXE:=gbwt

CWD:=$(shell pwd)
GBWT_HOME:=$(CWD)
PROF_UTILS=$(CWD)/../ProfileScripts/RoiCppHeaders
CXXFLAGS:= -O3 -ggdb -g3 -fopenmp
INCLUDE_FLAGS :=-I$(PROF_UTILS)/include -I$(SRC_DIR) -I$(GBWT_HOME)/deps/gbwt/include -I$(GBWT_HOME)/deps/sdsl-lite/include -I$(VTUNE_HOME)/include 
LD_LIB_DIR_FLAGS := -L$(PROF_UTILS)/lib -L$(GBWT_HOME)/deps/gbwt/lib -L$(GBWT_HOME)/deps/sdsl-lite/lib -L$(VTUNE_HOME)/lib64
LD_LIB_FLAGS := -lgbwt -lprofileUtils -lsdsl -littnotify -fopenmp -ldl
#ROOT is just a stem, we specify in later commands
OBJ_FILES_ROOT := $(OBJ_DIR)/loadParams $(OBJ_DIR)/main $(OBJ_DIR)/eval
OBJ_FILES_SINGLE := $(addsuffix .single.o, $(OBJ_FILES_ROOT))
OBJ_FILES_PROF := $(addsuffix .prof.o, $(OBJ_FILES_ROOT))
OBJ_FILES_OMP := $(addsuffix .omp.o, $(OBJ_FILES_ROOT))

all: $(OBJ_DIR) $(BIN_DIR) $(BIN_DIR)/$(EXE) $(BIN_DIR)/$(EXE).omp $(BIN_DIR)/$(EXE).prof

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

#BEGIN SINGLE THREADED REGION
$(OBJ_DIR)/%.single.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS)  $(INCLUDE_FLAGS) -c -o $@ $<

$(BIN_DIR)/$(EXE): $(OBJ_FILES_SINGLE)
	$(CXX) $(INCLUDE_FLAGS) -o $@ $^ $(LD_LIB_DIR_FLAGS) $(LD_LIB_FLAGS) 
#END SINGLE THREADED REGION

#BEGIN MULTI THREADED REGION
$(OBJ_DIR)/%.omp.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS)  $(INCLUDE_FLAGS) -c -o $@ $< -DTHREADING_ENABLED=1 -DTIMER_ENABLED=1

$(BIN_DIR)/$(EXE).omp: $(OBJ_FILES_OMP)
	$(CXX) $(INCLUDE_FLAGS) -o $@ $^ $(LD_LIB_DIR_FLAGS) $(LD_LIB_FLAGS) 
#END MULTI THREADED REGION

#BEGIN MULTI THREADED REGION
$(OBJ_DIR)/%.prof.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS)  $(INCLUDE_FLAGS) -c -o $@ $< -DTIMER_ENABLED=1 -DVTUNE_ENABLED=1 -DPIN_ENABLED=1

$(BIN_DIR)/$(EXE).prof: $(OBJ_FILES_PROF)
	$(CXX) $(INCLUDE_FLAGS) -o $@ $^ $(LD_LIB_DIR_FLAGS) $(LD_LIB_FLAGS) 
#END MULTI THREADED REGION


clean:
	rm -f $(BIN_DIR)/$(EXE) && rm -f src/*.o
