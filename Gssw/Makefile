#Note this is tested on vtune 2024.1
#VTUNE_HOME=/opt/intel/oneapi/vtune/latest

SRC_DIR:=src
BIN_DIR:=bin
OBJ_DIR:=obj

CXX ?= g++-11
EXE:=gssw

CWD:=$(shell pwd)
DEP_SRC_DIR:=$(CWD)/deps/vg/src
PROF_UTILS=$(CWD)/../ProfileScripts/RoiCppHeaders
CXXFLAGS:= -O3 -ggdb -g3 -fopenmp
INCLUDE_FLAGS := -I$(PROF_UTILS)/include -I$(CWD)/deps/Gssw/src -I$(CWD)/deps/vg/deps/nlohmann-json/single_include -I$(CWD)/deps/vg/src -I$(SRC_DIR) -I$(VTUNE_HOME)/include
LD_LIB_DIR_FLAGS := -L$(PROF_UTILS)/lib -L$(CWD)/deps/Gssw/lib -L$(VTUNE_HOME)/lib64
LD_LIB_FLAGS := -lgssw -lprofileUtils -littnotify -fopenmp -ldl

#ROOT is just a stem, we specify in later commands
OBJ_FILES_ROOT := $(OBJ_DIR)/eval.cpp $(OBJ_DIR)/loadParams.cpp $(OBJ_DIR)/gssw_to_json.cpp $(OBJ_DIR)/main.cpp
OBJ_FILES_SINGLE := $(addsuffix .single.o, $(OBJ_FILES_ROOT))
OBJ_FILES_PROF := $(addsuffix .prof.o, $(OBJ_FILES_ROOT))
OBJ_FILES_OMP := $(addsuffix .omp.o, $(OBJ_FILES_ROOT))
#OBJ_FILES := $(SRC_DIR)/eval.o $(SRC_DIR)/loadParams.o $(CWD)/deps/vg/src/gssw_to_json.o $(SRC_DIR)/main.o

all:$(OBJ_DIR) $(BIN_DIR) $(BIN_DIR)/$(EXE) $(BIN_DIR)/$(EXE).omp $(BIN_DIR)/$(EXE).prof

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

#TODO will not build if you don't have conda py38 environment
$(DEP_SRC_DIR)/vg.pb.h:
	cd deps/vg && /home/kaplannp/miniconda3/bin/activate py38 && . ./source_me.sh && make -j


#BEGIN SINGLE THREADED REGION 
$(OBJ_DIR)/gssw_to_json.cpp.single.o: $(DEP_SRC_DIR)/gssw_to_json.cpp $(DEP_SRC_DIR)/vg.pb.h
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c -o $@ $(DEP_SRC_DIR)/gssw_to_json.cpp

$(OBJ_DIR)/%.cpp.single.o: $(SRC_DIR)/%.cpp 
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c -o $@ $^

$(BIN_DIR)/$(EXE): $(OBJ_FILES_SINGLE)
	$(CXX) $(INCLUDE_FLAGS) -o $@ $^ $(LD_LIB_DIR_FLAGS) $(LD_LIB_FLAGS)
#END SINGLE THREADED REGION 

#BEGIN OMP THREADED REGION 
$(OBJ_DIR)/gssw_to_json.cpp.omp.o: $(DEP_SRC_DIR)/gssw_to_json.cpp $(DEP_SRC_DIR)/vg.pb.h
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c -o $@ $(DEP_SRC_DIR)/gssw_to_json.cpp -DTHREADING_ENABLED=1 -DTIMER_ENABLED=1

$(OBJ_DIR)/%.cpp.omp.o: $(SRC_DIR)/%.cpp 
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c -o $@ $^ -DTHREADING_ENABLED=1 -DTIMER_ENABLED=1

$(BIN_DIR)/$(EXE).omp: $(OBJ_FILES_OMP)
	$(CXX) $(INCLUDE_FLAGS) -o $@ $^ $(LD_LIB_DIR_FLAGS) $(LD_LIB_FLAGS)
#END OMP THREADED REGION 

#BEGIN PROFILABLE REGION 
$(OBJ_DIR)/gssw_to_json.cpp.prof.o: $(DEP_SRC_DIR)/gssw_to_json.cpp $(DEP_SRC_DIR)/vg.pb.h
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c -o $@ $(DEP_SRC_DIR)/gssw_to_json.cpp -DTIMER_ENABLED=1 -DVTUNE_ENABLED=1 -DPIN_ENABLED=1

$(OBJ_DIR)/%.cpp.prof.o: $(SRC_DIR)/%.cpp 
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c -o $@ $^ -DTIMER_ENABLED=1 -DVTUNE_ENABLED=1 -DPIN_ENABLED=1

$(BIN_DIR)/$(EXE).prof: $(OBJ_FILES_PROF)
	$(CXX) $(INCLUDE_FLAGS) -o $@ $^ $(LD_LIB_DIR_FLAGS) $(LD_LIB_FLAGS)
#END PROFILABLE REGION 


clean:
	rm -f $(BIN_DIR)/$(EXE)
	rm -f $(OBJ_DIR)/*.o
	#cd deps/vg && make clean
