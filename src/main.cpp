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
#include<memory>
// includes from other libraries
// includes from ORCHIDReader
#include"Config/ParseFunctions.h"
#include"Config/ConfigData.h"
#include"Config/DetData.h"
#include"Output/RootOutput.h"
#include"Input/OrchidFileReader.h"

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
    InputParser::ConfigData* confData = new InputParser::ConfigData;

    bool configParseGood = InputParser::parseValAndPrintConfigFile(confData, configFileName, std::cout);
    if(!configParseGood)
    {
        std::cout<<"Failed in config reading\n"<<std::endl;
        return 1;
    }
    InputParser::DetData* detData = new InputParser::DetData;
    bool detDataParseGood = InputParser::parseValAndPrintDetDataFile(detData, confData->arrayDataPath, std::cout);
    if(!detDataParseGood)
    {
        std::cout<<"Failed in detector data reading\n"<<std::endl;
        return 1;
    }
    //3) Calculate the board and channel to detector number mappings
    detData->calculateMappings();

    //4) show the user what data we read in
    std::cout << "The configuration data read in is: \n" << *confData << "\n";
    std::cout << "The detector data read in is: \n" << *detData << "\n";
    
    //5) create the output system
    std::cout << "Creating base output system"<<std::endl;
    Output::OutputSystem* output = new Output::OutputSystem(confData, detData);
    //Output::RootOutput* rootOutputter= new Output::RootOutput(&confData, &detData);
    std::unique_ptr<Output::OutputInterface> rootOutput(new Output::RootOutput(confData, detData));
    
    output->addOutputClass(std::move(rootOutput));
    
    //6) create the ORCHID data reader
    Input::OrchidFileReader* orchidReader = new Input::OrchidFileReader(confData, detData->detectorNum.size());

    //7) run the processing loop
    orchidReader->processFiles(output);
    
    //8) finalize the spectra
    output->processingDone();
    //8) clean up objects
    delete orchidReader;
    delete output;
    delete detData;
    delete confData;
    
    return 0;
}
