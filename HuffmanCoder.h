#include "Node_BT.h"
#include<vector>
#include<string>
#include<queue>
#include<map>

#define H std::pair<size_t, char>

struct Node
{
    H data;
    Node *left = nullptr;
    Node *right = nullptr;

    // This overload makes sure, that the nodes in priority queue will always be ordered in the same way
    friend bool operator<(const Node &a, const Node &b)
    {
        if (a.data.first == b.data.first & a.data.second == b.data.second) return a.right < b.right;
        else if (a.data.first == b.data.first) return a.data.second > b.data.second;
        else return a.data.first > b.data.first;
    }
    Node(const H &data) { this->data = data; }
    Node() {};
};

class HuffmanCoder
{
private:
    std::priority_queue<Node> pq;
    Node root;
    std::map<char, std::vector<bool>> codedChars;

    const void CreateNodes(std::istream &txtStream);
    const void CreateNodes(const std::vector<H> &frequencyTable);
    const void CreateTree();
    const void CodeChars(const Node &node, std::vector<bool> prefix);
    const void DecodeChars(const std::vector<bool> &outputVec, std::ofstream &outputStream);
    std::vector<bool> MakeEncodedVec(std::istream &str);
    const void DeleteTree(const Node &node);

public:
    std::vector<bool> Encode(std::istream &streamToEncode);
    std::string Decode(const std::map<char, std::vector<bool>> &codedChars, const std::string &encodedStr);
    const void Decode(const std::vector<H> &frequencyTable, const std::vector<bool> &outputVec, std::ofstream &outputStream);

    std::map<char, std::vector<bool>> Get_CodedChars() { return codedChars; }
    const Node &Get_Root() { return root; }
};