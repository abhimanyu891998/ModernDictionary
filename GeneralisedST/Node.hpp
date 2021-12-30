#pragma once
#include<bits/stdc++.h>
#include "./Edge.hpp"



struct Node {

    std::set<int> data;
    int lastIdx = 0;
    const static int START_SIZE = 0;
    const static int INCREMENT = 1;
    std::unordered_map<char, Edge*> edges;
    Node* suffix;
    int resultCount = -1;

    Node(){
        edges = {};
        suffix = nullptr;
        data = {};
    }

    ~Node() {
        delete suffix;
        for(auto & itr: edges) {
            delete itr.second;
        }
    }

    bool contains(int index) {
        return data.find(index)!=data.end();
    }

    std::vector<int> getData() { return getData(-1); }

    std::vector<int> getData(int numElements) {
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




    void addRef(int index) {
        if(contains(index)) {
            return;
        }

        data.insert(index);
        lastIdx = index;

        Node *iter = suffix;
        while(iter != nullptr) {
            if(iter->contains(index))
                break;
            iter->addRef(index);
            iter = iter->suffix;
        }

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

    int computeAndCacheCount() {
        computeAndCacheCountRecursive();
        return resultCount;
    }

    void addEdge(char ch, Edge* e) {
        edges[ch] = e;
    }

    Edge* getEdge(char ch) {
        return edges[ch];
    }



};





