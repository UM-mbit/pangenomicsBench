SRC_DIR:=src
BIN_DIR:=bin

CXX ?= g++
EXE:=gbwt

CWD:=$(shell pwd)
CXXFLAGS:= -O3 -ggdb -g3
INCLUDE_FLAGS := -I$(SRC_DIR)
LD_LIB_DIR_FLAGS := 
LD_LIB_FLAGS := 
OBJ_FILES := $(SRC_DIR)/loadParams.o $(SRC_DIR)/main.o $(SRC_DIR)/eval.o

all: $(BIN_DIR)/$(EXE)

%.o: %.cpp
	$(CXX) $(CXXCFLAGS)  $(INCLUDE_FLAGS) -c -o $@ $<

$(BIN_DIR)/$(EXE): $(OBJ_FILES)
	$(CXX) $(INCLUDE_FLAGS) -o $@ $^ $(LD_LIB_DIR_FLAGS) $(LD_LIB_FLAGS) 

clean:
	rm -f $(BIN_DIR)/$(EXE) && rm -f src/*.o
