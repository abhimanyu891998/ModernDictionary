#ifndef GPT_MAIN
#define GPT_MAIN
#pragma once

#include<bits/stdc++.h>

class PrefixTree {

public: 

struct node {
    bool isSuffix;
    unordered_map<char, node*> child;
    set<string> words;
}*head;

void PrefixTree() {
    head = new node();
    head->isSuffix = false;
    head->words = {};
}


void insert(string word, string suffixWord)
{   

	node *current = head;

	for(int i = 0 ; i < word.length(); ++i)
	{
		char letter = word[i];	//extrct first character of word
		if(current->child[letter] == NULL) {
            current->child[letter] = new node();
        }
	
		current = current->child[letter];		
	}
	current->isSuffix = true;
    current->words.insert(suffixWord);
}

void generateAndInsert(string word) {
    for(int i=0; i<word.length(); i++) {
        insert(word.substr(i), word);
    }
}

void dfs(node *n, vector<string> &wordList) {

    if(n->isSuffix) {
        wordList.insert(wordList.end(), n->words.begin(), n->words.end());
    }

    if(n->child.empty()) {
        return;
    }

    
    for(auto &[key, value]: n->child) {
        dfs(value, wordList);
    }

    return;
    
}

set<string> wordsWithSuffix(string suffix) {
    node *current = head;

    for(int i=0 ; i<suffix.length(); i++) {
        char letter = suffix[i];
        if(current->child[letter] == nullptr) {
            return {};
        }
        current = current->child[letter];
    } 

    return current->words;
}

vector<string> wordsWithSubstring(string substring) {
    node *current = head;

    for(int i=0 ; i<substring.length(); i++) {
        char letter = substring[i];
        if(current->child[letter] == nullptr) {
            return {};
        }
        current = current->child[letter];
    } 

    vector<string> wordsList;

    dfs(current, wordsList);

    return wordsList;
}

};