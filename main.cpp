//
//  main.cpp
//  Huffman-Encoder
//
//  Created by Sanya Srivastava on 9/21/20.
//

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <list>
#include <bitset>
using namespace std;

class Node {
public:
    Node(int freq, Node* left = nullptr, Node* right = nullptr, char ch = ' '): left(left), right(right), ch(ch), freq(freq) {}
    int getFreq() { return freq; }
    Node* getLeft() { return left; }
    Node* getRight() { return right; }
    char getChar() { return ch; }
private:
    char ch;
    int freq;
    Node* left, *right;
};

void encode(Node* root, unordered_map<char, string>& encodings, string path = "") {
    if (!root->getLeft() and !root->getRight()) {
        cout << root->getChar() << " " << path << endl;
        encodings[root->getChar()] = path;
        return;
    }
        
    if (root->getRight()) {
        encode(root->getRight(), encodings, path + "1");
    }
    if (root->getLeft()) {
        encode(root->getLeft(), encodings, path + "0");
    }
}

bool compare(Node*& a, Node*& b) {
    return a->getFreq() <= b->getFreq();
}

int main(int argc, const char * argv[]) {
    
    // get file
    ifstream file;
    file.open("example.txt");
    if (!file.is_open()) {
        cout << "Unable to open file";
        return 1;
    }
    
    // map characters to their frequencies
    unordered_map<char, int> freqMap;
    
    char ch;
    while (file.get(ch)) {
        cout << ch;
        freqMap[ch]++;
    }
    file.close();
    
    // create nodes for each mapping and store them in a sorted list
    list<Node*> nodes;
    for (auto it = freqMap.begin(); it != freqMap.end(); it++) {
        cout<<endl<<it->first<<" "<<it->second;
        Node* ptr = new Node(it->second, nullptr, nullptr, it->first);
        nodes.push_back(ptr);
    }
    
    nodes.sort(compare);
    
    for (auto it = nodes.begin(); it != nodes.end(); it++) {
        cout<<(*it)->getFreq()<<endl;
    }
    
    // create a binary tree
    while (nodes.size() > 1) {
        Node* item1 = nodes.front();
        nodes.pop_front();
        Node* item2 = nodes.front();
        nodes.pop_front();
        Node* ptr = new Node(item1->getFreq() + item2->getFreq(), item1, item2);
        nodes.push_back(ptr);
        nodes.sort(compare);
    }
    
    cout<<"size: "<<nodes.size()<<endl;
    cout<<"root freq: "<<(*(nodes).begin())->getFreq()<<endl;
    
    // determine the new bit-encoding for each character
    unordered_map<char, string> encodings;
    
    auto ptr = nodes.begin();
    encode(*ptr, encodings);
    
    cout<<"Encodings:\n";
    for (auto it = encodings.begin(); it != encodings.end(); it++) {
        cout << it->first << " " << it->second << endl;
    }
    
    // parse the input file and replace every character with its encoding
    ofstream compressed("compressed.dat");
    fstream temp("temp.txt");
    file.open("example.txt");
    
    while (file.get(ch)) {
        temp << encodings[ch];
    }
    
    int bitCount = 0;
    string packet = "";
    while (temp.get(ch)) {
        bitCount++;
        if (bitCount == 8) {
            cout<<packet<<endl;
            bitCount = 0;
            bitset<8> byte (packet);
            compressed << byte;
            packet = "";
        }
        packet += ch;
    }
    cout << packet << endl;
    bitset<8> byte (packet);
    compressed << byte;
    
    compressed.close();
    temp.close();
    file.close();
    
    // add encoding info to binary file as well
}
