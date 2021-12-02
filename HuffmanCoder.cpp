#include<algorithm>
#include<iostream>
#include<fstream>
#include "HuffmanCoder.h"

const void HuffmanCoder::CreateNodes(std::istream &txtStream)
{
    bool exists = false;
    std::vector<Node> temp;

    while (txtStream)
    {
        char ch = char(txtStream.get());
        for (auto &x: temp)
        {
            if (ch == x.data.second)
            {
                x.data.first++;
                exists = true;
                break;
            }
        }

        if (!exists) temp.emplace_back(H(1, ch));

        exists = false;
    }

    pq = std::priority_queue<Node>(temp.begin(), temp.end());
}

const void HuffmanCoder::CreateNodes(const std::vector<H> &frequencyTable)
{
    std::vector<Node> temp;
    for (const auto &i: frequencyTable)
        temp.push_back(Node(i));

    pq = std::priority_queue<Node>(temp.begin(), temp.end());
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

const void HuffmanCoder::CodeChars(const Node &node, std::vector<bool> prefix = std::vector<bool>(0))
{
    if (node.data.second != char(0))
    {
        codedChars[node.data.second] = prefix;
    }
    else
    {
        prefix.push_back(0);
        CodeChars(*node.left, prefix);
        
        prefix.back() = 1;
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

std::vector<bool> HuffmanCoder::MakeEncodedVec(std::istream &str)
{
    std::vector<bool> retStr;
    str.clear();
    str.seekg(0, std::ios::beg);

    while (str)
    {
        char ch = char(str.get());
        std::vector<bool> temp = codedChars[ch];
        retStr.insert(retStr.end(), temp.begin(), temp.end());
    }
    return retStr;
}

std::vector<bool> HuffmanCoder::Encode(std::istream &streamToEncode)
{
    pq.empty();
    codedChars.empty();
    root = Node();

    CreateNodes(streamToEncode);
    CreateTree();
    CodeChars(root);

    return MakeEncodedVec(streamToEncode);
}

const void HuffmanCoder::Decode(const std::vector<H> &frequencyTable, const std::vector<bool> &outputVec, std::ofstream &outputStream)
{
    pq.empty();
    codedChars.empty();
    root = Node();

    CreateNodes(frequencyTable);
    CreateTree();
    CodeChars(root);
    DecodeChars(outputVec, outputStream);
}
