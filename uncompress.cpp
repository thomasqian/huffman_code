#include "BitInputStream.hpp"
#include "BitOutputStream.hpp"
#include "HCTree.hpp"
#include "HCNode.hpp"
#include <iostream>
#include <vector>

using namespace std;

/**
 * Huffman code uncompression program.
 */

int read3Bytes(BitInputStream& bis) {
    int a = bis.readByte();
    int b = bis.readByte();
    int c = bis.readByte();

    a = a << 16;
    b = b << 8;

    return a + b + c;
}

int main(int argc, char** argv) {

    //check for proper args
    if (argc != 3) { 
        cerr << "./uncompress called with incorrect arguments." << endl;
        cerr << "Usage: ./uncompress infile outfile" << endl;
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

    cout << "Reading header from file \"" << argv[1] << "\"...";
    int distinctVals = 0, outBytes = 0;
    
    //reconstruct Huffman tree
    BitInputStream bis (in);
    vector<int> freqs (256, 0);
    HCTree tree = HCTree();

    //first byte of header tells us which header to use
    int headerType = bis.readByte();

    /*** HEADER IF LESS THAN 192 UNIQUE SYMBOLS ***/
    if (headerType) {
        distinctVals = headerType;
        int index;
        for (int i = 0; i < distinctVals; ++i) {
            index = bis.readByte();
            freqs[index] = read3Bytes(bis);
            outBytes += freqs[index];
        }
    }
    /*** HEADER IF GTE TO 192 UNIQUE SYMBOLS ***/
    else {
        for (int i = 0; i < freqs.size(); ++i) {
            freqs[i] = read3Bytes(bis);
            if (freqs[i]) {
                distinctVals++;
                outBytes += freqs[i];
            }
        }
    }

    //empty file case
    if (!in.good()) distinctVals = outBytes = 0;

    //print freqs for debugging
    /*for (int i = 0; i < 256; ++i) 
        if (freqs[i]) printf("%d: %d\n", i, freqs[i]);*/

    cout << " done." << endl;
    printf("Uncompressed file will have %d" 
        " unique symbols and size %d bytes.\n", distinctVals, outBytes);
    
    cout << "Building Huffman code tree...";
    
    //construct Huffman tree
    tree.build(freqs);
    
    cout << " done." << endl;
    cout << "Writing to file \"" << argv[2] << "\"...";

    //read bits and write symbol to output file
    BitOutputStream bos (out);
    int symbol;

    while (in.good() && outBytes > 0) {
        symbol = tree.decode(bis); 
        if (symbol == -1) break;
        bos.writeByte(symbol);
        outBytes--;
    }

    cout << " done." << endl;

    in.close();
    out.close();

    return 0;
}
