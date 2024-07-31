#include "loadParams.h"
#include <stdint.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <utility>

#include "GraphAlignerWrapper.h"
#include "GraphAlignerCommon.h"
#include "CommonUtils.h"

//boost libraries
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

std::pair<Params*, SerializableParams*> loadParams(std::string inputDir){
  std::string stager{""};
  stager = inputDir + "/Inputs/params.bin";
  SerializableParams* sp = new SerializableParams{};
  {
    std::ifstream ifs(stager);
    boost::archive::text_iarchive paramArchive(ifs);
    paramArchive >> *sp;
  }
  Params* params = new Params(*sp);
  return std::pair<Params*, SerializableParams*>(params, sp);
}

std::vector<std::string>* loadSequences(std::string inputDir){
  std::vector<std::string>* seqs = new std::vector<std::string>();
  std::string data("");
  std::string lineNum("");
  std::string line("");
  std::ifstream f(inputDir+"/Inputs/reads.txt");
  while (std::getline(f,line)) {
    std::istringstream lineStream(line);
    std::getline(lineStream, lineNum, ' ');
    //get the remainder (the data)
    std::getline(lineStream, data);
    seqs->push_back(data);
  }
  return seqs;
}

std::vector<std::string>* getRevSequence(std::vector<std::string>* seqs){
  std::vector<std::string>* revSeqs = new std::vector<std::string>();
  for (std::string& seq : *seqs){
    revSeqs->push_back(CommonUtils::ReverseComplement(seq));
  }
  return revSeqs;
}

std::vector<std::vector<ProcessedSeedHit>>* loadSeedHits(
      std::string inputDir, size_t numInputs){
  
  std::vector<std::vector<ProcessedSeedHit>>* clusters = 
      new std::vector<std::vector<ProcessedSeedHit>>(numInputs);
  std::ifstream f(inputDir+"/Inputs/clusters.json");
  nJson data = nJson::parse(f);
  for (int i = 0; i < numInputs; i++){
    for (nJson& hitJson : data[i]["seedHits"]){
      ProcessedSeedHit hit = ProcessedSeedHit(hitJson["seqPos"], 
                                              hitJson["alignmentGraphNodeId"]);
      (*clusters)[i].push_back(hit);
    }
  }
  return clusters;
}


size_t calcNumSlices(std::string& seq){
  //In the origininal code, this line is used, but WordSize seems to be
  //hardcoded to 64, so here I just use 64.
  //numSlices = (sequence.size() + WordConfiguration<Word>::WordSize - 1) / WordConfiguration<Word>::WordSize;
  return (seq.size() + WORD_SIZE - 1) / WORD_SIZE;
}

ReusableState* getReusableState(Common::Params* params){
	  ReusableState* reusableState = new ReusableState{ 
                                          params->graph, 
                                          params->alignmentBandwidth };
    return reusableState;
}


std::vector<std::vector<int64_t>>* getSliceMaxScores(std::string inputDir,
                                                     size_t numInputs){

  std::vector<std::vector<int64_t>>* sliceMaxScores = 
    new std::vector<std::vector<int64_t>>(numInputs);
  std::string lineNum("");
  std::string line("");
  std::string junk("");
  std::ifstream f(inputDir+"/Inputs/maxScores.txt");
  while (std::getline(f,line)) {
    std::string val("");
    std::istringstream lineStream(line);
    std::getline(lineStream, lineNum, ':');
    std::getline(lineStream, junk, ' ');
    while ( std::getline(lineStream, val, ',') ){
      (*sliceMaxScores)[std::stoll(lineNum)].push_back(std::stoll(val));
    }
  }
  return sliceMaxScores;
}

int ld_num_inputs(std::string in_dir){
  std::string data("");
  std::string lineNum("");
  std::ifstream f(in_dir+"/Inputs/reads.txt");

  //scan up until the appropriate line
  std::string line("");
  int i = 0;
  while (std::getline(f,line)){i++;}
  return i;
}
