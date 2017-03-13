/***************************************************************************//**
********************************************************************************
**
** @file CsvOutput.h
** @author James Till Matta
** @date 10 Mar, 2017
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
#include<chrono>
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
    virtual void endRun(const RunData& runData) final;
    virtual void done() final;
    
private:
    void writeEpochDateTimePairToStream(unsigned long long epochDuration, std::ostream& out);
    
    std::ofstream batchCsv;
    std::ofstream runDataFile;
    
    //store the total number of runs
    int numRuns=0;
    //store the last end of run time in us since epoch
    unsigned long long lastEndOfRun;
};

}
#endif //ORCHIDREADER_SRC_OUTPUT_CSVOUTPUT_H
