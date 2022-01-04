# ModernDictionary

A header library based implementation of a special type of Dictionary that has a capacity to accept more than 100k words. It supports multiple functions such as:

1.) SuffixSearch and SuffixCount : Returns all the words and their counts with given suffix.\
2.) SubstringSearch and SubstringCount :  Returns all the words and their counts with given substring.\
3.) PrefixSearch and PrefixCount : Returns all the words and their counts with given prefix.

The suffix and substring search implementation is based on building a Generalised Suffix Tree using Ukkonen's Algorithm and supports an online insertion of words using the implementation
based on Ukkonen's paper "Online insertion of Suffix Tree" : https://www.cs.helsinki.fi/u/ukkonen/SuffixT1withFigs.pdf. It enables building a generalised suffix tree (a suffix tree with many strings)\
in linear time. 

All the counts are cached while loading the words in the dictionary so that the count queries are really fast and lightweight. 

PrefixSearch is based on simple implementation of Trie. 
