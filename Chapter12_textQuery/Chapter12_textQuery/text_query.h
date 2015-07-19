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
	typedef set<line_no>::iterator iter;
	iter begin()const;
	iter end()const;
	shared_ptr<vector<string>> get_file()const;
private:
	string str;
	shared_ptr<vector<string>> file;
	shared_ptr<set<line_no>> lines;
};

class QueryBase{
	friend class Query;
public:
	typedef TextQuery::line_no line_no;
	virtual ~QueryBase() = default;
private:
	virtual QueryResult eval(const TextQuery & )const = 0;
	virtual string rep()const = 0;
};

class Query{
	friend Query operator~(const Query &);
	friend Query operator&(const Query &, const Query &);
	friend Query operator|(const Query &, const Query &);
	friend ostream & operator<<(ostream & os, const Query & query)
	{
		return os << query.rep() << endl;
	}
public:
	Query(const string &);
	QueryResult eval(const TextQuery & t)const {
		return q->eval(t);
	}
	string rep() const{
		return q->rep();
	}
private:
	Query(shared_ptr<QueryBase> query) :
		q(query){;}
	shared_ptr<QueryBase> q;
};

class  WordQuery : public QueryBase {	//û�й��г�Ա����Ϊ�ӿ���Ӧ����Query��
	friend class Query;
	WordQuery(const string & s)
		:query_word(s){}
	QueryResult eval(const TextQuery & q){
		return q.query(query_word);
	}
	string rep()const {
		return query_word;
	}
	string query_word;

};

class NotQuery : public QueryBase{
	friend class Query;
	friend Query operator~(const Query &);
	NotQuery(const Query & q) :query(q){}
	string  rep()const{
		return "~(" + query.rep() + ")";
	}
	QueryResult eval(const TextQuery &) const;
	Query query;

};

class BinaryQuery : public QueryBase{
	friend class Query;
protected:
	BinaryQuery(const Query & ls, const Query & rs, const string & op)
		:lhs(ls), rhs(rs), oprd(op){}
	string rep()const {
		return "(" + lhs.rep() + " " + oprd + " " + rhs.rep() + ")";
	}
	Query lhs, rhs;
	string oprd;
};

class AndQuery : public BinaryQuery{
	friend class Query;
	friend Query operator&(const Query & ls, const Query & rs);
	AndQuery(const Query & left, const Query & right) :
		BinaryQuery(left, right, "&"){}
	QueryResult eval(const TextQuery &)const;
	string rep()const {
		return BinaryQuery::rep();
	}
};
inline Query operator&(const Query & ls, const Query & rs)
{
	return shared_ptr<QueryBase>(new AndQuery(ls, rs));
}

class OrQuery : public BinaryQuery{
	friend class Query;
	friend Query operator|(const Query & ls, const Query & rs);
	OrQuery(const Query & left, const Query & right) :
		BinaryQuery(left, right, "|"){}
	QueryResult eval(const TextQuery &)const;
	string rep()const{
		return BinaryQuery::rep();
	}
};
inline Query operator|(const Query & ls, const Query &rs)
{
	return shared_ptr<QueryBase>(new OrQuery(ls, rs));
}


#endif