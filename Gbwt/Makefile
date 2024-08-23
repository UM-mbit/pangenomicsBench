OMP_ENABLED?=0
VTUNE_ENABLED?=0

#Note this is tested on vtune 2024.1
VTUNE_HOME=/opt/intel/oneapi/vtune/latest

SRC_DIR:=src
BIN_DIR:=bin

CXX ?= g++
EXE:=gbwt

CWD:=$(shell pwd)
GBWT_HOME:=$(CWD)
CXXFLAGS:= -O3 -ggdb -g3 -fopenmp -DOMP_ENABLED=$(OMP_ENABLED) -DVTUNE_ENABLED=$(VTUNE_ENABLED)
INCLUDE_FLAGS :=-I$(CWD)/../SharedDeps/include -I$(SRC_DIR) -I$(GBWT_HOME)/deps/gbwt/include -I$(GBWT_HOME)/deps/sdsl-lite/include -I$(VTUNE_HOME)/include 
LD_LIB_DIR_FLAGS := -L$(GBWT_HOME)/deps/gbwt/lib -L$(GBWT_HOME)/deps/sdsl-lite/lib -L$(VTUNE_HOME)/lib64
LD_LIB_FLAGS := -lgbwt -lsdsl -littnotify -fopenmp -ldl
OBJ_FILES := $(SRC_DIR)/loadParams.o $(SRC_DIR)/main.o $(SRC_DIR)/eval.o

all: $(BIN_DIR)/$(EXE)

%.o: %.cpp
	$(CXX) $(CXXFLAGS)  $(INCLUDE_FLAGS) -c -o $@ $<

$(BIN_DIR)/$(EXE): $(OBJ_FILES)
	$(CXX) $(INCLUDE_FLAGS) -o $@ $^ $(LD_LIB_DIR_FLAGS) $(LD_LIB_FLAGS) 

clean:
	rm -f $(BIN_DIR)/$(EXE) && rm -f src/*.o
