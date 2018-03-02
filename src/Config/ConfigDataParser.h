/***************************************************************************//**
********************************************************************************
**
** @file ConfigDataParser.h
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
** @details holds the grammar necessary to parse the first level input file
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHIDREADER_SRC_CONFIG_CONFIGDATAPARSER_H
#define ORCHIDREADER_SRC_CONFIG_CONFIGDATAPARSER_H

// includes for C system headers
// includes for C++ system headers
#include<iostream>
// includes from other libraries
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
// includes from ORCHIDReader
#include"ConfigData.h"
#include"UtilityParsers.h"

namespace InputParser
{

namespace Parsing
{

namespace qi = boost::spirit::qi;

// the main gammar
template <typename Iterator>
struct ConfigDataGrammar : qi::grammar<Iterator>
{
public:
    ConfigDataGrammar(ConfigData* cd) : ConfigDataGrammar::base_type(startRule), ptr(cd)
    {
        namespace phoenix = boost::phoenix;
        using qi::skip;
        using qi::blank;
        using qi::lexeme;
        using qi::float_;
        using qi::int_;
        using Utility::eol_;
        using Utility::boolSymbols_;
        using qi::fail;
        using qi::on_error;
        using Utility::separator;
        //define the rules to parse the parameters
        listFilePath        = (lexeme["ListFilePath"]        >> '=' > quotedString [phoenix::bind(&ConfigData::listFilePathSet,        ptr, qi::_1)] > separator);
        rootFilePath        = (lexeme["RootFilePath"]        >> '=' > quotedString [phoenix::bind(&ConfigData::rootFilePathSet,        ptr, qi::_1)] > separator);
        arrayDataPath       = (lexeme["ArrayDataPath"]       >> '=' > quotedString [phoenix::bind(&ConfigData::arrayDataPathSet,       ptr, qi::_1)] > separator);
        runCsvPath          = (lexeme["RunCsvPath"]          >> '=' > quotedString [phoenix::bind(&ConfigData::runCsvPathSet,          ptr, qi::_1)] > separator);
        detMetaDataPath     = (lexeme["DetMetaDataPath"]     >> '=' > quotedString [phoenix::bind(&ConfigData::detMetaDataPathSet,     ptr, qi::_1)] > separator);
        batchMetaDataPath   = (lexeme["BatchMetaDataPath"]   >> '=' > quotedString [phoenix::bind(&ConfigData::batchMetaDataPathSet,   ptr, qi::_1)] > separator);
        histIntegrationTime = (lexeme["HistIntegrationTime"] >> '=' > float_       [phoenix::bind(&ConfigData::histIntegrationTimeSet, ptr, qi::_1)] > separator);
        arrayXPos           = (lexeme["ArrayXPosition"]      >> '=' > float_       [phoenix::bind(&ConfigData::arrayXPosSet,           ptr, qi::_1)] > separator);
        arrayYPos           = (lexeme["ArrayYPosition"]      >> '=' > float_       [phoenix::bind(&ConfigData::arrayYPosSet,           ptr, qi::_1)] > separator);
        procFirstBuff       = (lexeme["ProcessFirstBuffer"]  >> '=' > boolSymbols_ [phoenix::bind(&ConfigData::processFirstBufferSet,  ptr, qi::_1)] > separator);
        genRootTree         = (lexeme["GenerateRootTree"]    >> '=' > boolSymbols_ [phoenix::bind(&ConfigData::generateRootTreeSet,    ptr, qi::_1)] > separator);
        rootTreeFilePath    = (lexeme["RootTreeFilePath"]    >> '=' > quotedString [phoenix::bind(&ConfigData::rootTreeFilePathSet,    ptr, qi::_1)] > separator);
        bufferLength        = (lexeme["BufferLength"]        >> '=' > '[' >> int_ [phoenix::bind(&ConfigData::bufferLengthAdd, ptr, qi::_1)] >> +(',' >> int_ [phoenix::bind(&ConfigData::bufferLengthAdd, ptr, qi::_1)]) >> ']' > separator);
        
        // define the start rule which holds the whole monstrosity and set the rule to skip blanks
        // if we skipped spaces we could not parse newlines as separators
        startRule = skip(blank) [configDataRule];
        configDataRule = *eol_ >> lexeme["[StartConfig]"] >> *eol_
            > (
                listFilePath ^ histIntegrationTime ^ arrayDataPath ^
                arrayXPos ^ arrayYPos ^ rootFilePath ^ runCsvPath ^
                detMetaDataPath ^ batchMetaDataPath ^ procFirstBuff ^
                genRootTree ^ rootTreeFilePath ^ bufferLength
            ) > lexeme["[EndConfig]"];
        
        on_error<fail>(startRule,
                    std::cout << phoenix::val("Error! Expecting ")
                              << qi::_4                             // what failed?
                              << phoenix::val(" here: \n\"")
                              << phoenix::construct<std::string>(qi::_3, qi::_2) // iterators to error-pos, end
                              << phoenix::val("\"")
                              << std::endl);
    }
private:
    //base rules for the file
    qi::rule<Iterator>                 startRule;
    qi::rule<Iterator, qi::blank_type> configDataRule;
    
    // special sub grammars
    Utility::QuotedString<Iterator> quotedString;
    
    // parameters
    qi::rule<Iterator, qi::blank_type> listFilePath, histIntegrationTime;
    qi::rule<Iterator, qi::blank_type> arrayDataPath, arrayXPos;
    qi::rule<Iterator, qi::blank_type> arrayYPos, rootFilePath;
    qi::rule<Iterator, qi::blank_type> runCsvPath, detMetaDataPath;
    qi::rule<Iterator, qi::blank_type> batchMetaDataPath, procFirstBuff;
    qi::rule<Iterator, qi::blank_type> genRootTree, rootTreeFilePath;
    qi::rule<Iterator, qi::blank_type> bufferLength;
    
    // hold the pointer that we are going to bind to
    ConfigData* ptr;
    
};

}

}

#endif //ORCHIDREADER_SRC_CONFIG_CONFIGDATAPARSER_H
