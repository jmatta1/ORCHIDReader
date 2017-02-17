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
#include"Output/OutputInterface.h"
#include"Output/RunData.h"
#include"Config/ConfigData.h"
#include"Config/DetData.h"
#include"Events/OrchidSlowControlsEvent.h"
#include"Events/DppPsdIntegralEvent.h"
#include"Events/NewFileEvent.h"

namespace Output
{

class OutputSystem
{
public:
    OutputSystem(InputParser::ConfigData* cData, InputParser::DetData* dData);
    //used in initial setup and finish
    void addOutputClass(std::unique_ptr<OutputInterface>&& outputter)
    {outputs.push_back(std::forward<std::unique_ptr<OutputInterface>>(outputter));}
    
    void processingDone();
    
    //for use by the output system
    void newFileEvent(Events::NewFileEvent& event);
    void slowControlsEvent(Events::OrchidSlowControlsEvent& event);
    void dppPsdIntegralEvent(Events::DppPsdIntegralEvent& event);
    
private:
    void assignRunTargetEnd(unsigned long long start);
    
    std::vector<std::unique_ptr<OutputInterface> > outputs;
    InputParser::ConfigData* confData;
    InputParser::DetData* detData;
    RunData runData;
    bool firstEvent=true;
    bool primeRollOverStage2 = false;
    unsigned long long currRollOverOffset = 0ULL;
    unsigned long long lastRollOverOffset = 0ULL;
    unsigned long long targetRunEndTime = 0ULL;
    unsigned long long lastEventTime = 0ULL;
    unsigned int slowControlsCount=0;
};

}

#endif //ORCHIDREADER_SRC_OUTPUT_OUTPUTSYSTEM_H
