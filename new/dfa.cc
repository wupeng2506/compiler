#include "dfa.h"


/////////////////////
bool checkRex(vector<TableCol*>& table, string& s);
void linkNfa(Nfa& n1, Nfa& n2);
void selectNfa(Nfa& n1, Nfa& n2);
void closureNfa(Nfa& n);
void setDfa(Nfa& nfa);
void transnull(NfaNode& node, vector<NfaNode*>& vnode);
void transnullByc(NfaNode& node, vector<NfaNode*>& vnode, char c);
bool checkTokens(string& tokens, Dfa& dfa, vector<char>& rs, vector<char>& error);
string rtnTokens(ifstream& f);
DfaNode* createDfa();
///////////////////////////
stack<Nfa> stck;
set<char> wayc;//all the terminal 
Dfa ndfa;
map<string, char> vs;//save all the strings
vector<RexNode> vrn;//save the nodes of Rex 
//int vval(0);
//////////////////////////////

string rtnTokens(ifstream& f){
	ifstream in("dfa.txt");
	if(!in.is_open()){cout << "open file dfa.txt error @_@" << endl; exit(0);}
	ifstream inn("bnf.txt");
	//ifstream innn("program.c-");
	vector<TableCol*>* tbl = createTable(inn);
	int i = 1;
	string tests = "";
	while(!f.eof()){
		string bufs;
		getline(f, bufs);
		tests += bufs;
	}
	while(!in.eof()){
		string bufs;
		getline(in, bufs);
		RexNode rn;
		rn.setRexNode(bufs);
		vrn.push_back(rn);
		if(!checkRex(*tbl, rn.rex)){
			cout << "regular text "<< i <<" line is wrong @_@, please check it" << endl;
			return NULL;
		}
		if(rn.rex[0] != '&'){
			//cout << "string :" << rn.rex << "char: "<<rn.val <<endl; 
			vs.insert(pair<string, char>(rn.rex.substr(0, rn.rex.size() - 1), rn.val));
		}
		i ++;
	}
	Nfa start = stck.top();
	//start.print();
	stck.pop();
	while(stck.size() != 0){
		selectNfa(start, stck.top());
		stck.pop();
	}
	//start.print();
	setDfa(start);
	multimap<char, DfaNode&>::iterator it  = ndfa.start->ways.find('/');
	it->second.ways.insert(pair<char, DfaNode&>('*', *createDfa()));
	vector<char> vc;
	vector<char> error;
	string rs = "";
	// cout << "wayc" << wayc.size() << endl;
	// cout << "num $"<<wayc.count('$') << endl;
	//print(ndfa.start);
	if(checkTokens(tests, ndfa, vc, error))cout << "OK" << endl;
	else cout << "NO" << endl;
	for(vector<char>::iterator it = vc.begin();it != vc.end();it ++){
		//cout << *it << "   ";
		if(*it != 'c'){
			stringstream sstream;
			sstream << *it;
			rs += sstream.str();
		}
	}
	for(vector<char>::iterator it = error.begin();it != error.end();it ++){
		cout << "can not realize this token : " << *it << endl;
	}
	in.close();
	inn.close();
	release();
	//innn.close();
	return rs;
}

//create the dfa of comment
DfaNode* createDfa(){
	int state = 1000;
	DfaNode *c1 = new DfaNode();
	DfaNode *c2 = new DfaNode();
	DfaNode *ctr = new DfaNode();
	DfaNode *c3 = new DfaNode();
	DfaNode *c4 = new DfaNode();
	c1->val = state;state ++;
	c1->over = false;
	c2->val = state;state ++;
	c2->over = false;
	ctr->val = state;state ++;
	ctr->over = false;
	c3->val = state;state ++;
	c3->over = false;
	c4->val = state;state ++;
	c4->over = true;
	c1->ways.insert(pair<char, DfaNode&>('/', *c2));
	c2->ways.insert(pair<char, DfaNode&>('*', *ctr));
	for(char c = 0;c <= '~';c ++){
		if(c != '*'){
			ctr->way.insert(pair<char, DfaNode*>(c, ctr));
		}
	}
	ctr->ways.insert(pair<char, DfaNode&>('*', *c3));
	c3->ways.insert(pair<char, DfaNode&>('/', *c4));
	return ctr;
}

//check the bnf is right or not
bool checkRex(vector<TableCol*>& table, string& s){
	s = s + "$";
	vector<size_t> stk;
	size_t line = 0; 
	vector<char> tokens;
	
	char tr, befc, token;
	for(int i = s.size() - 1;i >= 0;i --){
		tokens.push_back(s[i]);
	}
	while(tokens.size() > 0){
		tr = tokens[tokens.size() - 1];
		if((tr < 'A'||tr > 'Z')&&tr != '$'&&tr != '&'&&tr != '%'&&tr != '|'&&tr != '#')token = 'a';
		else token = tr;
		tokens.pop_back();
		if((tr < 'A'||tr > 'Z')&&tr != '$')wayc.insert(tr);
		// cout << "token : "<< token << endl;
		// cout << "line : " << line << endl;
		for(multimap<char, tableNode>::iterator it = table[line]->col.begin();it != table[line]->col.end();it ++){
			if(it->first == token){
				//s happens
				if(it->second.slt == 's'){
					stk.push_back(line);
					line = it->second.tm;
					break;
				}else if(it->second.slt == 'r'){
				//r happens
					if(it->second.tm == 0){
						//stck.top().print();
						return true;
					}else if(it->second.tm == 8){
						Nfa nfa;
						nfa.setNfa(befc);
						//nfa.val = vval;
						//vval ++;
						stck.push(nfa);
						//cout << "8" << endl;
					}else if(it->second.tm == 5){
						Nfa nfa = stck.top();
						stck.pop();
						closureNfa(nfa);
						stck.push(nfa);	
						//cout << "5" << endl;					
					}else if(it->second.tm == 3){
						Nfa n2 = stck.top();
						stck.pop();
						Nfa n1 = stck.top();
						stck.pop();
						linkNfa(n1, n2);
						stck.push(n1);
						//cout << "3" << endl;
						//n1.print();
					}else if(it->second.tm == 1){
						Nfa n2 = stck.top();
						//n2.print();
						stck.pop();
						Nfa n1 = stck.top();
						//n1.print();
						stck.pop();
						selectNfa(n1, n2);
						stck.push(n1);
						//cout << "1" << endl;
					}
					bnf *tmpb = &vbnf[it->second.tm];
					for(size_t ii = 0;ii < tmpb->after.size();ii ++){
						line = stk[stk.size() - 1];
						stk.pop_back();						
					}
					tokens.push_back(tr);
					tokens.push_back(tmpb->before);
					break;
				}
			}
			if(++it == table[line]->col.end())return false;
			it --;
		}
		befc = tr;
	}
	return false;
}

//link the two nfa, link n2 to n1
void linkNfa(Nfa& n1, Nfa& n2){
	n1.end->ways.insert(pair<char, NfaNode*>('$', n2.start));
	n1.end->over = false;
	n1.end = n2.end;
}


//create the nfa that select n1 and n2 ,add all to n1
void selectNfa(Nfa& n1, Nfa& n2){
	NfaNode *nod1 = new NfaNode();
	nod1->over = false;
	n1.end->over = false;
	n2.end->over = false;
	NfaNode *nod2 = new NfaNode();
	nod1->ways.insert(pair<char, NfaNode*>('$', n1.start));
	nod1->ways.insert(pair<char, NfaNode*>('$', n2.start));
	n1.end->ways.insert(pair<char, NfaNode*>('$', nod2));
	n2.end->ways.insert(pair<char, NfaNode*>('$', nod2));
	n1.start = nod1;
	n1.end = nod2;
	n1.end->over = true;
}


//create the closure of the nfa
void closureNfa(Nfa& n){
	n.end->over = false;
	NfaNode *start = new NfaNode();
	start->over = false;
	NfaNode *end = new NfaNode();
	end->over = true;
	start->ways.insert(pair<char, NfaNode*>('$', n.start));
	start->ways.insert(pair<char, NfaNode*>('$', end));
	n.end->ways.insert(pair<char, NfaNode*>('$', end));
	n.end->ways.insert(pair<char, NfaNode*>('$', n.start));
	n.start = start;
	n.end = end;
}



//check the tokens is right or not
bool checkTokens(string& tokens, Dfa& dfa, vector<char>& rs, vector<char>& error){
	tokens += "$";
	DfaNode *node = dfa.start;
	int startp = 0;
	//cout << "last : "<< tokens[tokens.size() - 1] << endl;
	for(size_t i = 0;i < tokens.size();i ++){
		bool chg = false;
		char token = tokens[i];
		for(map<char, DfaNode*>::iterator it = node->way.begin();it != node->way.end();it ++){
			if(token == it->first){
				node = it->second;
				chg = true;
				break;
			}
		}
		if(!chg){
			for(multimap<char, DfaNode&>::iterator it = node->ways.begin();it != node->ways.end();it ++){
				if(it->first == token){
					node = &(it->second);
					chg = true;
					break;
				}
			}
		}
		if(!chg){
			if(node->over == true){
				string tmps = tokens.substr(startp, i - startp);
				//cout << "tmps: " << tmps << endl;
				// cout << "vs[tmps]: "<< vs[tmps] << endl;
				char rsc;
				if(vs.find(tmps) == vs.end()){
					//cout << "1" << endl;
					if(tmps[0] >= '0'&&tmps[0] <= '9')rsc = 'n';
					else if(tmps[0] >= 'a'&&tmps[0]<= 'z') rsc = 'd';
					else if(tmps[0] == '/')rsc = 'c';
				}else{
					rsc = vs[tmps];
				}
				//cout << "rsc : "<<rsc << endl;
				rs.push_back(rsc);
				node = dfa.start;
				if(token == ' '||token == '\t'||token == '\n'){
					startp = i + 1;
				}else{
					startp = i;
					i --;
				}
			}else if(token == ' '||token == '\t'||token == '\n'){
				startp = i + 1;
			}else{
				if(token == '$')return true;
				else{
					rs.push_back('x');
					error.push_back(token);
				}
			}
			// if(wayc.count(token) != 0||token == ' '||token == '\t'||token == '\n'){
			// 	if(node->over == true){
			// 		string tmps = tokens.substr(startp, i - startp);
			// 		rs.push_back(vs[tmps]);
			// 		startp = i + 1;
			// 		if(wayc.count(token) != 0){
			// 			startp = i; 
			// 			i --;
			// 			node = dfa.start;
			// 		}
			// 	}else{
			// 		cout << "token :" << token <<"w"<< endl;
			// 		cout << "i : "<< i << endl;
			// 		cout << "1" << endl;
			// 		return false;
			// 	}
			// }else{
			// 	cout << "2" << endl;
			// 	return false;
			// }
		}
	}
	return false;
}

//set the dfa of the lexx
void setDfa(Nfa& nfa){
	int state = 0;
	stack<vector<NfaNode*>* > vnode;
	set<vector<NfaNode*> > snode;
	map<vector<NfaNode*>, DfaNode*> m;
	vector<NfaNode*> *start = new vector<NfaNode*>();
	transnull(*(nfa.start), *start);
	DfaNode *dnode = new DfaNode();
	dnode->val = false;
	ndfa.start = dnode;
	dnode->val = state;
	state ++;
	vnode.push(start);
	snode.insert(*start);
	m.insert(pair<vector<NfaNode*>, DfaNode*>(*start, dnode));
	DfaNode *curnode = dnode;
	while(vnode.size() != 0){
		vector<NfaNode*> *tmpv = vnode.top();
		vnode.pop();
		//every terminal
		//cout << "2" << endl;
		for(set<char>::iterator it = wayc.begin();it != wayc.end();it ++){
			vector<NfaNode*> *newv = new vector<NfaNode*>();
			
			//the node in the vector
			for(vector<NfaNode*>::iterator itt = tmpv->begin();itt != tmpv->end();itt ++){
				//search the ways that match the terminal
				for(multimap<char, NfaNode*>::iterator ite = (*itt)->ways.begin();ite != (*itt)->ways.end();ite ++){
					if(ite->first == *it){
						transnull(*(ite->second), *newv);
						break;
					}
				}
			}
			if(newv->size() != 0){
				if(snode.count(*newv) != 0){
					set<vector<NfaNode*> >::iterator iter = snode.find(*newv);
					DfaNode* tmp = m[*iter];
					curnode->way.insert(pair<char, DfaNode*>(*it, tmp));
				}else{
					//cout << "newv.size()" << newv->size() << endl;
					vnode.push(newv);
					snode.insert(*newv);
					DfaNode *dfanode = new DfaNode();
					dfanode->over = false;
					for(vector<NfaNode*>::iterator itera = newv->begin();itera != newv->end();itera ++){
						if((*itera)->over == true){
							//(*itera)->print();
							dfanode->over = true;
							break;
						}
					}
					dfanode->val = state;
					state++;
					curnode->ways.insert(pair<char, DfaNode&>(*it, *dfanode));
					m.insert(pair<vector<NfaNode*>, DfaNode*>(*newv, dfanode));
				}
			}
		}
		//cout << "vnode size() : " << vnode.size() << endl;
		if(vnode.size() > 0)curnode = m[*(vnode.top())];
	}
}


//trans the null func
void transnull(NfaNode& node, vector<NfaNode*>& vnode){
	vnode.push_back(&node);
	for(multimap<char, NfaNode*>::iterator it = node.ways.begin();it != node.ways.end();it ++){
		if(it->first == '$'){
			//vnode.push_back(it->second);
			transnull(*(it->second), vnode);
		}
	}
}

//trans the nfa by c
void transnullByc(NfaNode& node, vector<NfaNode*>& vnode, char c){
	for(multimap<char, NfaNode*>::iterator it = node.ways.begin();it != node.ways.end();it ++){
		if(it->first == c){
			vnode.push_back(it->second);
			transnull(*(it->second), vnode);
		}
	}
}