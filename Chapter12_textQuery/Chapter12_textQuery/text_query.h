#ifndef TEXT_QUERY_H
#define TEXT_QUERY_H
#include <iostream>
#include <vector>
#include <memory>
#include <set>
#include <map>
using namespace std;
class QueryResult;

//eval:����һ��text_query�����ҷ���һ��query_result��eval����ʹ�ø�����textQuery�����Ҳ�����֮ƥ�����
//rep�����ػ�����ѯ��string��ʾ��ʽ��evalʹ��rep����һ����ʾƥ������query_result����������ʹ��rep����ӡ��ѯ���ʽ

class TextQuery{
public:
	typedef vector<string> ::size_type line_no;
	TextQuery(ifstream &);
	QueryResult query(const string &)const;
private:
	shared_ptr<vector<string>> file;
	map < string, shared_ptr<set<line_no>> > wm;
};

class QueryResult
{
	friend ostream &  print(ostream & os, const QueryResult & qur);
public:
	typedef vector<string>::size_type line_no;

	//	QueryResult();
	QueryResult(const string & s,
		shared_ptr<vector<string>> f, shared_ptr<set<line_no>> l)
		:str(s), file(f), lines(l){}
private:
	string str;
	shared_ptr<vector<string>> file;
	shared_ptr<set<line_no>> lines;
};

#endif