#include "HCTree.hpp"

/**
 * Implementation of the HCTree class.
 * Used for Huffman coding.
 */

HCTree::~HCTree() {
    deleteAll(root);
}

void HCTree::build(const vector<int>& freqs) {
    //create nodes and update leaves vector
    for (int i = 0; i < freqs.size(); ++i) {
        if (freqs[i]) {
            leaves[i] = new HCNode(freqs[i], (byte)i, 0, 0, 0);
            pq.push(leaves[i]);
        }
    }

    //generate trie from forest(pq)
    HCNode *left, *right, *newNode;
    while (pq.size() > 1) {
        left = pq.top();
        pq.pop();
        right = pq.top();
        pq.pop();
        newNode = new HCNode(left->count + right->count, 0, left, right, 0);
        left->p = newNode;
        right->p = newNode;
        pq.push(newNode);
    }

    //remaining node is the root of the tree
    if (!pq.empty()) {
        root = pq.top();
        pq.pop();
    }
    if (!pq.empty()) {
        cerr << "Error building trie." << endl;
        exit(EXIT_FAILURE);
    }

    //flag that build has been called
    isBuilt = 1;
}

int HCTree::encode(byte symbol, BitOutputStream& out) {
    if (!isBuilt) {
        cerr << "HCTree Error: build must be called before encode." << endl;
        exit(EXIT_FAILURE);
    }
    
    //retrieve pointer to node with symbol to encode
    HCNode *curr = leaves[(int)symbol];

    //bit count for file output statistics
    bitCount = 0;

    //recusively traverse upwards to write to output properly
    encodeHelper(curr, out);

    return bitCount;
}

int HCTree::decode(BitInputStream& in) const {
    if (!isBuilt) {
        cerr << "HCTree Error: build must be called before decode." << endl;
        exit(EXIT_FAILURE);
    }

    if (!root) return -1; 

    //start from the root and traverse until we get to a leaf
    HCNode *curr = root;
    while (curr->c0) {
        if (in.readBit() == 0) curr = curr->c0;
        else curr = curr->c1;
    }

    return curr->symbol;
}

void HCTree::deleteAll(HCNode *root) {
    if (root == 0) return;
    deleteAll(root->c0);
    deleteAll(root->c1);
    delete(root);
    root = 0;
}

void HCTree::encodeHelper(HCNode *curr, BitOutputStream& out) {
    if (curr->p == 0) return;
    encodeHelper(curr->p, out);
    if (curr->p->c0 == curr) out.writeBit(0);
    else out.writeBit(1);
    bitCount++;
}
