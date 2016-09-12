//
//  Utils.hpp
//  PhyCPP
//
//  Created by Mathieu on 20/05/2015.
//

#ifndef __PhyCPP__utils__
#define __PhyCPP__utils__

#include <stdio.h>

#include <functional>
#include <string>
#include <sstream>
#include <stdexcept>


static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

static inline bool isFloat( const std::string &myString ) {
    std::istringstream iss(myString);
    float f;
    iss >> std::noskipws >> f; // noskipws considers leading whitespace invalid
    // Check the entire string was consumed and if either failbit or badbit is set
    return iss.eof() && !iss.fail();
}

static inline unsigned int stoui(const std::string &str) {
    unsigned long lresult = stoul(str, 0, 10);
    unsigned int result = lresult;
    if (result != lresult) throw std::out_of_range("Out of range");
    return result;
}

#endif /* defined(__PhyCPP__utils__) */
