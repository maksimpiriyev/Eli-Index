#include <iostream>
#include <fstream>
#include <istream>
#include <cstdio>
#include <queue>
#include <string>

#include <cmath>
#include "TreeIndex.h"
#include <locale>
#include <codecvt>
#include <fcntl.h>
using namespace std;
#define MAXNODECOUNT 40
#define COUNTTHRESHOLD 4000000

static map<wchar_t, int> dictCharToIndex = { { ' ', 0 }, { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'e', 5 }, { 'f', 6 }, { 'g', 7 }, { 'h', 8 }, { 'i', 9 },
{ 'j', 10 }, { 'k', 11 }, { 'l', 12 }, { 'm', 13 }, { 'n', 14 }, { 'o', 15 }, { 'p', 16 }, { 'q', 17 }, { 'r', 18 }, { 's', 19 },
{ 't', 20 }, { 'u', 21 }, { 'v', 22 }, { 'w', 23 }, { 'x', 24 }, { 'y', 25 }, { 'z', 26 }, { '.', 27 }, { '-', 28 }, { 's', 29 }

};
static map<int,wchar_t> dictIndexToChar = { { 0,' ' }, { 1,'a' }, { 2,'b' }, { 3,'c'}, { 4,'d' }, { 5,'e' }, { 6,'f' }, { 7,'g' }, {8, 'h' }, { 9,'i' },
{ 10,'j' }, { 11,'k' }, { 12,'l' }, { 13,'m' }, { 14,'n'}, { 15,'o' }, { 16,'p' }, { 17,'q' }, { 18,'r'}, { 19,'s' },
{ 20,'t' }, { 21,'u' }, { 22,'v' }, { 23,'w' }, { 24,'x'}, { 25,'y' }, { 26,'z' }, { 27,'.' }, { 28,'-'}, { 29,'s' }

};

namespace Index{

    typedef unsigned int   uint;
    typedef unsigned short word;
    typedef unsigned char  byte;
    
    int UTF16to8(const wchar_t* w, char* s ) {
        uint  c;
        word* p = (word*)w;
        byte* q = (byte*)s; byte* q0 = q;
        while( 1 ) {
            c = *p++;
            if( c==0 ) break;
            if( c<0x080 ) *q++ = c; else
                if( c<0x800 ) *q++ = 0xC0+(c>>6), *q++ = 0x80+(c&63); else
                    *q++ = 0xE0+(c>>12), *q++ = 0x80+((c>>6)&63), *q++ = 0x80+(c&63);
        }
        *q = 0;
        return q-q0;
    }

	Node::Node(){
		this->statCount = this->maxStatCount = this->nodeHeight = 0;
		this->textIndex = -1;
		this->text = NULL;
		this->leftNodeId = this->rightNodeId = -1;
        this->parentTextNodeId=-1;
	}

	TreeIndex::TreeIndex(const string fileName){
		this->fileName = fileName;
	//	file=fopen( "C:\\Users\\Maksim Piriyev\\index_en.tf", "rb+");
		
        const char* cfileName=fileName.c_str();
        printf("%s",cfileName);
		file = fopen( cfileName, "rb+");
		wordFile = fopen( (fileName + ".w").c_str(), "rb+");
       // int fd= (int)file;
        //fcntl(F_NOCACHE)
       cout<<endl<<"posix:"<< posix_fadvise(fileno(file), 0, 0, POSIX_FADV_RANDOM)<<endl;
        posix_fadvise(fileno(wordFile), 0, 0, POSIX_FADV_RANDOM);
        cout<<ENODEV<<" "<<EBADF<<" "<<EINVAL<<endl;
		if (file == NULL){
			file = fopen(cfileName, "wb+");
			wordFile = fopen( (fileName + ".w").c_str(), "wb+");
		}
		fseek(file, 0, SEEK_END);
		_size = ftell(file) / sizeof(Node);
		
	}

	TreeIndex::TreeIndex(){
		file = wordFile = NULL;
		_size = 0;
	}


	long TreeIndex::size(){
		return _size;

	}
	long TreeIndex::newNode(){
		long nodeId = _size++;
		Node* n = new Node();
		cache[nodeId] = n;
		saveQueue.insert(nodeId);
		return nodeId;
	}
	
	TreeIndex::~TreeIndex(){
		save();
		if (this->file != NULL) fclose(this->file);
		if (this->wordFile != NULL) fclose(this->wordFile);
		cache.clear();
	}
	void TreeIndex::loadTopWords(string fileName,int maxCount){
		wifstream in(fileName);
		wstring word;
		int counter = 1;
		in.imbue(std::locale(std::locale::classic(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::generate_header>));

		//maxCount = 100000;

		vector<wstring> lst;
		while (in >> word && counter<=maxCount){
			lst.push_back(word);
			++counter;
		}
		cout << "readed " <<fileName<< endl;
	//	const long THRESHOLD = 3000000;
		const long THRESHOLD = 0;

		for (size_t i = 0; i < lst.size(); i++)
		{
           // cout<<i<<endl;
			add(lst[i], lst.size() - i + THRESHOLD);
//			bool es = ensureStats();
//			cout << es << endl;
//			if (!es){
//				cout << "problem:" << i << endl;
//				return;
//			}

			if (i>0 && i % 3000 == 0){
				cout << i << endl;
//				 es = ensureStats();
//
//                if (!es){
//					cout << "problem:" << i << endl;
//					return;
//				}
//				save();
//				clearCache();
			}
		}
		save();
		clearCache();
        cout<<"finished"<<endl;
		bool es = ensureStats();
		cout << es << endl;
		if (!es){
			cout << "problem:" << 0 << endl;
			return;
		}
		
	

	}

	void TreeIndex::loadNgram(string fileName, int ngram){
		wifstream in(fileName);
		in.imbue(std::locale(std::locale::classic(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::generate_header>));

		int statCount;
		wstring words[10];
		int counter = 0;
		vector<long> stats;
		vector<wstring> lst;
		while (in >> statCount){
			wstring rslt;
			for (int i = 0; i < ngram; i++)
			{
				in >> words[i];
				if (i>0) rslt += L" ";
				rslt += words[i];
			}
			stats.push_back(statCount);
			lst.push_back(rslt);
			
			
			counter++;
			if (counter == 1266){
				int ff = 0;
				cout << "AS"<<endl;
			}
			if (counter % 100000 == 0){
				cout << "readed "<<counter<<endl;
				for (size_t i = 0; i < stats.size(); i++)
				{
					this->add(lst[i], stats[i]);
				}
				save();
				clearCache();
				cout << counter << endl;
				cout << treeHeight() << endl;
				stats.clear();
				lst.clear();

			}
		}
		for (size_t i = 0; i < stats.size(); i++)
		{
			this->add(lst[i], stats[i]);
		}
		this->save();
		this->clearCache();
		return;
		int j = stats.size() / 2;
		for (size_t i = 0; (i + j) <stats.size() && (j - i) >= 0; i++)
		{
			this->add(lst[j + i], stats[j + i]);
			if (i>0)
				this->add(lst[j - i], stats[j - i]);
			counter++;
			if (counter % 1000 == 0)
				this->save();
		}

	}
	bool TreeIndex::ensureStats(int node,long stat){
		if (node < 0) return true;
		auto n = this->get(node);
		if (node == 0) stat = n->maxStatCount;
		if (n->maxStatCount < n->statCount || stat < n->maxStatCount) return false;
		bool r = ensureStats(n->leftNodeId,stat);
		bool r2 = ensureStats(n->rightNodeId,stat);
		bool r4 = false;
		return r&&r2;
		return ensureStats(n->leftNodeId,stat) && ensureStats(n->rightNodeId,stat);
		//return true;
	}
	void TreeIndex::clearCache(){
		for (auto t = cache.begin(); t != cache.end(); t++)
		{
			auto i = *t;
			if(i.second->text!=NULL)
				delete[] i.second->text;
		}
		cache.clear();
	}
	int TreeIndex::cacheSize(){

		return cache.size();
	}
	Node* TreeIndex::get(long nodeId){
		if (nodeId < 0) return NULL;
		if (cache.count(nodeId) > 0) return cache[nodeId];
		
		//return NULL;
		long sz = this->size();
		if (sz <= nodeId) return NULL;


		Node* n = new Node();

		fseek(this->file, sizeof(Node)*nodeId, SEEK_SET);
		fread(n, sizeof(Node), 1, this->file);
		n->text = new wchar_t[n->textCount + 1];
		fseek(this->wordFile, n->textIndex, SEEK_SET);
		fread(n->text, sizeof(wchar_t), n->textCount + 1, this->wordFile);
		cache[nodeId] = n;
		return n;
	}

	void TreeIndex::save(){
		for (auto t = saveQueue.begin(); t != saveQueue.end(); t++)
		{
			long nodeId = *t;
			Node* n = cache[nodeId];
			if (n->textIndex == -1){
				fseek(this->wordFile, 0, SEEK_END);
				n->textIndex = ftell(wordFile);
				fwrite(n->text, sizeof(wchar_t), n->textCount + 1, this->wordFile);
			}
			fseek(this->file, sizeof(Node)*nodeId, SEEK_SET);
			fwrite(n, sizeof(Node), 1, this->file);
		}
		fflush(file);
		fflush(wordFile);

		saveQueue.clear();
		//	cache.clear();
	}
	int indexOf(int ch){

		return  dictCharToIndex[ch];
	}
	wchar_t charFromIndex(int i){
		return dictIndexToChar[i];
	}
	bool hasPrefix(wstring txt, wstring prefix){
		if (txt.length() < prefix.length()) return false;

		return prefix == txt.substr(0, prefix.length());
	}
	void TreeIndex::setText(Node* n, wstring txt){
		n->text = new wchar_t[txt.length() + 1];
		wcscpy(n->text, txt.c_str());
		n->textCount = txt.length();
	}
	void TreeIndex::add(wstring txt, long statCount,long value){
		//txt = txt.toLower();
		long nodeId = 0, subNodeId = -1;
		Node* n = this->get(nodeId);
		if (n == NULL) {
			nodeId = newNode();
			n = this->get(nodeId);
		}

		vector<long> l = { nodeId };

		bool did = true;
		while (did){
			did = false;
			if (n->text != NULL && wstring(n->text) == txt){
				//if (n->statCount < COUNTTHRESHOLD) n->statCount = COUNTTHRESHOLD;
				n->statCount += statCount;
                n->value = value;
				saveQueue.insert(nodeId);
				updateMaxStatCounts(l);
				return;
			}
			if (n->leftNodeId > 0 && wstring(n->text) > txt){
				nodeId = n->leftNodeId;
				n = this->get(n->leftNodeId);
				l.push_back(nodeId);
				did = true;
			}
			else if (n->rightNodeId > 0 && wstring(n->text) <= txt){
				nodeId = n->rightNodeId;
				n = this->get(n->rightNodeId);
				l.push_back(nodeId);
				did = true;
			}
		}
		if (n->text == NULL){
			setText(n, txt);
            saveQueue.insert(nodeId);
		}
		else if (n->leftNodeId < 0 && wstring(n->text) >= txt){
			saveQueue.insert(nodeId);

			long newNodeId = newNode();
			n->leftNodeId = newNodeId;
			n = this->get(newNodeId);
			setText(n, txt);
			l.push_back(newNodeId);
		}
		else if (n->rightNodeId < 0 && wstring(n->text) <= txt){
			saveQueue.insert(nodeId);

			long newNodeId = newNode();
			n->rightNodeId = newNodeId;
			n = this->get(newNodeId);
			setText(n, txt);
			l.push_back(newNodeId);

		}
		n->statCount = n->maxStatCount = statCount;
        n->value = value;

		updateMaxStatCounts(l);
	}
	int charSize(){

		return dictCharToIndex.size();
	}

    void TreeIndex::addSave(wstring txt, long statCount){
        add(txt,statCount);
        save();
    }
	void TreeIndex::updateMaxStatCounts(vector<long>& nodes){
		bool firsttime = true;
		long maxStatCount = 0;
		bool did = true;
		for (int i = nodes.size() - 1; i >= 0 && did; i--)
		{
			//did = false;
			Node* n = this->get(nodes[i]);
			if (firsttime){
				firsttime = false;
				if (n->maxStatCount < n->statCount){
					n->maxStatCount = n->statCount;
					saveQueue.insert(nodes[i]);
				}
			//	maxStatCount = n->maxStatCount;
				did = true;
				balanceNode(nodes[i]);
				continue;
			}
			/*if (n->maxStatCount < maxStatCount){
				n->maxStatCount = maxStatCount;
				saveQueue.insert(nodes[i]);
				did = true;
			}*/
			//maxStatCount = max(maxStatCount, n->maxStatCount);
			did |= balanceNode(nodes[i]);

		}
	}
	//void TreeIndex::incrementUsage(wstring txt, int statCount){
//		this->add(txt, statCount);
	//}
	bool TreeIndex::isExist(wstring txt){
		auto r = this->search(txt);
		if (r.size() > 0 && std::get<1>(r[0]) == txt) return true;
		return false;
	}
	int TreeIndex::treeHeight(){
		return nodeHeight(0);
	}
	int TreeIndex::nodeHeight(long nodeId){
		Node* n = this->get(nodeId);
		return n == NULL ? 0 : n->nodeHeight;
	}
	int TreeIndex::updateNodeHeight(long nodeId){
		Node* n = this->get(nodeId);
		if (n == NULL) return  0;
		Node* l = n->leftNodeId < 0 ? NULL : this->get(n->leftNodeId);
		Node* r = n->rightNodeId < 0 ? NULL : this->get(n->rightNodeId);
		int h = max(l == NULL ? 0 : l->nodeHeight, r == NULL ? 0 : r->nodeHeight) + 1;
		if (h != n->nodeHeight) saveQueue.insert(nodeId);

		return n->nodeHeight = h;
	}
	int TreeIndex::updateNodeStat(long nodeId){
		Node* n = this->get(nodeId);
		if (n == NULL) return  0;
		Node* l = n->leftNodeId < 0 ? NULL : this->get(n->leftNodeId);
		Node* r = n->rightNodeId < 0 ? NULL : this->get(n->rightNodeId);
		long st = max(n->statCount, max(l == NULL ? n->statCount : l->maxStatCount, r == NULL ? n->statCount : r->maxStatCount));
		if (st != n->maxStatCount) saveQueue.insert(nodeId);

		return n->maxStatCount = st;
	}

	int TreeIndex::balanceFactor(long nodeId){
		Node* n = this->get(nodeId);
		if (n == NULL) return 0;
		int lh = (n->leftNodeId == 0 ? 0 : nodeHeight(n->leftNodeId)), rh = n->rightNodeId == 0 ? 0 : nodeHeight(n->rightNodeId);
		return lh - rh;
	}

	bool TreeIndex::balanceNode(long nodeId){
		Node* n = this->get(nodeId);
		if (n == NULL) return false;
		int bf = balanceFactor(nodeId);
		bool did = false;
		if (bf <= -2){
			int lbf = balanceFactor(n->rightNodeId);
			if (lbf >= 1)
				rotateRight(n->rightNodeId);
			rotateLeft(nodeId);
			did = true;
		}
		else if (bf >= 2){
			int rbf = balanceFactor(n->leftNodeId);
			if (rbf <= -1)
				rotateLeft(n->leftNodeId);
			rotateRight(nodeId);
			did = true;
		}
		updateNodeHeight(nodeId);
		updateNodeStat(nodeId);
		return did;
	}
	void TreeIndex::rotateRight(long nodeId){
		Node* n = this->get(nodeId);
		int ln = n->leftNodeId;
		if (n == NULL || ln < 0) return;
		saveQueue.insert(nodeId);
		saveQueue.insert(ln);

		Node* l = this->get(n->leftNodeId);
		swap(*n, *l);
		l->leftNodeId = n->rightNodeId;
		n->rightNodeId = ln;
		updateNodeHeight(ln);
		updateNodeStat(ln);
		updateNodeHeight(nodeId);
		updateNodeStat(nodeId);

	}

	void TreeIndex::rotateLeft(long nodeId){
		Node* n = this->get(nodeId);
		int rn = n->rightNodeId;
		if (n == NULL || rn < 0) return;
		saveQueue.insert(nodeId);
		saveQueue.insert(rn);

		Node* r = this->get(n->rightNodeId);
		swap(*n, *r);
		r->rightNodeId = n->leftNodeId;
		n->leftNodeId = rn;
		updateNodeHeight(rn);
		updateNodeStat(rn);
		updateNodeHeight(nodeId);
		updateNodeStat(nodeId);
	}
	///////////////////////


	bool treesort(tuple<long, wstring,long>& p1, tuple<long, wstring,long>& p2){
		return std::get<0>(p1) == std::get<0>(p2) ?
			std::get<1>(p1).length() < std::get<1>(p2).length() : (std::get<0>(p2) < std::get<0>(p1));
	}
	tuple<long, wstring, long >* TreeIndex::find(wstring txt){
		long nodeId = 0;
		Node* n = this->get(nodeId);
		if (n == NULL)
			return NULL;

		bool did = true;
		while (did && n != NULL && (n->rightNodeId > 0 || n->leftNodeId > 0) ){
			did = false;
			if (wstring(n->text) == txt){
				return new tuple<long, wstring,long>(n->statCount, txt,n->value);
			}
			if (n->leftNodeId > 0 && wstring(n->text) >= txt){
				n = this->get(nodeId = n->leftNodeId);
				did = true;
			}
			else if (n->rightNodeId > 0 && wstring(n->text) < txt){
				n = this->get(nodeId = n->rightNodeId);
				did = true;
			}
		}
		return NULL;
	}
	vector<tuple<long, wstring,long> > TreeIndex::search(wstring txt, int maxCount){
      //  std::transform(txt.begin(), txt.end(), txt.begin(), ::tolower);

		vector<tuple<long, wstring,long> > rtn;
		long nodeId = 0;
		Node* n = this->get(nodeId);
		if (n == NULL)
			return rtn;
		wcout << txt << endl;
		bool did = true;
		while (did && n != NULL && (n->rightNodeId > 0 || n->leftNodeId > 0) && !hasPrefix(wstring(n->text), txt)){
			did = false;
			if (n->leftNodeId > 0 && wstring(n->text) >= txt){
				n = this->get(nodeId = n->leftNodeId);
				did = true;
			}
			else if (n->rightNodeId > 0 && wstring(n->text) < txt){
				n = this->get(nodeId = n->rightNodeId);
				did = true;
			}
		}
		if (!hasPrefix(wstring(n->text), txt)) return rtn;
		
		priority_queue<pair<int, long> > maxq;
		priority_queue<pair<int, long> > curq; // has the least stat value on top maxCount

		maxq.push(make_pair(n->maxStatCount, nodeId));
		while (!maxq.empty()){
				auto t = maxq.top();
				maxq.pop();

				n = this->get(t.second);
				curq.push(make_pair(-n->statCount, t.second));
			//	cout << n->maxStatCount << " - " << n->statCount << " - " << -curq.top().first << endl;
			
                if (curq.size() >= maxCount && n->statCount <= -this->get(curq.top().second)->statCount){
					break;
				}

				while (curq.size() > maxCount) curq.pop();

				Node* nt = this->get(nodeId = n->leftNodeId);
				while (nt  && !hasPrefix(wstring(nt->text), txt) ){
					nt = this->get(nodeId = nt->rightNodeId);
				}
				if (nt && hasPrefix(wstring(nt->text), txt))
					maxq.push(make_pair(nt->maxStatCount, nodeId));
		
				nt = this->get(nodeId = n->rightNodeId);
				while (nt  && !hasPrefix(wstring(nt->text), txt) ){
					nt = this->get(nodeId = nt->leftNodeId);
				}
				if (nt && hasPrefix(wstring(nt->text), txt))
					maxq.push(make_pair(nt->maxStatCount, nodeId));
				



		}
		//popping the last item if exists
		while (curq.size() > maxCount) curq.pop();
		
		while (!curq.empty())
		{
			n = this->get(curq.top().second);
			curq.pop();
			rtn.insert(rtn.begin(),tuple<long, wstring, long>(n->statCount, wstring(n->text), n->value));
		}
		//reverse(rtn.begin(), rtn.end());

		//sort(rtn.begin(), rtn.end(), treesort);
		cout << rtn.size() << endl;
		return rtn;
	}
	vector<wstring> TreeIndex::searchOnlyString(wstring txt, int maxCount ){
		auto t=search(txt,maxCount);
		vector<wstring> rtn;
		for (size_t i = 0; i < t.size(); i++)
		{
			rtn.push_back(std::get<1>(t[i]));
		}
		return rtn;
	}
    
    void TreeIndex::setDoubleKeys(vector<wstring>& chars ){
        mapDoubleKeys.clear();
        for (int i=0; i<chars.size(); i++) {
            for (int j=0; j<chars[i].length(); j++) {
                mapDoubleKeys[chars[[i][j]] = 'a';
            }
        }
    }
    vector<tuple<long, wstring,long > > TreeIndex::searchDouble(wstring txt, int maxCount = 100){
                              vector<tuple<long, wstring,long> > rtn;
                              long nodeId = 0;
                              Node* n = this->get(nodeId);
                              if (n == NULL)
                              return rtn;
                              wcout << txt << endl;
                              bool did = true;
                              while (did && n != NULL && (n->rightNodeId > 0 || n->leftNodeId > 0) && !hasPrefix(wstring(n->text), txt)){
                              did = false;
                              if (n->leftNodeId > 0 && wstring(n->text) >= txt){
                              n = this->get(nodeId = n->leftNodeId);
                              did = true;
                              }
                              else if (n->rightNodeId > 0 && wstring(n->text) < txt){
                              n = this->get(nodeId = n->rightNodeId);
                              did = true;
                              }
                              }
                              if (!hasPrefix(wstring(n->text), txt)) return rtn;
                              
                              priority_queue<pair<int, long> > maxq;
                              priority_queue<pair<int, long> > curq; // has the least stat value on top maxCount
                              
                              maxq.push(make_pair(n->maxStatCount, nodeId));
                              while (!maxq.empty()){
                              auto t = maxq.top();
                              maxq.pop();
                              
                              n = this->get(t.second);
                              curq.push(make_pair(-n->statCount, t.second));
                              //	cout << n->maxStatCount << " - " << n->statCount << " - " << -curq.top().first << endl;
                              
                              if (curq.size() >= maxCount && n->statCount <= -this->get(curq.top().second)->statCount){
                              break;
                              }
                              
                              while (curq.size() > maxCount) curq.pop();
                              
                              Node* nt = this->get(nodeId = n->leftNodeId);
                              while (nt  && !hasPrefix(wstring(nt->text), txt) ){
                              nt = this->get(nodeId = nt->rightNodeId);
                              }
                              if (nt && hasPrefix(wstring(nt->text), txt))
                              maxq.push(make_pair(nt->maxStatCount, nodeId));
                              
                              nt = this->get(nodeId = n->rightNodeId);
                              while (nt  && !hasPrefix(wstring(nt->text), txt) ){
                              nt = this->get(nodeId = nt->leftNodeId);
                              }
                              if (nt && hasPrefix(wstring(nt->text), txt))
                              maxq.push(make_pair(nt->maxStatCount, nodeId));
                              
                              
                              
                              
                              }
                              //popping the last item if exists
                              while (curq.size() > maxCount) curq.pop();
                              
                              while (!curq.empty())
                              {
                              n = this->get(curq.top().second);
                              curq.pop();
                              rtn.insert(rtn.begin(),tuple<long, wstring, long>(n->statCount, wstring(n->text), n->value));
                              }
                              //reverse(rtn.begin(), rtn.end());
                              
                              //sort(rtn.begin(), rtn.end(), treesort);
                              cout << rtn.size() << endl;
                              return rtn;
                            
                              }
}