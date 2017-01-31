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

void ConfigData::overallDbTempPathSet(const std::string& input)
{
    overallDbTempPath = input;
    overallDbTempPathSet_ = true;
}

void ConfigData::batchDbPathSet(const std::string& input)
{
    batchDbPath = input;
    batchDbPathSet_ = true;
}

void ConfigData::listFilePathSet(const std::string& input)
{
    listFilePath = input;
    listFilePathSet_ = true;
}

void ConfigData::logFilePathSet(const std::string& input)
{
    logFilePath = input;
    logFilePathSet_ = true;
}

void ConfigData::arrayDataPathSet(const std::string& input)
{
    arrayDataPath = input;
    arrayDataPathSet_ = true;
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

bool ConfigData::validate()
{
    bool retValue =  overallDbTempPathSet_ | batchDbPathSet_ |
                     listFilePathSet_ | histIntegrationTimeSet_ |
                     logFilePathSet_ | arrayDataPathSet_ |
                     arrayXPosSet_ | arrayYPosSet_;
    
    if(histIntegrationTime <= 1.0)
    {
        retValue = false;
    }
    return retValue;
}

void ConfigData::printValidationErrors()
{
    if(!overallDbTempPathSet_)
    {
        std::cout<<"    OverallDataTempPath must be set"<<std::endl;
    }
    if(!batchDbPathSet_)
    {
        std::cout<<"    BatchDbPath must be set"<<std::endl;
    }
    if(!listFilePathSet_)
    {
        std::cout<<"    ListFilePath must be set"<<std::endl;
    }
    if(!logFilePathSet_)
    {
        std::cout<<"    LogFilePath must be set"<<std::endl;
    }
    if(!arrayDataPathSet_)
    {
        std::cout<<"    ArrayDataPath must be set"<<std::endl;
    }
    if(!arrayXPosSet_)
    {
        std::cout<<"    ArrayXPosition must be set"<<std::endl;
    }
    if(!arrayYPosSet_)
    {
        std::cout<<"    ArrayYPosition must be set"<<std::endl;
    }
    if(!histIntegrationTimeSet_)
    {
        std::cout<<"    HistIntegrationTime must be set"<<std::endl;
    }
    if(histIntegrationTime <= 1.0)
    {
        std::cout<<"    HistogramIntegrationTime must be at least 1 second"<<std::endl;
    }
}


std::ostream& operator<<(std::ostream& os, const ConfigData& cd) 
{
return os << "[StartConfig]\n"
    << "    OverallDataTempPath = "   << cd.overallDbTempPath   << "\n"
    << "    BatchDbPath         = "   << cd.batchDbPath         << "\n"
    << "    ListFilePath        = "   << cd.listFilePath        << "\n"
    << "    LogFilePath         = "   << cd.logFilePath         << "\n"
    << "    ArrayDataPath       = "   << cd.arrayDataPath       << "\n"
    << "    ArrayXPosition      = "   << cd.arrayXPos           << "\n"
    << "    ArrayYPosition      = "   << cd.arrayYPos           << "\n"
    << "    HistIntegrationTime = "   << cd.histIntegrationTime << "\n"
    << "[EndConfig]\n";
}

}
