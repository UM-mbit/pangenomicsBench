OMP_ENABLED?=0
VTUNE_ENABLED?=0

#Note this is tested on vtune 2024.1
VTUNE_HOME=/opt/intel/oneapi/vtune/latest

SRC_DIR:=src
BIN_DIR:=bin

CXX ?= g++
EXE:=gssw

CWD:=$(shell pwd)
CXXFLAGS:= -O3 -ggdb -g3 -fopenmp -DOMP_ENABLED=$(OMP_ENABLED) -DVTUNE_ENABLED=$(VTUNE_ENABLED)
INCLUDE_FLAGS := -I$(CWD)/../SharedDeps/include -I$(CWD)/deps/Gssw/src -I$(CWD)/deps/vg/deps/nlohmann-json/single_include -I$(CWD)/deps/vg/src -I$(SRC_DIR) -I$(VTUNE_HOME)/include
LD_LIB_DIR_FLAGS := -L$(CWD)/deps/Gssw/lib -L$(VTUNE_HOME)/lib64
LD_LIB_FLAGS := -lgssw -littnotify -fopenmp -ldl
OBJ_FILES := $(SRC_DIR)/eval.o $(SRC_DIR)/loadParams.o $(CWD)/deps/vg/src/gssw_to_json.o $(SRC_DIR)/main.o

all: $(BIN_DIR)/$(EXE)

%.o: %.cpp
	$(CXX) $(CXXFLAGS)  $(INCLUDE_FLAGS) -c -o $@ $<

$(BIN_DIR)/$(EXE): $(OBJ_FILES)
	$(CXX) $(INCLUDE_FLAGS) -o $@ $^ $(LD_LIB_DIR_FLAGS) $(LD_LIB_FLAGS) 

clean:
	rm -f $(BIN_DIR)/$(EXE) && rm -f src/*.o
