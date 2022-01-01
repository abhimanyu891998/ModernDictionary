#include<bits/stdc++.h>
#include "./PrefixTree.hpp"

int main() {
    std::unique_ptr<prefixtree::Trie> t = std::make_unique<prefixtree::Trie>();

    t->insert("abcd");
    if(!t->wordExists("babcd"))
        t->insert("babcd");
 

    std::cout<<t->prefixWordCount("abc")<<std::endl;

    std::vector<std::string> wordlist = t->prefixWordList("abc");

    for(auto &i: wordlist) {
        std::cout<<i<<std::endl;
    }
}