#include "loadParams.h"
const int8_t* get_nt_table(size_t seqLen){
//This one is a little funky. In vg it is initialized according to a constant
//pattern that uses some for loops. Here we've just taken the first 255 elements
//and will use substrings to generate however many are needed
  static int8_t full_nt_table[255]{4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 4, 1, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 4, 1, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 71, 67, 84, 67, 0, -15, 32, -96, -94, 127, 0, 0, -128, -53, 32, -96, -94, 127, 0, 0, -128, -105, 33, -96, -94, 127, 0, 0, 48, -1, -122, -96, -94, 127, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 83, 50, 50, 53, 95, 52, 56, 50, 49, 52, 57, 0, -2, 127, 0, 0, 80, -1, -122, -96, -94, 127, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 83, 50, 50, 53, 95, 52, 56, 50, 49, 52, 57, 0, 71, 84, 67, 71, -92, 0, 0, 0, 65, 71, 65, 67, 13, 0, 0, 0, 0, 0, 0, 0, -92, 0, 0, 0, -91, 0, 0, 0, 0, 4, 4, 4, 4, 4};
  int8_t* nt_table = new int8_t[255];
  for (int i = 0; i < seqLen; i++){
    nt_table[i] = full_nt_table[i];
  }
  return nt_table;
}
