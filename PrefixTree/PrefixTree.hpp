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

    /**
     * A simple implementation of a generalized prefix tree, also called a Trie.
     * It is a data structure that helps in fast retrievel of words with a given prefix.
     * 
     * The time complexity of retrieval is generally O(m), where m is the size of the query string q.
     * It is more efficient as a lot of words in our modern dictionary can have similar prefixes.
     *
     */
    class Trie {

        private:
            std::shared_ptr<prefixtree::Node> head;

            /**
             * Performs a DFS from a given node, to all the nodes reachable from the inputNode n.
             * Takes in a vector as parameter by reference and modifies its contents based on the words it
             * finds along the way reachable from the given inputNode n.
            */
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
            
            /**
             * To insert a new word in the trie.
             * At the same time it also updates the prefix count for each of the nodes. 
             */
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

            /**
             * Return the count of words reperesented by given prefix
            */
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

            /**
             * Returns a list of words represented by a given prefix/
             */
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

            /**
             * Checks if a given word already exists in our tree.
             * This is to avoid redundant and duplicate words.
            */
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