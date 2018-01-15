/***************************************************************************//**
********************************************************************************
**
** @file DppPsdIntegralEvent.h
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
** @details Holds the definition of the class for DPP-PSD events without
** waveforms
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHIDREADER_SRC_EVENTS_DPPPSDINTEGRALEVENT_H
#define ORCHIDREADER_SRC_EVENTS_DPPPSDINTEGRALEVENT_H
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHIDReader

namespace Events
{

class DppPsdIntegralEvent
{
public:
    DppPsdIntegralEvent(){}
    DppPsdIntegralEvent(const DppPsdIntegralEvent& rhs);
    
    DppPsdIntegralEvent& operator=(const DppPsdIntegralEvent& rhs);
    bool operator<(const DppPsdIntegralEvent &rhs){return this->timeStamp<rhs.timeStamp;}
    
    int readEvent(char* buffer, unsigned long long approximateTime);
    
    unsigned long long appxTime=0ULL;
    unsigned char boardNumber=0;
    unsigned char channelNumber=0;
    unsigned long long timeStamp=0ULL;
    unsigned short longIntegral=0;
    unsigned short shortIntegral=0;
    unsigned char flags=0;//bit[0] = PUR flag, bit[1] = over-range, bit[2] = trigger lost
};

}

#endif //ORCHIDREADER_SRC_EVENTS_DPPPSDINTEGRALEVENT_H
