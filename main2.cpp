#include<bits/stdc++.h>
#include<chrono>
using namespace std;

void preProcessText(string &s) {
    s.erase(remove_if(s.begin(),s.end(), [&](char c) -> bool {
        return (c == '-' || !isalpha(c));
    }), s.end());

    transform(s.begin(), s.end(), s.begin(), [&](unsigned char c) -> char{
        return tolower(c);
    });

    return;
}

struct node {
    bool isSuffix;
    unordered_map<char, node*> child;
    set<string> words;
}*head;

void init() {
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


int main() {



    init();
    // string s = "frappe";
    // generateAndInsert(s);
    // s = "froyo";
    // generateAndInsert(s);
    // s = "fan";
    // generateAndInsert(s);
    // s = "fanan";
    // generateAndInsert(s);
    // s = "france";
    // generateAndInsert(s);
    // s = "frat";
    // generateAndInsert(s);

    fstream f("./input.txt");
    string s;

    auto start = chrono::high_resolution_clock::now();
    while(getline(f,s)) {
        preProcessText(s);
        generateAndInsert(s);
    }
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop-start);
    cout<<"duration "<<duration.count()<<endl;

    s = "lamp";
    vector<string> words = wordsWithSubstring(s);
    //int numb = words_with_prefix(s);
    //cout<<numb<<endl;
    for(auto &a: words) {
        cout<<a<<" ";
    }

    //cout<<search("france");

    return 0;
}