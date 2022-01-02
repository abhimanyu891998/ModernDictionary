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

/**
 * Node in a generalised suffix tree.
*/
struct Node {

    std::set<int> data; //Contains indexes of words represented by the substring till this node.
    std::set<int> suffixData; // If it is a leaf node, this will store the indexes of words represented by this suffix.
    std::unordered_map<char, std::shared_ptr<Edge>> edges; //Set of edges starting from this node.
    std::shared_ptr<Node> suffix; // Suffix link from Ukkonen's paper. Describes the path of a word from root to this node. 
    int resultCount; // Total number of words represented by substrings to this node and the underlying ones. This value is cached during the load time of dictionary.
    int suffixCount; //Total number of words represented by suffix to this node. 

    /**
     * Constructor to create a new node
    */
    Node():edges({}), suffix(nullptr), data({}), resultCount(-1),suffixCount(-1){
    }


    bool contains(const int &index) {
        return data.find(index)!=data.end();
    }

    std::vector<int> getData() { return getData(-1); }

    /**
     * To search based on substrings, helps to search for a certain number of words as passed in the paramter.
     * param: numElements - -1 will return all the elements, any other positive number will return the maximum possible words upto numElements
     * return: vector of indices of the words in the main wordBuffer.
    */
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

    /**
     * Returns indices of words in wordBuffer, that have suffixes associated to this node
    */
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

    /**
     * Recrusively caches the substring counts of all nodes from the root node.
     */
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

    /**
     * Recursively caches the suffix counts of all nodes from the root node.
     */
    void calculateAndCacheSuffixCountRecursive() {
        this->suffixCount = this->suffixData.size();
        for(auto &[c,edge]: edges) {
            edge->getDest()->calculateAndCacheSuffixCountRecursive();
        }
    }

    /**
     * Called from the root node to cache the substring counts
     */
    int calculateAndCacheSubstringCount() {
        computeAndCacheCountRecursive();
        return resultCount;
    }

    /**
     * Called from the root node to cache the suffix counts
     */
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



