#include "Parser.h"
#include<fstream>
#include<iostream>
#include<chrono>

Parser::Parser()
{
    hc = new HuffmanCoder;
}

const void Parser::Compress(const std::string &inputFilePath, const std::string &outputFilePath)
{
    //hc = new HuffmanCoder;
    std::vector<bool> encodedVec;

    std::filebuf fb;
    if(fb.open(inputFilePath, std::ios::in))
    {
        std::istream inputStream(&fb);
        encodedVec = hc->Encode(inputStream);
        fb.close();
    }

    MakeFrequencyBytes(hc->Get_Root());    // Header
    MakeBytes(encodedVec);                 // Compressed text (PSEUDO_EOF incorporated in the file itself)

    std::ofstream outputStream(outputFilePath, std::ofstream::binary);

    outputStream.write(&SOH, 1); // Write SOH

    // Write header
    for (const auto &fq: frequencyTable)
    {
        outputStream.write(&fq.second, 1);                      // Writes  character
        outputStream.write((char*)&fq.first, sizeof(fq.first)); // Writes the number of occurences of the character
    }
    outputStream.write(&STX, 1); // Write STX


    std::cout << bytesToWrite.size();
    // Write encoded text
    for (const auto &i: bytesToWrite)
    {
        char charToWrite = (char)i.to_ulong();
        outputStream.write(&charToWrite, 1);
    }
    outputStream.close();

}

const void Parser::Decompress(const std::string &inputFilePath, const std::string &outputFilePath)
{
    std::ifstream inputStream(inputFilePath, std::ifstream::binary);
    if (inputStream.get() != SOH) throw std::runtime_error("Invalid file!");

    std::vector<H> frequencies;
    char c;
    size_t fq;

    while (inputStream)
    {
        c = inputStream.get();
        if (c == STX) break;
        inputStream.read((char*)&fq, sizeof(fq));
        frequencies.push_back(H(fq, c));
    }

    while (inputStream)
        CharToBoolVec(inputStream.get());

    std::ofstream outputStream(outputFilePath);
    if (outputStream.is_open())
        hc->Decode(frequencies, readBits, outputStream);
    outputStream.close();
}

const void Parser::MakeFrequencyBytes(const Node &node)
{
    if (node.data.second != char(0))
    {
        frequencyTable.push_back(node.data);
    }
    else
    {
        MakeFrequencyBytes(*node.left);
        MakeFrequencyBytes(*node.right);
    }
}

const void Parser::MakeBytes(std::vector<bool> encodedVec)
{
    std::string buffer;
    size_t rem = encodedVec.size() % 8;

    for (const auto &i: encodedVec)
    {
        buffer += char(i + '0');
        if (buffer.length() == 8)
        {
            bytesToWrite.push_back(Byte(buffer));
            buffer = "";
        }
    }

    if (rem > 0)
    {
        buffer.resize(8, '0');
        bytesToWrite.push_back(Byte(buffer));
    }
}

const void Parser::CharToBoolVec(const char &ch)
{
    std::vector<bool> temp;
    std::string bits = Byte(ch).to_string();

    for (const auto &i: bits)
        temp.push_back((bool)(i - '0'));

    readBits.insert(readBits.end(), temp.begin(), temp.end());
}

auto Parser::DetermineDataType(const size_t &num)
{
}