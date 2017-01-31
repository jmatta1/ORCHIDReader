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
// includes from other libraries
#include <boost/spirit/include/qi.hpp>
// includes from ORCHIDReader
#include"ConfigDataParser.h"
#include"ConfigData.h"
#include"DetDataParser.h"
#include"DetData.h"

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
                                const std::string& inputFileName, std::ostream& os)
{
    os << "Reading Configuration Data from "<<inputFileName<<std::endl;
    bool parseSuccess = parseConfigFile(inParams, inputFileName);
    if(!parseSuccess)
    {
        os<<"Could not parse "<<inputFileName<<std::endl;
        return false;
    }
    os << "Configuration read is as follows: "<<std::endl;
    os << *inParams << std::endl;
    
    if(!inParams->validate())
    {
        os << "Could not validate configuration data" << std::endl;
        inParams->printValidationErrors();
        return false;
    }
    return true;
}

bool parseDetDataFile(DetData* inParams, const std::string& inputFileName)
{
    //make the parser, this line will make the compiler *GRIND* as it has to
    //work its way through the huge amount template stuff
    Parsing::DetDataParser<It> detDataParser(inParams);
    //set up the file
    std::ifstream input(inputFileName);
    //unset the skip whitespace
    input.unsetf(std::ios::skipws);
    //set up the iterators
    It start(input), stop;
    //parse the damn thing
    return boost::spirit::qi::parse(start, stop, detDataParser);
}

bool parseValAndPrintDetDataFile(DetData* inParams,
                                 const std::string& inputFileName,
                                 std::ostream& os)
{
    os << "Reading Detector Data from "<<inputFileName<<std::endl;
    bool parseSuccess = parseDetDataFile(inParams, inputFileName);
    if(!parseSuccess)
    {
        os<<"Could not parse "<<inputFileName<<std::endl;
        return false;
    }
    os << "Detector Data read is as follows: "<<std::endl;
    os << *inParams << std::endl;
    
    if(!inParams->validate())
    {
        os << "Could not validate Detector Data file" << std::endl;
        inParams->printValidationErrors();
        return false;
    }
    return true;
}

}
