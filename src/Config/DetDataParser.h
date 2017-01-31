/***************************************************************************//**
********************************************************************************
**
** @file DetDataParser.h
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
** @details holds the grammar necessary to parse the detector data file
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHIDREADER_SRC_CONFIG_DETDATAPARSER_H
#define ORCHIDREADER_SRC_CONFIG_DETDATAPARSER_H

// includes for C system headers
// includes for C++ system headers
// includes from other libraries
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
// includes from ORCHIDReader
#include"DetData.h"
#include"UtilityParsers.h"

namespace InputParser
{

namespace Parsing
{

template <typename Iterator>
struct DetDataParser : qi::grammar<Iterator>
{
public:
    DetDataParser(DetData* cd) : DetDataParser::base_type(startRule), ptr(cd)
    {
        using qi::skip;
        using qi::eoi;
        using qi::blank;
        using qi::lexeme;
        using qi::fail;
        using qi::on_error;
        using qi::int_;
        using qi::hex;
        using qi::float_;
        using Utility::boolSymbols_;
        using Utility::eol_;
        namespace phoenix = boost::phoenix;
        
        // define the start rule which holds the whole monstrosity and set the rule to skip blanks
        // if we skipped spaces we could not parse newlines as separators
        startRule = skip(blank) [fileRule];
        fileRule  = (
                       *eol_
                       > lineRule
                       > *(+eol_ >> lineRule)
                       > *eol_
                       > eoi
                       );
        lineRule = (subLineRule1 > subLineRule2);
        subLineRule1 = (int_    [phoenix::bind(&DetData::addDetectorNum,     ptr, qi::_1)] > lexeme[','] >
                       int_     [phoenix::bind(&DetData::addDigiBoardNum,    ptr, qi::_1)] > lexeme[','] >
                       int_     [phoenix::bind(&DetData::addDigiChanNum,     ptr, qi::_1)] > lexeme[','] >
                       int_     [phoenix::bind(&DetData::addMpodBoardNum,    ptr, qi::_1)] > lexeme[','] >
                       int_     [phoenix::bind(&DetData::addMpodChanNum,     ptr, qi::_1)] > lexeme[','] >
                       float_   [phoenix::bind(&DetData::addDetXOffset,      ptr, qi::_1)] > lexeme[',']);
        subLineRule2 = (float_   [phoenix::bind(&DetData::addDetYOffset,      ptr, qi::_1)] > lexeme[','] >
                       float_   [phoenix::bind(&DetData::addDetZOffset,      ptr, qi::_1)] > lexeme[','] >
                       detType_ [phoenix::bind(&DetData::addDetType,         ptr, qi::_1)] > lexeme[','] >
                       float_   [phoenix::bind(&DetData::addPsdProjEnThresh, ptr, qi::_1)] > lexeme[','] >
                       float_   [phoenix::bind(&DetData::addEnProjPsdThresh, ptr, qi::_1)]);
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
    qi::rule<Iterator>                  startRule;
    qi::rule<Iterator, qi::blank_type > fileRule;
    qi::rule<Iterator, qi::blank_type > lineRule;
    qi::rule<Iterator, qi::blank_type > subLineRule1;
    qi::rule<Iterator, qi::blank_type > subLineRule2;
    
    // hold the pointer that we are going to bind to
    DetData* ptr;

    struct DetType_ : qi::symbols<char, DetType>
    {
        DetType_(){
            this->add("NaI",    DetType::NaI)
                    ("LS",      DetType::LS)
                    ("CeBr3",   DetType::CeBr3)
                    ("HeMod",   DetType::HeMod)
                    ("HeUnmod", DetType::HeUnmod);
        }
    } detType_;
    
};

}
}
#endif //ORCHIDREADER_SRC_CONFIG_DETDATAPARSER_H
