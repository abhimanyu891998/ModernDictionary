#pragma once
#include<bits/stdc++.h>
#include "./Node.hpp"

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


