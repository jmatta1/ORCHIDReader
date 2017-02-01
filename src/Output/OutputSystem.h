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
// includes from other libraries
// includes from ORCHIDReader
#include"OutputInterface.h"
#include"Events/OrchidSlowControlsEvent.h"
#include"Events/DppPsdIntegralEvent.h"

namespace Output
{

class OutputSystem
{
public:
    
    //used in initial setup and finish
    void addOutputClass(OutputInterface* outputter);
    void processingDone();
    
    //for use by the output system
    void passSlowControlsEvent(const Events::OrchidSlowControlsEvent& event);
    void passDppPsdIntegralEvent(const Events::DppPsdIntegralEvent& event);
    
private:
    std::vector<OutputInterface*> outputs;
};

}

#endif //ORCHIDREADER_SRC_OUTPUT_OUTPUTSYSTEM_H
