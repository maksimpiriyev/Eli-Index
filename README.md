# Eli-Index : Ranked Prefix Search for Large Data on External Memory
Previously I wrote (https://github.com/maksimpiriyev/Search-As-You-Type-Index-master) about fast prefix search for ~10,000 key-value pairs designed for mobile,Now,Eli-Index designed to handle more than 1,000,000 key-value pairs,make fast prefix search on them,and furthermore bring the most ranked one from index.It stores index and data at the same time on drive not in RAM (caches the nodes while it needs)


Most of the prefix search indices are designed for server side use,they store index in the RAM so that has intialization latency which prenvents opening application more faster in mobile.Eli-Index is designed to overcome this problem.
Eli-Index is designed over AVL Tree,Segmented Tree and Heap.Its nodes has the features of both AVL Tree and Segmented Tree.It makes prefix search over AVL Tree ,and gets the result list in sorted order while iterating on the nodes of segmented tree with the help of heap.  It might also be the first AVL Tree implemented for string prefix search as I couldn't find any paper or implementation that use AVL Tree for string Prefix Search. Eli-Index has preferred to use AVL Tree instead of Trie Index because if Trie Index is implemented in Hard Drive,Either it is fast and uses more memory or it is slower because of more random file access.
Class structure:
```c++
  class TreeIndex{
	
	public:
		TreeIndex(const string fileName);
		void save();
		long size();
		vector<tuple<long, wstring,long > > search(wstring txt, int maxCount = 100);
		vector<wstring> searchOnlyString(wstring txt, int maxCount = 100);
		tuple<long, wstring,long >* find(wstring txt);
		void add(wstring txt, long statCount = 1);
		void loadNgram(string fileName, int ngram);
		void loadTopWords(string fileName,int maxCount=-1);
		bool isExist(wstring s);
		void setText(Node* n, wstring txt);
		void clearCache();
		int cacheSize();
		int treeHeight();
	};
```
Its usage :
```c++
  TreeIndex xx("index_en.tf");
	cout << xx.treeHeight() << endl;

	xx.add(L"predict");
	xx.add(L"prediction");
	auto xff = xx.search(L"pre",100);
	for (size_t j = 0; j < xff.size(); j++)
	{
		wcout << std::get<1>(xff[j]) << endl;
	}
	
```
#Insert Comlexity
Insert complexity is as same as the AVL tree O(logN)
# Search Complexity for top K words
O(K* lgK* lgN* lglgN) .


# Eli-Index vs SQL
Note that nowadays most of the databases have prefix search with their full-text search functionality.Lets say you have more than 1,000,000 words and 100,000 of them starts with "a*" and you want search all of them ordered by their rank.This time Eli-Index and SQL will have very similar performance results.But If you search top 10 ranked(user-defined) results from these 100,000 words then Eli-Index will drammatically beat SQL,because SQL is abstract it retrieves and the sorts for column and gets top results, instead El-Index finds top nodes first by default

# OS Caches and Initial Latency
Some  Operating Systems caches whole file before you access it,and it brings extra latency in the first search,but meanwhile makes faster the other searches.But, you can use fadvise,fcntrl etc to disable cache or tell the read pattern is random
