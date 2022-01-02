#ifndef GPT_NODE
#define GPT_NODE
#pragma once

#include<unordered_map>
#include<iostream>
#include<memory>
#include<string>

namespace prefixtree {

    /**
     * Node for the prefix tree of all the strings.
     * Contains information related to whether this node
     * represents a word or not. If it does,
     * what word is it. It also has edges originating
     * from this node.
     *
     */
    struct Node {   
        int64_t prefixCount;
        bool isWord;
        std::unordered_map<char, std::shared_ptr<Node>> children;
        std::string word;

        Node():prefixCount(0), isWord(false), word(""), children({}){

        }
    };
}


#endif