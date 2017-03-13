/***************************************************************************//**
********************************************************************************
**
** @file OrchidSlowControlsEvent.cpp
** @author James Till Matta
** @date 09 Feb, 2017
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the implementation of the class for ORCHID slow controls
** events
**
********************************************************************************
*******************************************************************************/
#include"OrchidSlowControlsEvent.h"
// includes for C system headers
// includes for C++ system headers
#include<iostream>
#include<iomanip>
// includes from other libraries
// includes from ORCHIDReader
#include"Utility/GetData.h"
namespace Events
{

OrchidSlowControlsEvent::OrchidSlowControlsEvent(int numChan)
{
    boardNumber = new int[numChan];
    channelNumber = new int[numChan];
    terminalVoltage = new float[numChan];
    senseVoltage = new float[numChan];
    setVoltage = new float[numChan];
    current = new float[numChan];
    rampUpRate = new float[numChan];
    rampDownRate = new float[numChan];
    maxCurrent = new float[numChan];
    maxVoltage = new float[numChan];
    currentTripTime = new int[numChan];
    temperature = new int[numChan];
    maxTemperature = new int[numChan];
    outputSwitch = new bool[numChan];
    channelStatus = new ChannelStatus[numChan];
}

OrchidSlowControlsEvent::~OrchidSlowControlsEvent()
{
    delete[] boardNumber;
    delete[] channelNumber;
    delete[] terminalVoltage;
    delete[] senseVoltage;
    delete[] setVoltage;
    delete[] current;
    delete[] rampUpRate;
    delete[] rampDownRate;
    delete[] maxCurrent;
    delete[] maxVoltage;
    delete[] currentTripTime;
    delete[] temperature;
    delete[] maxTemperature;
    delete[] outputSwitch;
    delete[] channelStatus;
}

int OrchidSlowControlsEvent::readEvent(char* buffer, unsigned long long approximateTime)
{
    appxTime = approximateTime;
    int ind = 0;
    //the first two bytes are the binary size
    short binarySize = getData<short>(buffer, ind);
    ind += 2;
    //the next byte is the event type, skip it
    ind += 2;
    //the next 4 bytes are the number of voltage channels
    numVoltageChannels = getData<int>(buffer, ind);
    ind += 4;
    //the next 4 bytes are the number of temperature channels
    numTemperatureChannels = getData<int>(buffer, ind);
    ind += 4;
    //the next 4 bytes are the representation of the crate status flags
    crateStatus.loadFromValue(getData<unsigned int>(buffer, ind));
    ind += 4;
    //now loop through the individual voltage channel information
    for(int i=0; i<numVoltageChannels && ind < binarySize; ++i)
    {
        //the first two bytes of channel information are the board number
        boardNumber[i] = getData<short>(buffer, ind);
        ind += 2;
        //the next two bytes of channel info is the channel number
        channelNumber[i] = getData<short>(buffer, ind);
        ind += 2;
        //the next 4 bytes are a float representation of the terminal voltage in volts
        terminalVoltage[i] = getData<float>(buffer, ind);
        ind += 4;
        //the next 4 bytes are a float representation of the sense voltage in volts
        senseVoltage[i] = getData<float>(buffer, ind);
        ind += 4;
        //the next 4 bytes are a float representation of the current in microamps
        current[i] = getData<float>(buffer, ind);
        ind += 4;
        //the next 4 bytes are an int representation of the temperature of the channel (in celsius)
        temperature[i] = getData<int>(buffer, ind);
        ind += 4;
        //the last 4 bytes are an int representation of the channel status
        channelStatus[i].loadFromValue(getData<unsigned int>(buffer, ind));
        ind += 4;
    }
    return binarySize;
}

}
