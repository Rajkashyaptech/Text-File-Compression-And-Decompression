#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <queue>
#include <bitset>

using namespace std;

// Huffman Tree Node Structure
struct Node {
    char ch;
    int freq;
    Node *left, *right;
    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

// Comparator for Min-Heap (Priority Queue)
struct compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

// Function to generate Huffman Codes
void generateCodes(Node* root, string code, map<char, string>& huffmanCode) {
    if (!root) return;

    // If it's a leaf node, store the code
    if (!root->left && !root->right) {
        huffmanCode[root->ch] = code;
    }

    generateCodes(root->left, code + "0", huffmanCode);
    generateCodes(root->right, code + "1", huffmanCode);
}

// Build Huffman Tree from frequency map
Node* buildHuffmanTree(map<char, int>& freqMap) {
    priority_queue<Node*, vector<Node*>, compare> minHeap;

    // Create leaf nodes for each character and add to heap
    for (auto pair : freqMap) {
        minHeap.push(new Node(pair.first, pair.second));
    }

    // Build tree
    while (minHeap.size() > 1) {
        Node *left = minHeap.top(); minHeap.pop();
        Node *right = minHeap.top(); minHeap.pop();

        Node *internalNode = new Node('\0', left->freq + right->freq);
        internalNode->left = left;
        internalNode->right = right;
        minHeap.push(internalNode);
    }

    return minHeap.top(); // Root of the Huffman Tree
}

// Function to encode the input text using the Huffman Codes
string encode(string text, map<char, string>& huffmanCode) {
    string encodedString = "";
    for (char ch : text) {
        encodedString += huffmanCode[ch];
    }
    return encodedString;
}

// Function to decode the encoded binary string using the Huffman Tree
string decode(Node* root, string encodedStr) {
    string decodedString = "";
    Node* currentNode = root;
    for (char bit : encodedStr) {
        if (bit == '0') currentNode = currentNode->left;
        else currentNode = currentNode->right;

        if (!currentNode->left && !currentNode->right) {
            decodedString += currentNode->ch;
            currentNode = root;
        }
    }
    return decodedString;
}

// Helper function to save encoded text as a binary file
void saveBinaryFile(const string& encodedText, const string& filename) {
    ofstream outFile(filename, ios::binary);
    int extraBits = 8 - (encodedText.size() % 8); // Pad to complete the byte
    string paddedText = encodedText + string(extraBits, '0');

    outFile << bitset<8>(extraBits); // Store the number of padded bits
    for (size_t i = 0; i < paddedText.size(); i += 8) {
        bitset<8> byte(paddedText.substr(i, 8));
        outFile.put(static_cast<unsigned char>(byte.to_ulong()));
    }
    outFile.close();
}

// Helper function to read binary file and decode it
string readBinaryFile(const string& filename, Node* root) {
    ifstream inFile(filename, ios::binary);
    char extraBitsChar;
    inFile.get(extraBitsChar);
    int extraBits = bitset<8>(extraBitsChar).to_ulong();

    string encodedBinary = "";
    char byte;
    while (inFile.get(byte)) {
        encodedBinary += bitset<8>(byte).to_string();
    }
    encodedBinary = encodedBinary.substr(0, encodedBinary.size() - extraBits);
    inFile.close();

    return decode(root, encodedBinary);
}

int main() {
    string inputText;
    ifstream inputFile("input.txt");

    if (inputFile.is_open()) {
        getline(inputFile, inputText, '\0'); // Read the whole file
        inputFile.close();
    } else {
        cout << "Could not open input file!" << endl;
        return 1;
    }

    // Step 1: Calculate frequency of each character
    map<char, int> freqMap;
    for (char ch : inputText) {
        freqMap[ch]++;
    }

    // Step 2: Build Huffman Tree
    Node* root = buildHuffmanTree(freqMap);

    // Step 3: Generate Huffman Codes
    map<char, string> huffmanCode;
    generateCodes(root, "", huffmanCode);

    // Step 4: Encode the input file
    string encodedText = encode(inputText, huffmanCode);
    saveBinaryFile(encodedText, "compressed.bin");

    cout << "File compressed successfully!" << endl;

    // Step 5: Decompress the binary file
    string decodedText = readBinaryFile("compressed.bin", root);

    ofstream outputFile("output.txt");
    outputFile << decodedText;
    outputFile.close();

    cout << "File decompressed successfully!" << endl;

    return 0;
}
