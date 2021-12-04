#include<fstream>
#include "HuffmanCoder.h"


const void HuffmanCoder::Compress(const std::string& inputFilePath, const std::string& outputFilePath)
{
    std::filebuf fb;
    if(!fb.open(inputFilePath, std::ios::in)) throw std::runtime_error("The file to compress was not foud!");
    std::istream inputStream(&fb);
    Preprocess(inputStream);

    std::vector<char*> headerChars;
    std::vector<bool> headerLeafNodes;
    TraversePreorder(headerChars, headerLeafNodes, root);

    std::ofstream outputStream(outputFilePath, std::ofstream::binary);

    outputStream.write(&SOH, 1); // Write SOH
    for (const auto &ch: headerChars) // Write header
        outputStream.write(ch, 1);
    outputStream.write(&GS, 1); // Write GS - separates characters in preorder traversal from leaf nodes
    WriteLeafNodes(outputStream, headerLeafNodes);

    outputStream.write(&STX, 1); // Write STX - separates header from encoded text itself
    WriteEncoded(outputStream, inputStream);
    outputStream.close();
}

const void HuffmanCoder::Decompress(const std::string &inputFilePath, const std::string &outputFilePath)
{
    std::ifstream inputStream(inputFilePath, std::ifstream::binary);
    if (inputStream.get() != SOH) throw std::runtime_error("The file to decompress is invalid!");

    std::vector<char> preorderTraversal;
    std::vector<bool> leafNodes;
    std::string buffer;
    char c;

    while (inputStream)
    {
        c = inputStream.get();
        if (c == GS) break;
        preorderTraversal.push_back(c);
    }

    size_t ln = preorderTraversal.size();
    size_t n = 0;
    while (inputStream)
    {
        c = inputStream.get();
        buffer = Byte(c).to_string();
        for (const auto& i: buffer)
        {
            leafNodes.push_back(i - '0');
            n++;
            if (n == ln) goto headerEnded;
        }
    }
    buffer.empty();

    headerEnded:
    if (inputStream.get() != STX) throw std::runtime_error("File is corrupted");

    root = *MakeTreeFromPreorder(preorderTraversal, leafNodes);

    std::ofstream outputStream(outputFilePath);
    if (outputStream.is_open())
        DecodeChars(inputStream, outputStream);

    inputStream.close();
    outputStream.close();
}

const void HuffmanCoder::CreateNodes(std::istream &txtStream)
{
    
    bool exists = false;
    std::map<char, size_t> frequencies;

    while (txtStream)
        frequencies[char(txtStream.get())]++;
        
    for (const auto& i: frequencies)
        pq.emplace(H(i.second, i.first));
    
}

const void HuffmanCoder::CreateTree()
{
    Node *left, *right, *parent;
    size_t sum;
    while (pq.size() > 1)
    {
        left = new Node(pq.top());
        pq.pop();
        right = new Node(pq.top());
        pq.pop();

        sum = left->data.first + right->data.first;

        parent = new Node(H(sum, char(0)));
        parent->left = left;
        parent->right = right;
        pq.push(*parent);

        root = *parent;
    }

    pq.pop();
    //return parent;
}

const void HuffmanCoder::CodeChars(const Node &node, std::string prefix = "")
{
    if (node.data.second != char(0))
    {
        codedChars[node.data.second] = prefix;
    }
    else
    {
        prefix += "0";
        CodeChars(*node.left, prefix);
        
        prefix.pop_back();
        prefix += "1";
        CodeChars(*node.right, prefix);
    }
}

const void HuffmanCoder::DecodeChars(std::istream& inputStream, std::ofstream& outputStream)
{
    std::string buffer;
    bool bits;
    Node *current = &root;

    while (inputStream)
    {
        buffer = Byte(inputStream.get()).to_string();
        for (const auto& bit: buffer)
        {
            bool i = (bool)(bit - '0');
            if (i == 0) current = current->left;
            else        current = current->right;

            if (current->data.second != char(0))
            {
                if (current->data.second == char(-1)) goto PSEUDO_EOF;
                outputStream << current->data.second;
                current = &root;
            }
        }
    }
    PSEUDO_EOF:
    buffer.empty();
}

const void HuffmanCoder::Preprocess(std::istream& streamToEncode)
{
    pq.empty();
    codedChars.empty();

    CreateNodes(streamToEncode);
    CreateTree();
    CodeChars(root);
}

const void HuffmanCoder::TraversePreorder(std::vector<char*>& headerChars, std::vector<bool>& headerLeafNodes, Node& root)
{
    headerChars.push_back(&root.data.second);
    if (root.data.second != char(0))
        headerLeafNodes.push_back(1);
    else
    {
        headerLeafNodes.push_back(0);
        TraversePreorder(headerChars, headerLeafNodes, *root.left);
        TraversePreorder(headerChars, headerLeafNodes, *root.right);
    }
}

const void HuffmanCoder::WriteLeafNodes(std::ofstream& outStream, const std::vector<bool>& leafNodes)
{
    std::string buffer = "";
    for (const auto& i: leafNodes)
    {
        buffer += char(i + '0');

        if (buffer.length() == 8)
            WriteBytes(outStream, buffer);
    }
    if (buffer.length() < 8)
    {
        buffer.resize(8, '0');
        WriteBytes(outStream, buffer);
    }
}

const void HuffmanCoder::WriteEncoded(std::ofstream& outStream, std::istream& inStream)
{
    std::string buffer = "";
    std::string code;

    inStream.clear();
    inStream.seekg(0, std::ios::beg);

    while (inStream)
    {
        char ch = inStream.get();
        code = codedChars[ch];
        for (const auto& i: code)
        {
            buffer += i;
            if (buffer.length() == 8)
                WriteBytes(outStream, buffer);
        }
    }
    if (buffer.length() < 8)
    {
        buffer.resize(8, '0');
        WriteBytes(outStream, buffer);
    }
}

const void HuffmanCoder::WriteBytes(std::ofstream& outStream, std::string& buffer)
{
    char c = (char)Byte(buffer).to_ulong();
    outStream.write(&c, 1);
    buffer = "";
}

Node* HuffmanCoder::MakeTreeFromPreorder(std::vector<char>& preorderTraversal, std::vector<bool>& leafNodes)
{
    Node *node = new Node(H(0, preorderTraversal.front()));
    preorderTraversal.erase(preorderTraversal.begin());

    bool leafNode = leafNodes.front();
    leafNodes.erase(leafNodes.begin());

    if (!leafNode)
    {
        node->left = MakeTreeFromPreorder(preorderTraversal, leafNodes);
        node->right = MakeTreeFromPreorder(preorderTraversal, leafNodes);
    }
    return node;
}
