#include<vector>
#include<string>
#include<queue>
#include<map>
#include<bitset>

#define Byte std::bitset<8>
#define H std::pair<size_t, char>

struct Node
{
    H data;
    Node *left;
    Node *right;
    
    Node (const H& data_ = H(0, 0)) : data(data_), left(nullptr), right(nullptr) {}
    
    friend bool operator<(const Node& a, const Node& b) { return a.data.first > b.data.first; }
};

class HuffmanCoder
{
private:

    const char SOH = char(1); // SOH = start of header
    const char STX = char(2); // STX = start of text
    const char GS = char(29); // GS = group separator
    
    std::priority_queue<Node> pq;
    Node root;
    std::map<char, std::string> codedChars;

    const void CreateNodes(std::istream &txtStream);
    const void CreateTree();
    const void CodeChars(const Node &node, std::string prefix);
    const void DecodeChars(std::istream& inputStream, std::ofstream& outputStream);

    const void Preprocess(std::istream& streamToEncode);
    const void TraversePreorder(std::vector<char*>& headerChars, std::vector<bool>& headerLeafNodes, Node& root);
    const void WriteLeafNodes(std::ofstream& outStream, const std::vector<bool>& leafNodes);
    const void WriteEncoded(std::ofstream& outStream, std::istream& inStream);
    const void WriteBytes(std::ofstream& outStream, std::string& buffer);

    Node* MakeTreeFromPreorder(std::vector<char>& preorderTraversal, std::vector<bool>& leafNodes);

public:
    const void Compress(const std::string &inputFilePath, const std::string &outputFilePath);
    const void Decompress(const std::string &inputFilePath, const std::string &outputFilePath);
};