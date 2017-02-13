/***************************************************************************//**
********************************************************************************
**
** @file OrchidFileReader.h
** @author James Till Matta
** @date 03 Feb, 2017
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the definition of the class which reads the ORCHID files
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHIDREADER_SRC_INPUT_ORCHIDFILEREADER_H
#define ORCHIDREADER_SRC_INPUT_ORCHIDFILEREADER_H
// includes for C system headers
// includes for C++ system headers
#include<string>
#include<vector>
// includes from other libraries
// includes from ORCHIDReader
#include"Config/ConfigData.h"
#include"Output/RootOutput.h"
#include"Events/DppPsdIntegralEvent.h"
#include"Events/InputFileSwapEvent.h"
#include"Events/OrchidSlowControlsEvent.h"

namespace Input 
{

struct BufferHeaderData
{
public:
    unsigned int eventCount;
    unsigned int bufferNumber;
    unsigned long long bufferStartTime;
    unsigned long long bufferStopTime;
    
    BufferHeaderData& operator=(const BufferHeaderData& rhs);
    int readFromBuffer(char* buffer);
};

struct FileHeaderData
{
public:
    unsigned short orchidMajorVer;
    unsigned short orchidMinorVer;
    unsigned short orchidPatchVer;
    unsigned long long fileStartTime;
    std::string fileStartTimeString;
    std::string runTitle;
    unsigned long runNumber;
    unsigned long sequenceNumber;
    
    FileHeaderData& operator=(const FileHeaderData& rhs);
    void readFromBuffer(char* buffer);
};

class OrchidFileReader
{
public:
    OrchidFileReader(InputParser::ConfigData* cData, int numDet);
    ~OrchidFileReader();
    
    void processFiles(Output::RootOutput* output);
    
private:
    void readListFile();
    void processDataBuffer(Output::RootOutput* output, int startInd);
    
    std::vector<std::string> fileList;
    char* buffer;
    InputParser::ConfigData* confData;
    BufferHeaderData currBufferData;
    FileHeaderData currFileData;
    unsigned long long currentFileSize;
    
    Events::DppPsdIntegralEvent dppEvent;
    Events::InputFileSwapEvent fileEvent;
    Events::OrchidSlowControlsEvent scEvent;
};

}
#endif //ORCHIDREADER_SRC_INPUT_ORCHIDFILEREADER_H
