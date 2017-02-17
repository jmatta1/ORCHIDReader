/***************************************************************************//**
********************************************************************************
**
** @file OutputInterface.h
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
** @details Holds the abstract base class that all output systems must follow
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHIDREADER_SRC_OUTPUT_OUTPUTINTERFACE_H
#define ORCHIDREADER_SRC_OUTPUT_OUTPUTINTERFACE_H
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHIDReader
#include"Events/OrchidSlowControlsEvent.h"
#include"Events/DppPsdIntegralEvent.h"
#include"Events/NewFileEvent.h"
#include"Output/RunData.h"

namespace Output
{

class OutputInterface
{
public:
    virtual ~OutputInterface(){}
    
    // processing for individual events
    virtual void slowControlsEvent(const Events::OrchidSlowControlsEvent& event) = 0;
    virtual void dppPsdIntegralEvent(const Events::DppPsdIntegralEvent& event) = 0;
    
    //larger scale processing
    virtual void newRun(int runNum, unsigned long long startT) = 0;
    virtual void endRun(RunData* runData) = 0;
    virtual void done() = 0;
};

}

#endif //ORCHIDREADER_SRC_OUTPUT_OUTPUTINTERFACE_H
