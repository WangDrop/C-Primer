#include <iostream>
#include <string>
using namespace std;
class HasPtr
{
public:
	HasPtr(const string & s = "")
		:strPtr(new string(s)), useCount(new size_t(1)), i(0){}
	HasPtr(const HasPtr & rhs)
		:strPtr(rhs.strPtr), useCount(rhs.useCount), i(rhs.i){
		(*useCount)++;
	}
	HasPtr& operator=(const HasPtr & rhs);
	friend void swap(HasPtr & , HasPtr &);
	~HasPtr();
private:
	string * strPtr;
	size_t * useCount;
	int i;
};

inline void swap(HasPtr & lhs, HasPtr & rhs)
{
	using std::swap;//只有在添加了这一行代码之后，下面两行代码才能将原始版本的代码覆盖。
	swap(lhs.strPtr, rhs.strPtr);
	swap(lhs.useCount, rhs.useCount);
}

HasPtr& HasPtr::operator=(const HasPtr & rhs)
{
	++(*rhs.useCount);
	if (--*useCount == 0){
		delete useCount;
		delete strPtr;
	}
	strPtr = rhs.strPtr;
	useCount = rhs.useCount;
	i = rhs.i;
	return *this;
}

HasPtr::~HasPtr()
{
	if (--*useCount == 0){
		delete useCount;
		delete strPtr;
	}
}


