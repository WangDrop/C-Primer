#ifndef TEXT_QUERY_H
#define TEXT_QUERY_H
#include <iostream>
#include <vector>
#include <memory>
#include <set>
#include <map>
using namespace std;
class QueryResult;

//eval:接受一个text_query对象并且返回一个query_result，eval函数使用给定的textQuery对象并且查找与之匹配的行
//rep：返回基础查询的string表示形式，eval使用rep创建一个表示匹配结果的query_result，输出运算符使用rep来打印查询表达式

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