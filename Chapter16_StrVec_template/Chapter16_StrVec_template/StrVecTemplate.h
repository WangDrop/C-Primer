#ifndef STRVEC_TEMPLATE
#define STRVEC_TEMPLATE
#include <iostream>
#include <memory>
#include <string>
template <class T>
class StrVec{
public:
	StrVec()
		:elements(nullptr), firstFree(nullptr), last(nullptr){}
	StrVec(const StrVec &);
	StrVec & operator=(const StrVec &);
	~StrVec();
	void push_back(const T &);
	size_t size()const{ return first_free - elements; }
	size_t capacity() const{ return last - elements;  }
	T *begin() const{ return elements; }
	T *end(){ return first_free; }
private:
	static allocator<T> alloc;
	void checkAlloc()
	{
		if (size() == capacity())
			reallocate();
	}
	pair<T *, T *>allocCopy(const T *, const T*);
	void free();
	void reallocate();
	T * elements;
	T * firstFree;
	T * last;
};
#endif