#include<bits/stdc++.h>
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
    std::unique_ptr<GeneralizedSuffixTree> in = std::make_unique<GeneralizedSuffixTree>();
    std::string word = "qabcz";
    int index = 0;
    in->put(word, index);
    std::string word2 = "cdabc";
    in->put(word2,1);
    in->put("tabchij", 2);
    std::string word3 = "fraabc";
    in->put(word3, 3);
    std::string word4 = "abc";
    in->put(word4, 4);
    in->put(word4,5);
    in->computeCount();
    in->computeSuffxCount();

    std::vector<int> op = in->search("abc");
    std::vector<int> suffix = in->searchSuffix("abcz");

    for(auto &i : suffix) {
        std::cout<<i<<std::endl;
    }

    std::cout<<in->searchWithCount("abc")<<std::endl;
    std::cout<<in->searchWithSuffixCount("abc")<<std::endl;

    // map<int,std::string> stringMap;
    // fstream f("../inputshit.txt");
    // std::string s;
    // auto start = chrono::high_resolution_clock::now();
    // int idx = 0;
    // while(getline(f,s)) {
    //     preProcessText(s);
    //     stringMap[idx] = s;
    //     in->put(s,idx);
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