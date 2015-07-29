#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <stdio.h>
using namespace std;

namespace Index{
	class Node{
	public:
		Node();
		//~Node();
		wchar_t* text;
        long textIndex;
        int  textCount;
        long leftNodeId;
        long rightNodeId;
        int nodeHeight;
        long statCount;
		long maxStatCount;
		long value;
        int valueCount;
		long parentTextNodeId;


	}/*__attribute__((packed))*/;

	class TreeIndex{
	protected:
		int balanceFactor(long nodeId);
		int nodeHeight(long nodeId);
		int updateNodeHeight(long nodeId);
		int updateNodeStat(long nodeId);
		bool balanceNode(long nodeId);
		void rotateRight(long nodeId);
		void rotateLeft(long nodeId);
		///////////////////////
		Node* get(long nodeId);
		long newNode();
		FILE* file;
		FILE* wordFile;
		map<long, Node*> cache;
		set<long> saveQueue;
		//TreeIndex();
		long _size;
		
		void updateMaxStatCounts(vector<long>& nodes);
		string fileName;
        ///double keys
        map<wchar_t,char> mapDoubleKeys;
	public:
		TreeIndex(const string fileName);
		TreeIndex();
		~TreeIndex();
		void save();
		long size();
		vector<tuple<long, wstring,long > > search(wstring txt, int maxCount = 100);
		vector<wstring> searchOnlyString(wstring txt, int maxCount = 100);
		tuple<long, wstring,long >* find(wstring txt);
		void add(wstring txt, long statCount = 1/*rank*/,long value = 0);
        void addSave(wstring txt, long statCount = 1);
	//	void incrementUsage(wstring txt, int statCount = 1);
		void loadNgram(string fileName, int ngram);
		void loadTopWords(string fileName,int maxCount=-1);
		bool isExist(wstring s);
		void setText(Node* n, wstring txt);
		void clearCache();
		int cacheSize();
		int treeHeight();
		bool ensureStats(int node=0,long stat=0);
        
        /// double keys
        void setDoubleKeys(vector<wstring>& chars );
        vector<tuple<long, wstring,long > > searchDouble(wstring txt, int maxCount = 100);

	};

	class MultiNode{
	public:
		MultiNode();
		wchar_t* text;
		long textIndex;
		int  textCount;
		long statCount;
	};

	class MultiTreeIndex:TreeIndex{
		protected:
			FILE* multiNodeFile;
		public:
			//MultiValueTreeIndex(wstring fileName);
			//MultiValueTreeIndex();
			//~MultiValueTreeIndex();
			void add(wstring txt,wstring value, long statCount = 1);
			vector<tuple<long, wstring, long > > search(wstring txt, int maxCount = 100);
			vector<wstring> searchOnlyString(wstring txt, int maxCount = 100);
			vector<tuple<long, wstring> > find(wstring txt);

	};

}