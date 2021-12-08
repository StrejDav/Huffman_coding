#include<iostream>
#include<string>
#include<chrono>
#include<sstream>
#include"HuffmanCoder.cpp"

using namespace std;

int main()
{
    // Definition of variables used
    HuffmanCoder *hf = new HuffmanCoder;
    string inserted;
    string arguments[3];
    stringstream ssin;
    int i;
    chrono::steady_clock::time_point begin;
    chrono::steady_clock::time_point end;

    cout << "Please write your intention in the following manner: compress/decompress input_file output_file\n"
         << "Example: compress test.txt compressed.hf\n"
         << "Type exit to end\n";
    while(true)
    {
        // Write inputed text into inserted
        getline(cin, inserted);
        if (inserted == "exit") break;
        
        // Converts string to array with spae as a separator
        i = 0;
        ssin = stringstream(inserted);
        while(ssin.good() && i < 4)
        {
            ssin >> arguments[i];
            i++;
        }

        // Processes the inputed text
        try
        {
            if (arguments[0] == "compress")
            {
                begin = chrono::steady_clock::now();
                hf->Compress(arguments[1], arguments[2]);
                end = chrono::steady_clock::now();
                cout << "Compression successful. It took " << (chrono::duration_cast<chrono::microseconds>(end - begin).count()) /1000000.0 << "s\n";
            }
            else if (arguments[0] == "decompress")
            {
                begin = chrono::steady_clock::now();
                hf->Decompress(arguments[1], arguments[2]);
                end = chrono::steady_clock::now();
                cout << "Decompression successful. It took " << (chrono::duration_cast<chrono::microseconds>(end - begin).count()) /1000000.0 << "s\n";
            }
            else cout << "Invalid input! Try again.\n";
        }
        catch(const exception& e)
        {
            cout << e.what() << " Try again.\n";
        }
    }
    delete hf;
}
