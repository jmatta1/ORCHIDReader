/***************************************************************************//**
********************************************************************************
**
** @file OrchidSlowControlsEvent.h
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
** @details Holds the class for DPP-PSD events without waveforms
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHIDREADER_SRC_EVENTS_ORCHIDSLOWCONTROLSEVENT_H
#define ORCHIDREADER_SRC_EVENTS_ORCHIDSLOWCONTROLSEVENT_H
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHIDReader

namespace Events
{

class OrchidSlowControlsEvent
{
public:
    void readEvent(char* buffer, unsigned long long approximateTime);
    unsigned long long apprTime;
    unsigned char boardNumber;
    unsigned char channelNumber;
    unsigned long long timeStamp;
    unsigned short longIntegral;
    unsigned short shortIntegral;
    unsigned char flags;//bit[0] = PUR flag, bit[1] = over-range, bit[2] = trigger lost
};

}

#endif //ORCHIDREADER_SRC_EVENTS_ORCHIDSLOWCONTROLSEVENT_H
