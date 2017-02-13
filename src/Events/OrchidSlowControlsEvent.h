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
** @details Holds the class for ORCHID slow controls events
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHIDREADER_SRC_EVENTS_ORCHIDSLOWCONTROLSEVENT_H
#define ORCHIDREADER_SRC_EVENTS_ORCHIDSLOWCONTROLSEVENT_H
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHIDReader
#include"Events/OrchidSlowControlsSupport.h"

namespace Events
{

class OrchidSlowControlsEvent
{
public:
    OrchidSlowControlsEvent(int numChan);
    ~OrchidSlowControlsEvent();
    int readEvent(char* buffer, unsigned long long approximateTime);
    unsigned long long appxTime;
    short numVoltageChannels = 0;
    short numTemperatureChannels = 0;
    int*    boardNumber;
    int*    channelNumber;
    float*  terminalVoltage;
    float*  senseVoltage;
    float*  setVoltage;
    float*  current;
    float*  rampUpRate;
    float*  rampDownRate;
    float*  maxCurrent;
    float*  maxVoltage;
    int*    currentTripTime;
    int*    temperature;
    int*    maxTemperature;
    bool*   outputSwitch;
    ChannelStatus*   channelStatus;
    CrateStatus      crateStatus;
};

}

#endif //ORCHIDREADER_SRC_EVENTS_ORCHIDSLOWCONTROLSEVENT_H
