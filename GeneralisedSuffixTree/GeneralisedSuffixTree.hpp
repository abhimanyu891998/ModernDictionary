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

/**
 * An implementation of a generalized suffix tree based on Ukkonen's algorithm. The specifics of generalized
 * algorithm were derived from Ukkonen's research paper titled "On-line construction of suffix trees", the paper
 * can be found at http://www.cs.helsinki.fi/u/ukkonen/SuffixT1withFigs.pdf.
 * 
 * Helps in faster building of suffix tree O(n) time, as opposed to O(n^2) time taken by a normal approach. Here n is 
 * the length of one word. This is a unique approach, unlike a conventional approach that concatenates multiple strings into one
 * long string and then inserts into the tree based on Ukkonen's algorithm. That approach is not scalable for a large number of strings.
 * This tree instead, is used to build an index over many strings and has an online property as well, which means you can insert strings to this
 * tree even after the first initialization.
 * 
 * Main operations are insert words, search words with a given substring and search words with a given suffix.
 * 
 * The overall complexity of retrieval operation is O(m) where m is the length of query string to search within the tree.
 * 
 * In short, the tree has a set of nodes and labeled edges. The labels on these edges have a simple constraint, no two edges starting from the same node, can
 * have the same character. Because of this, a (startNode, stringSuffix) pair can denote a unique path within the tree, this path can be 
 * sequentially traversed using all the edges, starting from startNode such that e1.label + e2.label + e3.label .... is equal to the stringSuffix, here e1, e2, e3 are
 * edges.
 * 
 * Essentially, the union of all edge labels from root to a leaf, represents set of strings contained within this tree. There are other strings, contained
 * implicitly within the tree as well, as some string might be suffixes of a string, but also a substring for another longer string.
 * 
 * This implicit path is dealt with in the testAndSplit method and is implemented as described in the Ukkonen's paper.
 */
class GeneralizedSuffixTree {

    private:

    std::shared_ptr<Node> root = std::make_shared<Node>();
    /**
     * Last leaf added during an update operation.
    */
    std::shared_ptr<Node> activeLeaf = root;

    /**
     * Returns the tree node (if present) that corresponds to a given string.
     */
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
                    // the label on the edge does not correspond to the string we want to search.
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

    /**
     * Starting from the inputNode, updates the tree by adding stringPart.
     * 
     * It returns a pair of node pointer and string, for the string that has been added so far.
     * 
     * params : inputNode : the node to start from , stringPart: the string to add to the tree, rest: the rest of the string, value: the index value of word in wordBuffer
     */
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
                //Node already present. 
                leaf = tempEdge->getDest();
            }
            else {
                //We build a new leaf that originates from the given node. 
                leaf = std::make_shared<Node>();
                leaf->addRef(value);
                leaf->addSuffix(value);
                std::shared_ptr<Edge> newEdge = std::make_shared<Edge>(rest,leaf);
                r->addEdge(newChar, newEdge);
            }

            // Updating the suffix link for newly created leaf
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

    /**
     * Returns a pair of pointer to the node and the string, which is a farthest descendant of the input node that can be 
     * reached by following a path of edges, this string will be a prefix of the inputstr and the remaining part
     * will have to be appended to the concatenation of labels to get the final inputstr. 
     * 
     * This is required to the support the online property of this tree, as described in the paper.
     */
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

    /**
     * This is a crucial step to convert the implicit suffix tree, into a complete suffix tree.
     * It tests whether the string stringPart + t is contained in the subtree that has inputs as root.
     * If that is not true, but there is a path of edges such that 
     * e1.label + e2.label + .... + endOfString = stringPart
     * and there is also an edge g such that g.label = stringPart + rest. 
     * Then this g edge needs to split into two edges, one having endOfString as label and the other having
     * rest as label.
     * 
     */
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

    /**
     * Searches for all the words that contain a given substring word
     * Returns indices of those words in the wordBuffer contained in the dictionary.
     */
    std::vector<int> search(const std::string &word) {
        return search(word, -1);
    }

    /**
     * Can return the words that contain a given substring word,
     * takes in an additional param, results, this can be supplied if you
     * want to limit the number of results that you want to receive.
     */
    std::vector<int> search(std::string word, int results) {
        std::shared_ptr<Node> tmpNode = searchNode(word);
        
        if(tmpNode == nullptr) {
            return {};
        }

        return tmpNode->getData(results);
    }

    /**
     * Searches for all the words that contain a given suffix.
     */
    std::vector<int> searchSuffix(const std::string &word) {

        std::shared_ptr<Node> tmpNode = searchNode(word);

        if(tmpNode == nullptr) {
            return {};
        }

        return tmpNode->getSuffixData();
    }

    /**
     * Returns the count of words represented by a given substring word
     */
    int searchWithCount(std::string word) {
        std::shared_ptr<Node> tmpNode = searchNode(word);
        if(tmpNode == nullptr) {
            return 0;
        }

        return tmpNode->resultCount;
    }

    /**
     * Returns the count of words represented by a given suffix word
     */
    int searchWithSuffixCount(std::string word) {
        std::shared_ptr<Node> tmpNode = searchNode(word);
        if(tmpNode == nullptr)
            return 0;

        return tmpNode->suffixCount;
    }

    /**
     * Adds the specified index to the GST under the given key.
     * Entries are inserted in a non-decreasing order.
    */
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