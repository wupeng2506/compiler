#ifndef _DFA_H_
#define _DFA_H_

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <stack>
#include <stdlib.h>
#include <sstream>
#include "nfa.h"
#include "nfa.cc"

using namespace std;

struct RexNode{
	char val;
	string rex;
	void setRexNode(string& s){
		val = s[0];
		rex = s.substr(2, s.size() - 2);
	}
};


//NFA node struct
struct NfaNode{
	int val;
	bool over;
	multimap<char, NfaNode*> ways;
	void print(){
		cout <<"node : "<< val << endl;
		cout << "ways size () : "<< ways.size() << endl;
		for(multimap<char, NfaNode*>::iterator it = ways.begin();it != ways.end();it ++){
			cout << "way char: "<< it->first;
			cout << " way val:"<<it->second->val << endl;
		}
		// for(multimap<char, NfaNode*>::iterator it = ways.begin();it != ways.end();it ++){
		// 	//it->second->print();
		// }
		//cout << endl;
	}
};
int vval(0);
//Nfa
struct Nfa{
	NfaNode *start;
	NfaNode *end;
	void setNfa(char c){
		NfaNode *n1 = new NfaNode();
		n1->val = vval;
		vval++;
		NfaNode *n2 = new NfaNode();
		n2->val = vval;
		vval ++;
		n1->ways.insert(pair<char, NfaNode*>(c, n2));
		start = n1;
		end = n2;
		start->over = false;
		end->over = true;
	}
	void print(){
		start->print();
	}
};
///////////////////////////


///////////////////////////

#endif