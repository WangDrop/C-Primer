#include <iostream>
#include <string>
#include <initializer_list>
#include <memory>
#include <algorithm>
#include <utility>
using namespace std;
class StrVec
{
public:
	StrVec()
	:elements(nullptr), firstFree(nullptr), last(nullptr){}
	StrVec(initializer_list<string> & ls);
	StrVec(const StrVec & );
	StrVec(StrVec && s)noexcept;
	StrVec &operator=(StrVec && rhs)noexcept;
	StrVec & operator=(const StrVec & rhs);
	~StrVec();
	void pushBack(const string & );
	void popBack();
	size_t size()const{ return firstFree - elements; }
	size_t capacity()const { return last - elements; }
	string *begin()const{ return elements; }
	string *end()const{ return firstFree; }
private:
	static allocator<string> alloc;
	void checkAlloc(){
		if (size() == capacity()) reallocate();
	}
	pair<string *, string *> allocCopy(const string*, const string *);
	void free();
	void reallocate();
	string * elements;
	string * firstFree;
	string * last;
};


void StrVec::pushBack(const string & str)
{
	checkAlloc();
	alloc.construct(firstFree++, str);
}

void StrVec::popBack()
{
	alloc.destroy(--firstFree);
}

pair<string *, string*> StrVec::allocCopy(const string *
								begin, const string * end)
{
	auto data = alloc.allocate(end - begin);
	return{ data, uninitialized_copy(begin, end, data) };
}

void StrVec::free()
{
	if (elements){
		for (auto p = firstFree; p != elements;){
			alloc.destroy(--firstFree);
		}
		alloc.deallocate(elements, last - elements);
	}
}
/*
Free的另外一个版本，用for_each和lambda写的
void StrVec::free()
{
	if (elements){
	for_each(elements, firstFree, [](string * pStr){alloc.destroy(pStr); });
	}
	alloc.deallocate(elements, last - elements);
}
*/

//这是后来定义的一个移动构造函数
StrVec::StrVec(StrVec && s)noexcept
	:elements(s.elements), firstFree(s.firstFree), last(s.last)
{
	s.elements = s.firstFree = s.last = nullptr;
}
//移动赋值运算符
StrVec &StrVec::operator=(StrVec&& rhs)noexcept
{
	if (this == &rhs){
		free();
		elements = rhs.elements;
		firstFree = rhs.firstFree;
		last = rhs.last;
		elements = firstFree = last = nullptr;
	}
	return *this;
}
StrVec::StrVec(const StrVec & rhs)
{
	auto p = (const_cast<StrVec &> (rhs)).allocCopy(rhs.begin(), rhs.end());
	elements = p.first;
	firstFree = p.second;
}

StrVec::StrVec(initializer_list<string> & ls)
{
	auto p = alloc.allocate(ls.size() * 2);
	elements = p;
	for (auto p = ls.begin(); p != ls.end(); ++p){
		alloc.construct(p++, *p);
	}
	firstFree = p;
	last = elements + ls.size() * 2;
}

StrVec & StrVec::operator=(const StrVec & rhs)
{
	if (this != &rhs){
		auto p = (const_cast<StrVec &> (rhs))//这里不cast就不能通过编译
			.allocCopy(rhs.begin(), rhs.end());//不知道是为什么
		elements = p.first;
		elements = p.second;
	}
	return *this;

}

void StrVec::reallocate()
{
	auto newcapacity = size() ? size() * 2 : 1;
	auto newdata = alloc.allocate(newcapacity);
	auto dest = newdata;
	auto elem = elements;
	for (auto i = 0; i != size(); i++){
		alloc.construct(dest++, std::move(*elem++));
	}
	free();
	elements = newdata;
	firstFree = elem;
	last = elements + newcapacity;
}
