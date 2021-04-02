#include <iostream>
#include <string>
#include <fstream>
#include "FileFinder.h"

using namespace std;
using std::string;
using std::fstream;

typedef struct WAV_HEADER {
    char                RIFF[4];        // RIFF Header      Magic header
    unsigned long       ChunkSize;      // RIFF Chunk Size  
    char                WAVE[4];        // WAVE Header      
    char                fmt[4];         // FMT header       
    unsigned long       Subchunk1Size;  // Size of the fmt chunk                                
    unsigned short      AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM 
    unsigned short      NumOfChan;      // Number of channels 1=Mono 2=Sterio                   
    unsigned long       SamplesPerSec;  // Sampling Frequency in Hz                             
    unsigned long       bytesPerSec;    // bytes per second 
    unsigned short      blockAlign;     // 2=16-bit mono, 4=16-bit stereo 
    unsigned short      bitsPerSample;  // Number of bits per sample      
    char                Subchunk2ID[4]; // "data"  string   
    unsigned long       Subchunk2Size;  // Sampled data length    

} wav_hdr;

// Function prototypes 
int getFileSize(FILE* inFile);

const int headSize = sizeof(wav_hdr);

int main(int argc, char* argv[]) {
    FileFinder finder(2, 2);

    vector<wstring> vec;

    wstring root = L"c:", pattern = L".*\.wav$";

    finder.Find(root, pattern, vec);

    for (vector<wstring>::const_iterator itr = vec.begin(); itr != vec.end(); itr++)
    {
        std::wcout << *itr << std::endl;

        //FILE* file;

        //auto error = _wfopen_s(&file, (*itr).c_str(), L"r");

        ifstream fin(*itr);

        wav_hdr header;

        istream& streamHeader = fin.read((char*)&header, headSize);

        //int filelength = getFileSize(file);

        unsigned char* buffer = new unsigned char[header.Subchunk2Size];
        unique_ptr<unsigned char> pointer(buffer);

        //char* buffer = new char[header.Subchunk2Size];
        //unique_ptr<char> pointer(buffer);

        int numBytes = header.Subchunk2Size;

        istream& streamData = fin.read((char*)buffer, numBytes);

        //fread(buffer, header.Subchunk2Size, 1, file);

        fin.close();

        int bytesPerSample = header.bitsPerSample / 8;
        int numSamples = numBytes / bytesPerSample;

        continue;

        int i = 0;

        while (i < header.Subchunk2Size)
            cout << buffer[i++];

        //cout << "File is                    :" << filelength << " bytes." << endl;

        cout << "RIFF header                :" << header.RIFF[0]
            << header.RIFF[1]
            << header.RIFF[2]
            << header.RIFF[3] << endl;

        cout << "WAVE header                :" << header.WAVE[0]
            << header.WAVE[1]
            << header.WAVE[2]
            << header.WAVE[3]
            << endl;

        cout << "FMT                        :" << header.fmt[0]
            << header.fmt[1]
            << header.fmt[2]
            << header.fmt[3]
            << endl;

        cout << "Data size                  :" << header.ChunkSize << endl;

        // Display the sampling Rate form the header
        cout << "Sampling Rate              :" << header.SamplesPerSec << endl;
        cout << "Number of bits used        :" << header.bitsPerSample << endl;
        cout << "Number of channels         :" << header.NumOfChan << endl;
        cout << "Number of bytes per second :" << header.bytesPerSec << endl;
        cout << "Data length                :" << header.Subchunk2Size << endl;
        cout << "Audio Format               :" << header.AudioFormat << endl;
        // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM 


        cout << "Block align                :" << header.blockAlign << endl;

        cout << "Data string                :" << header.Subchunk2ID[0]
            << header.Subchunk2ID[1]
            << header.Subchunk2ID[2]
            << header.Subchunk2ID[3]
            << endl;
    }

    ////////////} while (answer == "y");


    ////////getchar();
    ////return 0;
}
// find the file size 
int getFileSize(FILE* inFile) {
    int fileSize = 0;
    fseek(inFile, 0, SEEK_END);

    fileSize = ftell(inFile);

    fseek(inFile, 0, SEEK_SET);
    return fileSize;
}