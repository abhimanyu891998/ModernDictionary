#ifndef DICT_MODERN
#define DICT_MODERN
#pragma once

#include<memory>
#include<unordered_map>
#include<vector>
#include<string>
#include<iostream>
#include "../GeneralisedSuffixTree/GeneralisedSuffixTree.hpp"
#include "../PrefixTree/PrefixTree.hpp"
#include "../utils/utils.hpp"

namespace dictionary {
    
    class ModernDictionary {

        private:

        std::vector<std::string> wordBuffer;
        int lastWordIdx;
        std::unique_ptr<gst::GeneralizedSuffixTree> suffixTree;
        std::unique_ptr<prefixtree::Trie> trie;
        enum operations {S1, S2, S3, S4, C1, C2, C3, C4};
        std::unordered_map<std::string, operations> stringOpToEnum; //To map string operation code to enum

        public:

        ModernDictionary():wordBuffer({}), 
            lastWordIdx(-1), 
            suffixTree(std::make_unique<gst::GeneralizedSuffixTree>()), 
            trie(std::make_unique<prefixtree::Trie>()), 
            stringOpToEnum({
            {"S1", S1},
            {"S2", S2},
            {"S3", S3},
            {"S4", S4},
            {"C1", C1},
            {"C2", C2},
            {"C3", C3},
            {"C4", C4}
            }){

        }

        void insertWord(const std::string &word) {
            if(!trie->wordExists(word)) {
                wordBuffer.push_back(word);
                ++lastWordIdx;
                trie->insert(word);
                suffixTree->insert(word,lastWordIdx);
            }
        }

        void calculateAndCacheCounts() {
            suffixTree->computeCount();
            suffixTree->computeSuffixCount();
        }

        std::string processOperations(const std::string &op, std::string &arg) {

            util::preProcessText(arg);

            switch(stringOpToEnum[op]) {
                
                case S1:
                    return util::vecToString(searchPrefix(arg));
                    break;

                case S2:
                    return util::vecToString(searchSuffix(arg));
                    break;

                case S3:
                    return util::vecToString(searchSubstring(arg));
                    break;

                case S4:
                    return util::vecToString(searchChar(arg));
                    break;

                case C1:
                    return std::to_string(countPrefix(arg));
                    break;

                case C2:
                    return std::to_string(countSuffix(arg));
                    break;

                case C3:
                    return std::to_string(countSubstring(arg));
                    break;

                case C4:
                    return std::to_string(countChar(arg));
                    break;

                default:
                    return "Invalid query type provided \n";

            }

        }

        std::vector<std::string> searchPrefix(std::string &prefix) {
            return trie->prefixWordList(prefix);
        }

        std::vector<std::string> searchSuffix(std::string &suffix) {
            std::vector<int> wordIndexes = suffixTree->searchSuffix(suffix);
            std::vector<std::string> suffixWords = {};

            for(auto &i: wordIndexes) {
                suffixWords.push_back(wordBuffer[i]);
            }

            return suffixWords;
        }

        std::vector<std::string> searchSubstring(std::string &substr) {
            std::vector<int> wordIndexes = suffixTree->search(substr);
            std::vector<std::string> substrWords = {};

            for(auto &i: wordIndexes) {
                substrWords.push_back(wordBuffer[i]);
            }

            return substrWords;
        }

        std::vector<std::string> searchChar(std::string &charSubstr) {
            std::vector<int> wordIndexes = suffixTree->search(charSubstr);
            std::vector<std::string> charSubstrWords = {};

            for(auto &i: wordIndexes) {
                charSubstrWords.push_back(wordBuffer[i]);
            }

            return charSubstrWords;
        }

        int countPrefix(std::string &prefix) {
            return trie->prefixWordCount(prefix);
        }

        int countSuffix(std::string &suffix) {
            return suffixTree->searchWithSuffixCount(suffix);
        }

        int countSubstring(std::string &substr) {
            return suffixTree->searchWithCount(substr);
        }

        int countChar(std::string &charSubstr) {
            return suffixTree->searchWithCount(charSubstr);
        }

    };

}


#endif