#include<bits/stdc++.h>
// #include "./Node.hpp"
// #include "./Edge.hpp"

typedef struct Edge Edge;
typedef struct Node Node;
typedef struct GeneralizedSuffixTree GeneralizedSuffixTree;

bool startsWith(std::string &prefix, std::string & arg) {
    return ((prefix.size() <= arg.size()) && (std::equal(prefix.begin(), prefix.end(), arg.begin())));
}

struct Edge {
    std::string label;
    struct std::shared_ptr<Node> dest;

    std::string getLabel() {return label;}

    struct std::shared_ptr<Node> getDest() {
        return dest;
    }

    Edge(std::string label, std::shared_ptr<Node> dest):label(label), dest(dest) {

    }
};

struct Node {

    std::set<int> data;
    std::set<int> suffixData;
    int lastIdx = 0;
    const static int START_SIZE = 0;
    const static int INCREMENT = 1;
    std::unordered_map<char, std::shared_ptr<Edge>> edges;
    std::shared_ptr<Node> suffix;
    int resultCount = -1;

    Node(){
        edges = {};
        suffix = nullptr;
        data = {};
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

        std::shared_ptr<Node> iter = suffix;
        while(iter != nullptr) {
            if(iter->contains(index))
                break;
            iter->addRef(index);
            iter = iter->suffix;
        }

    }

    void addSuffix(int index) {
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

    int computeAndCacheCount() {
        computeAndCacheCountRecursive();
        return resultCount;
    }

    void addEdge(char ch, std::shared_ptr<Edge> e) {
        edges[ch] = e;
    }

    std::shared_ptr<Edge> getEdge(char ch) {
        return edges[ch];
    }



};




struct GeneralizedSuffixTree {
    int last = 0;
    std::shared_ptr<Node> root = std::make_shared<Node>();
    std::shared_ptr<Node> activeLeaf = root;

    std::vector<int> search(std::string word) {
        return search(word, -1);
    }

    std::vector<int> search(std::string word, int results) {
        std::shared_ptr<Node> tmpNode = searchNode(word);
        
        if(tmpNode == nullptr) {
            return {};
        }

        for(auto &i: tmpNode->suffixData) {
            std::cout<<"suffixDatais"<<std::endl;
            std::cout<<i<<std::endl;
        }
        return tmpNode->getData(results);
    }

    std::pair<std::vector<int>,int> searchWithCount(std::string word, int to) {
        std::shared_ptr<Node> tmpNode = searchNode(word);
        if(tmpNode == nullptr) {
            return {{},0};
        }

        return {tmpNode->getData(to), tmpNode->resultCount};
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
            while(g!=nullptr && ((g->label.size() <= str.size()) && (std::equal(g->label.begin(), g->label.end(), str.begin())))) {
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
                g->label = newLabel;
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
                    e->label = e->getLabel().substr(remainder.length());
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
    using namespace std;
    std::unique_ptr<GeneralizedSuffixTree> in = make_unique<GeneralizedSuffixTree>();
    // GeneralizedSuffixTree *in = new GeneralizedSuffixTree();
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

    // map<int,std::string> stringMap;
    // fstream f("../input.txt");
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

    // vector<int> op = in->search("mon");
    // set<std::string> opSet;
    // for(auto &i: op) {
    //     opSet.insert(stringMap[i]);
    // }

    // for(auto &s : opSet) {
    //     cout<<s<<endl;
    // }

    // cout<<opSet.size()<<endl;

    vector<int> op = in->search("abc");
    // for(auto &i: op) {
    //     cout<<i<<endl;
    // }

}