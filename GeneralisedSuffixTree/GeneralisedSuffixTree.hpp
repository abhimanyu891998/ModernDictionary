#ifndef GST_UKKONENS_MAIN
#define GST_UKKONENS_MAIN
#pragma once

#include<memory>
#include<algorithm>
#include<string>
#include<vector>
#include<set>
#include<unordered_map>
#include "./Node.hpp"
#include "./Edge.hpp"
#include "../utils/utils.hpp"

namespace gst {

class GeneralizedSuffixTree {

    private:

    std::shared_ptr<Node> root = std::make_shared<Node>();
    std::shared_ptr<Node> activeLeaf = root;

    std::shared_ptr<Node> searchNode(const std::string &word) {
        std::shared_ptr<Node> currentNode = root;
        std::shared_ptr<Edge> currentEdge = nullptr;

        for(size_t i=0; i<word.length(); i++) {
            char ch = word[i];
            currentEdge = currentNode->edges[ch];
            if(nullptr == currentEdge) {
                return nullptr;
            }
            else {
                std::string label = currentEdge->getLabel();
                size_t lenToMatch = std::min(word.length()-i, label.length());

                if(word.substr(i, lenToMatch) != label.substr(0, lenToMatch)) {
                    return nullptr;
                }

                if(label.length() >= word.length()-i) {
                    return currentEdge->getDest();
                }
                else {
                    currentNode = currentEdge->getDest();
                    i+=lenToMatch-1;
                }
            }
        }
        return nullptr;
    }

    std::pair<std::shared_ptr<Node>, std::string> update(std::shared_ptr<Node> inputNode, std::string stringPart, std::string rest, int value) {

        std::shared_ptr<Node> s = inputNode;
        std::string tempstr = stringPart;
        char newChar = stringPart[stringPart.length()-1];

        std::shared_ptr<Node> oldRoot = root;
        std::pair<bool, std::shared_ptr<Node>> ret = testAndSplit(s, tempstr.substr(0, tempstr.length()-1), newChar, rest, value);

        std::shared_ptr<Node> r = ret.second;
        bool endpoint = ret.first;

        std::shared_ptr<Node> leaf;

        while(!endpoint) {
            std::shared_ptr<Edge> tempEdge = r->getEdge(newChar);

            if(tempEdge!=nullptr) {
            leaf = tempEdge->getDest();
            }
            else {
                leaf = std::make_shared<Node>();
                leaf->addRef(value);
                leaf->addSuffix(value);
                std::shared_ptr<Edge> newEdge = std::make_shared<Edge>(rest,leaf);
                r->addEdge(newChar, newEdge);
            }

            if(activeLeaf!=root) {
                activeLeaf->suffix = leaf;
            }

            activeLeaf = leaf;

            if(oldRoot != root) {
                oldRoot->suffix = r;
            }

            oldRoot = r;

            if(s->suffix == nullptr) {
                if(s!=root) {
                    return {};
                }

                tempstr = tempstr.substr(1);
            }
            else {
                std::pair<std::shared_ptr<Node>, std::string> canonizeReturn = canonize(s->suffix, util::safeCutLastChar(tempstr));
                s = canonizeReturn.first;
                tempstr = canonizeReturn.second + tempstr[tempstr.length()-1];
            }

            ret = testAndSplit(s, util::safeCutLastChar(tempstr), newChar, rest, value);
            r = ret.second;
            endpoint= ret.first;

        }

        if(oldRoot != root) {
            oldRoot->suffix = r;
        }

        oldRoot = root;

        return {s, tempstr};
    }

    std::pair<std::shared_ptr<Node>, std::string> canonize(std::shared_ptr<Node> s, const std::string &inputstr) {
        if(inputstr=="") {
            return {s,inputstr};
        }

        else {
            std::shared_ptr<Node> currentNode = s;
            std::string str = inputstr;
            std::shared_ptr<Edge> g = s->getEdge(str[0]);
            while(g!=nullptr && (util::startsWith(g->getLabel(), str))) {
                str = str.substr(g->getLabel().length());
                currentNode = g->getDest();
                if(str.length() > 0) {
                    g = currentNode->getEdge(str[0]);
                }
            }

            return {currentNode, str};
        }
        return {};
    }

    std::pair<bool, std::shared_ptr<Node>> testAndSplit(std::shared_ptr<Node> inputs, std::string stringPart, char t, std::string remainder, int value) {

        std::pair<std::shared_ptr<Node>, std::string> ret = canonize(inputs, stringPart);
        std::shared_ptr<Node> s = ret.first;
        std::string str = ret.second;

        if(str!="") {
            std::shared_ptr<Edge> g = s->getEdge(str[0]);
            std::string label = g->getLabel();

            if(label.length() > str.length() && label[str.length()] == t) {
                return {true, s};
            }
            else {
                std::string newLabel = label.substr(str.length());

                if(!(util::startsWith(str,label))) {
                    return {};
                }

                std::shared_ptr<Node> r = std::make_shared<Node>();
                std::shared_ptr<Edge> newEdge = std::make_shared<Edge>(str, r);
                g->setLabel(newLabel);
                r->addEdge(newLabel[0], g);
                s->addEdge(str[0], newEdge);

                return {false, r};
            }
        }
        else {
            std::shared_ptr<Edge> e = s->getEdge(t);
            if(e == nullptr) {
                return {false, s};
            }
            else {
                std::string label = e->getLabel();
                if(remainder == e->getLabel()) {
                    e->getDest()->addRef(value);
                    e->getDest()->addSuffix(value);
                    return {true,s};
                }
                
                else if(util::startsWith(label,remainder)) {
                    return {true, s};
                }
                else if(util::startsWith(remainder,label)) {
                    std::shared_ptr<Node>  newNode = std::make_shared<Node>();
                    newNode->addRef(value);
                    newNode->addSuffix(value);

                    std::shared_ptr<Edge> newEdge = std::make_shared<Edge>(remainder, newNode);
                    e->setLabel(e->getLabel().substr(remainder.length()));
                    newNode->addEdge(e->getLabel()[0], e);
                    s->addEdge(t, newEdge);

                    return {false, s};
                }
                else {
                    return {true, s};
                }
            }
        }
    }

    public:

    std::vector<int> search(const std::string &word) {
        return search(word, -1);
    }

    std::vector<int> search(std::string word, int results) {
    std::shared_ptr<Node> tmpNode = searchNode(word);
    
    if(tmpNode == nullptr) {
        return {};
    }

    return tmpNode->getData(results);
    }

    std::vector<int> searchSuffix(const std::string &word) {

        std::shared_ptr<Node> tmpNode = searchNode(word);

        if(tmpNode == nullptr) {
            return {};
        }

        return tmpNode->getSuffixData();
    }


    int searchWithCount(std::string word) {
        std::shared_ptr<Node> tmpNode = searchNode(word);
        if(tmpNode == nullptr) {
            return 0;
        }

        return tmpNode->resultCount;
    }

    int searchWithSuffixCount(std::string word) {
        std::shared_ptr<Node> tmpNode = searchNode(word);
        if(tmpNode == nullptr)
            return 0;

        return tmpNode->suffixCount;
    }

    void insert(const std::string &key, const int &index) {
   
        activeLeaf = root;

        std::string remainder = key;
        std::shared_ptr<Node> s = root;

        std::string text = "";
        for(size_t i=0; i<remainder.length() ; i++) {
            text.push_back(remainder[i]);
            std::pair<std::shared_ptr<Node>, std::string> active = update(s, text, remainder.substr(i), index);
            active = canonize(active.first, active.second);
            s = active.first;
            text = active.second;
        }

        if(activeLeaf->suffix == nullptr && activeLeaf!=root && activeLeaf!=s) {
            activeLeaf->suffix = s;
        }
    }

    std::shared_ptr<Node> getRoot() const {
        return root;
    }

    int computeCount() {
        return root->calculateAndCacheSubstringCount();
    }

    int computeSuffixCount() {
        return root->calculateAndCacheSuffixCount();
    }


};


}


#endif