#include<bits/stdc++.h>
// #include "./Node.hpp"
// #include "./Edge.hpp"

typedef struct Edge Edge;
typedef struct Node Node;
typedef struct GeneralizedSuffixTree GeneralizedSuffixTree;

struct Edge {
    std::string label;
    struct Node* dest;


    ~Edge(){
        delete dest;
    }

    std::string getLabel() {return label;}

    struct Node* getDest() {
        return dest;
    }

    Edge(std::string label, Node* dest):label(label), dest(dest) {

    }
};

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




struct GeneralizedSuffixTree {
    int last = 0;
    Node* root = new Node();
    Node* activeLeaf = root;

    ~GeneralizedSuffixTree() {
        delete root;
        delete activeLeaf;
    }

    std::vector<int> search(std::string word) {
        return search(word, -1);
    }

    std::vector<int> search(std::string word, int results) {
        Node *tmpNode = searchNode(word);
        
        if(tmpNode == nullptr) {
            return {};
        }

        return tmpNode->getData(results);
    }

    std::pair<std::vector<int>,int> searchWithCount(std::string word, int to) {
        Node* tmpNode = searchNode(word);
        if(tmpNode == nullptr) {
            return {{},0};
        }

        return {tmpNode->getData(to), tmpNode->resultCount};
    }


    Node* searchNode(std::string word) {
        Node* currentNode = root;
        Edge* currentEdge = nullptr;

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
        Node* s = root;

        std::string text = "";
        for(int i=0; i<remainder.length() ; i++) {
            text.push_back(remainder[i]);
            std::pair<Node*, std::string> active = update(s, text, remainder.substr(i), index);
            active = canonize(active.first, active.second);
            s = active.first;
            text = active.second;
        }

        if(activeLeaf->suffix == nullptr && activeLeaf!=root && activeLeaf!=s) {
            activeLeaf->suffix = s;
        }
    }

    std::pair<Node*, std::string> update(Node* inputNode, std::string stringPart, std::string rest, int value) {
        Node* s = inputNode;
        std::string tempstr = stringPart;
        char newChar = stringPart[stringPart.length()-1];

        Node* oldRoot = root;
        std::pair<bool, Node*> ret = testAndSplit(s, tempstr.substr(0, tempstr.length()-1), newChar, rest, value);

        Node* r = ret.second;
        bool endpoint = ret.first;

        Node* leaf;

        while(!endpoint) {
            Edge* tempEdge = r->getEdge(newChar);
            if(tempEdge!=nullptr) {
            leaf = tempEdge->getDest();
            }
            else {
                leaf = new Node();
                leaf->addRef(value);
                Edge* newEdge = new Edge(rest, leaf);
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
                std::pair<Node*, std::string> canret = canonize(s->suffix, safeCutLastChar(tempstr));
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

    Node* getRoot() {
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


    std::pair<Node*, std::string> canonize(Node* s, std::string inputstr) {
        if(inputstr=="") {
            return {s,inputstr};
        }

        else {
            Node* currentNode = s;
            std::string str = inputstr;
            Edge* g = s->getEdge(str[0]);
            while(g!=nullptr && (std::mismatch(g->getLabel().begin(), g->getLabel().end(), str.begin(), str.end()).first == g->getLabel().end())) {
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

    std::pair<bool, Node*> testAndSplit(Node *inputs, std::string stringPart, char t, std::string remainder, int value) {

        std::pair<Node*, std::string> ret = canonize(inputs, stringPart);
        Node* s = ret.first;
        std::string str = ret.second;

        if(str!="") {
            Edge* g = s->getEdge(str[0]);
            std::string label = g->getLabel();

            if(label.length() > str.length() && label[str.length()] == t) {
                return {true, s};
            }
            else {
                std::string newLabel = label.substr(str.length());
                // label.startsWith(str);
                if(!(std::mismatch(str.begin(), str.end(), label.begin(), label.end()).first == str.end())) {
                    return {};
                }

                Node* r = new Node();
                Edge* newEdge = new Edge(str,r);
                g->label = newLabel;
                r->addEdge(newLabel[0], g);
                s->addEdge(str[0], newEdge);

                return {false, r};
            }
        }
        else {
            Edge *e = s->getEdge(t);
            if(e == nullptr) {
                return {false, s};
            }
            else {
                std::string label = e->getLabel();
                if(remainder == e->getLabel()) {
                    e->getDest()->addRef(value);
                    return {true,s};
                }
                else if((std::mismatch(label.begin(), label.end(), remainder.begin(), remainder.end()).first == label.end())) {
                    return {true, s};
                }
                else if((std::mismatch(remainder.begin(), remainder.end(), label.begin(), label.end()).first == remainder.end())) {
                    Node* newNode = new Node();
                    newNode->addRef(value);

                    Edge* newEdge = new Edge(remainder, newNode);
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



int main() {
    using namespace std;
    GeneralizedSuffixTree *in = new GeneralizedSuffixTree();
    std::string word = "cacao";
    int index = 0;
    in->put(word, index);
    std::string word2 = "cancel";
    in->put(word2,1);
    std::string word3 = "francel";
    in->put(word3, 2);
    vector<int> op = in->search("cel");
    for(auto &i: op) {
        cout<<i<<endl;
    }
    // Hello* h = new Hello();
    // std::cout<<h->n;
}