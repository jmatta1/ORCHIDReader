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
#include"TParameter.h"
#include"TFile.h"
#include"TTree.h"
#include"TH1D.h"
#include"TH2F.h"
// includes from ORCHIDReader
#include"Events/OrchidSlowControlsEvent.h"
#include"Events/DppPsdIntegralEvent.h"
#include"Output/RunData.h"
#include"Config/DetData.h"
#include"Config/ConfigData.h"

namespace Output
{

using std::ostringstream;
static const int NumEnChannels = 8192;
static const int EnPerBin = 65536/8192;
static const int NumPsdChannels = 1024;
static const double PsdPerBin = 1.0f/1024.0f;

RootOutput::RootOutput(InputParser::ConfigData* cData, InputParser::DetData* dData):
    confData(cData), detData(dData), treeData(dData->detectorNum.size())
{
    //create the file
    outfile = new TFile(cData->rootFilePath.c_str(),"RECREATE");
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
    firstDetEventOfRun = new bool[numDetectors];
    runStartTimeStamp = new unsigned long long[numDetectors];
    lastTimeStamp = new unsigned long long[numDetectors];
    for(int i=0; i<numDetectors; ++i)
    {
        detRun2DHists[i] = nullptr;
        detSum2DHists[i] = nullptr;
        enProjWithCutoff[i] = nullptr;
        enProjWithoutCutoff[i] = nullptr;
        psdProjWithCutoff[i] = nullptr;
        psdProjWithoutCutoff[i] = nullptr;
        eventTimeHists[i] = nullptr;
        enProjWithCutoffSum[i] = nullptr;
        enProjWithoutCutoffSum[i] = nullptr;
        psdProjWithCutoffSum[i] = nullptr;
        psdProjWithoutCutoffSum[i] = nullptr;
        firstDetEventOfRun[i] = true;
        runStartTimeStamp[i] = 0ULL;
        lastTimeStamp[i] = 0ULL;
    }
    this->initSums();
    //prepare the root tree
    this->prepTree();
    //write constants for the batch out to the file
    outfile->cd();
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
    // store the per detector info in a TTree
    TTree* detTree = new TTree("detData", "Detector Data Tree");
    int detNum, dBrdNum, dChnNum, mBrdNum, mChnNum, detType;
    batchTree->Branch("detNum", &detNum,"detNum/I");
    batchTree->Branch("digiBrdNum", &dBrdNum,"digiBrdNum/I");
    batchTree->Branch("digiChanNum", &dChnNum,"digiChanNum/I");
    batchTree->Branch("mpodBrdNum", &mBrdNum,"mpodBrdNum/I");
    batchTree->Branch("mpodChanNum", &mChnNum,"mpodChanNum/I");
    batchTree->Branch("detType", &detType,"detType/I");
    float detXPos, detYPos, detZPos;
    batchTree->Branch("xpos", &detXPos,"xpos/I");
    batchTree->Branch("ypos", &detYPos,"ypos/I");
    batchTree->Branch("zpos", &detZPos,"zpos/I");
    outfile->cd();
    for(int i=0; i<numDetectors; ++i)
    {
        detNum = detData->detectorNum[i];
        dBrdNum = detData->digiBoardNum[i];
        dChnNum = detData->digiChanNum[i];
        mBrdNum = detData->mpodBoardNum[i];
        mChnNum = detData->mpodChanNum[i];
        detXPos = (detData->detXOffset[i] + confData->arrayXPos);
        detYPos = (detData->detYOffset[i] + confData->arrayYPos);
        detZPos = detData->detZOffset[i];
        switch(detData->detType[i])
        {
        case InputParser::DetType::NaI:
            detType = 0;
            break;
        case InputParser::DetType::LS:
            detType = 1;
            break;
        case InputParser::DetType::HeMod:
            detType = 2;
            break;
        case InputParser::DetType::HeUnmod:
            detType = 3;
            break;
        case InputParser::DetType::CeBr3:
            detType = 4;
            break;
        default:
            detType = 50;
            break;
        }
        outfile->cd();
        detTree->Fill();
    }
    outfile->cd();
    detTree->Write();
    outfile->Flush();
    delete detTree;
    maxTimeEdge = (1.1f*confData->histIntegrationTime/0.001f);
    numTimeBin = static_cast<int>(maxTimeEdge/100.0f);
}

void RootOutput::initSums()
{
    for(int i=0; i<numDetectors; ++i)
    {
        ostringstream histNamer;
        histNamer << "Det_" << detData->detectorNum[i] << "_Sum_2D";
        detSum2DHists[i] = new TH2F(histNamer.str().c_str(),"PSD Vs lGate Batch Sum",NumEnChannels,0,65536,NumPsdChannels,0.0,1.0);
        histNamer.str("");
        histNamer.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_Sum_px_thresh";
        enProjWithCutoffSum[i] = new TH1D(histNamer.str().c_str(),"lGate Projection With Cutoff Batch Sum",NumEnChannels,0,65536);
        histNamer.str("");
        histNamer.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_Sum_px";
        enProjWithoutCutoffSum[i] = new TH1D(histNamer.str().c_str(),"lGate Projection Batch Sum",NumEnChannels,0,65536);
        histNamer.str("");
        histNamer.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_Sum_py_thresh";
        psdProjWithCutoffSum[i] = new TH1D(histNamer.str().c_str(),"PSD Projection With Cutoff Batch Sum",NumPsdChannels,0.0,1.0);
        histNamer.str("");
        histNamer.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_Sum_py";
        psdProjWithoutCutoffSum[i] = new TH1D(histNamer.str().c_str(),"PSD Projection Batch Sum",NumPsdChannels,0.0,1.0);
    }
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
    batchTree->Branch("rawCounts", treeData.rawCounts, leafNamer.str().c_str());
    leafNamer.str("");
    leafNamer.clear();
    leafNamer << "rawRates["<<numDetectors<<"]/D";
    batchTree->Branch("rawRates", treeData.rawRates, leafNamer.str().c_str());
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
    delete[] firstDetEventOfRun;
    delete[] runStartTimeStamp;
    delete[] lastTimeStamp;
    
    //since done was called the batch tree was written
    delete batchTree;
    //since done was called the outfile was flushed
    delete outfile;
    //we do not own confData and detData, so don't try to delete them
}

void RootOutput::slowControlsEvent(const Events::OrchidSlowControlsEvent& event)
{//we do nothing with slow controls events, for now everything we want is handled 1 level up
    return;
}

void RootOutput::dppPsdIntegralEvent(const Events::DppPsdIntegralEvent& event)
{
    int detNum = detData->digiToDet(event.boardNumber,event.channelNumber);
    int detInd = detData->detToInd(detNum);
    //handle the first event of the detector in question
    if(firstDetEventOfRun[detInd])
    {
        firstDetEventOfRun[detInd] = false;
        runStartTimeStamp[detInd] = event.timeStamp;
        
    }
    //now proceed to handle event normally
    lastTimeStamp[detInd] = event.timeStamp;
    double longGate = static_cast<double>(event.longIntegral);
    double psd = ((longGate-static_cast<double>(event.shortIntegral))/longGate);
    int enBin = 1 + static_cast<int>(longGate/EnPerBin);
    int psdBin = 0;
    if (event.longIntegral == 0 || psd>1.0)
    {   psdBin = 1025;}
    else if(psd<0)
    {   psdBin = 0;}
    else
    {   psdBin = 1 + static_cast<int>(psd/PsdPerBin);}
    int globalBin = detRun2DHists[detInd]->GetBin(enBin,psdBin);
    //detRun2DHists[detInd]->Fill(longGate,psd);
    detRun2DHists[detInd]->AddBinContent(globalBin, 1.0);
    //enProjWithoutCutoff[detInd]->Fill(longGate);
    enProjWithoutCutoff[detInd]->AddBinContent(enBin);
    //psdProjWithoutCutoff[detInd]->Fill(psd);
    psdProjWithoutCutoff[detInd]->AddBinContent(psdBin);
    if(longGate < detData->psdProjEnThresh[detInd])
    {
        //psdProjWithCutoff[detInd]->Fill(psd);
        psdProjWithCutoff[detInd]->AddBinContent(psdBin);
    }
    if(psd < detData->enProjPsdThresh[detInd])
    {
        //enProjWithCutoff[detInd]->Fill(longGate);
        enProjWithCutoff[detInd]->AddBinContent(enBin);
    }
    if(event.timeStamp >= runStartTimeStamp[detInd])
    {
        double evTimeMs = static_cast<double>(event.timeStamp - runStartTimeStamp[detInd])/500000.0;
        int timeBin = 1 + static_cast<int>(evTimeMs/100);
        //eventTimeHists[detInd]->Fill(evTimeMs);
        eventTimeHists[detInd]->AddBinContent(timeBin, 1.0);
    }
}

void RootOutput::newRun(int runNum, unsigned long long startT)
{
    runNumber = runNum;
    startTime = startT;
    initRun();
}

void RootOutput::initRun()
{
    unsigned long long stSeconds = (startTime/1000000);
    for(int i=0; i<numDetectors; ++i)
    {
        ostringstream histNamer;
        ostringstream histTitler;
        histNamer << "Det_" << detData->detectorNum[i] << "_Run_" << runNumber << "_2D";
        histTitler << "Det. " << detData->detectorNum[i] << " PSD Vs lGate Starting @ " << stSeconds;
        detRun2DHists[i] = new TH2F(histNamer.str().c_str(), histTitler.str().c_str(),
                                    NumEnChannels, 0, 65536, NumPsdChannels, 0.0, 1.0);
        
        histNamer.str(""); histNamer.clear();
        histTitler.str(""); histTitler.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_Run_" << runNumber << "_px_thresh";
        histTitler << "Det. " << detData->detectorNum[i] << " lGate Projection With Cutoff Starting @ " << stSeconds;
        enProjWithCutoff[i] = new TH1D(histNamer.str().c_str(), histTitler.str().c_str(),
                                       NumEnChannels, 0, 65536);
        
        
        histNamer.str(""); histNamer.clear();
        histTitler.str(""); histTitler.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_Run_" << runNumber << "_px";
        histTitler << "Det. " << detData->detectorNum[i] << " lGate Projection Starting @ " << stSeconds;
        enProjWithoutCutoff[i] = new TH1D(histNamer.str().c_str(), histTitler.str().c_str(),
                                          NumEnChannels, 0, 65536);
        
        
        histNamer.str(""); histNamer.clear();
        histTitler.str(""); histTitler.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_Run_" << runNumber << "_py_thresh";
        histTitler << "Det. " << detData->detectorNum[i] << " PSD Projection With Cutoff Starting @ " << stSeconds;
        psdProjWithCutoff[i] = new TH1D(histNamer.str().c_str(), histTitler.str().c_str(),
                                        NumPsdChannels, 0.0, 1.0);
        
        
        histNamer.str(""); histNamer.clear();
        histTitler.str(""); histTitler.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_Run_" << runNumber << "_py";
        histTitler << "Det. " << detData->detectorNum[i] << " PSD Projection Starting @ " << stSeconds;
        psdProjWithoutCutoff[i] = new TH1D(histNamer.str().c_str(), histTitler.str().c_str(),
                                           NumPsdChannels, 0.0, 1.0);
        
        
        histNamer.str(""); histNamer.clear();
        histTitler.str(""); histTitler.clear();
        histNamer << "Det_" << detData->detectorNum[i] << "_Run_" << runNumber << "_times";
        histTitler << "Det. " << detData->detectorNum[i] << " EventTimes Starting @ " << stSeconds;
        eventTimeHists[i] = new TH1D(histNamer.str().c_str(), histTitler.str().c_str(),
                                     numTimeBin, 0.0, maxTimeEdge);
        
        this->firstDetEventOfRun[i] = true;
        this->runStartTimeStamp[i] = 0ULL;
        this->lastTimeStamp[i] = 0ULL;
    }
}

void RootOutput::endRun(const RunData& runData)
{
    //pack the treeData struct with the run data
    treeData.runNumber = runData.runNumber;
    treeData.startTime = runData.startTime;
    treeData.stopTime = runData.stopTime;
    treeData.centerTime = runData.centerTime;
    treeData.runTime = runData.runTime;
    treeData.numDetectors = runData.numDetectors;
    for(int i=0; i<treeData.numDetectors; ++i)
    {
        treeData.detNum[i] = runData.detNum[i];
        treeData.avgChanVolt[i] = runData.avgChanVolt[i];
        treeData.avgChanCurrent[i] = runData.avgChanCurrent[i];
        treeData.avgHVChanTemp[i] = runData.avgHVChanTemp[i];
        treeData.rawCounts[i] = runData.rawCounts[i];
        treeData.rawRates[i] = runData.rawRates[i];
    }
    //make sure the output file is set
    outfile->cd();
    //dump this tree entry
    batchTree->Fill();
    for(int i=0; i<numDetectors; ++i)
    {
        //setup the per run histogram statistics
        detRun2DHists[i]->ResetStats();
        enProjWithCutoff[i]->ResetStats();
        enProjWithoutCutoff[i]->ResetStats();
        psdProjWithCutoff[i]->ResetStats();
        psdProjWithoutCutoff[i]->ResetStats();
        eventTimeHists[i]->ResetStats();
        
        //add the per run histograms to the sum histograms
        detSum2DHists[i]->Add(detRun2DHists[i],1.0);
        enProjWithCutoffSum[i]->Add(enProjWithCutoff[i],1.0);
        enProjWithoutCutoffSum[i]->Add(enProjWithoutCutoff[i],1.0);
        psdProjWithCutoffSum[i]->Add(psdProjWithCutoff[i],1.0);
        psdProjWithoutCutoffSum[i]->Add(psdProjWithoutCutoff[i],1.0);
        
        //write the per run histograms
        outfile->cd();
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
    //flush the file to make sure hists are sent to disk
    outfile->Flush();
}

void RootOutput::done()
{
    //close out the sum spectra
    this->closeSums();
    //now ensure the batch tree is written
    outfile->cd();
    batchTree->Write();
    outfile->Flush();
    //now we are done
}

void RootOutput::closeSums()
{
    outfile->cd();
    for(int i=0; i<numDetectors; ++i)
    {
        //write the per run histograms
        outfile->cd();
        detSum2DHists[i]->Write();
        enProjWithCutoffSum[i]->Write();
        enProjWithoutCutoffSum[i]->Write();
        psdProjWithCutoffSum[i]->Write();
        psdProjWithoutCutoffSum[i]->Write();
        //delete the per run histograms
        delete detSum2DHists[i];
        delete enProjWithCutoffSum[i];
        delete enProjWithoutCutoffSum[i];
        delete psdProjWithCutoffSum[i];
        delete psdProjWithoutCutoffSum[i];
    }
    //flush the file to make sure hists are sent to disk
    outfile->Flush();
}

}
