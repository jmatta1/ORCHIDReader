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

void OutputSystem::addOutputClass(OutputInterface* outputter)
{
    outputs.push_back(outputter);
}

void OutputSystem::processingDone()
{
    for(auto x: outputs) x->done();
}

void OutputSystem::passSlowControlsEvent(const Events::OrchidSlowControlsEvent& event)
{
    for(auto x: outputs) x->slowControlsEvent(event);
}

void OutputSystem::passDppPsdIntegralEvent(const Events::DppPsdIntegralEvent& event)
{
    for(auto x: outputs) x->dppPsdIntegralEvent(event);
}

}
