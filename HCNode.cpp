#include "HCNode.hpp"

/**
 * Implementation of the HCNode class.
 * Used for Huffman coding.
 */

/**
 * Less-than operator for HCNode.
 * Calls global function to compare node priorities.
 */
bool HCNode::operator<(const HCNode& other) {
    HCNode copy = other;
    return comp(this, &copy);
}

/**
 * Less-than comparison.
 * Smaller counts have higher priority.
 * Break ties using lexicographical order.
 */
bool comp(HCNode* one, HCNode* other) {
    if (one->count < other->count) return false;
    else if (one->count == other->count) 
        if (one->symbol < other->symbol) return false;
    return true;
}
