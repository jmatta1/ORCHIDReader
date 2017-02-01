/***************************************************************************//**
********************************************************************************
**
** @file main.cpp
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
** @details Holds the implementation of the program's main function
**
********************************************************************************
*******************************************************************************/
// includes for C system headers
// includes for C++ system headers
#include<iostream>
#include<string>
// includes from other libraries
#define BOOST_LOG_DYN_LINK 1
#include<boost/log/utility/setup.hpp>
#include<boost/log/utility/setup/file.hpp>
#include<boost/log/sinks/text_file_backend.hpp>
#include<boost/log/utility/setup/common_attributes.hpp>
#include<boost/core/null_deleter.hpp>
// includes from ORCHIDReader
#include"Config/ParseFunctions.h"
#include"Config/ConfigData.h"
#include"Config/DetData.h"
#include"Utility/OrchidReaderLogger.h"

int main(int argc, char* argv[])
{
    //1) first make sure we were passed the appropriate number of arguments
    if(argc != 2)
    {
        std::cout<<"Usage:\n";
        std::cout<<"  "<<argv[0]<<" ConfigurationFile"<<std::endl;
        return 0;
    }

    //2) read in the configuration files
    std::cout<<"\n";
    std::string configFileName(argv[1]);
    InputParser::ConfigData confData;

    bool configParseGood = InputParser::parseValAndPrintConfigFile(&confData, configFileName, std::cout);
    if(!configParseGood)
    {
        std::cout<<"Failed in config reading\n"<<std::endl;
        return 1;
    }
    InputParser::DetData detData;
    bool detDataParseGood = InputParser::parseValAndPrintDetDataFile(&detData, confData.arrayDataPath, std::cout);
    if(!detDataParseGood)
    {
        std::cout<<"Failed in detector data reading\n"<<std::endl;
        return 1;
    }
    //3) initialize the logging file
    std::cout<<"Logging Start\n\n"<<std::endl;
    boost::log::register_simple_formatter_factory< LogSeverity, char >("Severity");
    boost::log::add_common_attributes();
    typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> FileSink;
    boost::shared_ptr<FileSink> fileSink = boost::log::add_file_log(
                                               boost::log::keywords::file_name = confData.logFilePath,
                                               boost::log::keywords::auto_flush = true,
                                               boost::log::keywords::format = "[%TimeStamp%] <%Severity%>: %Message%");  //give messages a time and severity
    boost::log::core::get()->add_sink(fileSink);
    
    //set up to dump to the console as well
    boost::shared_ptr<std::ostream> coutStream(&std::cout, boost::null_deleter());
    typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend> text_sink;
    boost::shared_ptr<text_sink> coutSink = boost::make_shared<text_sink>();
    coutSink->locked_backend()->add_stream(coutStream);
    boost::log::core::get()->add_sink(coutSink);
    
    LoggerType& lg = Logger::get();
    //4) show the user what data we read in
    BOOST_LOG_SEV(lg, Information) << "The configuration data read in is: \n" << confData << "\n";
    BOOST_LOG_SEV(lg, Information) << "The detector data read in is: \n" << detData << "\n";
    
    //5) create the output system
    
    //6) create the output sub classes and add them to the system
    
    //7) create the ORCHID data reader
    
    //8) run the processing loop
    
    //9) finalize the spectra
    
    return 0;
}
