/***************************************************************************//**
********************************************************************************
**
** @file ConfigData.cpp
** @author James Till Matta
** @date 21 Jan, 2017
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details holds the implementation of the structure that stores the data from
** the first level input file
**
********************************************************************************
*******************************************************************************/
#include"ConfigData.h"
// includes for C system headers
// includes for C++ system headers
#include<iostream>
// includes from other libraries
// includes from ORCHIDReader

namespace InputParser
{

void ConfigData::listFilePathSet(const std::string& input)
{
    listFilePath = input;
    listFilePathSet_ = true;
}

void ConfigData::rootFilePathSet(const std::string& input)
{
    rootFilePath = input;
    rootFilePathSet_ = true;
}

void ConfigData::arrayDataPathSet(const std::string& input)
{
    arrayDataPath = input;
    arrayDataPathSet_ = true;
}

void ConfigData::runCsvPathSet(const std::string& input)
{
    runCsvPath = input;
    runCsvPathSet_ = true;
}

void ConfigData::detMetaDataPathSet(const std::string& input)
{
    detMetaDataPath = input;
    detMetaDataPathSet_ = true;
}

void ConfigData::batchMetaDataPathSet(const std::string& input)
{
    batchMetaDataPath = input;
    batchMetaDataPathSet_ = true;
}

void ConfigData::rootTreeFilePathSet(const std::string& input)
{
    rootTreeFilePath = input;
    rootTreeFilePathSet_ = true;
}

void ConfigData::histIntegrationTimeSet(const float& input)
{
    histIntegrationTime = input;
    histIntegrationTimeSet_ = true;
}

void ConfigData::arrayXPosSet(const float& input)
{
    arrayXPos = input;
    arrayXPosSet_ = true;
}

void ConfigData::arrayYPosSet(const float& input)
{
    arrayYPos = input;
    arrayYPosSet_ = true;
}

void ConfigData::processFirstBufferSet(const bool& input)
{
    processFirstBuffer = input;
    processFirstBufferSet_ = true;
}

void ConfigData::generateRootTreeSet(const bool& input)
{
    generateRootTree = input;
    generateRootTreeSet_ = true;
}

void ConfigData::bufferLengthAdd(const int& input)
{
    bufferLengths.push_back(input);
}

bool ConfigData::validate()
{
    bool retValue =  listFilePathSet_ && histIntegrationTimeSet_ &&
                     arrayDataPathSet_ && arrayXPosSet_ && arrayYPosSet_ &&
                     rootFilePathSet_ && runCsvPathSet_ && detMetaDataPathSet_ &&
                     batchMetaDataPathSet_ && processFirstBufferSet_ &&
                     generateRootTreeSet_ && (bufferLengths.size() > 0);

    if(generateRootTree && !rootTreeFilePathSet_)
    {
        retValue = false;
    }
    
    if(histIntegrationTime <= 1.0)
    {
        retValue = false;
    }
    return retValue;
}

void ConfigData::printValidationErrors()
{
    if(!rootFilePathSet_)
    {
        std::cout<<"    RootFilePath must be set"<<std::endl;
    }
    if(!listFilePathSet_)
    {
        std::cout<<"    ListFilePath must be set"<<std::endl;
    }
    if(!arrayDataPathSet_)
    {
        std::cout<<"    ArrayDataPath must be set"<<std::endl;
    }
    if(!runCsvPathSet_)
    {
        std::cout<<"    RunCsvPath must be set"<<std::endl;
    }
    if(!detMetaDataPathSet_)
    {
        std::cout<<"    DetMetaDataPath must be set"<<std::endl;
    }
    if(!batchMetaDataPathSet_)
    {
        std::cout<<"    BatchMetaDataPath must be set"<<std::endl;
    }
    if(!arrayXPosSet_)
    {
        std::cout<<"    ArrayXPosition must be set"<<std::endl;
    }
    if(!arrayYPosSet_)
    {
        std::cout<<"    ArrayYPosition must be set"<<std::endl;
    }
    if(!processFirstBufferSet_)
    {
        std::cout<<"    ProcessFirstBuffer must be set"<<std::endl;
    }
    if(!histIntegrationTimeSet_)
    {
        std::cout<<"    HistIntegrationTime must be set"<<std::endl;
    }
    if(!generateRootTreeSet_)
    {
        std::cout<<"    GenerateRootTree must be set"<<std::endl;
    }
    if(bufferLengths.size() < 1)
    {
        std::cout<<"    BufferLengths must be set"<<std::endl;
    }
    if(generateRootTree && !rootTreeFilePathSet_)
    {
        std::cout<<"    RootTreeFilePath must be set if GenerateRootTree is true"<<std::endl;
    }
    if(histIntegrationTime <= 1.0)
    {
        std::cout<<"    HistogramIntegrationTime must be at least 1 second"<<std::endl;
    }
}


std::ostream& operator<<(std::ostream& os, const ConfigData& cd) 
{
    std::string temp1 = (cd.processFirstBuffer)?"True":"False";
    std::string temp2 = (cd.generateRootTree)?"True":"False";
    os << "[StartConfig]\n"
       << "    RootFilePath        = "   << cd.rootFilePath         << "\n"
       << "    ListFilePath        = "   << cd.listFilePath         << "\n"
       << "    ArrayDataPath       = "   << cd.arrayDataPath        << "\n"
       << "    RunCsvPath          = "   << cd.runCsvPath           << "\n"
       << "    DetMetaDataPath     = "   << cd.detMetaDataPath      << "\n"
       << "    BatchMetaDataPath   = "   << cd.batchMetaDataPath    << "\n"
       << "    ArrayXPosition      = "   << cd.arrayXPos            << "\n"
       << "    ArrayYPosition      = "   << cd.arrayYPos            << "\n"
       << "    ProcessFirstBuffer  = "   << temp1                   << "\n"
       << "    GenerateRootTree    = "   << temp2                   << "\n"
       << "    RootTreeFilePath    = "   << cd.rootTreeFilePath     << "\n"
       << "    HistIntegrationTime = "   << cd.histIntegrationTime  << "\n"
       << "    BufferLengths       = ["  << cd.bufferLengths[0];
    for(int i=1; i < cd.bufferLengths.size(); ++i)
    {
        os << ",\n                           " << cd.bufferLengths[i];
    }
    return (os << "]\n[EndConfig]\n");
}

}
