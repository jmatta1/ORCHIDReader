/***************************************************************************//**
********************************************************************************
**
** @file ParseFunctions.cpp
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
** @details Holds the implementations of functions that are to be used to
**  parse the config files
**
********************************************************************************
*******************************************************************************/

#include"ParseFunctions.h"
// includes for C system headers
// includes for C++ system headers
#include<fstream>
#include<iostream>
// includes from other libraries
#include <boost/spirit/include/qi.hpp>
// includes from ORCHIDReader
#include"ConfigDataParser.h"
#include"ConfigData.h"

namespace InputParser
{

typedef boost::spirit::istream_iterator It;

bool parseConfigFile(ConfigData* inParams, const std::string& inputFileName)
{
    //make the parser, this line will make the compiler *GRIND* as it has to
    //work its way through the huge amount template stuff
    Parsing::ConfigDataGrammar<It> configParser(inParams);
    //set up the file
    std::ifstream input(inputFileName);
    //unset the skip whitespace
    input.unsetf(std::ios::skipws);
    //set up the iterators
    It start(input), stop;
    //parse the damn thing
    return boost::spirit::qi::parse(start, stop, configParser);
}

bool parseValAndPrintConfigFile(ConfigData* inParams,
                                const std::string& inputFileName)
{
    std::cout << "Reading input configuration from "<<inputFileName<<std::endl;
    bool parseSuccess = parseConfigFile(inParams, inputFileName);
    if(!parseSuccess)
    {
        std::cout<<"Could not parse "<<inputFileName<<std::endl;
        return false;
    }
    std::cout<<"Configuration read is as follows: "<<std::endl;
    std::cout << *inParams << std::endl;
    
    if(!inParams->validate())
    {
        std::cout << "Could not validate configuration file" << std::endl;
        inParams->printValidationErrors();
        return false;
    }
    return true;
}

}
