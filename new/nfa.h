#ifndef _NFA_H_
#define _NFA_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <stdlib.h>
#include <algorithm>

using namespace std;
typedef struct Dfa ddfa;
//普通BNF结构


struct bnf{
	char before;
	vector<char> after;
	bnf(char a) : before(a) {}
	//初始化BNF
	void setAfter(string s){
		for(int i = 3;i < s.size();i ++)after.push_back(s[i]);
	}
	//print 
	void print(){
		cout << before << "->" ;
		for(vector<char>::iterator it = after.begin();it != after.end();it ++){
			cout << *it ;
		}
		cout << endl;
	}
};

//lr0的BNF结构

struct lr0_bnf{
	char before;
	vector<char> checked, after;
	set<char> lalr1;
	//when lr0 turn to be terminal , judge if it is equal to bnf
	bool equalbnf(bnf& b){
		if(before == b.before&&checked == b.after)return true;
		return false;
	}
	//初始化lr0的 bnf
	void setBoth(string s){
		bool b = true;
		for(int i = 0;i < s.size();i ++){
			if(s[i] == '.')b = false;
			if(b)checked.push_back(s[i]);
			else after.push_back(s[i]);
		}
	}
	//使 . 向前移动一个单位
	void moveForward(){
		//cout <<"10" << endl;
		vector<char>::iterator it = after.begin();
		//cout << "checked.size: " << checked.size()<< endl;
		char temp = *it;
		//cout << "10.10" << endl;
		checked.push_back(temp);
		after.erase(it);
	}
	//根据BNF来初始化
	void fromBNF(bnf& b, size_t t){
		before = b.before;
		for(size_t i = 0;i < b.after.size(); i++){
			if(i < t)checked.push_back((b.after)[i]);
			else after.push_back((b.after)[i]);
		}
	}
	//检查两个lr0 bnf是否相同
	bool equal(lr0_bnf& b){
		if(b.before == before&&b.checked == checked&&b.after == after)return true;
		return false;
	}
	void print(){
		cout << before << "->";
		vector<char>::iterator it = checked.begin();
		while(it != checked.end()){
			cout << *it;
			it ++;
		}
		cout <<".";
		it = after.begin();
		while(it != after.end()){
			cout << *it;
			it ++;
		}
		cout <<",";
		set<char>::iterator itt = lalr1.begin();
		while(itt != lalr1.end()){
			cout << *itt;
			itt ++;
		}
		cout << endl;
	}
};

//DFA结点
struct DfaNode{
	int val;
	vector<lr0_bnf> sv;//状态集合
	multimap<char, DfaNode&> ways;//边集合
	map<char, DfaNode*> way;	
	bool over;
	//print the DfaNode
	void print(){
		cout << "node : " << val << "over : " << over << endl;
		vector<lr0_bnf>::iterator it = sv.begin();
		while(it != sv.end()){
			(*it).print();
			it ++;
		}
		multimap<char, DfaNode&>::iterator i = ways.begin();
		cout << "ways  size: " << ways.size() <<endl;
		while(i != ways.end()){
			cout << (i->second).val <<"	"<<i->first <<  endl;
			i ++;
		}
		cout << "way size: "<< way.size() << endl;
		for(map<char, DfaNode*>::iterator ite = way.begin();ite != way.end();ite ++){
			cout << ite->second->val <<"	" << ite->first << endl;
		}
		cout << endl;
	}
};
	//找到状态并返回
DfaNode* find(DfaNode* next, vector<DfaNode*>& vdfa){
	//vector<DfaNode*>::iterator itt = vdfa.begin();
	for(vector<DfaNode*>::iterator i = vdfa.begin();i != vdfa.end();i ++){
		DfaNode* tmp = *i;
		size_t sum = 0;		
		for(vector<lr0_bnf>::iterator it = next->sv.begin();it != next->sv.end();it ++){
			for(vector<lr0_bnf>::iterator itt = tmp->sv.begin();itt != tmp->sv.end();itt ++){
				if((*it).equal(*itt)){
					sum ++;
					// (*it).print();
					// cout <<"val: "<< (*i)->val << endl;
					// cout <<"size: "<< ((*i)->sv).size() << endl;
				}
			}
		}
		if(sum == next->sv.size())return tmp;
	}
	return NULL;
}


//Dfa的数据结构
struct Dfa{
	DfaNode* start;//开始状态
};
//print
void print(DfaNode* start){
	start->print();		
	multimap<char, DfaNode&>::iterator it = (start->ways).begin();
	while(it != (start->ways).end()){
		print(&(it->second));
		it ++;
	}
}

struct tableNode{
	int tm;//where to go
	char slt;//select s or r
	tableNode(int i, char c) : tm(i), slt(c){}
	//print
	void print() {
		cout << slt << tm ;
	}
};

struct TableCol{
	int val;//hang
	multimap<char, tableNode> col;//colmn
	//print
	void print(){
		cout << val << "	";
		for(multimap<char, tableNode>::iterator it = col.begin();it != col.end();it ++){
			cout << it->first <<" : ";
			it->second.print();
			cout <<"	"; 	
		}
		cout << endl;
	}
};

void printTable(vector<TableCol*>& table){
	//sort the map
	for(size_t i = 0;i < table.size();i ++){
		for(size_t j = i;j < table.size();j ++){
			if(i == table[j]->val){
				swap(table[i], table[j]);
			}
		}
	}
	for(vector<TableCol*>::iterator it = table.begin();it != table.end();it ++){
		(*it)->print();
	}
}

////////////////////////////////////////////////////////

#endif
