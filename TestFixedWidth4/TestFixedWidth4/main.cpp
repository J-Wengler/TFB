/*
 * Copyright (c) 2016-present, Yann Collet, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under both the BSD-style license (found in the
 * LICENSE file in the root directory of this source tree) and the GPLv2 (found
 * in the COPYING file in the root directory of this source tree).
 * You may select, at your option, one of the above-listed licenses.
 */


#include <stdio.h>     // fprintf
#include <iostream>
#include <stdlib.h>    // free
#include </opt/local/include/zlib.h>
#include </usr/local/include/zstd.h>      // presumes zstd library is installed
#include "/Users/jameswengler/TFB/zstd-dev 2/examples/common.h"    // Helper functions, CHECK(), and CHECK_ZSTD()
#include <string>
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

using namespace std;

const int CHUNK_SIZE = 1000;

bool isNumber (char* ctFile, int curIndex)
{
    if (ctFile[curIndex * 2] == 'n')
    {
        return true;
    }
    else
    {
        return false;
    }
}



//This function passes in a string by reference and removes the whitespace to the right
//Used to format the output
static inline void trimRightWhitespace(std::string &s)
{
    unsigned long int endOfWhitespace = 0;
    unsigned long int stringSize = s.size();
    for (unsigned long int i = stringSize; i > 0; i--)
    {
        if (s[i] != ' ' && s[i] != '\0'  && s[i] != '\n')
        {
            endOfWhitespace = i;
            break;
        }
        
    }
    
    s.erase(endOfWhitespace + 1, s.size());
    
}

//This function takes an index and a mmap file, then returns the integar (as an int) found at that position
//Used to build the lineIndex array
size_t getIntFromCCFile(int coorFileMaxLength, char * coorFile, int indexToStart)
{
    char substring[coorFileMaxLength];
    memmove(substring, &coorFile[indexToStart], coorFileMaxLength);
    int position = atoi(substring);
    
    return position;
}

//This function accepts a char* filePath then returns a char* mmap file
//Used to open the data and .cc file
char* openMmapFile(const char* filePath)
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

//This funtion reads a single integar from a file
//Used for ll and mccl file
int readScalarFromFile(string filePath)
{
    ifstream grabInt(filePath);
    int intToGet;
    grabInt >> intToGet;
    
    return intToGet;
}

//This function reads a single integar from agrv
//Used for numRows
int long long readScalarFromArgv(string arguement)
{
    istringstream getRows(arguement);
    int long long scalar;
    getRows >> scalar;
    
    return scalar;
}

//This function returns a vector of the columns that the user wants to project
//Used to make a vector from the _columns_tsv file
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
    
    return lineIndex;
}
//This function takes in a mmap file, a coordinate, the width of the column, and a string by reference
//The string is modified to contain whatever is at the specified coordinate with no trailing whitespace
//Used to format the output
void createTrimmedValue(char * mmapFile, long int coorToGrab, long long int width, string &myString)
{
    char substringFromFile[width];
    memmove(substringFromFile, &mmapFile[coorToGrab], width);
    substringFromFile[width] = '\0';
    myString.assign(substringFromFile);
    trimRightWhitespace(myString);
}

//This function passes in 2 arrays by reference, and then using the lineIndex array, populates them with the
//start position and width of each column the user wants to project
//Used to create the arrays containing the data for each column
void parseDataCoords(unsigned long int lineIndexSize, int* lineIndices, char * coordsFile, int coordsFileMaxLength, long long int* startPositions, long long int* widths)
{
    for (int i = 0; i < lineIndexSize; i++)
    {
        int column = lineIndices[i];
        int indexToStart = (column * (coordsFileMaxLength + 1));
        int startPos = getIntFromCCFile(coordsFileMaxLength, coordsFile, indexToStart);
        startPositions[i] = startPos;
        
        long long int endPos = getIntFromCCFile(coordsFileMaxLength, coordsFile, (indexToStart + coordsFileMaxLength + 1));
        long long int width = (endPos - startPos);
        widths[i] = width;
    }
    
}

vector<int> makeQueryColVector(string csvValues)
{
    vector<int> indices;
    size_t found = csvValues.find(",");
    if (found == string::npos)
    {
        cerr << "No comma found in columns to query, expected 'col,col' got '" << csvValues << "'";
        exit(1);
    }
    else
    {
        string firstCol = csvValues.substr(0, found);
        string secondCol = csvValues.substr(found + 1, csvValues.size() - 1);
        indices.push_back(atoi(firstCol.c_str()));
        indices.push_back(atoi(secondCol.c_str()));
        
    }
    
    return indices;
}

vector<unsigned long int> filterRows (vector<int> queryColIndices, int long long numRows, long long int* colCoords, int lineLength, long long int* colWidths, char* dataMapFile, char* ctFile)
{
    vector<unsigned long int> matchingRows;
    //By default the header row is included
    matchingRows.push_back(0);
    for (unsigned long int i = 1; i < numRows; i++)
    {
        int colsAdded = 0;
        for (int j = 0; j < queryColIndices.size(); j++)
        {
            long int coorToGrab = (colCoords[j] + (i * lineLength));
            long long int width = colWidths[j];
            string strToAdd = "";
            createTrimmedValue(dataMapFile, coorToGrab, width, strToAdd);
            //cout << strToAdd << endl;
            int curIndex = queryColIndices[j];
            if (isNumber(ctFile, curIndex))
            {
                float tempInt = atof(strToAdd.c_str());
                if (tempInt >= .1)
                {
                    colsAdded++;
                }
                else
                {
                    break;
                }
                
            }
            else
            {
                if (strToAdd[0] == 'A' || strToAdd[strToAdd.size() - 1] == 'Z')
                {
                    colsAdded++;
                }
                else
                {
                    break;
                }
                
            }
        }
        if(colsAdded == queryColIndices.size())
        {
            matchingRows.push_back(i);
        }
    }
    return matchingRows;
}






static void decompressFile_orDie(const char* fname)
{
//    std::string outFile = "~/decompressedOutput.txt";
//    const char* outFilePath = outFile.c_str();
    FILE* const fin  = fopen_orDie(fname, "rb");
    size_t const buffInSize = ZSTD_DStreamInSize();
    void*  const buffIn  = malloc_orDie(buffInSize);
    FILE* const fout = stdout;
    size_t const buffOutSize = ZSTD_DStreamOutSize();  /* Guarantee to successfully flush at least one complete compressed block in all circumstances. */
    void*  const buffOut = malloc_orDie(buffOutSize);

    ZSTD_DCtx* const dctx = ZSTD_createDCtx();
    CHECK(dctx != NULL, "ZSTD_createDCtx() failed!");

    /* This loop assumes that the input file is one or more concatenated zstd
     * streams. This example won't work if there is trailing non-zstd data at
     * the end, but streaming decompression in general handles this case.
     * ZSTD_decompressStream() returns 0 exactly when the frame is completed,
     * and doesn't consume input after the frame.
     */
    size_t const toRead = buffInSize;
    size_t read;
    size_t lastRet = 0;
    int isEmpty = 1;
    while ( (read = fread_orDie(buffIn, toRead, fin)) ) {
        
        isEmpty = 0;
        ZSTD_inBuffer input = { buffIn, read, 0 };
        /* Given a valid frame, zstd won't consume the last byte of the frame
         * until it has flushed all of the decompressed data of the frame.
         * Therefore, instead of checking if the return code is 0, we can
         * decompress just check if input.pos < input.size.
         */
        while (input.pos < input.size) {
            ZSTD_outBuffer output = { buffOut, buffOutSize, 0};
            /* The return code is zero if the frame is complete, but there may
             * be multiple frames concatenated together. Zstd will automatically
             * reset the context when a frame is complete. Still, calling
             * ZSTD_DCtx_reset() can be useful to reset the context to a clean
             * state, for instance if the last decompression call returned an
             * error.
             */
            size_t const ret = ZSTD_decompressStream(dctx, &output , &input);
            CHECK_ZSTD(ret);
            fwrite_orDie(buffOut, output.pos, fout);
            lastRet = ret;
            break;
            
        }
    }

    if (isEmpty) {
        fprintf(stderr, "input is empty\n");
        exit(1);
    }

    if (lastRet != 0) {
        /* The last return value from ZSTD_decompressStream did not end on a
         * frame, but we reached the end of the file! We assume this is an
         * error, and the input was truncated.
         */
        fprintf(stderr, "EOF before end of stream: %zu\n", lastRet);
        exit(1);
    }

    ZSTD_freeDCtx(dctx);
    fclose_orDie(fin);
    fclose_orDie(fout);
    free(buffIn);
    free(buffOut);
}

static void decompressOneLine(const char* fname, size_t pos, char*  outFileName)
{
    
    //DON'T USE "W+" IT WILL WIPE THE INPUT FILE
    
//    FILE* const fin  = fopen(fname, "r+");
//    size_t const buffInSize = ZSTD_DStreamInSize();
//    void*  const buffIn  = malloc_orDie(buffInSize);
//    FILE* const fout = fopen(outFileName, "w+");
//    size_t const buffOutSize = ZSTD_DStreamOutSize();  /* Guarantee to successfully flush at least one complete compressed block in all circumstances. */
//    void*  const buffOut = malloc_orDie(buffOutSize);
//
//    ZSTD_DCtx* const dctx = ZSTD_createDCtx();
//    CHECK(dctx != NULL, "ZSTD_createDCtx() failed!");
//
//    /* This loop assumes that the input file is one or more concatenated zstd
//    * streams. This example won't work if there is trailing non-zstd data at
//    * the end, but streaming decompression in general handles this case.
//    * ZSTD_decompressStream() returns 0 exactly when the frame is completed,
//    * and doesn't consume input after the frame.
//    */
//    size_t const toRead = buffInSize;
//    size_t read;
//    size_t lastRet = 0;
//    int isEmpty = 1;
//    while ( (read = fread_orDie(buffIn, 10001, fin)) ) {
//
//        isEmpty = 0;
//        ZSTD_inBuffer input = { buffIn, read, pos };
//        /* Given a valid frame, zstd won't consume the last byte of the frame
//        * until it has flushed all of the decompressed data of the frame.
//        * Therefore, instead of checking if the return code is 0, we can
//        * decompress just check if input.pos < input.size.
//        */
//        while (input.pos < input.size) {
//            ZSTD_outBuffer output = { buffOut, buffOutSize, 0};
//            /* The return code is zero if the frame is complete, but there may
//            * be multiple frames concatenated together. Zstd will automatically
//            * reset the context when a frame is complete. Still, calling
//            * ZSTD_DCtx_reset() can be useful to reset the context to a clean
//            * state, for instance if the last decompression call returned an
//            * error.
//            */
//            size_t const ret = ZSTD_decompressStream(dctx, &output , &input);
//            fwrite_orDie(buffOut, output.pos, fout);
//            break;
//
//        }
//
//        ZSTD_freeDCtx(dctx);
//        fclose_orDie(fin);
//        fclose_orDie(fout);
//        free(buffIn);
//        free(buffOut);
//    }
    
    FILE* const fin  = fopen(fname, "r+");
    size_t const buffInSize = ZSTD_DStreamInSize();
    void*  const buffIn  = malloc_orDie(buffInSize);
    FILE* const fout = fopen(outFileName, "w+");
    size_t const buffOutSize = ZSTD_DStreamOutSize();  /* Guarantee to successfully flush at least one complete compressed block in all circumstances. */
    void*  const buffOut = malloc_orDie(buffOutSize);

    ZSTD_DCtx* const dctx = ZSTD_createDCtx();
    CHECK(dctx != NULL, "ZSTD_createDCtx() failed!");

    /* This loop assumes that the input file is one or more concatenated zstd
    * streams. This example won't work if there is trailing non-zstd data at
    * the end, but streaming decompression in general handles this case.
    * ZSTD_decompressStream() returns 0 exactly when the frame is completed,
    * and doesn't consume input after the frame.
    */
    size_t const toRead = buffInSize;
    size_t read;
    size_t lastRet = 0;
    int isEmpty = 1;
    fseek(fin, pos, SEEK_CUR);
    while ( (read = fread(buffIn, 1, 10000, fin)) ) {
        
        isEmpty = 0;
        ZSTD_inBuffer input = { buffIn, read, 0 };
        /* Given a valid frame, zstd won't consume the last byte of the frame
        * until it has flushed all of the decompressed data of the frame.
        * Therefore, instead of checking if the return code is 0, we can
        * decompress just check if input.pos < input.size.
        */
        while (input.pos < input.size) {
            ZSTD_outBuffer output = { buffOut, buffOutSize, 0};
            /* The return code is zero if the frame is complete, but there may
            * be multiple frames concatenated together. Zstd will automatically
            * reset the context when a frame is complete. Still, calling
            * ZSTD_DCtx_reset() can be useful to reset the context to a clean
            * state, for instance if the last decompression call returned an
            * error.
            */
            size_t const ret = ZSTD_decompressStream(dctx, &output , &input);
            fwrite_orDie(buffOut, output.pos, fout);
            break;
            
        }
        
        ZSTD_freeDCtx(dctx);
        fclose_orDie(fin);
        fclose_orDie(fout);
        //free(buffIn);
        //free(buffOut);
    }
       
    
}


vector<unsigned long int> filterRowsTransposed (const char * transposedFile, vector<int> queryColIndices){
    
    string pathToMccl(transposedFile);
    pathToMccl += ".mrsl";
    
    string rowStart(transposedFile);
    rowStart += ".rowstart";
    
    const char* rowStartCharStar = rowStart.c_str();
    
    char* rowStartFile = openMmapFile(rowStartCharStar);
    
    int mccl = readScalarFromFile(pathToMccl);
    
    vector<unsigned long int> matchingRows;
    
    /* Decompress */
    
    for(int i = 0; i < queryColIndices.size(); i++){
        
        int pos = (mccl + 1) * queryColIndices.at(i);
        size_t index = getIntFromCCFile(mccl, rowStartFile, pos);
        
        string outFile = "./OutFileCol";
        outFile += std::to_string(queryColIndices.at(i));
        outFile += ".txt";
        decompressOneLine(transposedFile, index, (char*)outFile.c_str());
        
        //Perform analysis of file
        
        remove((char*)outFile.c_str());
        
    }
    
    
    return matchingRows;
    
    
}





int main(int argc, const char** argv)
{
    //const char* const exeName = argv[0];
    const char*  inFilename = argv[1];
    const char*  dataPath = argv[2];
    const char*  colNamesFilePath = argv[3];
    string queryColIndicesStr = argv[4];
    const char* outFilePath = argv[5];
    
    string pathToLlFile(dataPath);
    pathToLlFile += ".ll";
    
    string colFile(dataPath);
    colFile += ".cc";
    const char* pathToColFile = colFile.c_str();
    
    string pathToMCCL(dataPath);
    pathToMCCL += ".mccl";
    
    //Opens the line length file, pulls out an integer, and assigns it to lineLength
    int lineLength = readScalarFromFile(pathToLlFile);
    
    //Opens a memory mapped file to the .fwf2 data file
    char *dataMapFile = openMmapFile(dataPath);
    
    //Opens a memory mapped file to the .cc file
    char *ccMapFile = openMmapFile(pathToColFile);
    
    //Uses an ifstream to pull out an int for the maximum column coordinate length (max number of characters per line)
    int maxColumnCoordLength = readScalarFromFile(pathToMCCL);
    
    //Uses an ifstream to pull out each index for the column to be grabbed
    vector<int> lineIndex = createLineIndex(colNamesFilePath);
    unsigned long int lineIndexSize = lineIndex.size();
    int* lineIndexPointerArray = &lineIndex[0];
    
    //Create 2 arrays to be used in ParseDataCoordinates
    long long int colCoords[lineIndexSize];
    long long int colWidths[lineIndexSize];
    
    //Calls ParseDataCoordinates that populates the above arrays with the starting postitions and widths
    parseDataCoords(lineIndexSize, lineIndexPointerArray, ccMapFile, maxColumnCoordLength, colCoords, colWidths);
    
    //Create a vector from queryColIndicesStr
    vector<int> queryColIndices = makeQueryColVector(queryColIndicesStr);
    long long int colCoordsQuery[queryColIndices.size()];
    long long int colWidthsQuery[queryColIndices.size()];
    unsigned long int queryColSize = queryColIndices.size();
    int*queryColPointerArray = &queryColIndices[0];
    
    parseDataCoords(queryColSize, queryColPointerArray, ccMapFile, maxColumnCoordLength, colCoordsQuery, colWidthsQuery);
    
    vector<unsigned long int> matchingRows = filterRowsTransposed(inFilename, queryColIndices);
    
  
    
  

    
    
    
    
    
    
    
    
    
    
    
    /*
    string pathToLlFile = argv[1];
    char* dataPath = argv[2];
    char* pathToColFile = argv[3];
    char* outFilePath = argv[4];
    string pathToMCCL = argv[5];
    string colNamesFilePath = argv[6];
    string intNumRows = argv[7];
    char* pathToCtFile = argv[8];
    string queryColIndicesStr = argv[9];
    */
    
    
    return 0;
}


