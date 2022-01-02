#ifndef GST_UKKONENS_EDGE
#define GST_UKKONENS_EDGE
#pragma once

#include<memory>
#include "./Node.hpp"

namespace gst {
    
typedef struct Node Node;

/**
* Edge in the generalised suffix tree, the label corresponds to part of suffix this edge represents. 
* The dest represents the destination node which is either a leaf or an intermediate node.
*/
class Edge {
    
    private:
    std::string label;
    std::shared_ptr<Node> dest;

    public:
    Edge(std::string label, std::shared_ptr<Node> dest):label(label), dest(dest) {

    }

    std::string getLabel() {return this->label;}

    std::shared_ptr<Node> getDest() {
        return this->dest;
    }

    void setLabel(std::string label) {this->label = label;}


};

}


#endif

