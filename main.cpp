#include<iostream>
#include<string>
#include<queue>
#include"Parser.cpp"

using namespace std;

int main()
{
    Parser p;
    
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    p.Compress("test.txt", "compressed.hf");
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    cout << "Compress: " << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) /1000000.0  << "\n";

    begin = std::chrono::steady_clock::now();
    p.Decompress("compressed.hf", "decompressed.txt");
    end = std::chrono::steady_clock::now();
    cout << "Decompress: " << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) /1000000.0  << "\n";
}