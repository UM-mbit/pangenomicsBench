SRC_DIR:=src
BIN_DIR:=bin

CXX ?= g++
EXE:=gssw

CWD:=$(shell pwd)
CXXFLAGS:= -O3 -ggdb -g3
INCLUDE_FLAGS := -I$(CWD)/deps/Gssw/src -I$(CWD)/deps/vg/deps/nlohmann-json/single_include -I$(CWD)/deps/vg/src -I$(SRC_DIR)
LD_LIB_DIR_FLAGS := -L$(CWD)/deps/Gssw/lib
LD_LIB_FLAGS := -lgssw
OBJ_FILES := $(SRC_DIR)/eval.o $(SRC_DIR)/loadParams.o $(CWD)/deps/vg/src/gssw_to_json.o $(SRC_DIR)/main.o

all: $(BIN_DIR)/$(EXE)

%.o: %.cpp
	$(CXX) $(CXXCFLAGS)  $(INCLUDE_FLAGS) -c -o $@ $<

$(BIN_DIR)/$(EXE): $(OBJ_FILES)
	$(CXX) $(INCLUDE_FLAGS) -o $@ $^ $(LD_LIB_DIR_FLAGS) $(LD_LIB_FLAGS) 

clean:
	rm -f $(BIN_DIR)/$(EXE) && rm -f src/*.o
