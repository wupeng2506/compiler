#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include "dfa.cc"

using namespace std;

int main(){
	ifstream in("program.c-");
	ifstream parse("parse.txt");
	if(!in.is_open()||!parse.is_open()){cout << "program file can not open @_@" << endl;exit(0); }
	string tokens = rtnTokens(in);
	cout << tokens << endl;
	vector<TableCol*>* tb = createTable(parse);
	if(checkBnf(*tb, tokens))cout << "OK" << endl;
	else cout<< "NO" << endl;
	in.close();
	parse.close();
	release();
	return 0;
}
