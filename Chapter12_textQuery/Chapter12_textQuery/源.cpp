#include <iostream>
#include <sstream>
#include <fstream>
#include "text_query.h"
using namespace std;

TextQuery::TextQuery(ifstream & ifile)
	:file(new vector<string>)
{
	string temp;
	while (getline(ifile, temp))
	{
		file->push_back(temp);
		int n = file->size() - 1;
		istringstream iss(temp);
		string tmp;
		while (iss >> tmp){
			auto & lines = wm[tmp];
			if (!lines)
				lines.reset(new set<line_no>);
			lines->insert(n);
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

QueryResult::iter QueryResult::begin()const
{
	return lines->begin();
}

QueryResult::iter QueryResult::end()const
{
	return lines->end();
}

shared_ptr<vector<string>> QueryResult::get_file()const
{
	return shared_ptr<vector<string>>(make_shared<vector<string>>(file));
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

QueryResult	OrQuery::eval(const TextQuery & text)const
{
	auto right = rhs.eval(text), left = lhs.eval(text);
	auto ret_lines =
		make_shared<set<line_no>>(left.begin(), right.end());
	ret_lines->insert(right.begin(), right.end());
	return QueryResult(rep(), left.get_file(), ret_lines);
}

QueryResult AndQuery::eval(const TextQuery & text)const
{
	auto right = rhs.eval(text), left = lhs.eval(text);
	auto ret_lines =
		make_shared<set<line_no>>(left.begin(), left.end());
	ret_lines->insert(right.begin(), right.end());
	return QueryResult(rep(), left.get_file(), ret_lines);
}


QueryResult NotQuery::eval(const TextQuery & text)const
{
	auto result = query.eval(text);
	auto ret_lines = make_shared<set<line_no>>();
	auto sz = result.get_file()->size();
	auto beg = result.begin(), ed = result.end();
	for (size_t n = 0; n != sz; ++n){
		if (beg == ed || *beg != n)
			ret_lines->insert(n);
		else
			++beg;//注意，这里处理的很精妙
	}
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

