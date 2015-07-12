#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <initializer_list>
using namespace std;

class StrBlob{
public:
	typedef vector<string>::size_type size_type;
	StrBlob(){ ; }
	StrBlob(initializer_list<string> il);
	size_type size()const;
	void push_back(const string & t);
	void pop_back();
	string & front();
	string & back();
private:
	shared_ptr<vector<string>> data;
	void check(size_type i, const string & msg)const;

};

StrBlob::StrBlob(initializer_list<string> il)
	:data(make_shared<vector<string>>(il))
{}

StrBlob::size_type StrBlob::size()const
{
	return data->size();
}

void StrBlob::push_back(const string & )