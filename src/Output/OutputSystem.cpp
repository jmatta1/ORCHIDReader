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
** from the file reader and passes those events to the list of subclasses for
** them to process how they wish
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

RunData::RunData(int numDet):numDetectors(numDet)
{
    detNum = new int[numDetectors];
    roughCorrection = new float[numDetectors];
    avgChanVolt = new float[numDetectors];
    avgChanCurrent = new float[numDetectors];
    avgHVChanTemp = new int[numDetectors];
    rawCounts = new unsigned long long[numDetectors];
    rawRates = new double[numDetectors];
}

RunData::~RunData()
{
    delete[] detNum;
    delete[] roughCorrection;
    delete[] avgChanVolt;
    delete[] avgChanCurrent;
    delete[] avgHVChanTemp;
    delete[] rawCounts;
    delete[] rawRates;
}

void OutputSystem::addOutputClass(std::unique_ptr<OutputInterface>&& outputter)
{
    outputs.push_back(std::forward<std::unique_ptr<OutputInterface>>(outputter));
}

void OutputSystem::processingDone()
{
    for(auto&& x: outputs) x->done();
}

void OutputSystem::passSlowControlsEvent(const Events::OrchidSlowControlsEvent& event)
{
    for(auto&& x: outputs) x->slowControlsEvent(event);
}

void OutputSystem::passDppPsdIntegralEvent(const Events::DppPsdIntegralEvent& event)
{
    for(auto&& x: outputs) x->dppPsdIntegralEvent(event);
}

}
