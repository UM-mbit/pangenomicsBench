VTUNE_HOME=/opt/intel/oneapi/vtune/latest

SRC_DIR:=src
BIN_DIR:=bin
OBJ_DIR:=obj

CXX ?= g++
CC ?= gcc
EXE:=gwfa


PROF_UTILS=$(CWD)/../ProfileScripts/RoiCppHeaders
MINIGRAPH_DIR=$(CWD)/deps/minigraphKernel
CWD:=$(shell pwd)
CXXFLAGS:= -O0 -ggdb -g3 -fopenmp
CFLAGS:= -O0 -ggdb -g3 -fopenmp
INCLUDE_FLAGS :=-I$(PROF_UTILS)/include -I$(SRC_DIR) -I$(MINIGRAPH_DIR) -I$(VTUNE_HOME)/include 
LD_LIB_DIR_FLAGS := -L$(PROF_UTILS)/lib -L$(VTUNE_HOME)/lib64 
LD_LIB_FLAGS := -lprofileUtils -littnotify -fopenmp -ldl -lz -lpthread -ldl
#TODO you're gonna need more than just these obj files
CPP_FILES := main.cpp loadParams.cpp eval.cpp
C_FILES := kalloc.c gfa-io.c gfa-ed.c gfa-base.c
#CPP_SRC_FILES := $(addprefix $(SRC_DIR)/, $(CPP_FILES))
#C_SRC_FILES := $(addprefix $(MINIGRAPH_DIR)/, $(C_FILES))
C_OBJ_FILES := $(patsubst %, $(OBJ_DIR)/%.o, $(C_FILES))
CPP_OBJ_FILES_SINGLE := $(patsubst %, $(OBJ_DIR)/%.single.o, $(CPP_FILES))
CPP_OBJ_FILES_PROF := $(patsubst %, $(OBJ_DIR)/%.prof.o, $(CPP_FILES))
CPP_OBJ_FILES_OMP := $(patsubst %, $(OBJ_DIR)/%.omp.o, $(CPP_FILES))

#all:$(OBJ_DIR) $(BIN_DIR) $(BIN_DIR)/$(EXE).omp $(BIN_DIR)/$(EXE).prof $(BIN_DIR)/$(EXE)
all:$(OBJ_DIR) $(BIN_DIR) $(BIN_DIR)/$(EXE) $(BIN_DIR)/$(EXE).omp $(BIN_DIR)/$(EXE).prof

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR)/%.cpp.single.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c -o $@ $^
$(OBJ_DIR)/%.cpp.omp.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c -o $@ $^ -DTHREADING_ENABLED=1 -DTIMER_ENABLED=1
$(OBJ_DIR)/%.cpp.prof.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c -o $@ $^ -DTIMER_ENABLED=1 -DVTUNE_ENABLED=1 -DPIN_ENABLED=1

$(OBJ_DIR)/%.c.o: $(MINIGRAPH_DIR)/%.c
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c -o $@ $^

$(BIN_DIR)/$(EXE): $(CPP_OBJ_FILES_SINGLE) $(C_OBJ_FILES)
	$(CXX) $(INCLUDE_FLAGS) -o $@ $^ $(LD_LIB_DIR_FLAGS) $(LD_LIB_FLAGS) 
$(BIN_DIR)/$(EXE).omp: $(CPP_OBJ_FILES_OMP) $(C_OBJ_FILES)
	$(CXX) $(INCLUDE_FLAGS) -o $@ $^ $(LD_LIB_DIR_FLAGS) $(LD_LIB_FLAGS) 
$(BIN_DIR)/$(EXE).prof: $(CPP_OBJ_FILES_PROF) $(C_OBJ_FILES)
	$(CXX) $(INCLUDE_FLAGS) -o $@ $^ $(LD_LIB_DIR_FLAGS) $(LD_LIB_FLAGS) 


clean:
	rm -f $(BIN_DIR)/*
	rm -f $(OBJ_DIR)/*
