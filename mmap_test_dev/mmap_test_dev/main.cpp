///////////////////////////////////
//                              //
//      Piccolo Lab             //
//      Fixed Width Files DEV   //
//      James Wengler           //
//                              //
//////////////////////////////////


#include <algorithm>
#include <sstream>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fstream>
#include <vector>
#include <ctype.h>
#include <unordered_map>
#include <string>
using namespace std;

const int CHUNK_SIZE = 1000;

typedef unordered_map<int, pair <long long int, int>> dictionary;

static inline void trimRightWhitespace(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

int getIntFromCCFile(int coorFileMaxLength, char * coorFile, int indexToStart)
{
    char substring[coorFileMaxLength];
    memmove(substring, &coorFile[indexToStart], coorFileMaxLength);
    int position = atoi(substring);
    
    return position;
    
}

char* openMmapFile(char* filePath)
{
    int intFileObject = open(filePath, O_RDONLY, 0);
    if (intFileObject < 0)
    {
        cerr << "Unable to open " << filePath << " for input." << endl;
        exit(1);
    }
    //lseek finds the end of the file (to be used in opening a datamapped file)
    long long dataFileSize = lseek(intFileObject, 0, SEEK_END);
    char *dataFile = reinterpret_cast<char*>(mmap(NULL, dataFileSize, PROT_READ, MAP_FILE | MAP_SHARED, intFileObject, 0));
    
    return dataFile;
}

int readScalarFromFile(string filePath)
{
    ifstream grabInt(filePath);
    int intToGet;
    grabInt >> intToGet;
    return intToGet;
}

int long long readScalarFromArgv(string arguement)
{
    istringstream getRows(arguement);
    int long long scalar;
    getRows >> scalar;
    
    return scalar;
}

vector<int> createLineIndex(string filePath)
{
    string stringToReadFrom = "";
    vector<int> lineIndex = {};
    //"odd" is responsible for only pulling the integers out of the file by skipping all the strings (input file is int-string-int-string etc)
    int odd = 0;    ifstream columns(filePath);
    while (columns >> stringToReadFrom)
    {
        if (odd % 2 == 0)
        {
            //Converts the string (From the file) to an int (for use later)
            istringstream toInt(stringToReadFrom);
            int numericID;
            toInt >> numericID;
            lineIndex.push_back(numericID);
        }
        odd++;
    }
    //arraySize = lineIndex.size();
    
    return lineIndex;
    
}

void createTrimmedValue(char * mmapFile, long int coorToGrab, int width, string &myString)
{
    char substringFromFile[width];
    memmove(substringFromFile, &mmapFile[coorToGrab], width);
    substringFromFile[width] = '\0';
    myString.assign(substringFromFile);
    trimRightWhitespace(myString);
    
}

void ParseDataCoordinates(int lineIndexSize, int* lineIndex, char * coorFile, int coorFileMaxLength, int* startPositions, int* widths)
{
    
    for (int i = 0; i < lineIndexSize; i++)
    {
        int key = lineIndex[i];
        long long int indexToStart = (key * (coorFileMaxLength + 1));
        int startPos = getIntFromCCFile(coorFileMaxLength, coorFile, indexToStart);
        startPositions[i] = startPos;
        
        int endPos = getIntFromCCFile(coorFileMaxLength, coorFile, (indexToStart + coorFileMaxLength + 1));
        int width = (endPos - startPos);
        widths[i] = width;
    }
    
}


int main(int argc, char** argv)
{
    
    //All argv arguements
    string pathToLlFile = argv[1];
    char* dataPath = argv[2];
    char* pathToColFile = argv[3];
    char* pathToOutput = argv[4];
    string pathToMCCL = argv[5];
    string pathToColTsv = argv[6];
    string intNumRows = argv[7];
    
    
    //Opens the line length file, pulls out an integer, and assigns it to lineLength
    int lineLength = readScalarFromFile(pathToLlFile);

    
    //Opens a memory mapped file to the .fwf2 data file
    char *dataFile = openMmapFile(dataPath);
    
    //Uses istringstream to pull an int from the command line
    int long long numRows = readScalarFromArgv(intNumRows);
    
    //Opens a memory mapped file to the .cc file
    char *colFile = openMmapFile(pathToColFile);
    
    
    //Uses an ifstream to pull out an int for the maximum column coordinate length (max number of characters per line)
    int maxColumnCoordLength = readScalarFromFile(pathToMCCL);

    
    
    //Uses an ifstream to pull out each index for the column to be grabbed
    vector<int> lineIndex = createLineIndex(pathToColTsv);
    int lineIndexSize = lineIndex.size();
    int* lineIndexPointerArray = &lineIndex[0];
    
    
    int startPositions[lineIndexSize];
    int colWidths[lineIndexSize];
    
    
    //Uses a FILE object to open argv[4] as an output file
    //Implements chunking to reduce writing calls to the file
    //Writes to the file using fprintf (C syntax, and notably faster than C++)
    //"chunk" is the string that is built to be written to the file
    string chunk = "";
    int chunkCount = 0;
    FILE* outFile =  fopen(pathToOutput, "w");
    if (outFile == NULL)
    {
        cerr << "Failed to open output file (was NULL)" << endl;
        exit(1);
    }
    ParseDataCoordinates(lineIndexSize, lineIndexPointerArray, colFile, maxColumnCoordLength, startPositions, colWidths);
    for (unsigned long int i = 0; i <= (numRows); i++)
    {
        for (int j = 0; j < lineIndexSize - 1; j++)
        {
            
            long int coorToGrab = (startPositions[j] + (i * lineLength));
            int width = colWidths[j];
            string strToAdd = "";
            createTrimmedValue(dataFile, coorToGrab, width, strToAdd);
            strToAdd+= '\t';
            chunk += strToAdd;
            
        }
        long int coorToGrab = (startPositions[lineIndexSize - 1] + (i * lineLength));
        int width = colWidths[lineIndexSize - 1];
        string strToAdd = "";
        createTrimmedValue(dataFile, coorToGrab, width, strToAdd);
        strToAdd+= '\n';
        chunk+=strToAdd;
        
        //Checks if the current chunk is still less in size than CHUNK_SIZE (a global variable)
        //if not, the chunk is written to the file
        if (chunkCount < CHUNK_SIZE)
        {
            chunkCount++;
        }
        else
        {
            //the .c_str() function converts chunk from char[] to char*[]
            fprintf(outFile,"%s", chunk.c_str());
            chunk = "";
            chunkCount = 0;
        }
        
    }
    
    
    //After the for loop, adds the remaing chunk to the file
    if (chunk.size() > 0)
    {
        
        fprintf(outFile,"%s", chunk.c_str());
    }
    
    
    return 0;
}
