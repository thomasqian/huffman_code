#include "BitInputStream.hpp"
#include "BitOutputStream.hpp"
#include "HCTree.hpp"
#include "HCNode.hpp"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

/**
 * Huffman code compression program.
 */

void write3Bytes(BitOutputStream& bos, int i) {
    char *c = (char*)&i;
    for (int j = 2; j >= 0; --j) bos.writeByte(*(c + j));
}

int main(int argc, char** argv) {

    //check for proper args
    if (argc != 3) { 
        cerr << "./compress called with incorrect arguments." << endl;
        cerr << "Usage: ./compress infile outfile" << endl;
        exit(EXIT_FAILURE);
    }

    ifstream in;
    in.open(argv[1], ios::binary);
    if (!in.good()) {
        cerr << "Could not open input file for reading." << endl;
        exit(EXIT_FAILURE);
    } 

    ofstream out;
    out.open(argv[2], ios::binary);
    if (!out.good()) {
        cerr << "Could not open output file for writing." << endl;
        exit(EXIT_FAILURE);
    }

    //read file and create frequency array
    BitInputStream bis (in);
    vector<int> freqs (256, 0);
    int symbol, inBytes = 0, distinctVals = 0;

    cout << "Reading from file \"" << argv[1] << "\"...";

    while (1) {
        symbol = bis.readByte();
        if (!in.good()) break;
        inBytes++;
        if (!freqs[symbol]++) distinctVals++;
    }

    //frequency count for debugging
    /*for (int i = 0; i < 256; ++i) 
        if (freqs[i]) printf("%d: %d\n", i, freqs[i]);*/

    cout << " done." << endl;
    printf("Found %d unique symbols in input file of size %d bytes.\n", 
        distinctVals, inBytes);

    cout << "Building Huffman code tree...";

    //construct Huffman tree
    HCTree tree = HCTree();
    tree.build(freqs);

    cout << " done." << endl;
    cout << "Writing to file \"" << argv[2] << "\"...";

    //output tree header to output file
    int outBytes = 0;
    BitOutputStream bos (out);

    /**
     * At 192 unique symbols, the efficiency of using 3 bytes
     * to store each frequency equals the efficiency of using
     * 4 bytes to store each unique symbol/frequency pair.
     */
    /*** HEADER IF LESS THAN 192 UNIQUE SYMBOLS ***/
    if (distinctVals < 192) {
        //first byte tells uncompress which header to use
        bos.writeByte(distinctVals);
        for (int i = 0; i < freqs.size(); ++i) { 
            //only wite if symbol is present
            if (freqs[i]) {
                bos.writeByte(i);
                write3Bytes(bos, freqs[i]);
                outBytes += 4;
            }
        }
    } 
    /*** HEADER IF GTE TO 192 UNIQUE SYMBOLS ***/
    else {
        //first byte tells uncompress which header to use
        bos.writeByte(0);
        //write all frequencies using 3 bytes each
        for (int i = 0; i < freqs.size(); ++i) { 
            write3Bytes(bos, freqs[i]);
            outBytes += 3;
        }
    }

    //rewind file to beginning
    in.clear();
    in.seekg(0);

    //count bits for output writing statistics
    int bitCount = 0;

    //encode file into binary
    while (1) {
        symbol = bis.readByte();
        if (!in.good()) break;
        bitCount += tree.encode(symbol, bos);
    }
    bos.flush();
    bitCount += 8;

    //add encoded bits to header bytes
    outBytes += bitCount/8;

    cout << " done." << endl;
    printf("Output file has size %d bytes.\n", outBytes);

    in.close();
    out.close();

    return 0;
}

