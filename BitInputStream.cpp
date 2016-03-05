#include "BitInputStream.hpp"

/**
 * BitInputStream for Huffman coding.
 */

int BitInputStream::readBit() {
    if (buf_index == 0) {
        buf = in.get();
        buf_index = 8;
    }

    buf_index--;
    return bitVal(buf, buf_index + 1);
}

int BitInputStream::readByte() {
    return in.get();
}

int BitInputStream::readInt() {
    char c[4];
    in.read(c, 4);
    return *((int*)c);
}

char BitInputStream::bitVal(char c, int n) {
    return c >> (n - 1) & 1;
}
