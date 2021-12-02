#include<bitset>
#include "HuffmanCoder.cpp"

const char SOH = char(1); // SOH = start of header
const char STX = char(2); // STX = start of text

typedef std::bitset<8> Byte;

class Parser
{
private:
    std::string stringToParse;
    HuffmanCoder *hc;
    std::vector<Byte> bytesToWrite;
    std::vector<std::pair<size_t, char>> frequencyTable;
    std::vector<bool> readBits;

    const void MakeBytes(std::vector<bool> encodedVec);
    const void MakeFrequencyBytes(const Node &node);
    auto DetermineDataType(const size_t &num);
    const void CharToBoolVec(const char &ch);

public:
    Parser();
    const void Compress(const std::string &inputFilePath, const std::string &outputFilePath);
    const void Decompress(const std::string &inputFilePath, const std::string &outputFilePath);
    const Node &Get_Root() { return hc->Get_Root(); }
};
