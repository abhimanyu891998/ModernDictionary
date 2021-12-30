#include<bits/stdc++.h>
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

struct node
{
	int prefix_count;
	bool isEnd;
	unordered_map<char, node*> child;
    string word;
}*head;

void init()
{
	head = new node();
	head->isEnd = false;
	head->prefix_count = 0;
    head->word = "";
}

void insert(string word)
{   

	node *current = head;
	current->prefix_count++;
	
	for(int i = 0 ; i < word.length(); ++i)
	{
		char letter = word[i];	//extrct first character of word
		if(current->child[letter] == NULL) {
            current->child[letter] = new node();
        }
	

		current->child[letter]->prefix_count++;
		current = current->child[letter];		
	}
	current->isEnd = true;
    current->word = word;
}

bool search(string word)
{
	node *current = head;
	for(int i = 0 ; i < word.length(); ++i)
	{
		int letter = (int)word[i] - (int)'a';
		if(current->child[letter] == NULL)
			return false;		//not found
		current = current->child[letter];
	}
	return current->isEnd;
}

int words_with_prefix(string prefix)
{
	node *current = head;
	for(int i = 0; i < prefix.length() ; ++i)
	{
		char letter = prefix[i];
		if(current->child[letter] == NULL)
			return 0;
		else
			current = current->child[letter];
	}
	return current->prefix_count;
}


void dfs(node *n, vector<string> &wordList) {

    if(n->isEnd) {
        wordList.push_back(n->word);
    }

    if(n->child.empty()) {
        return;
    }

    
    for(auto &[key, value]: n->child) {
        dfs(value, wordList);
    }

    return;


    
}


vector<string> words_list_prefix(string prefix) {
    node *current = head;
    for(int i=0; i<prefix.length() ; i++) {
        char letter = prefix[i];
        if(current->child[letter] == NULL)
            return {};
        else
            current = current->child[letter];
    }

    vector<string> wordList;

    dfs(current, wordList);

    return wordList;
  
}


int main() {

    init();
    fstream f("./input.txt");
    string s;
    auto start = chrono::high_resolution_clock::now();
    while(getline(f,s)) {
        preProcessText(s);
        insert(s);
    }
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop-start);
    cout<<"duration "<<duration.count()<<endl;

    s = "coat";
    vector<string> words = words_list_prefix(s);
    int numb = words_with_prefix(s);
    cout<<numb<<endl;
    for(auto &a: words) {
        cout<<a<<" ";
    }

    //cout<<search("france");

    return 0;
}