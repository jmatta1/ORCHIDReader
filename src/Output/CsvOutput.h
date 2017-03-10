/***************************************************************************//**
********************************************************************************
**
** @file CsvOutput.h
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
** @details Holds the definition of the class which outputs batch and run data
** into csv files
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHIDREADER_SRC_OUTPUT_CSVOUTPUT_H
#define ORCHIDREADER_SRC_OUTPUT_CSVOUTPUT_H
// includes for C system headers
// includes for C++ system headers
#include<fstream>
// includes from other libraries
// includes from ORCHIDReader
#include"OutputInterface.h"

namespace InputParser
{
    class ConfigData;
    class DetData;
}

namespace Output
{

class CsvOutput : public OutputInterface
{
public:
    CsvOutput(InputParser::ConfigData* cData, InputParser::DetData* dData);
    virtual ~CsvOutput() final;

    // processing for individual events
    virtual void slowControlsEvent(const Events::OrchidSlowControlsEvent& event) final;
    virtual void dppPsdIntegralEvent(const Events::DppPsdIntegralEvent& event) final;
    
    //larger scale processing
    virtual void newRun(int runNum, unsigned long long startT) final;
    virtual void endRun(RunData* runData) final;
    virtual void done() final;
    
private:
    std::ofstream batchCsv;
    std::ofstream runDataFile;
    
    bool firstRun = true;
};

}
#endif //ORCHIDREADER_SRC_OUTPUT_CSVOUTPUT_H
