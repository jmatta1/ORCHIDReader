#include<iostream>

int main(int argc, char* argv[])
{
    //1) first make sure we were passed the appropriate number of arguments
    if(argc != 2)
    {
        std::cout<<"Usage:\n";
        std::cout<<"  "<<argv[0]<<" ConfigurationFile"<<std::endl;
        return 0;
    }
    
    //2) read in the configuration file
    std::cout<<"Reading the configuration file at: "<<argv[1]<<std::endl;
    
    //3) create the ORCHID data reader
    
    //4) create the output system
    
    //5) Pull events from the reader and dump them to output system
    
    return 0;
}
