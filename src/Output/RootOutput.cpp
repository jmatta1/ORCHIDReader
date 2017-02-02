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
#include"root/TParameter.h"
#include"root/TVector.h"
#include"root/TList.h"
// includes from ORCHIDReader

namespace Output
{

static const int NumEnChannels = 8192;
static const int NumPsdChannels = 1024;

void BatchTreeData::clearData()
{
    runNumber = 0;
    startTime = 0;
    stopTime = 0;
    centerTime = 0;
    runTime = 0.0;
    numDetectors = 0;
    for(int i=0; i<32; ++i)
    {
        rawCounts[i] = 0;
        rawRates[i] = 0.0;
        avgChanVolt[i] = 0.0;
        avgChanCurrent[i] = 0.0;
        avgHVChanTemp[i] = 0.0;
    }
}

void BatchTreeData::setRates()
{
    for(int i=0; i<32; ++i)
    {
        rawRates[i] = static_cast<double>(rawCounts[i])/runTime;
    }
}

void BatchTreeData::setAverages(int numEntries)
{
    float entries = static_cast<float>(numEntries);
    for(int i=0; i<32; ++i)
    {
        avgChanVolt[i] /= entries;
        avgChanCurrent[i] /= entries;
        avgHVChanTemp[i] /= entries;
    }
}

RootOutput::RootOutput(InputParser::ConfigData* cData, InputParser::DetData* dData):
    confData(cData), detData(dData)
{
    //set up the first level of arrays
    numDetectors = detData->detectorNum.size();
    detRun2DHists = new TH2F*[numDetectors];
    detSum2DHists = new TH2F*[numDetectors];
    enProjWithCutoff = new TH1D*[numDetectors];
    enProjWithoutCutoff = new TH1D*[numDetectors];
    psdProjWithCutoff = new TH1D*[numDetectors];
    psdProjWithoutCutoff = new TH1D*[numDetectors];
    eventTimeHists = new TH1D*[numDetectors];
    enProjWithCutoffSum = new TH1D*[numDetectors];
    enProjWithoutCutoffSum = new TH1D*[numDetectors];
    psdProjWithCutoffSum = new TH1D*[numDetectors];
    psdProjWithoutCutoffSum = new TH1D*[numDetectors];
    //create the file
    outfile = new TFile(cData->rootFilePath.c_str(),"RECREATE");
    //prepare the root tree
    this->prepTree();
    //write constants for the batch out to the file
    TParameter<int>* tempInt = new TParameter<int>("numDet", numDetectors);
    tempInt->Write();
    TParameter<float>* arrayX = new TParameter<float>("arrayXPos", confData->arrayXPos);
    arrayX->Write();
    TParameter<float>* arrayY = new TParameter<float>("arrayYPos", confData->arrayYPos);
    arrayY->Write();
    outfile->Flush();
    delete tempInt;
    delete arrayX;
    delete arrayY;
    //detector numbers
    TVector<int>* detVec = new TVector<int>(numDetectors);
    TVector<int>* digiBrdVec = new TVector<int>(numDetectors);
    TVector<int>* digiChanVec = new TVector<int>(numDetectors);
    TVector<int>* mpodBrdVec = new TVector<int>(numDetectors);
    TVector<int>* mpodChanVec = new TVector<int>(numDetectors);
    TVector<float>* xPositions = new TVector<float>(numDetectors);
    TVector<float>* yPositions = new TVector<float>(numDetectors);
    TVector<float>* zPositions = new TVector<float>(numDetectors);
    TVector<int>* detTypeVec = new TVector<int>(numDetectors);
    for(int i=0; i<numDetectors; ++i)
    {
        detVec[i] = detData->detectorNum[i];
        digiBrdVec[i] = detData->digiBoardNum[i];
        digiChanVec[i] = detData->digiChanNum[i];
        mpodBrdVec[i] = detData->mpodBoardNum[i];
        mpodChanVec[i] = detData->mpodChanNum[i];
        xPositions[i] = (detData->detXOffset[i] + confData->arrayXPos);
        yPositions[i] = (detData->detYOffset[i] + confData->arrayYPos);
        zPositions[i] = detData->detZOffset[i];
        switch(detData->detType[i])
        {
        case InputParser::DetType::NaI:
            detTypeVec[i] = 0;
            break;
        case InputParser::DetType::LS:
            detTypeVec[i] = 1;
            break;
        case InputParser::DetType::HeMod:
            detTypeVec[i] = 2;
            break;
        case InputParser::DetType::HeUnmod:
            detTypeVec[i] = 3;
            break;
        case InputParser::DetType::CeBr3:
            detTypeVec[i] = 4;
            break;
        default:
            detTypeVec[i] = 50;
            break;
        }
    }
    detVec->Write("detNum");
    digiBrdVec->Write("digiBrdNum");
    digiChanVec->Write("digiChanNum");
    mpodBrdVec->Write("mpodBrdNum");
    mpodChanVec->Write("mpodChanNum");
    xPositions->Write("detXPos");
    yPositions->Write("detYPos");
    zPositions->Write("detZPos");
    detTypeVec->Write("detType");
    outfile->Flush();
    delete detVec;
    delete digiBrdVec;
    delete digiChanVec;
    delete mpodBrdVec;
    delete mpodChanVec;
    delete xPositions;
    delete yPositions;
    delete zPositions;
    delete detTypeVec;
    runNumber = 0;
    initted = false;
    maxTimeEdge = (1.1f*confData->histIntegrationTime/0.001f);
    numTimeBin = static_cast<int>(maxTimeEdge/100.0f);
}

RootOutput::~RootOutput()
{
    //since done was called before this, the first level of hist array should be
    //all that needs deleting wrt those
    delete[] detRun2DHists;
    delete[] detSum2DHists;
    delete[] enProjWithCutoff;
    delete[] enProjWithoutCutoff;
    delete[] psdProjWithCutoff;
    delete[] psdProjWithoutCutoff;
    delete[] eventTimeHists;
    delete[] enProjWithCutoffSum;
    delete[] enProjWithoutCutoffSum;
    delete[] psdProjWithCutoffSum;
    delete[] psdProjWithoutCutoffSum;
    
    //since done was called the batch tree was written
    delete batchTree;
    //since done was called the outfile was flushed
    delete outfile;
    //we do not own confData and detData, so don't try to delete them
}

void RootOutput::slowControlsEvent(const Events::OrchidSlowControlsEvent& event)
{
    if(firstEvent || event.appxTime > runEndEpochTarget)
    {//drop slow controlls events that occur between runs, they should be a tiny
        //minority and would otherwise be a pain to take care of
        return;
    }
    //if we are here the event is during an active run
    for(int i=0; i<event.numVoltageChannels; ++i)
    {
        int detNum = detData->mpodToDet(event.boardNumber[i],event.channelNumber[i]);
        int detInd = detData->detToInd(detNum);
        treeData.avgChanVolt[detInd] += event.terminalVoltage[i];
        treeData.avgChanCurrent[detInd] += event.current[i];
        treeData.avgHVChanTemp[detInd] += event.temperature[i];
    }
    ++numSlowCtrls;
}

void RootOutput::dppPsdIntegralEvent(const Events::DppPsdIntegralEvent& event)
{
    if(firstEvent)
    {
        this->initRun();
        firstEvent = false;
    }
}

void RootOutput::inputFileSwitch(const Events::InputFileSwapEvent& event)
{
    
}

void RootOutput::done()
{
    
}

void RootOutput::initRun()
{
    numSlowCtrls = 0;
    ++runNumber;
    for(int i=0; i<numDetectors; ++i)
    {
        ostringstream histNamer;
        histNamer << "Det_" << detData->detectorNum[i] << "_Run_" << runNumber << "_2D";
        detRun2DHists[i] = new TH2F(histNamer.str().c_str(),"PSD Vs ADC",NumEnChannels,0,65536,NumPsdChannels,0.0,1.0);
        histNamer.str("");
        histNamer.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_Run_" << runNumber << "_px_thresh";
        enProjWithCutoff[i] = new TH1D(histNamer.str().c_str(),"Energy Projection With Cutoff",NumEnChannels,0,65536);
        histNamer.str("");
        histNamer.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_Run_" << runNumber << "_px";
        enProjWithoutCutoff[i] = new TH1D(histNamer.str().c_str(),"Energy Projection",NumEnChannels,0,65536);
        histNamer.str("");
        histNamer.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_Run_" << runNumber << "_py_thresh";
        psdProjWithCutoff[i] = new TH1D(histNamer.str().c_str(),"PSD Projection With Cutoff",NumPsdChannels,0.0,1.0);
        histNamer.str("");
        histNamer.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_Run_" << runNumber << "_py";
        psdProjWithoutCutoff[i] = new TH1D(histNamer.str().c_str(),"PSD Projection",NumPsdChannels,0.0,1.0);
        histNamer.str("");
        histNamer.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_Run_" << runNumber << "_times";
        eventTimeHists[i] = new TH1D(histNamer.str().c_str(),"EventTimes",numTimeBin,0.0,maxTimeEdge);
        runStartTimeStamp[i] = 0;
        lastTimeStamp[i] = 0;
    }
}

void RootOutput::closeRun()
{
    outfile->cd();
    //now make prep the runs entry into the tree
    this->prepTreeEntry();
    batchTree->Fill();
    for(int i=0; i<numDetectors; ++i)
    {
        //write the per run histograms
        detRun2DHists[i]->Write();
        enProjWithCutoff[i]->Write();
        enProjWithoutCutoff[i]->Write();
        psdProjWithCutoff[i]->Write();
        psdProjWithoutCutoff[i]->Write();
        eventTimeHists[i]->Write();
        //delete the per run histograms
        delete detRun2DHists[i];
        delete enProjWithCutoff[i];
        delete enProjWithoutCutoff[i];
        delete psdProjWithCutoff[i];
        delete psdProjWithoutCutoff[i];
        delete eventTimeHists[i];
    }
    treeData.clearData();
    //flush the file to make sure hists are sent to disk
    outfile->Flush();
}

void RootOutput::prepTree()
{
    //create the tree
    batchTree = new TTree("BatchTree","Per Run Data");
    //now create the scalar branches
    batchTree->Branch("runN", &treeData.runNumber,"runN/I");
    batchTree->Branch("startT", &treeData.startTime,"startT/l");
    batchTree->Branch("stopT", &treeData.stopTime,"stopT/l");
    batchTree->Branch("centerT", &treeData.centerTime,"centerT/l");
    batchTree->Branch("runT", &treeData.runTime,"runT/D");
    //now create the vector branches
    std::ostringstream leafNamer;
    leafNamer << "roughDtCorr["<<numDetectors<<"]/F";
    batchTree->Branch("roughDtCorr", treeData.roughCorrection, leafNamer.str().c_str());
    leafNamer.str("");
    leafNamer.clear();
    leafNamer << "avgChanVolt["<<numDetectors<<"]/F";
    batchTree->Branch("avgChanVolt", treeData.avgChanVolt, leafNamer.str().c_str());
    leafNamer.str("");
    leafNamer.clear();
    leafNamer << "avgChanCurr["<<numDetectors<<"]/F";
    batchTree->Branch("avgChanCurr", treeData.avgChanCurrent, leafNamer.str().c_str());
    leafNamer.str("");
    leafNamer.clear();
    leafNamer << "avgHVChanTemp["<<numDetectors<<"]/F";
    batchTree->Branch("avgHVChanTemp", treeData.avgHVChanTemp, leafNamer.str().c_str());
    leafNamer.str("");
    leafNamer.clear();
    leafNamer << "rawCounts["<<numDetectors<<"]/l";
    batchTree->Branch("rawCounts", treeData.avgHVChanTemp, leafNamer.str().c_str());
    leafNamer.str("");
    leafNamer.clear();
    leafNamer << "rawRates["<<numDetectors<<"]/D";
    batchTree->Branch("rawRates", treeData.avgHVChanTemp, leafNamer.str().c_str());
}

void RootOutput::prepTreeEntry()
{
    treeData.runNumber = runNumber;
    treeData.startTime = runStartEpoch;
    treeData.stopTime = lastEpoch;
    treeData.centerTime = (treeData.stopTime + treeData.startTime)/2000000;
    treeData.runTime = ((static_cast<double>(treeData.stopTime) - static_cast<double>(treeData.startTime))/1000000.0);
    treeData.setAverages(numSlowCtrls);
    treeData.setRates();
    this->doRoughDtCorrections();
}

void RootOutput::doRoughDtCorrections()
{
    for(int i=0; i< numDetectors; ++i)
    {
        //from the first bin of the time spectra to the last written bin, find the number of empty bins
        if(lastTimeStamp[i] < runStartTimeStamp[i])
        {
            lastTimeStamp[i] += 140737488355328ULL;
        }
        int lastBin = eventTimeHists[i]->FindBin((lastTimeStamp[i]-runStartTimeStamp[i])/(500000));
        float emptyBins = 0;
        float fullBins = 0;
        for(int j=1; j<lastBin; ++j)
        {
            double binContent = eventTimeHists[i]->GetBinContent(j);
            if(binContent < 1.0)
            {
                emptyBins += 1.0f;
            }
            else
            {
                fullBins += 1.0f;
            }
        }
        treeData.roughCorrection[i] = (fullBins/(fullBins+emptyBins));
    }
}

}
