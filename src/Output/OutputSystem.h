/***************************************************************************//**
********************************************************************************
**
** @file OutputSystem.h
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
** @details Holds the definition of the class which initially accepts events
** from the file reader and passes those events to the list of subclasses for
** them to process how they wish
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHIDREADER_SRC_OUTPUT_OUTPUTSYSTEM_H
#define ORCHIDREADER_SRC_OUTPUT_OUTPUTSYSTEM_H
// includes for C system headers
// includes for C++ system headers
#include<vector>
#include<memory>
// includes from other libraries
// includes from ORCHIDReader
#include"OutputInterface.h"
#include"Config/ConfigData.h"
#include"Config/DetData.h"
#include"Events/OrchidSlowControlsEvent.h"
#include"Events/DppPsdIntegralEvent.h"
#include"Events/InputFileSwapEvent.h"

namespace Output
{

struct RunData
{
    RunData(int numDet);
    ~RunData();
    void resetCounters();
    
    int runNumber;
    unsigned long long startTime;
    unsigned long long stopTime;
    unsigned long long centerTime;
    double runTime;
    int numDetectors;
    float* roughCorrection;
    int* detNum;
    float* avgChanVolt;
    float* avgChanCurrent;
    float* avgHVChanTemp;
    unsigned long long* rawCounts;
    double* rawRates;
};

class OutputSystem
{
public:
    OutputSystem(InputParser::ConfigData* cData, InputParser::DetData* dData);
    //used in initial setup and finish
    void addOutputClass(std::unique_ptr<OutputInterface>&& outputter);
    void processingDone();
    
    //for use by the output system
    void passSlowControlsEvent(const Events::OrchidSlowControlsEvent& event);
    void passDppPsdIntegralEvent(const Events::DppPsdIntegralEvent& event);
    void passFileSwapEvent(const Events::InputFileSwapEvent& event);
    
private:
    std::vector<std::unique_ptr<OutputInterface> > outputs;
    bool firstEvent=true;
    
};

}

#endif //ORCHIDREADER_SRC_OUTPUT_OUTPUTSYSTEM_H
