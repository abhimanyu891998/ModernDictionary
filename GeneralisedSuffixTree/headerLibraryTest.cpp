#include<memory>
#include<iostream>
#include<vector>
#include<string>

#include "./GeneralisedSuffixTree.hpp"

void preProcessText(std::string &s) {
    s.erase(remove_if(s.begin(),s.end(), [&](char c) -> bool {
        return (c == '-' || !isalpha(c));
    }), s.end());

    transform(s.begin(), s.end(), s.begin(), [&](unsigned char c) -> char{
        return tolower(c);
    });

    return;
}

int main() {
    std::unique_ptr<gst::GeneralizedSuffixTree> in = std::make_unique<gst::GeneralizedSuffixTree>();
    std::string word = "qabcz";
    int index = 0;
    in->insert(word, index);
    std::string word2 = "cdabc";
    in->insert(word2,1);
    in->insert("tabchij", 2);
    std::string word3 = "fraabc";
    in->insert(word3, 3);
    std::string word4 = "abc";
    in->insert(word4, 4);
    in->insert(word4,5);
    in->computeCount();
    in->computeSuffxCount();

    std::vector<int> op = in->search("abc");
    std::vector<int> suffix = in->searchSuffix("abcz");

    for(auto &i : suffix) {
        std::cout<<i<<std::endl;
    }

    std::cout<<in->searchWithCount("abc")<<std::endl;
    std::cout<<in->searchWithSuffixCount("cz")<<std::endl;

    // map<int,std::string> stringMap;
    // fstream f("../inputshit.txt");
    // std::string s;
    // auto start = chrono::high_resolution_clock::now();
    // int idx = 0;
    // while(getline(f,s)) {
    //     preProcessText(s);
    //     stringMap[idx] = s;
    //     in->insert(s,idx);
    //     idx++;
    // }
    // auto stop = chrono::high_resolution_clock::now();
    // auto duration = chrono::duration_cast<chrono::microseconds>(stop-start);
    // cout<<"duration "<<duration.count()<<endl;

    // vector<int> op = in->searchSuffix("mon");
    // set<std::string> opSet;
    // for(auto &i: op) {
    //     opSet.insert(stringMap[i]);
    // }

    // for(auto &s : opSet) {
    //     cout<<s<<endl;
    // }

    return 0;
}