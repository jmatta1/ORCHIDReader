/***************************************************************************//**
********************************************************************************
**
** @file RootOutput.cpp
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
** @details Holds the implementation of the system for root output
**
********************************************************************************
*******************************************************************************/
#include"RootOutput.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHIDReader

namespace Output
{

void BatchTreeData::clearRates()
{
    for(int i=0; i<numDetectors; ++i)
    {
        rawCounts[i] = 0;
        rawRates[i] = 0.0;
    }
}

void BatchTreeData::setRates()
{
    for(int i=0; i<numDetectors; ++i)
    {
        rawRates[i] = static_cast<double>(rawCounts[i])/runTime;
    }
}

}
