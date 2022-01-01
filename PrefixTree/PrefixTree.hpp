#ifndef GPT_MAIN
#define GPT_MAIN
#pragma once

#include<memory>
#include<string>
#include<algorithm>
#include<unordered_map>
#include<set>
#include<vector>
#include "./Node.hpp"

namespace prefixtree {

    class Trie {

        private:
            std::shared_ptr<prefixtree::Node> head;

            void dfs(std::shared_ptr<prefixtree::Node> n, std::vector<std::string> &wordList) {

                if(n->isWord) {
                    wordList.push_back(n->word);
                }

                if(n->children.empty()) {
                    return;
                }

                for( auto &[character, node]: n->children) {
                    dfs(node, wordList);
                }

            }

        public:
            Trie():head(std::make_shared<prefixtree::Node>()) {

            }

            void insert(const std::string & word) {

                std::shared_ptr<prefixtree::Node> current = head;
                current->prefixCount++;


                for(size_t i=0; i<word.length() ; ++i)  {

                    char letter = word[i]; 

                    if(current->children[letter] == nullptr) {
                        current->children[letter] = std::make_shared<prefixtree::Node>();
                    }
                    
                    current->children[letter]->prefixCount++;
                    current = current->children[letter];
                }


                current->isWord = true;
                current->word = word;

            }

            unsigned int prefixWordCount(const std::string &prefix) {

                std::shared_ptr<prefixtree::Node> current = head;

                for(size_t i=0; i< prefix.length() ; ++i) {
                    char letter = prefix[i];
                    if(current->children[letter] == nullptr) {
                        return 0;
                    }
                    else {
                        current = current->children[letter];
                    }
                }

                return current->prefixCount;
            }

            std::vector<std::string> prefixWordList(const std::string &prefix) {

                std::shared_ptr<prefixtree::Node> current = head;

                for(size_t i=0; i<prefix.length() ; i++) {
                    char letter = prefix[i];
                    if(current->children[letter] == nullptr)
                        return {};

                    else
                        current = current->children[letter];
                }

                std::vector<std::string> wordList;

                dfs(current, wordList);

                return wordList;
            }

            bool wordExists(const std::string &word) {
                
                std::shared_ptr<prefixtree::Node> current = head;

                for(size_t i=0 ; i<word.length() ; i++) {
                    char letter = word[i];
                    if(current->children[letter] == nullptr) {
                        return false;
                    }

                    current = current->children[letter];
                }

                if(current->isWord)
                    return true;

                return false;

            }


    };


}


#endif