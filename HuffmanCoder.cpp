#include<fstream>
#include "HuffmanCoder.h"

/**
 * @brief Compresses inputFilePath to outputFilePath
 * 
 * @param inputFilePath File to compress
 * @param outputFilePath Compressed file
 */
const void HuffmanCoder::Compress(const std::string& inputFilePath, const std::string& outputFilePath)
{
    // Clear private variables
    preorderTraversal.clear();
    leafNodes.clear();

    // Define input and output streams
    std::filebuf fb;
    if(!fb.open(inputFilePath, std::ios::in)) throw std::runtime_error("The file to compress was not foud!");
    std::istream inputStream(&fb);
    std::ofstream outputStream(outputFilePath, std::ofstream::binary);

    // Preprocess file
    PreprocessCompress(inputStream);

    // Create header
    outputStream.write(&SOH, 1); // Write SOH
    for (const auto &ch: preorderTraversal) // Write charactesr in preorder traversal
        outputStream.write(&ch, 1);
    outputStream.write(&GS, 1); // Write GS - separates characters in preorder traversal from leaf nodes
    WriteLeafNodes(outputStream, leafNodes);

    outputStream.write(&STX, 1); // Write STX - separates header from encoded text itself
    WriteEncoded(outputStream, inputStream);
    outputStream.close();
}

/**
 * @brief Decompresses inputFilePath to outputFilePath
 * 
 * @param inputFilePath File to decompress
 * @param outputFilePath Decompressed file
 */
const void HuffmanCoder::Decompress(const std::string &inputFilePath, const std::string &outputFilePath)
{
    // Clear private variables
    preorderTraversal.clear();
    leafNodes.clear();

    // Define input and output streams
    std::ifstream inputStream(inputFilePath, std::ifstream::binary);
    std::ofstream outputStream(outputFilePath);
    if (!inputStream.is_open()) throw std::runtime_error("The compressed file was not foud!");
    if (inputStream.get() != SOH) throw std::runtime_error("The file to decompress is invalid!");

    // Preprocess file
    PreprocessDeompress(inputStream);

    if (outputStream.is_open())
        DecodeChars(inputStream, outputStream);

    inputStream.close();
    outputStream.close();
}

/**
 * @brief Preprocesses file to compress - creates huffman tree, makes coding for each character and makes header vectors
 * 
 * @param streamToEncode File stream to compress
 */
const void HuffmanCoder::PreprocessCompress(std::istream& streamToEncode)
{
    CreateNodes(streamToEncode);
    MakeTree();
    CodeChars(root);
    TraversePreorder(preorderTraversal, leafNodes, root);
}

/**
 * @brief Preprocesses compressed file - creates tree from header
 * 
 * @param inpStream File stream to decompress
 */
const void HuffmanCoder::PreprocessDeompress(std::istream& inpStream)
{
    ReadHeader(inpStream);
    root = *MakeTreeFromPreorder(preorderTraversal, leafNodes);
    if (inpStream.get() != STX) throw std::runtime_error("File is corrupted");
}

/**
 * @brief Reads header and write characters in preorder traversal and leaf nodes to private vectors
 * 
 * @param inpStream File stream to decompress
 */
const void HuffmanCoder::ReadHeader(std::istream& inpStream)
{
    std::string buffer;
    char c;

    while (inpStream)
    {
        c = inpStream.get();
        if (c == GS) break;
        preorderTraversal.push_back(c);
    }

    size_t ln = preorderTraversal.size();
    size_t n = 0;
    while (inpStream)
    {
        c = inpStream.get();
        buffer = Byte(c).to_string();
        for (const auto& i: buffer)
        {
            leafNodes.push_back(i - '0');
            n++;
            if (n == ln) return;
        }
    }

}

/**
 * @brief Creates nodes to make the huffman tree from and insert them into priority queue
 * 
 * @param inpStream File stream to compress
 */
const void HuffmanCoder::CreateNodes(std::istream &inpStream)
{
    
    bool exists = false;
    std::map<char, size_t> frequencies;

    while (inpStream)
        frequencies[char(inpStream.get())]++;
        
    for (const auto& i: frequencies)
        pq.emplace(H(i.second, i.first));
    
}

/**
 * @brief Makes huffman binary tree
 * 
 */
const void HuffmanCoder::MakeTree()
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

        parent = new Node(H(sum, NUL));
        parent->left = left;
        parent->right = right;
        pq.push(*parent);

        root = *parent;
    }
    pq.pop();
}

/**
 * @brief Makes binary codes for each character from the huffman tree
 * 
 * @param node 
 * @param prefix 
 */
const void HuffmanCoder::CodeChars(const Node &node, std::string prefix)
{
    if (node.data.second != NUL)
        codedChars[node.data.second] = prefix;
    else
    {
        prefix += "0";
        CodeChars(*node.left, prefix);
        
        prefix.pop_back();
        prefix += "1";
        CodeChars(*node.right, prefix);
    }
}

/**
 * @brief Decodes characters from inputStream and writes them to outputStream
 * 
 * @param inputStream Compressed file stream
 * @param outputStream File stream in which the decompressed text will be written to
 */
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

            if (current->data.second != NUL)
            {
                if (current->data.second == PSEUDO_EOF) return;
                outputStream << current->data.second;
                current = &root;
            }
        }
    }
}

/**
 * @brief Makes vectors that represent characters from the huffman tree in preorder traversal and if a given node is a leaf node or not
 * 
 * @param headerChars Vector in which the characters will be written to
 * @param headerLeafNodes Vector in which the status if a given node is a leaf node or not will be written to
 * @param root Root of the huffman binary tree
 */
const void HuffmanCoder::TraversePreorder(std::vector<char>& headerChars, std::vector<bool>& headerLeafNodes, Node& root)
{
    headerChars.push_back(root.data.second);
    if (root.data.second != NUL)
        headerLeafNodes.push_back(1);
    else
    {
        headerLeafNodes.push_back(0);
        TraversePreorder(headerChars, headerLeafNodes, *root.left);
        TraversePreorder(headerChars, headerLeafNodes, *root.right);
    }
}

/**
 * @brief Write the vector which represents if a given node is a leaf node to the compressed file
 * 
 * @param outStream File stream in which the compressed text will be written to
 * @param leafNodes Vector that contains the status if a given node is a leaf node or not
 */
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

/**
 * @brief Writes bits of the coded chars into the compressed file
 * 
 * @param outStream File stream in which the compressed text will be written to
 * @param inStream File to compress
 */
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

/**
 * @brief Writes buffer into the outStream file stream
 * 
 * @param outStream Output file stream
 * @param buffer Buffer containing 8 bits which will be written to outStream
 */
const void HuffmanCoder::WriteBytes(std::ofstream& outStream, std::string& buffer)
{
    char c = (char)Byte(buffer).to_ulong();
    outStream.write(&c, 1);
    buffer = "";
}

/**
 * @brief Creats huffman binary tree from characters in preorder traversal order and vector representing if a given node is a leaf node
 * 
 * @param preorderTraversal Characters in preorder traversal order
 * @param leafNodes Vector representing if a given node is a leaf node
 * @return Node* Parent node of two of its children
 */
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
