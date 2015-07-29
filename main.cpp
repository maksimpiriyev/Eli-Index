#include <iostream>
#include <string>
#include <chrono>
#include <fstream>
#include "TreeIndex.h"
using namespace std;
using namespace Index;

int main(){
	
	

	
	const int maxCount = 1000000;
//	TreeIndex en("index_en1.tf");
//	en.loadTopWords("./langs/en.txt", maxCount);
//	return 0;
	/*
	en.loadNgram(".\\ngrams\\t.txt", 2);

	TreeIndex ru(L"index_ru.tf");
	ru.loadTopWords(".\\langs\\ru.txt", maxCount);
	
	

	TreeIndex fr(L"index_fr.tf");
	fr.loadTopWords(".\\langs\\fr.txt", maxCount);

	TreeIndex de(L"index_de.tf");
	de.loadTopWords(".\\langs\\de.txt", maxCount);

	TreeIndex es(L"index_es.tf");
	es.loadTopWords(".\\langs\\es.txt", maxCount);

	TreeIndex tr(L"index_tr.tf"); 
	tr.loadTopWords(".\\langs\\tr.txt", maxCount);

	TreeIndex da(L"index_da.tf");
	da.loadTopWords(".\\langs\\da.txt", maxCount);

	TreeIndex pl(L"index_pl.tf");
	pl.loadTopWords(".\\langs\\pl.txt", maxCount);

	TreeIndex pt(L"index_pt.tf");
	pt.loadTopWords(".\\langs\\pt.txt", maxCount);

	

	TreeIndex sv(L"index_sv.tf");
	sv.loadTopWords(".\\langs\\sv.txt", maxCount);


	return 0;*/


	TreeIndex xx("index_en1.tf");
	cout << xx.treeHeight() << endl;

	chrono::time_point<chrono::system_clock> start,end;
	start = chrono::system_clock::now();
	
//    wifstream in("./langs/en.txt");
//    wstring word;
//    int counter = 1;
//    //in.imbue(std::locale(std::locale::classic(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::generate_header>));
//    
//    //maxCount = 100000;
//    
//    wstring prefix=L"air";
//    vector<wstring> lst;
//    while (in >> word && counter<=maxCount){
//        lst.push_back(word);
//        if(word.size()>=prefix.size() && word.compare(0,prefix.size(),prefix)==0){
//            wcout<<word<<endl;
//        }
//        ++counter;
//    }
//    cout<<counter<<endl;
//    start = chrono::system_clock::now();
//    for (int i=0; i<lst.size(); i++) {
//        auto word=lst[i];
//        if(word.size()>=prefix.size() && word.compare(0,prefix.size(),prefix)==0){
//            wcout<<word<<endl;
//        }
//    }
    for (size_t i = 0; i < 1; i++)
	{
		/*xx.add(L"predict");
		xx.add(L"predict");*/
		auto xff = xx.search(L"air",100);
		for (size_t j = 0; j < xff.size(); j++)
		{
			wcout << std::get<1>(xff[j]) << endl;
		}
		if (xff.size()>0){
			wcout<<std::get<1>(xff[0]) << endl;
		}

	}
	
	end = chrono::system_clock::now();
	chrono::duration<double> d = end - start;
		cout<<d.count()<<endl;
	//return 0;
	//x.add("5");
	//x.add("3");
	//x.add("4");

	//x.add("bbca");
	////x.add("bbca");
	//x.save();
	//x.add("bbca");\
	//x.save();
	//auto f = x.search("bbca");
	//x.add("bbc");
	//x.add("bb");
	//x.save();
	//f = x.search("bbc");
	//x.add("bbc");
	///*x.add("bbca");
	//x.add("bbcd");*/
	//x.save();
	// f = x.search("cc");
	//f = x.search("bbca");
	//f = x.search("bbc");
	//f = x.search("b");
	//x.incrementUsage("bbcd");
	//x.save();
	//f = x.search("bbc");
	//
}