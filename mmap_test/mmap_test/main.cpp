///////////////////////////////////
//                              //
//      Piccolo Lab             //
//      Fixed Width Files       //
//      James Wengler           //
//                              //
//////////////////////////////////


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
using namespace std;

const int CHUNK_SIZE = 1000;

typedef unordered_map<int, pair <long long int, int>> dictionary;

static inline void trimRightWhitespace(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}


dictionary ParseDataCoordinates(vector<int> lineIndex, char * coorFile, int coorFileMaxLength, long long int fullStrLength)
{
   
    dictionary IndexsStartPositionsWidths;
    for (int i = 0; i < lineIndex.size(); i++)
    {
        int key = lineIndex[i];
        long long int indexToStart = (key * (coorFileMaxLength + 1)) - 1;
        char substringFromFile[coorFileMaxLength];
        memmove(substringFromFile, &coorFile[indexToStart], coorFileMaxLength);
        //copy(substring, &coorFile[indexToStart], coorFileMaxLength);
        string stringToReadFrom = "";
        stringToReadFrom.assign(substringFromFile, coorFileMaxLength);
        istringstream getInt(stringToReadFrom);
        int startPos;
        getInt >> startPos;
        if (indexToStart == (fullStrLength - coorFileMaxLength))
        {
            int width =  ((fullStrLength / (coorFileMaxLength + 1) - 1) - startPos);
            IndexsStartPositionsWidths[key].first = startPos;
            IndexsStartPositionsWidths[key].second = width;
        }
        else
        {
            char substringFromFile[coorFileMaxLength];
            memcpy(substringFromFile, &coorFile[(indexToStart + 1) + (coorFileMaxLength + 1)], (coorFileMaxLength + 1));
            string stringToReadFrom = "";
            stringToReadFrom.assign(substringFromFile, coorFileMaxLength);
            istringstream getInt(stringToReadFrom);
            int endPos;
            getInt >> endPos;
            int width = (endPos - startPos);
            IndexsStartPositionsWidths[key].first = startPos;
            IndexsStartPositionsWidths[key].second = width;
        }
    }
    
    
    return IndexsStartPositionsWidths;
}


int main(int argc, char** argv)
{
    
//     auto begin = std::chrono::steady_clock::now();
    
    
    
    //All argv arguements
    string pathToLlFile = argv[1];
    char* dataPath = argv[2];
    char* pathToColFile = argv[3];
    char* pathToOutput = argv[4];
    string pathToMCCL = argv[5];
    string pathToColTsv = argv[6];
    string pathToRowFile = argv[7];
    
    
    
    
    
    //Opens the line length file, pulls out an integer, and assigns it to lineLength
    ifstream llFile(pathToLlFile);
    int lineLength;
    llFile >> lineLength;
    
    //Creates a int object that can be opened using datamapping for the data input file
    int pathToData = open(dataPath, O_RDONLY, 0);
    if (pathToData < 0)
    {
        cerr << "Unable to open " << argv[2] << " for input." << endl;
        exit(1);
    }
    //lseek finds the end of the file (to be used in opening a datamapped file)
    long long dataFileSize = lseek(pathToData, 0, SEEK_END);
    char *dataFile = reinterpret_cast<char*>(mmap(NULL, dataFileSize, PROT_READ, MAP_FILE | MAP_SHARED, pathToData, 0));
    
    //Using istringstream, calculates the number of rows in the file and stores it as numRows
    istringstream getRows(argv[7]);
    int long long numRows;
    getRows >> numRows;
 
    //Creates a int object that can be opened using datamapping for the column input file
    int cols = open(pathToColFile, O_RDONLY, 0);
    if (cols < 0)
    {
        cerr << "Unable to open " << argv[3] << " for input." << endl;
        exit(1);
    }
    //lseek finds the end of the file (to be used in opening a datamapped file)
    long long colFileSize = lseek(cols, 0, SEEK_END);
    char *colFile = reinterpret_cast<char*>(mmap(NULL, colFileSize, PROT_READ, MAP_FILE | MAP_SHARED, cols, 0));
    
    
    //Uses an ifstream to pull out an int for the maximum column coordinate length (max number of characters per line)
    ifstream mcclFile(pathToMCCL);
    int maxColumnCoordLength;
    mcclFile >> maxColumnCoordLength;

    
    //Uses an ifstream to pull out each index for the column to be grabbed
    string stringToReadFrom;
    vector<int> idsToGet = {};
    //"odd" is responsible for only pulling the integers out of the file by skipping all the strings (input file is int-string-int-string etc)
    int odd = 0;
    ifstream columns(pathToColTsv);
    while (columns >> stringToReadFrom)
    {
        if (odd % 2 == 0)
        {
            //Converts the string (From the file) to an int (for use later)
            istringstream toInt(stringToReadFrom);
            int numericID;
            toInt >> numericID;
            idsToGet.push_back(numericID);
        }
        odd++;
    }
    

    
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
    dictionary indexStartWidth = ParseDataCoordinates(idsToGet, colFile, maxColumnCoordLength, colFileSize);
    for (unsigned long int i = 0; i <= (numRows); i++)
    {
        for (int j = 0; j < idsToGet.size(); j++)
        {
            
            long int coorToGrab = (indexStartWidth[idsToGet[j]].first + (i * lineLength));
            char substringFromFile[indexStartWidth[idsToGet[j]].second];
            memmove(substringFromFile, &dataFile[coorToGrab], indexStartWidth[idsToGet[j]].second);
            substringFromFile[indexStartWidth[idsToGet[j]].second] = '\0';
            string strToAddToChunk = "";
            strToAddToChunk.assign(substringFromFile);
            trimRightWhitespace(strToAddToChunk);
            if (j == (idsToGet.size() - 1))
            {
                strToAddToChunk += "\n";
                chunk += strToAddToChunk;
                
            }
            else
            {
                strToAddToChunk += "\t";
                chunk += strToAddToChunk;
            }
            
            
        }

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
    
    
    
    
    
    

//    auto end = std::chrono::steady_clock::now();
//    cout << "Took " << (chrono::duration_cast<chrono::milliseconds>(end - begin).count()) / 1000 << " seconds." << endl;
    
    return 0;
}
