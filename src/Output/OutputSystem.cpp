/***************************************************************************//**
********************************************************************************
**
** @file OutputSystem.cpp
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
** @details Holds the implementation of the class which initially accepts events
** from the file reader and passes those events to the list of interface
** subclasses for them to process how they wish
**
********************************************************************************
*******************************************************************************/
#include"Output/OutputSystem.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHIDReader

namespace Output
{

static const unsigned long long TSRollOverHiCkPoint = 125000000000000ULL;
static const unsigned long long TSRollOverHiThreshold = 125060000000000ULL;
static const unsigned long long TSRollOverLoThreshold = 90000000000ULL;
static const unsigned long long TSRollOverOffset = 140737488355328ULL;


OutputSystem::OutputSystem(InputParser::ConfigData *cData,
                           InputParser::DetData *dData):
    runData(dData->detectorNum.size()), confData(cData), detData(dData)
{
    for(int i=0; i<runData.numDetectors; ++i)
    {
        runData.detNum[i] = detData->detectorNum[i];
    }
}

void OutputSystem::newFileEvent(Events::NewFileEvent& event)
{
    if(firstEvent)
    {
        if(!event.firstFile)
        {
            std::cout<<"First file not marked for first event"<<std::endl;
            throw std::runtime_error("FirstEvent was not marked firstFile");
        }
        runData.resetCounters();
        runData.startTime = event.newFileFirstBufferStartTime;
        runData.runNumber = 0;
        assignRunTargetEnd(runData.startTime);
        firstEvent = false;
        lastEventTime = event.newFileFirstBufferStartTime;
        std::cout << "Starting run: " << runData.runNumber << " which starts at: " << runData.startTime << std::endl;
        for(auto&& x: outputs) x->newRun(runData.runNumber, runData.startTime);
    }
    else
    {
        //check if there is a weird mistake of some sort
        if(event.firstFile)
        {
            std::cout << "First file marked for non first event" << std::endl;
            throw std::runtime_error("firstFile marked for non firstEvent");
        }
        //otherwise, check if we are ending a run
        if(event.newFileFirstBufferStartTime > targetRunEndTime || !(event.sameRun))
        {//close out the run
            runData.stopTime = event.previousFileLastBufferEndTime;
            runData.closeRun(slowControlsCount);
            std::cout << "  Ending run: " << runData.runNumber << std::endl;
            for(auto&& x: outputs) x->endRun(&runData);
            slowControlsCount = 0;
            runData.resetCounters();
            runData.startTime = event.newFileFirstBufferStartTime;
            runData.runNumber += 1;
            assignRunTargetEnd(runData.startTime);
            std::cout << "Starting run: " << runData.runNumber << " which starts at: " << runData.startTime << std::endl;
            for(auto&& x: outputs) x->newRun(runData.runNumber, runData.startTime);
        }
        lastEventTime = event.newFileFirstBufferStartTime;
        //also handle if a new run occurred, if it did then the timestamps reset
        //and the rollover system needs to be reset
        if(!event.sameRun)
        {
            primeRollOverStage2 = false;
            currRollOverOffset = 0ULL;
            lastRollOverOffset = 0ULL;
        }
    }
}

void OutputSystem::slowControlsEvent(Events::OrchidSlowControlsEvent& event)
{
    //check for a mistake of some sort
    if(firstEvent)
    {
        std::cout << "First event should be new file event, not slow ctrl" << std::endl;
        throw std::runtime_error("FirstEvent was not a NewFileEvent");
    }
    //otherwise check if we need to handle generating a new run
    if(event.appxTime > targetRunEndTime)
    {
        runData.stopTime = lastEventTime;
        runData.closeRun(slowControlsCount);
        std::cout << "  Ending run: " << runData.runNumber << std::endl;
        for(auto&& x: outputs) x->endRun(&runData);
        slowControlsCount = 0;
        runData.resetCounters();
        runData.startTime = event.appxTime;
        runData.runNumber += 1;
        assignRunTargetEnd(runData.startTime);
        std::cout << "Starting run: " << runData.runNumber << " which starts at: " << runData.startTime << std::endl;
        for(auto&& x: outputs) x->newRun(runData.runNumber, runData.startTime);
    }
    for(int i=0; i<runData.numDetectors; ++i)
    {
        int detNum = detData->mpodToDet(event.boardNumber[i],event.channelNumber[i]);
        int detInd = detData->detToInd(detNum);
        runData.avgChanCurrent[detInd] += event.current[i];
        runData.avgChanVolt[detInd] += event.terminalVoltage[i];
        runData.avgHVChanTemp[detInd] += event.temperature[i];
    }
    ++slowControlsCount;
    lastEventTime = event.appxTime;
    for(auto&& x: outputs) x->slowControlsEvent(event);
}

void OutputSystem::dppPsdIntegralEvent(Events::DppPsdIntegralEvent& event)
{
    //check for a mistake
    if(firstEvent)
    {
        std::cout << "First Event should be new file event, not dppPsd" << std::endl;
        throw std::runtime_error("FirstEvent was not a NewFileEvent");
    }
    //now check if we need to handle generating a new run
    if(event.appxTime > targetRunEndTime)
    {
        runData.stopTime = lastEventTime;
        runData.closeRun(slowControlsCount);
        std::cout << "  Ending run: " << runData.runNumber << std::endl;
        for(auto&& x: outputs) x->endRun(&runData);
        slowControlsCount = 0;
        runData.resetCounters();
        runData.startTime = event.appxTime;
        runData.runNumber += 1;
        assignRunTargetEnd(runData.startTime);
        std::cout << "Starting run: " << runData.runNumber << " which starts at: " << runData.startTime << std::endl;
        for(auto&& x: outputs) x->newRun(runData.runNumber, runData.startTime);
    }
    //now do the checks for handling time stamp rollovers in the face of
    //unordered timestamps
    //first handle seeing if see need to handle rollover offsets
    //check ts rollover stage 1, where currRollOverOffset gets set 1 rollover
    //higher than last rollover offset
    if(!primeRollOverStage2 && event.timeStamp > TSRollOverHiThreshold)
    {
        currRollOverOffset += TSRollOverOffset;
        primeRollOverStage2 = true;
    }
    //now handle rollover stage 2 where lastRollOverOffset is made equal to
    //currRollOverOffset
    if(primeRollOverStage2 && event.timeStamp > TSRollOverLoThreshold)
    {
        lastRollOverOffset = currRollOverOffset;
    }
    //now handle adding the correct offsets
    if(event.timeStamp < TSRollOverHiCkPoint)
    {
        event.timeStamp += currRollOverOffset;
    }
    else
    {
        event.timeStamp += lastRollOverOffset;
    }
    int detNum = detData->digiToDet(event.boardNumber, event.channelNumber);
    int detInd = detData->detToInd(detNum);
    runData.rawCounts[detInd] += 1;
    lastEventTime = event.appxTime;
    for(auto&& x: outputs) x->dppPsdIntegralEvent(event);
}

void OutputSystem::processingDone()
{
    if(firstEvent)
    {
        std::cout << "Calling processing done before any events were added" << std::endl;
        throw std::runtime_error("FirstEvent true in processingDone()");
    }
    runData.stopTime = lastEventTime;
    runData.closeRun(slowControlsCount);
    std::cout << "  Ending run: " << runData.runNumber << std::endl;
    for(auto&& x: outputs) x->endRun(&runData);
    std::cout << "Ending processing" << std::endl;
    for(auto&& x: outputs) x->done();
    std::cout << "Post processing done" << std::endl;
}

void OutputSystem::assignRunTargetEnd(unsigned long long start)
{
    unsigned long long timeDiff = static_cast<unsigned long long>(static_cast<double>(confData->histIntegrationTime) * 1000000.0);
    targetRunEndTime = (start + timeDiff);
}

}
