#include <iostream>
#include <string>
#include <fstream>
#include "FileFinder.h"
#include <regex>

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

inline float Convert(unsigned char buffer[], int size)
{
    if (size == 2)
    {
        int val = buffer[0];
        val <<= 8;
        val |= buffer[1];

        if (val != 0)
        {
            int a = 0;
        }

        return static_cast<float>(val);
    }

    return 0;
}

//Wav Header
struct wav_header_t
{
    char chunkID[4]; //"RIFF" = 0x46464952
    unsigned long chunkSize; //28 [+ sizeof(wExtraFormatBytes) + wExtraFormatBytes] + sum(sizeof(chunk.id) + sizeof(chunk.size) + chunk.size)
    char format[4]; //"WAVE" = 0x45564157
    char subchunk1ID[4]; //"fmt " = 0x20746D66
    unsigned long subchunk1Size; //16 [+ sizeof(wExtraFormatBytes) + wExtraFormatBytes]
    unsigned short audioFormat;
    unsigned short numChannels;
    unsigned long sampleRate;
    unsigned long byteRate;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
    //[WORD wExtraFormatBytes;]
    //[Extra format bytes]
};

//Chunks
struct chunk_t
{
    char ID[4]; //"data" = 0x61746164
    unsigned long size;  //Chunk data bytes
};

void WavReader(wstring& fileName)//, wstring& fileNameNew)
{
    FILE* fin = 0;
    
    errno_t error =  _wfopen_s(&fin, fileName.c_str(), L"rb");

    if (error == 0 && fin)
    {
        //Read WAV header
        wav_header_t header;
        fread(&header, sizeof(header), 1, fin);

        //Print WAV header
        printf("WAV File Header read:\n");
        printf("File Type: %s\n", header.chunkID);
        printf("File Size: %ld\n", header.chunkSize);
        printf("WAV Marker: %s\n", header.format);
        printf("Format Name: %s\n", header.subchunk1ID);
        printf("Format Length: %ld\n", header.subchunk1Size);
        printf("Format Type: %hd\n", header.audioFormat);
        printf("Number of Channels: %hd\n", header.numChannels);
        printf("Sample Rate: %ld\n", header.sampleRate);
        printf("Sample Rate * Bits/Sample * Channels / 8: %ld\n", header.byteRate);
        printf("Bits per Sample * Channels / 8.1: %hd\n", header.blockAlign);
        printf("Bits per Sample: %hd\n", header.bitsPerSample);

        //skip wExtraFormatBytes & extra format bytes
        //fseek(f, header.chunkSize - 16, SEEK_CUR);

        //Reading file
        chunk_t chunk;
        printf("id\t" "size\n");
        //go to data chunk
        bool dataFound = false;

        while (!dataFound)
        {
            fread(&chunk, sizeof(chunk), 1, fin);

            dataFound = *(unsigned int*)&chunk.ID == 0x61746164;

            //skip chunk data bytes
            if (!dataFound)
                fseek(fin, chunk.size, SEEK_CUR);
        }

        //Number of samples
        int sample_size = header.bitsPerSample / 8;
        int samples_count = chunk.size * 8 / header.bitsPerSample;
        printf("Samples count = %i\n", samples_count);

        short int* value = new short int[samples_count];
        memset(value, 0, sizeof(short int) * samples_count);

        //Reading data
        int i = 0;

        while (i < samples_count)
            fread(&value[i++], sample_size, 1, fin);

        fclose(fin);

        auto pattern = L"\\.wav$";
        auto replace = L"_.wav";

        auto fileNameNew = std::regex_replace(fileName, std::wregex(pattern, std::regex_constants::icase), replace);

        //Write data into the file
        FILE* fout = 0;
        
        error = _wfopen_s(&fout, fileNameNew.c_str(), L"w");
        
        if (error == 0 && fout)
        {
            for (int i = 0; i < samples_count; i++)
                fprintf(fout, "%d\n", value[i]);

            fclose(fout);
        }
    }
}

int main(int argc, char* argv[]) {
    FileFinder finder(2, 2);

    vector<wstring> vec;

    wstring root = L"c:", pattern = L".*\.wav$";

    finder.Find(root, pattern, vec);

    for (vector<wstring>::const_iterator itr = vec.begin(); itr != vec.end(); itr++)
    {
        std::wcout << *itr << std::endl;

        //wifstream fin(*itr);

        ifstream fin(*itr);

        wav_hdr header;

        //wistream& streamHeader = fin.read((wchar_t*)&header, headSize);

        istream& streamHeader = fin.read((char*)&header, headSize);

        //int filelength = getFileSize(file);

        wav_hdr* myPointer = &header;

        unsigned char* buffer = new unsigned char[header.Subchunk2Size];
        unique_ptr<unsigned char> pointer(buffer);

        //char* buffer = new char[header.Subchunk2Size];
        //unique_ptr<char> pointer(buffer);

        int numBytes = header.Subchunk2Size;

        //wistream& streamData = fin.read((wchar_t*)buffer, numBytes);

        //fread(buffer, header.Subchunk2Size, 1, file);

        fin.close();

        int bytesPerSample = header.bitsPerSample / 8;
        int numSamples = numBytes / bytesPerSample;

        float* floatValues = new float[numSamples];

        unsigned char* bufferon = new unsigned char[bytesPerSample];

        for (int i = 0; i < numBytes; i += bytesPerSample)
        {
            for (int j = 0; j < bytesPerSample; j++)
                bufferon[j] = buffer[i + j];

            float f = Convert(bufferon, bytesPerSample);

            cout << f << " ";
        }

        continue;

        int i = 0;

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