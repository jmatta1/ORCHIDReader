/***************************************************************************//**
********************************************************************************
**
** @file ParseFunctions.h
** @author James Till Matta
** @date 31 Jan, 2017
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the definitions of functions that are to be used to
**  parse the config files
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHIDREADER_SRC_CONFIG_PARSERFUNCTIONS_H
#define ORCHIDREADER_SRC_CONFIG_PARSERFUNCTIONS_H

// includes for C system headers
// includes for C++ system headers
#include<string>
#include<iostream>
// includes from other libraries
// includes from ORCHIDReader

namespace InputParser
{

//predeclare the InputParameters class to get a pointer to it
class ConfigData;
class DetData;

//function prototype for the parsing function
bool parseConfigFile(ConfigData* inParams,
                     const std::string& inputFileName);

bool parseValAndPrintConfigFile(ConfigData* inParams,
                                const std::string& inputFileName,
                                std::ostream& os);

bool parseDetDataFile(DetData* inParams,
                      const std::string& inputFileName);

bool parseValAndPrintDetDataFile(DetData* inParams,
                                 const std::string& inputFileName,
                                 std::ostream& os);

}
#endif  // ORCHIDREADER_SRC_CONFIG_PARSERFUNCTIONS_H
