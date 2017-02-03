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
    std::string listFilePath = "";
    std::string rootFilePath = "";
    std::string arrayDataPath = "";
    float histIntegrationTime = 0.0;
    float arrayXPos = 0.0;
    float arrayYPos = 0.0;
    
    void listFilePathSet(const std::string& input);
    void rootFilePathSet(const std::string& input);
    void arrayDataPathSet(const std::string& input);
    void histIntegrationTimeSet(const float& input);
    void arrayXPosSet(const float& input);
    void arrayYPosSet(const float& input);
    
    bool validate();
    void printValidationErrors();
    
    // ostream operator to allow outputting of the block class
    friend std::ostream& operator<<(std::ostream& os, const ConfigData& gb);
    
private:
    bool listFilePathSet_ = false;
    bool rootFilePathSet_ = false;
    bool arrayDataPathSet_ = false;
    bool histIntegrationTimeSet_ = false;
    bool arrayXPosSet_ = false;
    bool arrayYPosSet_ = false;
};

}

#endif //ORCHIDREADER_SRC_CONFIG_CONFIGDATA_H
