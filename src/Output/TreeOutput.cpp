/***************************************************************************//**
********************************************************************************
**
** @file TreeOutput.cpp
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
** @details Holds the implementation of the system for root tree output
**
********************************************************************************
*******************************************************************************/
#include"TreeOutput.h"
// includes for C system headers
// includes for C++ system headers
// includes from other libraries
#include"TFile.h"
#include"TTree.h"
// includes from ORCHIDReader
#include"Events/OrchidSlowControlsEvent.h"
#include"Events/DppPsdIntegralEvent.h"
#include"Output/RunData.h"
#include"Config/DetData.h"
#include"Config/ConfigData.h"

namespace Output
{

TreeOutput::TreeOutput(InputParser::ConfigData* cData, InputParser::DetData* dData):
    confData(cData), detData(dData)
{
    //first open the root file where the tree will be dumped
    outfile = new TFile(confData->rootTreeFilePath.c_str(),"RECREATE");
    outfile->cd();
    eventTree = new TTree("EventTree","Event Tree");
    this->prepTree();
}

TreeOutput::~TreeOutput()
{
    //since the tree must be written and flushed by now, simply delete the tree
    //then delete the file
    delete eventTree;
    delete outfile;
}

void TreeOutput::slowControlsEvent(const Events::OrchidSlowControlsEvent& event)
{//slow controls events mean nothing to the tree (for now), ignore them
    return;
}

void TreeOutput::dppPsdIntegralEvent(const Events::DppPsdIntegralEvent& event)
{
    treeData.appxTime = event.appxTime;
    treeData.boardNumber = event.boardNumber;
    treeData.channelNumber = event.channelNumber;
    treeData.flags = event.flags;
    treeData.longIntegral = event.longIntegral;
    treeData.shortIntegral = event.shortIntegral;
    treeData.timeStamp = event.timeStamp;
    float headFrac = static_cast<float>(event.shortIntegral)/static_cast<float>(event.longIntegral);
    treeData.psd = (1.0f - headFrac); //this gives you tail fraction
    treeData.detNum = detData->digiToDet(event.boardNumber, event.channelNumber);
    eventTree->Fill();
}

void TreeOutput::newRun(int runNum, unsigned long long startT)
{//runs mean nothing to the tree, ignore them
    return;
}

void TreeOutput::endRun(const RunData& runData)
{//runs mean nothing to the tree, ignore them
    return;
}

void TreeOutput::done()
{
    outfile->cd();
    eventTree->Write();
    outfile->Flush();
}

void TreeOutput::prepTree()
{
    eventTree->Branch("detNum",    &(treeData.detNum),       "detNum/S");
    eventTree->Branch("apprxTime", &(treeData.appxTime),     "apprxTime/l");
    eventTree->Branch("timeStamp", &(treeData.timeStamp),    "timeStamp/l");
    eventTree->Branch("brdNum",    &(treeData.boardNumber),  "brdNum/b");
    eventTree->Branch("chnNum",    &(treeData.channelNumber),"chnNum/b");
    eventTree->Branch("shrtInt",   &(treeData.shortIntegral),"shrtInt/s");
    eventTree->Branch("longInt",   &(treeData.longIntegral), "longInt/s");
    eventTree->Branch("psd",       &(treeData.psd),          "psd/F");
    eventTree->Branch("flags",     &(treeData.flags),        "psd/b");
}

}
