#include<vector>
#include<string>
#include<queue>
#include<map>
#include<bitset>

typedef std::bitset<8> Byte;
typedef std::pair<size_t, char> H;

struct Node
{
    H data;
    Node *left = nullptr;
    Node *right = nullptr;
    
    Node (const H& data_ = H(0, 0)) : data(data_) {}
    
    friend bool operator<(const Node& a, const Node& b)
    {
        if (a.data.first == b.data.first & a.data.second == b.data.second) return a.right < b.right;
        else if (a.data.first == b.data.first) return a.data.second > b.data.second;
        else return a.data.first > b.data.first;
    }
};

class HuffmanCoder
{
private:

    const char SOH = char(1); // SOH = start of header
    const char STX = char(2); // STX = start of text
    
    std::priority_queue<Node> pq;
    Node root;
    std::map<char, std::string> codedChars;

    const void CreateNodes(std::istream &txtStream);
    const void CreateTree();
    const void CodeChars(const Node &node, std::string prefix);
    const void DecodeChars(const std::vector<bool> &outputVec, std::ofstream &outputStream);

    const void Preprocess(std::istream& streamToEncode);
    const void TraversePreorder(std::vector<char*>& headerChars, std::vector<bool>& headerLeafNodes, Node& root);
    const void WriteLeafNodes(std::ofstream& outStream, const std::vector<bool>& leafNodes);
    const void WriteEncoded(std::ofstream& outStream, std::istream& inStream);
    const void WriteBytes(std::ofstream& outStream, std::string& buffer);
    const void WriteBuffer();

public:
    const void Decode(const std::vector<H> &frequencyTable, const std::vector<bool> &outputVec, std::ofstream &outputStream);

    const void Compress(const std::string &inputFilePath, const std::string &outputFilePath);
    const void Decompress(const std::string &inputFilePath, const std::string &outputFilePath);
};