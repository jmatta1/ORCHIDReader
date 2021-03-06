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
#include"Output/OutputSystem.h"
#include"Events/DppPsdIntegralEvent.h"
#include"Events/NewFileEvent.h"
#include"Events/OrchidSlowControlsEvent.h"
#include"BufferHeaderData.h"
#include"FileHeaderData.h"

namespace Input 
{

class OrchidFileReader
{
public:
    OrchidFileReader(InputParser::ConfigData* cData, int numDet);
    ~OrchidFileReader();
    
    void processFiles(Output::OutputSystem* output);
    
private:
    void readListFile();
    void processDataBuffer(Output::OutputSystem* output, int startInd);
    
    std::vector<std::string> fileList;
    char* buffer;
    InputParser::ConfigData* confData;
    BufferHeaderData currBufferData;
    FileHeaderData currFileData;
    unsigned long long currentFileSize;
    
    Events::DppPsdIntegralEvent dppEvent;
    Events::NewFileEvent fileEvent;
    Events::OrchidSlowControlsEvent scEvent;
};

}
#endif //ORCHIDREADER_SRC_INPUT_ORCHIDFILEREADER_H
