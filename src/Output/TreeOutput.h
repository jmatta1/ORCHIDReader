/***************************************************************************//**
********************************************************************************
**
** @file TreeOutput.h
** @author James Till Matta
** @date 14 Mar, 2017
** @brief
**
** @copyright Copyright (C) 2016 James Till Matta
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** @details Holds the definition of the system for root tree output
**
********************************************************************************
*******************************************************************************/
#ifndef ORCHIDREADER_SRC_OUTPUT_TREEOUTPUT_H
#define ORCHIDREADER_SRC_OUTPUT_TREEOUTPUT_H
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
// includes from ORCHIDReader
#include"Output/OutputInterface.h"

namespace InputParser
{
    class ConfigData;
    class DetData;
}

class TFile;
class TTree;

namespace Output
{

struct EventTreeData
{
    short detNum;
    unsigned long long appxTime;
    unsigned long long timeStamp;
    unsigned char boardNumber;
    unsigned char channelNumber;
    unsigned short shortIntegral;
    unsigned short longIntegral;
    float psd;
    unsigned char flags;
};

class TreeOutput : public OutputInterface
{
public:
    TreeOutput(InputParser::ConfigData* cData, InputParser::DetData* dData);
    virtual ~TreeOutput();
    
    virtual void slowControlsEvent(const Events::OrchidSlowControlsEvent& event) final;
    virtual void dppPsdIntegralEvent(const Events::DppPsdIntegralEvent& event) final;
    
    virtual void newRun(int runNum, unsigned long long startT) final;
    virtual void endRun(const RunData& runData) final;
    virtual void done() final;
    
private:
    //private member functions
    void prepTree();
    
    //member data
    //data output members
    TFile* outfile;
    //Tree pointer and struct
    TTree* eventTree;
    EventTreeData treeData;
    
    //config data
    InputParser::ConfigData* confData;
    InputParser::DetData* detData;
};

}

#endif //ORCHIDREADER_SRC_OUTPUT_TREEOUTPUT_H
