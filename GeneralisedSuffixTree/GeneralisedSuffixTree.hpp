#ifndef GST_UKKONENS_MAIN
#define GST_UKKONENS_MAIN
#pragma once
#include<bits/stdc++.h>
#include "./Node.hpp"
#include "./Edge.hpp"


struct GeneralizedSuffixTree {

    int last = 0;
    std::shared_ptr<Node> root = std::make_shared<Node>();
    std::shared_ptr<Node> activeLeaf = root;

    std::vector<int> search(std::string word) {
        return search(word, -1);
    }

    std::vector<int> searchSuffix(std::string word) {
        return searchSuffix(word, -1);
    }

    std::vector<int> searchSuffix(std::string word, int results) {
        std::shared_ptr<Node> tmpNode = searchNode(word);
        if(tmpNode == nullptr) {
            return {};
        }

        return tmpNode->getSuffixData();
    }

    std::vector<int> search(std::string word, int results) {
        std::shared_ptr<Node> tmpNode = searchNode(word);
        
        if(tmpNode == nullptr) {
            return {};
        }

        return tmpNode->getData(results);
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

    bool startsWith(const std::string &prefix, const std::string & arg) {
        return ((prefix.size() <= arg.size()) && (std::equal(prefix.begin(), prefix.end(), arg.begin())));
    }


    std::shared_ptr<Node> searchNode(std::string word) {
        std::shared_ptr<Node> currentNode = root;
        std::shared_ptr<Edge> currentEdge = nullptr;

        for(int i=0; i<word.length(); i++) {
            char ch = word[i];
            currentEdge = currentNode->edges[ch];
            if(nullptr == currentEdge) {
                return nullptr;
            }
            else {
                std::string label = currentEdge->getLabel();
                int lenToMatch = std::min(word.length()-i, label.length());
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

    void put(std::string key, int index) {
   
        last = index;

        activeLeaf = root;
        std::string remainder = key;
        std::shared_ptr<Node> s = root;

        std::string text = "";
        for(int i=0; i<remainder.length() ; i++) {
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
                std::pair<std::shared_ptr<Node>, std::string> canret = canonize(s->suffix, safeCutLastChar(tempstr));
                s = canret.first;
                tempstr = canret.second + tempstr[tempstr.length()-1];
            }

            ret = testAndSplit(s, safeCutLastChar(tempstr), newChar, rest, value);
            r = ret.second;
            endpoint= ret.first;

        }

        if(oldRoot != root) {
            oldRoot->suffix = r;
        }

        oldRoot = root;

        return {s, tempstr};
    }

    std::shared_ptr<Node> getRoot() {
        return root;
    }

    int computeCount() {
        return root->computeAndCacheCount();
    }

    int computeSuffxCount() {
        return root->computeAndCacheSuffixCount();
    }


    std::string safeCutLastChar(std::string seq) {
        if(seq.length() == 0)
            return "";
        return seq.substr(0, seq.length()-1);
    }


    std::pair<std::shared_ptr<Node>, std::string> canonize(std::shared_ptr<Node> s, std::string inputstr) {
        if(inputstr=="") {
            return {s,inputstr};
        }

        else {
            std::shared_ptr<Node> currentNode = s;
            std::string str = inputstr;
            std::shared_ptr<Edge> g = s->getEdge(str[0]);
            while(g!=nullptr && (startsWith(g->getLabel(), str))) {
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
                // label.startsWith(str);
                
                if(!(startsWith(str,label))) {
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
                
                else if(startsWith(label,remainder)) {
                    return {true, s};
                }
                else if(startsWith(remainder,label)) {
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
};

#endif