/***************************************************************************//**
********************************************************************************
**
** @file main.cpp
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
** @details Holds the implementation of the program's main function
**
********************************************************************************
*******************************************************************************/
// includes for C system headers
// includes for C++ system headers
#include<iostream>
#include<string>
// includes from other libraries
// includes from ORCHIDReader
#include"Config/ParseFunctions.h"
#include"Config/ConfigData.h"
#include"Config/DetData.h"

int main(int argc, char* argv[])
{
    //1) first make sure we were passed the appropriate number of arguments
    if(argc != 2)
    {
        std::cout<<"Usage:\n";
        std::cout<<"  "<<argv[0]<<" ConfigurationFile"<<std::endl;
        return 0;
    }
    
    //2) read in the configuration files
    std::cout<<"\n";
    std::string configFileName(argv[1]);
    InputParser::ConfigData confData;
    bool configParseGood = InputParser::parseValAndPrintConfigFile(&confData,
                                                                   configFileName,
                                                                   std::cout);
    if(!configParseGood)
    {
        std::cout<<"Failed in config reading\n"<<std::endl;
        return 1;
    }
    InputParser::DetData detData;
    bool detDataParseGood = InputParser::parseValAndPrintDetDataFile(&detData,
                                                                    confData.arrayDataPath,
                                                                    std::cout);
    if(!detDataParseGood)
    {
        std::cout<<"Failed in detector data reading\n"<<std::endl;
        return 1;
    }
    //3) create the ORCHID data reader
    
    //4) create the output system
    
    //5) Pull events from the reader and dump them to output system
    
    return 0;
}
