//
//  CmdLine.hpp
//  PhyCPP
//
//  Created by Mathieu Fourment on 2/03/2016.
//

#ifndef CmdLine_hpp
#define CmdLine_hpp

#include <stdio.h>
#include <string>
#include <stdlib.h>

#include "Utils.hpp"


static void getCmdOption(char ** begin, char ** end, const std::string & option, std::string &variable){
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end){
        variable = std::string(*itr);
    }
}

static void getCmdOptionInt(char ** begin, char ** end, const std::string & option, int &variable){
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end){
        variable = atoi(*itr);
    }
}

static void getCmdOptionUnsignedInt(char ** begin, char ** end, const std::string & option, unsigned int &variable){
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end){
        variable = stoui(*itr);
        //variable = atoi(*itr);
    }
}

static void getCmdOptionLong(char ** begin, char ** end, const std::string & option, long &variable){
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end){
        variable = atol(*itr);
    }
}

static void getCmdOptionLongLong(char ** begin, char ** end, const std::string & option, long &variable){
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end){
        variable = atoll(*itr);
    }
}

static bool containCmdOption(char ** begin, char ** end, const std::string & option){
    char ** itr = std::find(begin, end, option);
    return (itr != end && ++itr != end);
}

#endif /* CmdLine_hpp */
