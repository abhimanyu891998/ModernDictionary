#ifndef GST_UKKONENS_EDGE
#define GST_UKKONENS_EDGE
#pragma once

#include<memory>
#include "./Node.hpp"

namespace gst {
    
typedef struct Node Node;

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

