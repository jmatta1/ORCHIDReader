/***************************************************************************//**
********************************************************************************
**
** @file ConfigData.h
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
** @details holds the definition of the structure that stores the data from the
** first level input file
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHIDREADER_SRC_CONFIG_CONFIGDATA_H
#define ORCHIDREADER_SRC_CONFIG_CONFIGDATA_H

// includes for C system headers
// includes for C++ system headers
#include<string>
// includes from other libraries
// includes from ORCHIDReader


namespace InputParser
{
class ConfigData
{
public:
    std::string overallDbTempPath = "";
    std::string batchDbPath = "";
    std::string listFilePath = "";
    float histIntegrationTime = 0.0;
    std::string logFilePath = "";
    std::string arrayDataPath = "";
    float arrayXPos = 0.0;
    float arrayYPos = 0.0;
    
    void overallDbTempPathSet(const std::string& input);
    void batchDbPathSet(const std::string& input);
    void listFilePathSet(const std::string& input);
    void logFilePathSet(const std::string& input);
    void arrayDataPathSet(const std::string& input);
    void histIntegrationTimeSet(const float& input);
    void arrayXPosSet(const float& input);
    void arrayYPosSet(const float& input);
    
    bool validate();
    void printValidationErrors();
    
    // ostream operator to allow outputting of the block class
    friend std::ostream& operator<<(std::ostream& os, const ConfigData& gb);
    
private:
    bool overallDbTempPathSet_ = false;
    bool batchDbPathSet_ = false;
    bool listFilePathSet_ = false;
    bool histIntegrationTimeSet_ = false;
    bool logFilePathSet_ = false;
    bool arrayDataPathSet_ = false;
    bool arrayXPosSet_ = false;
    bool arrayYPosSet_ = false;
};

}

#endif //ORCHIDREADER_SRC_CONFIG_CONFIGDATA_H
