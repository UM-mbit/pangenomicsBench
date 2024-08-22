OMP_ENABLED?=0
VTUNE_ENABLED?=0

VTUNE_HOME=/opt/intel/oneapi/vtune/latest

SRC_DIR:=src
BIN_DIR:=bin
OBJ_DIR:=obj

CXX ?= g++
EXE:=dummy

CWD:=$(shell pwd)
CXXFLAGS:= -O3 -ggdb -g3 -fopenmp -DOMP_ENABLED=$(OMP_ENABLED) -DVTUNE_ENABLED=$(VTUNE_ENABLED)
INCLUDE_FLAGS :=-I$(CWD)/../SharedDeps/include -I$(SRC_DIR)  -I$(VTUNE_HOME)/include 
LD_LIB_DIR_FLAGS := -L$(VTUNE_HOME)/lib64
LD_LIB_FLAGS := -littnotify -fopenmp -ldl
OBJ_FILES := $(OBJ_DIR)/main.o $(OBJ_DIR)/pinRoi.o

all: $(BIN_DIR)/$(EXE)

$(OBJ_DIR)/pinRoi.o: ../SharedDeps/SharedSrc/pinRoi.cpp
	$(CXX) $(CXXFLAGS)  $(INCLUDE_FLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: src/main.cpp
	$(CXX) $(CXXFLAGS)  $(INCLUDE_FLAGS) -c -o $@ $<

$(BIN_DIR)/$(EXE): $(OBJ_FILES)
	$(CXX) $(INCLUDE_FLAGS) -o $@ $^ $(LD_LIB_DIR_FLAGS) $(LD_LIB_FLAGS) 

clean:
	rm -f $(BIN_DIR)/*
	rm -f $(OBJ_DIR)/*
