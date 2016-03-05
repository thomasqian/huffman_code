#include "BitOutputStream.hpp"

/**
 * BitOutputStream for Huffman code.
 */

void BitOutputStream::writeBit(int bit) {
    if (buf_index == 0) {
        out.put(buf);
        out.flush();
        buf = 0;
        buf_index = 8;
    }

    buf_index--;
    buf = setBit(buf, bit, buf_index + 1);
}

void BitOutputStream::writeByte(int i) {
    out.put(i);
}

void BitOutputStream::writeInt(int i) {
    out.write((char*)&i, 4);
}

void BitOutputStream::flush() {
    out.put(buf);
    out.flush();
    buf = buf_index = 0;
}

char BitOutputStream::setBit(char c, int bit, int n) {
    char ch;
    if (bit == 1) ch = c | (1 << n - 1);
    else ch = c & ~(1 << n - 1);
    return ch;
}
