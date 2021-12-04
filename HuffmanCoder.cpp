#include<algorithm>
#include<iostream>
#include<fstream>
#include "HuffmanCoder.h"

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

const void HuffmanCoder::DecodeChars(const std::vector<bool> &outputVec, std::ofstream &outputStream)
{
    Node *current = &root;
    for (const auto &i: outputVec)
    {
        if (i == 0) current = current->left;
        else        current = current->right;

        if (current->data.second != char(0))
        {
            if (current->data.second == char(-1)) break;
            outputStream << current->data.second;
            current = &root;
        }
    }
}

const void HuffmanCoder::Decode(const std::vector<H> &frequencyTable, const std::vector<bool> &outputVec, std::ofstream &outputStream)
{
    pq.empty();
    codedChars.empty();
    //root = Node();

    CreateTree();
    CodeChars(root);
    DecodeChars(outputVec, outputStream);
}


const void HuffmanCoder::Compress(const std::string& inputFilePath, const std::string& outputFilePath)
{
    std::filebuf fb;
    fb.open(inputFilePath, std::ios::in);
    std::istream inputStream(&fb);
    Preprocess(inputStream);

    std::vector<char*> headerChars;
    std::vector<bool> headerLeafNodes;
    TraversePreorder(headerChars, headerLeafNodes, root);

    std::ofstream outputStream(outputFilePath, std::ofstream::binary);

    outputStream.write(&SOH, 1); // Write SOH

    // Write header
    for (const auto &ch: headerChars)
        outputStream.write(ch, 1);

    WriteLeafNodes(outputStream, headerLeafNodes);

    outputStream.write(&STX, 1); // Write STX

    WriteEncoded(outputStream, inputStream);

    outputStream.close();
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
    {
        headerLeafNodes.push_back(1);
    }
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
    size_t ln;
    std::string code;

    inStream.clear();
    inStream.seekg(0, std::ios::beg);

    while (inStream)
    {
        char ch = inStream.get();
        code = codedChars[ch];
        ln = code.length();
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