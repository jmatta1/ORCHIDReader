/***************************************************************************//**
********************************************************************************
**
** @file SortingBuffer.h
** @author James Till Matta
** @date 03 Feb, 2017
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the definition of the class stores long arrays of events so
** they can be sorted in time order
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHIDREADER_SRC_INPUT_SORTINGBUFFER_H
#define ORCHIDREADER_SRC_INPUT_SORTINGBUFFER_H

// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHIDReader
#include"Events/DppPsdIntegralEvent.h"
#include"Events/OrchidSlowControlsEvent.h"
#include"Events/NewFileEvent.h"


namespace Input 
{
namespace InputInternal
{

class SortingBuffer
{
public:
    SortingBuffer(int chBuffSize);
private:
    
};

}
}

#endif
