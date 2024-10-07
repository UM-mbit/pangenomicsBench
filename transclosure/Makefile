#VTUNE_HOME=/opt/intel/oneapi/vtune/latest

SRC_DIR:=src
BIN_DIR:=bin

CXX ?= g++
EXE:=transclosure

#PROF_UTILS=$(CWD)/../ProfileScripts/RoiCppHeaders
CWD:=$(shell pwd)
CXXFLAGS:= -Wall -O3
INCLUDE_FLAGS := -Ideps/seqwish/src #-I$(PROF_UTILS)/include -I$(VTUNE_HOME)/include
LD_LIB_DIR_FLAGS := -L$(CWD)/deps/seqwish/lib #-L$(PROF_UTILS)/lib -L$(VTUNE_HOME)/lib64
LD_LIB_FLAGS := -lseqwish #-lprofileUtils -littnotify -ldl
OBJ_FILES := $(SRC_DIR)/main.single.o
#OBJ_FILES_PROF := $(SRC_DIR)/main.prof.o

all: $(BIN_DIR)/$(EXE) #$(BIN_DIR)/$(EXE).prof

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
