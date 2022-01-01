#ifndef GST_UKKONENS_NODE
#define GST_UKKONENS_NODE
#pragma once

#include<set>
#include<algorithm>
#include<memory>
#include<vector>
#include<string>
#include "./Edge.hpp"

namespace gst {

struct Node {

    std::set<int> data;
    std::set<int> suffixData;
    std::unordered_map<char, std::shared_ptr<Edge>> edges;
    std::shared_ptr<Node> suffix;
    int resultCount = -1;
    int suffixCount = -1;

    Node(){
        edges = {};
        suffix = nullptr;
        data = {};
    }

    bool contains(const int &index) {
        return data.find(index)!=data.end();
    }

    std::vector<int> getData() { return getData(-1); }

    std::vector<int> getData(const int &numElements) {
        std::set<int> output;
        for(auto &num: data) {
            output.insert(num);
            if(output.size() == numElements) {
                std::vector<int> opVec(output.begin(), output.end());
                return opVec;
            }
        }

        for(auto &[c, edge] : edges) {
            if(-1 == numElements || output.size() < numElements) {
                for(int &num: (edge->getDest())->getData()) {
                    output.insert(num);
                    if(output.size() == numElements) {
                    std::vector<int> opVec(output.begin(), output.end());
                    return opVec;
                    }
                }
            }
        }

        std::vector<int> op(output.begin(), output.end());

        return op;
    }

    std::vector<int> getSuffixData() const {
        return std::vector<int>(suffixData.begin(), suffixData.end());
    }

    void addRef(const int &index) {

        if(contains(index)) {
            return;
        }

        data.insert(index);

        std::shared_ptr<Node> iter = suffix;
        while(iter != nullptr) {
            if(iter->contains(index))
                break;
            iter->addRef(index);
            iter = iter->suffix;
        }
    }

    void addSuffix(const int &index) {
        if(suffixData.find(index) != suffixData.end()) {
            return;
        }

        suffixData.insert(index);
    }

    std::set<int> computeAndCacheCountRecursive() {
        std::set<int> ret = data;
        for(auto &[c, edge]: edges) {
            for(auto &n : (edge->getDest())->computeAndCacheCountRecursive()) {
                ret.insert(n);
            }
        }

        resultCount = ret.size();
        return ret;
    }

    void calculateAndCacheSuffixCountRecursive() {
        this->suffixCount = this->suffixData.size();
        for(auto &[c,edge]: edges) {
            edge->getDest()->calculateAndCacheSuffixCountRecursive();
        }
    }

    int calculateAndCacheSubstringCount() {
        computeAndCacheCountRecursive();
        return resultCount;
    }

    int calculateAndCacheSuffixCount() {
        calculateAndCacheSuffixCountRecursive();
        return suffixCount;
    }

    void addEdge(const char &ch, std::shared_ptr<Edge> e) {
        edges[ch] = e;
    }

    std::shared_ptr<Edge> getEdge(const char &ch) {
        return edges[ch];
    }

};

}



#endif



