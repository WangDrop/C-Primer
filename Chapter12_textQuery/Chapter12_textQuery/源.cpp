#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <set>

using namespace std;

class QueryResult;
class TextQuery{
public:
	typedef vector<string> ::size_type line_no;
	TextQuery(ifstream & );
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


TextQuery::TextQuery(ifstream & ifile)
	:file(new vector<string>)
{
	string temp;
	while (getline(ifile, temp))
	{
		file->push_back(temp);
	}
	line_no line_count = 1;
	for (auto it = file->begin(); it != file->end(); ++it, ++line_count)
	{
		cout << *it << endl;
		istringstream iss(*it);
		string tmp;
		while (iss >> tmp)
		{
			auto & lines = wm[tmp];
			if (!lines)
				lines.reset(new set<line_no>);
			lines->insert(line_count);
		}
	}
}

QueryResult TextQuery::query(const string & str) const
{
	static shared_ptr<set<line_no>> nodata(new set<line_no>);
	auto pos = wm.find(str);
	if (pos != wm.end())
		return QueryResult(str, file, pos->second);
	else
		return QueryResult(str, file, nodata);
}





ostream &  print(ostream & os, const QueryResult & qur)
{
	os << qur.str << " occers " << qur.lines->size() << ((qur.lines->size() > 1) ? " time " : " times") << endl;
	for (set<QueryResult::line_no>::iterator it = qur.lines->begin(); it != qur.lines->end(); ++it)
	{
		os << "\t(line " << *it + 1 << ") " << (*qur.file)[*it] << endl;
	}
	return os;

}
int main()
{
	ifstream infile("C:\\Users\\WC\\Documents\\Visual Studio 2013\\Projects\\Chapter12_textQuery\\Chapter12_textQuery\\sample.txt");
	TextQuery text(infile);
	while (true)
	{
		cout << "Enter a word to check, and q for quit : ";
		string s;
		if (!(cin >> s) || s == "q") break;
		print(cout, text.query(s)) << endl;
	}

}

