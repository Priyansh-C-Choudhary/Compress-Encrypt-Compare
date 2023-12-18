#include <iostream>
#include <fstream>
#include <string>
#include <aes.h>
#include <des.h>
#include <modes.h>
#include <files.h>
#include <filters.h>
#include <osrng.h>

using namespace std;
using namespace CryptoPP;

SecByteBlock generateIV()
{
    SecByteBlock iv(AES::BLOCKSIZE);
    AutoSeededRandomPool prng;
    prng.GenerateBlock(iv, iv.size());
    return iv;
}

// Function to perform AES encryption
void aesEncrypt(const string& inputFileName, const string& outputFileName, const string& key)
{
    try
    {
        string ciphertext;
        SecByteBlock iv = generateIV();

        CBC_Mode<AES>::Encryption aesEncryption(reinterpret_cast<const CryptoPP::byte*>(key.c_str()), AES::DEFAULT_KEYLENGTH, iv);
        FileSource fileSource(inputFileName.c_str(), true,
            new StreamTransformationFilter(aesEncryption,
                new StringSink(ciphertext)
            )
        );

        ofstream outputFile(outputFileName, ios::binary);

        // Save the IV in the output file before the ciphertext
        outputFile.write(reinterpret_cast<const char*>(iv.BytePtr()), AES::BLOCKSIZE);
        outputFile << ciphertext;
        cout << "AES Encryption Successful.\n";
    }
    catch (const Exception& e)
    {
        cerr << "Error: " << e.what() << endl;
    }
}

// Function to perform AES decryption
void aesDecrypt(const string& inputFileName, const string& outputFileName, const string& key)
{
    try
    {
        string decryptedtext;
        SecByteBlock iv(AES::BLOCKSIZE);

        // Read the IV from the input file
        ifstream inputFile(inputFileName, ios::binary);
        inputFile.read(reinterpret_cast<char*>(iv.BytePtr()), AES::BLOCKSIZE);

        CBC_Mode<AES>::Decryption aesDecryption(reinterpret_cast<const CryptoPP::byte*>(key.c_str()), AES::DEFAULT_KEYLENGTH, iv);
        FileSource fileSource(inputFileName.c_str(), true,
            new StreamTransformationFilter(aesDecryption,
                new StringSink(decryptedtext)
            )
        );

        ofstream outputFile(outputFileName);
        outputFile << decryptedtext;
        cout << "AES Decryption Successful.\n";
    }
    catch (const Exception& e)
    {
        cerr << "Error: " << e.what() << endl;
    }
}

// Function to perform DES encryption
void desEncrypt(const string& inputFileName, const string& outputFileName, const string& key)
{
    try
    {
        string ciphertext;
        CBC_Mode<DES>::Encryption desEncryption(reinterpret_cast<const CryptoPP::byte*>(key.c_str()), DES::DEFAULT_KEYLENGTH);
        FileSource fileSource(inputFileName.c_str(), true,
            new StreamTransformationFilter(desEncryption,
                new StringSink(ciphertext)
            )
        );

        ofstream outputFile(outputFileName, ios::binary);
        outputFile << ciphertext;
        cout << "DES Encryption Successful.\n";
    }
    catch (const Exception& e)
    {
        cerr << "Error: " << e.what() << endl;
    }
}

// Function to perform DES decryption
void desDecrypt(const string& inputFileName, const string& outputFileName, const string& key)
{
    try
    {
        string decryptedtext;
        CBC_Mode<DES>::Decryption desDecryption(reinterpret_cast<const CryptoPP::byte*>(key.c_str()), DES::DEFAULT_KEYLENGTH);
        FileSource fileSource(inputFileName.c_str(), true,
            new StreamTransformationFilter(desDecryption,
                new StringSink(decryptedtext)
            )
        );

        ofstream outputFile(outputFileName);
        outputFile << decryptedtext;
        cout << "DES Decryption Successful.\n";
    }
    catch (const Exception& e)
    {
        cerr << "Error: " << e.what() << endl;
    }
}

// Function to perform 3DES encryption
void tripleDESEncrypt(const string& inputFileName, const string& outputFileName, const string& key)
{
    try
    {
        string ciphertext;
        CBC_Mode<DES_EDE3>::Encryption des3Encryption(reinterpret_cast<const CryptoPP::byte*>(key.c_str()), DES_EDE3::DEFAULT_KEYLENGTH);
        FileSource fileSource(inputFileName.c_str(), true,
            new StreamTransformationFilter(des3Encryption,
                new StringSink(ciphertext)
            )
        );

        ofstream outputFile(outputFileName, ios::binary);
        outputFile << ciphertext;
        cout << "3DES Encryption Successful.\n";
    }
    catch (const Exception& e)
    {
        cerr << "Error: " << e.what() << endl;
    }
}

// Function to perform 3DES decryption
void tripleDESDecrypt(const string& inputFileName, const string& outputFileName, const string& key)
{
    try
    {
        string decryptedtext;
        CBC_Mode<DES_EDE3>::Decryption des3Decryption(reinterpret_cast<const CryptoPP::byte*>(key.c_str()), DES_EDE3::DEFAULT_KEYLENGTH);
        FileSource fileSource(inputFileName.c_str(), true,
            new StreamTransformationFilter(des3Decryption,
                new StringSink(decryptedtext)
            )
        );

        ofstream outputFile(outputFileName);
        outputFile << decryptedtext;
        cout << "3DES Decryption Successful.\n";
    }
    catch (const Exception& e)
    {
        cerr << "Error: " << e.what() << endl;
    }
}

// Main function
int main()
{
    int choice;
    string inputFileName, outputFileName, key;

    cout << "===FILE ENCRYPTION SYSTEM===\n";

    while (true)
    {
        cout << "\n[1] AES Encryption\n[2] AES Decryption\n";
        cout << "[3] DES Encryption\n[4] DES Decryption\n";
        cout << "[5] 3DES Encryption\n[6] 3DES Decryption\n";
        cout << "[7] Huffman Compression\n[8] Huffman Decompression\n";
        cout << "[9] Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 9)
            break;

        cout << "Enter the input file name: ";
        cin >> inputFileName;
        cout << "Enter the output file name: ";
        cin >> outputFileName;
        cout << "Enter the encryption/decryption key: ";
        cin >> key;

        switch (choice)
        {
        case 1:
            aesEncrypt(inputFileName, outputFileName, key);
            break;
        case 2:
            aesDecrypt(inputFileName, outputFileName, key);
            break;
        case 3:
            desEncrypt(inputFileName, outputFileName, key);
            break;
        case 4:
            desDecrypt(inputFileName, outputFileName, key);
            break;
        case 5:
            tripleDESEncrypt(inputFileName, outputFileName, key);
            break;
        case 6:
            tripleDESDecrypt(inputFileName, outputFileName, key);
            break;
        default:
            cout << "Invalid Choice\n";
            break;
        }
    }

    return 0;
}