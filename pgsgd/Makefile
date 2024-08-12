SRC_DIR:=src
BIN_DIR:=bin

CXX ?= g++
EXE:=pgsgd

CWD:=$(shell pwd)
CXXFLAGS:= -O3 -ggdb -g3
INCLUDE_FLAGS := -I$(CWD)/deps/odgi/src -I$(CWD)/deps/odgi/deps/libhandlegraph/src/include -I$(CWD)/deps/odgi/deps/DYNAMIC/include -I$(CWD)/deps/odgi/deps/hopscotch-map/include -I$(CWD)/deps/odgi/deps/sparsepp/sparsepp -I$(CWD)/deps/odgi/deps/flat_hash_map -I$(CWD)/deps/odgi/deps/atomicbitvector/include -I$(CWD)/deps/odgi/deps/Xoshiro-cpp -I$(CWD)/deps/odgi/deps/dirtyzipf
LD_LIB_DIR_FLAGS := -L$(CWD)/deps/odgi/lib
LD_LIB_FLAGS := 
OBJ_FILES := $(SRC_DIR)/main.o

all: $(BIN_DIR)/$(EXE)

%.o: %.cpp
	$(CXX) $(CXXCFLAGS)  $(INCLUDE_FLAGS) -c -o $@ $<

$(BIN_DIR)/$(EXE): $(OBJ_FILES)
	$(CXX) $(INCLUDE_FLAGS) -o $@ $^ $(LD_LIB_DIR_FLAGS) $(LD_LIB_FLAGS) 

clean:
	rm -f $(BIN_DIR)/$(EXE) && rm -f src/*.o
