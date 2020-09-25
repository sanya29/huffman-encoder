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

void decode(Node* root, char ch, string path) {
    if (!path.length()) {
        return;
    }
    if (path[0] == '1') {
        if (root->getRight()) {
            decode(root->getRight(), ch, path.substr(1));
        } else {
            root->right = new Node(0);
            if (path.length() == 1) {
                root->ch = ch;
                cout<<"\nsetting "<<ch;
                return;
            } else {
                decode(root->getRight(), ch, path.substr(1));
            }
        }
    } else if (path[0] == '0') {
        if (root->getLeft()) {
            decode(root->getLeft(), ch, path.substr(1));
        } else {
            root->left = new Node(0);
            if (path.length() == 1) {
                root->ch = ch;
                cout<<"\nsetting "<<ch;
                return;
            } else {
                decode(root->getLeft(), ch, path.substr(1));
            }
        }
    } else {
        cout<<"\nUMM\n";
        exit(2020);
    }
}

void preorder(Node* root) {
    if (!root)
        return;
    cout << root->ch;
    preorder(root->left);
    preorder(root->right);
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
    preorder(*ptr);
    
    cout<<"Encodings:\n";
    
    // add encoding info to binary file
    ofstream compressed("compressed.dat"); // ios::binary
    ofstream temp("temp.txt");
    file.open("example.txt");
    
    int numEncodings = 1;
    for (auto it = encodings.begin(); it != encodings.end(); it++, numEncodings++) {
        compressed << it->first << " " << it->second << endl;
//        compressed.write((char*)& (it->first), 1);
//        compressed.write((char*)& (it->second), sizeof(it->second));
    }
    
    // parse the input file and replace every character with its encoding
    while (file.get(ch)) {
        temp << encodings[ch];
    }
    
    temp.close();
    compressed.close();
    ifstream temp2("temp.txt");
    ofstream compressed2("compressed.dat", ios::app);
    temp2.seekg(1,ios::beg);
    
    int bitCount = 0;
    string packet = "";
    while (temp2.get(ch)) {
        bitCount++;
        cout<<ch;
        if (bitCount == 8) {
//            cout<<packet<<endl;
            bitCount = 0;
            bitset<8> byte (packet);
            char grain;
            grain = (char) byte.to_ulong();
            compressed2 << grain;
//            compressed.write((char*)& grain, 1);
            packet = "";
        }
        packet += ch;
    }
    cout << packet << endl;
    bitset<8> byte (packet);
    char grain;
    grain = (char) byte.to_ulong();
    compressed2 << grain;
    compressed2.close();
//    compressed.write((char*)& grain, sizeof(char));
    
//    DECODE
    
    ifstream decoder("compressed.dat");
    
    unordered_map<char, string> decodings;
    
    cout << endl << numEncodings;
    int getEncodes = 1;
    string line;
    while (getEncodes < numEncodings) {
        getline(decoder, line);
        if (line.length() == 0) {
            cout << "hi\n";
            getline(decoder, line);
            cout << "char: \\n rest: " << line.substr(1) << endl;
            getEncodes++;
            decodings['\n'] = line.substr(1);
        } else {
            cout << "line: "<<line << " char: "<<line[0]<<" rest: "<<line.substr(2)<<endl;
            decodings[line[0]] = line.substr(2);
        }
        getEncodes++;
    }
    
    cout << "Decoding: \n";
    for (auto it = decodings.begin(); it != decodings.end(); it++) {
        cout << it->first << " " << it->second << endl;
    }
    
    // recreate a huffman tree
    
    Node* root = new Node(0);
    for (auto it = decodings.begin(); it != decodings.end(); it++) {
        decode(root, it->first, it->second);
    }
    cout<<"\npreorder traversal\n";
    preorder(root);
    
    // cleaning up
    decoder.close();
    temp.close();
    file.close();

}
