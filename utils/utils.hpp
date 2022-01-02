#ifndef UTILS_DICT
#define UTILS_DICT
#pragma once

#include<string>
#include<chrono>

/**
 * Useful utility functions have been added here.
 * 
*/
namespace util {

/**
 * Removes hyphens or anything this is not alphabets. Also converts everything to lower case.
*/
void preProcessText(std::string &s) {

    s.erase(remove_if(s.begin(),s.end(), [&](char c) -> bool {
        return (c == '-' || !isalpha(c));
    }), s.end());

    transform(s.begin(), s.end(), s.begin(), [&](unsigned char c) -> char{
        return tolower(c);
    });

    return;
}

/**
 * Convert a vector of string a single space separated string.
*/
std::string vecToString(const std::vector<std::string> &input) {

    std::string op = "";

    for(auto &s: input) {
        op+= s + " ";
    }

    if(op.length()>=1)
        op.pop_back(); //To remove extra space in the end
    
    return op;
}

bool startsWith(const std::string &prefix, const std::string & arg) {
    return ((prefix.size() <= arg.size()) && (std::equal(prefix.begin(), prefix.end(), arg.begin())));
}

std::string safeCutLastChar(std::string seq) {
    if(seq.length() == 0)
        return "";
    return seq.substr(0, seq.length()-1);
}

/**
 * Timer to keep a track of time taken by operations.
*/
class Timer {
private:
    std::chrono::time_point<std::chrono::steady_clock> startP;
public:
    Timer() {}
    void start() { startP = std::chrono::steady_clock::now(); }
    int64_t nsPassed() { auto end = std::chrono::steady_clock::now(); return std::chrono::duration_cast<std::chrono::nanoseconds>(end - startP).count(); }
    int64_t usPassed() { auto end = std::chrono::steady_clock::now(); return std::chrono::duration_cast<std::chrono::microseconds>(end - startP).count(); }
    int64_t msPassed() { auto end = std::chrono::steady_clock::now(); return std::chrono::duration_cast<std::chrono::milliseconds>(end - startP).count(); }
    int64_t sPassed()  { auto end = std::chrono::steady_clock::now(); return std::chrono::duration_cast<std::chrono::seconds>(end-startP).count(); }
};


}

#endif