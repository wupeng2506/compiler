#include "nfa.h"

///////////////////////////////////////////
vector<TableCol*>* createTable(ifstream& in);
void setBNFs(string s, vector<bnf>& v, vector<lr0_bnf>& vv);
void transnull(vector<lr0_bnf>& v,DfaNode& node, lr0_bnf& bnf);
void expand(DfaNode& start);
void setLaLr(DfaNode& node);
void setFirst(vector<bnf>& v, map<char, set<char> >& m);
void setTable(DfaNode& node);
bool checkBnf(vector<TableCol*>& table, string& s);
void release();
////////////////////////////////////////////////
Dfa dfa;
vector<DfaNode*> vdfa;//储存所有nod
vector<bnf> vbnf;//mem all the bnf
vector<lr0_bnf> vlr0;//所有的lr0_bnf
set<char> tmls;//save all the terminals and the non-terminals
vector<TableCol*> table;//save the table;
map<char, set<char> > first;//mem all the first
set<int> alr;//the nodes that are alreay expanded
int state(0);
//////////////////////////////////////////////
// int main(){
// 	ifstream in("parse.txt");
// 	createTable(in);
// 	return 0;
// }

void release(){
	dfa.start = NULL;
	vdfa.clear();
	vbnf.clear();
	vlr0.clear();
	tmls.clear();
	table.clear();
	first.clear();
	alr.clear();
	state = 0;
}

vector<TableCol*>* createTable(ifstream& in){
	if(!in.is_open()){cout<< "open bnf file error@_@ " << endl; exit(1);}
	string bufs;
	vector<bnf> vbnf;
	while(!in.eof()){
		getline(in, bufs);
		setBNFs(bufs, vbnf, vlr0);//set BNF
	}
	DfaNode start;
	start.sv.push_back(vlr0[0]);
	transnull(vlr0, start, vlr0[0]);
	vdfa.push_back(&start);
	dfa.start = &start;
	start.val = state;
	state ++;
	expand(start);
	//test
	setFirst(vbnf, first);
	start.sv[0].lalr1.insert('$');//set the first lalr1
	setLaLr(start);
	setTable(start);
	//print(&start);
	printTable(table);
	// string s = "vd(v){id;f(don)d=n;ed=n;r;}";
	// if(checkBnf(table, s)) cout << "OK" << endl;
	// else cout << "NO" << endl;
	return &table;
}

//judge the string is right to bnf or not
bool checkBnf(vector<TableCol*>& table, string& s){
	s = s + "$";
	vector<size_t> stk;
	size_t line = 0; 
	vector<char> tokens;
	for(int i = s.size() - 1;i >= 0;i --){
		tokens.push_back(s[i]);
	}
	while(tokens.size() > 0){
		char token = tokens[tokens.size() - 1];
		tokens.pop_back();
		cout << "token: " << token << endl;
		cout << "line : "<< line << endl;
		for(multimap<char, tableNode>::iterator it = table[line]->col.begin();it != table[line]->col.end();it ++){
			if(it->first == token){
				//s happens
				if(it->second.slt == 's'){
					stk.push_back(line);
					line = it->second.tm;
					break;
				}else if(it->second.slt == 'r'){
				//r happens
					if(it->second.tm == 0)return true;
					bnf *tmpb = &vbnf[it->second.tm];
					for(size_t ii = 0;ii < tmpb->after.size();ii ++){
						line = stk[stk.size() - 1];
						stk.pop_back();						
					}
					tokens.push_back(token);
					tokens.push_back(tmpb->before);
					break;
				}
			}
			if(++it == table[line]->col.end())return false;
			it --;
		}
	}
	return false;
}

//根据读入的字符串创建相应的BNF和lr0的BNF..
void setBNFs(string s, vector<bnf>& v, vector<lr0_bnf>& vv){
	if(s.size() < 3)return;
	bnf b(s[0]);
	b.setAfter(s);
	vbnf.push_back(b);
	v.push_back(b);
	for(int i = 3;i <= s.size();i ++){
		lr0_bnf bb;
		bb.fromBNF(b, i - 3);//初始化
		vv.push_back(bb);
	}
}

void setFirst(vector<bnf>& v, map<char, set<char> >& m){
	bool b = true;
	set<char> nont;//all the non-terminal
	set<char> over;//all the over nonteminal
	for(vector<bnf>::iterator i = v.begin();i != v.end();i++){
		nont.insert((*i).before);
	}
	while(b){
		b = false;
		for(vector<bnf>::iterator i = v.begin();i != v.end();i++){
			if((*i).after.size() != 0){
				char temp = (*i).after[0];
				char bef = (*i).before;
				//cout << temp << endl;
				if((temp >= 'A'&&temp <= 'Z')||(temp >= 'x'&&temp <= 'z')){
					int befs = m[bef].size();
					//add the fisrst to now
					for(set<char>::iterator it = m[temp].begin();it != m[temp].end();it ++){
						m[bef].insert((*it));
					}
					if(befs != m[bef].size())b = true;
					//over.insert(bef);
				}else if((temp < 'A'||temp > 'Z')&&(temp < 'x'||temp > 'z')){
					//over.insert(temp);
					int befs = m[bef].size();
					m[bef].insert(temp);
					if(befs != m[bef].size())b = true;
					//over.insert(bef);
				}
			}
		}
	}
}

//DFA的空变换
void transnull(vector<lr0_bnf>& v,DfaNode& node, lr0_bnf& bnf){
	//cout << "null"  << node.sv.size()<< endl;
	//bnf.print();
	size_t bef = node.sv.size();
	if(bnf.after.size() == 0)return;
	char temp = (bnf.after)[0];
	vector<char> vv;//the else character
	set<char> ss;//save the non terminals
	vector<lr0_bnf>::iterator it;
	//cout << "20.10" << endl;
	vv.push_back(temp);
	ss.insert(temp);
	for(size_t i = 0;i < vv.size();i ++){
		temp = vv[i];
		//cout << "vv size" << temp << endl;
		//cout << "temp :" << temp << endl;
		it = v.begin();
		while(it != v.end()){
			if((*it).before == temp&&(*it).checked.size() == 0){
				node.sv.push_back(*it);
				if((*it).after.size() != 0&&(*it).after[0] != temp&&ss.count((*it).after[0]) == 0&&(((*it).after[0] >= 'A'&&(*it).after[0] <= 'Z')||((*it).after[0] >= 'x'&&(*it).after[0] <= 'z'))){
					vv.push_back((*it).after[0]);//add into vv
					ss.insert((*it).after[0]);
					//cout << "put in :"<<(*it).after[0] << endl;
				}
			}
			it ++;
		}
	}
}

//扩展DFA
void expand(DfaNode& start){
	//cout << start.val << endl;
	if(alr.count(start.val) == 0)alr.insert(start.val);
	else return;
	vector<lr0_bnf>::iterator it = start.sv.begin();
	set<char> next;
	//find the next char
	while(it != start.sv.end()){
		if((*it).after.size() != 0){
			next.insert((*it).after[0]);//扩展所需的字
		}
		it ++;
	}
	if(next.size() == 0)return;
	//set up the next state
 	for(set<char>::iterator i = next.begin();i != next.end();i ++){
		DfaNode *node = new DfaNode();//set up a new node
		it = start.sv.begin();
		while(it != start.sv.end()){
			if((*it).after.size() > 0&&(*it).after[0] == *i){
				lr0_bnf tmpb = *it;
				tmpb.moveForward();
				(node->sv).push_back(tmpb);
			}
			it ++;
		}
		if(node->sv.size() > 20){
			return ;
		}
		//cout << *i << "	node size(): " << node->sv.size() << endl;
		DfaNode* tmp = find(node, vdfa);
		if(tmp != NULL){
			start.way.insert(pair<char, DfaNode*>(*i, tmp));
		}
		if(tmp == NULL){
			tmp = node;//tmp is the new state
			size_t bef  = (tmp->sv).size();
			for(size_t ii = 0;ii < bef;ii ++){
				transnull(vlr0, *tmp, (tmp->sv)[ii]);
			}
			tmp->val = state;
			vdfa.push_back(tmp);
			state ++;
			start.ways.insert(pair<char, DfaNode&>(*i, *tmp));
		}
	}
	multimap<char, DfaNode&>::iterator iit = start.ways.begin();
	while(iit != start.ways.end()){
		expand(iit->second);
		iit ++;
	}
}



//set the lalr1 in the lr0 node
void setLaLr(DfaNode& node){
	//make the null trans
	for(vector<lr0_bnf>::iterator i = node.sv.begin();i != node.sv.end();i ++){
		//if((*i).lalr1.size()  == 0){
			for(vector<lr0_bnf>::iterator it = node.sv.begin();it != node.sv.end();it ++){
				
				//trans the null
				if((*it).after.size() != 0&&(*it).after[0] == (*i).before){
					for(set<char>::iterator iii = (*it).lalr1.begin();iii != (*it).lalr1.end();iii ++){
						(*i).lalr1.insert(*iii);
					}
					//char temp = (*it).after[0];
					char temp = (*it).after.size() > 1?(*it).after[1]:'$';
					// if(temp == 'S')cout << (*i).before << endl;
					//the next char is a non terminal
					if((temp >= 'A'&&temp <= 'Z')||(temp >= 'x'&&temp <= 'z')){
						for(set<char>::iterator itt = first[temp].begin();itt != first[temp].end();itt ++){
							(*i).lalr1.insert(*itt);
						}
					}else{
						(*i).lalr1.insert(temp);//temp is a terminal
					}
				}
			}
		//}
	}

	//sort the nodes by states
	set<int> tmp;
	for(map<char, DfaNode*>::iterator it = node.way.begin();it != node.way.end();it ++){
		int temp = (it->second)->val;
		tmp.insert(temp);
	}
	for(multimap<char, DfaNode&>::iterator it = node.ways.begin();it != node.ways.end();it ++){
		int temp = (it->second).val;
		tmp.insert(temp);
	}
	//change the state into another state node
	//by sort
	set<DfaNode*> tmpv;
	for(set<int>::iterator i = tmp.begin();i != tmp.end();i ++){
		//the way
		for(map<char, DfaNode*>::iterator it = node.way.begin();it != node.way.end();it ++){
			//(it->second->sv)[0].print();
			if((it->second)->val == *i){
				DfaNode* tmpn = it->second;
				//the lr0_bnf in node
				for(vector<lr0_bnf>::iterator iter = node.sv.begin();iter != node.sv.end();iter++){
					if((*iter).after.size() != 0&&it->first == (*iter).after[0]){
						//in another node

						for(vector<lr0_bnf>::iterator ite = (tmpn->sv).begin();ite != (tmpn->sv).end();ite ++){
							//(*ite).print();
							if((*iter).before == (*ite).before&&(*ite).checked.size() != 0&&(*ite).checked[(*ite).checked.size() - 1] == it->first){
								//copy the lalr1
								//(it->second->sv)[0].print();	
								int beff = (*ite).lalr1.size();
								for(set<char>::iterator ii = (*iter).lalr1.begin();ii != (*iter).lalr1.end();ii ++){
									(*ite).lalr1.insert(*ii);
									// (*ite).print();
									// cout << *ii << endl;
								}
								if(beff != (*ite).lalr1.size())tmpv.insert(it->second);
							}
							
						}
					}
					
				}
			}

		}
		//the ways god forgive me
		for(multimap<char, DfaNode&>::iterator it = node.ways.begin();it != node.ways.end();it ++){
			if((it->second).val == *i){
				//cout << *i << endl;
				DfaNode* tmpn = &(it->second);
				//the lr0_bnf in node
				for(vector<lr0_bnf>::iterator iter = node.sv.begin();iter != node.sv.end();iter++){
					if((*iter).after.size() != 0&&it->first == (*iter).after[0]){
						//in another node
						for(vector<lr0_bnf>::iterator ite = (tmpn->sv).begin();ite != (tmpn->sv).end();ite++){

							if((*iter).before == (*ite).before&&(*ite).checked.size() != 0&&(*ite).checked[(*ite).checked.size() - 1] == it->first){
								//copy the lalr1
								int beff = (*ite).lalr1.size();
								for(set<char>::iterator ii = (*iter).lalr1.begin();ii != (*iter).lalr1.end();ii ++){
									(*ite).lalr1.insert(*ii);
								}
								if(beff != (*ite).lalr1.size())tmpv.insert(&(it->second));
							}
						}
					}
				}
			}
		}	
	}

	//go to another node 
	//cout << node.val << endl;
	// for(set<int>::iterator it = tmp.begin();it != tmp.end();it ++){
	// 	for(multimap<char, DfaNode&>::iterator itt = node.ways.begin();itt != node.ways.end();itt ++){
	// 		if((itt->second).val  == *it&&*it != node.val){
	// 			setLaLr(itt->second);
	// 			break;
	// 		}
	// 	}
	// 		for(set<DfaNode*>::iterator itt = tmpv.begin();itt != tmpv.end();itt ++){
	// 				setLaLr(*(*itt)); 
	// 		}
	// // }
	// for(set<DfaNode&>::iterator it = tmpw.begin();it != tmpw.end();it ++){
	// 	setLaLr(*it);
	// }
	for(set<DfaNode*>::iterator itt = tmpv.begin();itt != tmpv.end();itt ++){
		setLaLr(*(*itt)); 
	}
}

//set the table
void setTable(DfaNode& node){
	TableCol *coll = new TableCol();
	coll->val = node.val;
	//where s happens
	set<char> tmps;//save the s node number
	for(multimap<char, DfaNode&>::iterator it = node.ways.begin();it != node.ways.end();it ++){
		tableNode tnode((it->second).val, 's');
		coll->col.insert(pair<char, tableNode>(it->first, tnode));
		tmls.insert(it->first);
		tmps.insert(it->first);
	}
	for(map<char, DfaNode*>::iterator it = node.way.begin();it != node.way.end();it ++){
		tableNode tnode((it->second)->val, 's');
		coll->col.insert(pair<char, tableNode>(it->first, tnode));
		tmls.insert(it->first);
		tmps.insert(it->first);
	}
	//the situation that r happens

	for(vector<lr0_bnf>::iterator i = node.sv.begin();i != node.sv.end();i ++){
		if((*i).after.size() == 0){
			for(size_t it = 0;it < vbnf.size();it ++){
				if((*i).equalbnf(vbnf[it])){
					tableNode tnode(it, 'r');
					for(set<char>::iterator ite = (*i).lalr1.begin();ite != (*i).lalr1.end(); ite++){
						if(tmps.count(*ite) == 0){
							coll->col.insert(pair<char, tableNode>(*ite ,tnode));
							tmls.insert(*ite);
						}
					}
				}
			}
		}
	}
	
	// if(node.ways.size() == 0&&node.way.size() == 0){
	// 	for(vector<lr0_bnf>::iterator i = node.sv.begin();i != node.sv.end();i ++){
	// 		for(size_t it = 0;it < vbnf.size();it ++){
	// 			if((*i).equalbnf(vbnf[it])){
	// 				tableNode tnode(it, 'r');
	// 				for(set<char>::iterator ite = (*i).lalr1.begin();ite != (*i).lalr1.end(); ite++){
	// 					coll->col.insert(pair<char, tableNode>(*ite ,tnode));
	// 					tmls.insert(*ite);
	// 				}
	// 				break;
	// 			}
	// 		}
	// 	}
	// }else{
	// 	//the situation that s happened;
		
	// }
	table.push_back(coll);
	//go on !!
	for(multimap<char, DfaNode&>::iterator it = node.ways.begin();it != node.ways.end();it ++){
		setTable(it->second);
	}
}
