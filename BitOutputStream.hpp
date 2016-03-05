#ifndef BITOUTPUTSTREAM_HPP
#define BITOUTPUTSTREAM_HPP

#include <iostream>

using namespace std;

class BitOutputStream {

private:
  ostream& out;   
  char buf;    
  int buf_index;     

public:
  //***CHANGED BUF_INDEX DEFAULT FROM 0 TO 8***
  BitOutputStream(ostream& s) : out(s), buf(0), buf_index(8) { }

  // write a bit to the bit stream
  void writeBit(int bit);

  // write a byte to the stream (used for header)
  void writeByte(int i);

  // write an integer to the stream (used for header)
  void writeInt(int i);

  void flush();

private:
  char setBit(char c, int bit, int n);
};

#endif 
