/***************************************************************************//**
********************************************************************************
**
** @author James Till Matta
**
** @copyright Copyright (C) 2018 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the definition of the class which reads the ORCHID files
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHIDREADER_SRC_INPUT_FILEHEADERDATA_H
#define ORCHIDREADER_SRC_INPUT_FILEHEADERDATA_H
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

struct FileHeaderData
{
public:
    unsigned short orchidMajorVer;
    unsigned short orchidMinorVer;
    unsigned short orchidPatchVer;
    unsigned long long fileStartTime;
    std::string fileStartTimeString;
    std::string runTitle;
    unsigned int runNumber;
    unsigned int sequenceNumber;
    
    FileHeaderData& operator=(const FileHeaderData& rhs);
    void readFromBuffer(char* buffer);
};

}
#endif //ORCHIDREADER_SRC_INPUT_FILEHEADERDATA_H
