/***************************************************************************//**
********************************************************************************
**
** @file DppPsdIntegralEvent.cpp
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
** @details Holds the implementation of the class for DPP-PSD events without
** waveforms
**
********************************************************************************
*******************************************************************************/
#include"Events/DppPsdIntegralEvent.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHIDReader
#include"Utility/GetData.h"

namespace Events
{

int DppPsdIntegralEvent::readEvent(char* buffer, unsigned long long approximateTime)
{
    appxTime = approximateTime;
    //index into the buffer
    int ind = 0;
    //the first byte is the size which is 15, the second byte is the event id
    //which is 2, therefore we skip the first two bytes
    ind += 2;
    //the third byte is the board number
    boardNumber = getData<unsigned char>(buffer, ind);
    ++ind;
    //the fourth byte is the channel number
    channelNumber = getData<unsigned char>(buffer, ind);
    ++ind;
    //the next 4 bytes are bits [0,30] of the timestamp
    unsigned int loTime = getData<unsigned int>(buffer, ind);
    ind += 4;
    //the next 2 bytes are bits [31,47] of the timestamp
    unsigned short hiTime = getData<unsigned short>(buffer, ind);
    ind += 2;
    timeStamp = ((static_cast<unsigned long long>(hiTime)<<31) | static_cast<unsigned long long>(loTime));
    //the next 2 bytes are the 16 bit long integral
    longIntegral = getData<unsigned short>(buffer, ind);
    ind += 2;
    //the next 2 bytes are the 15 bit short integral
    shortIntegral = getData<unsigned short>(buffer, ind);
    ind += 2;
    //the last byte contains relevant flags
    flags = getData<unsigned char>(buffer, ind);
    //now we have read all 15 bytes of the event so return the size we read
    return 15;
}

}
