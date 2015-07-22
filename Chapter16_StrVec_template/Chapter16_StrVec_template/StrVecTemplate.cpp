#include "StrVecTemplate.h"
using namespace std;

template<class T>
void StrVec<T>::push_back(const T & obj)
{
	checkAlloc();
	alloc.construct(first_free++, obj);
}

template<class T>
pair<T *, T*>
StrVec<T>::alloc_n_copy(const T * b, const T * e)
{
	auto data = alloc.allocate(e - b);
	return{ data, uinitialzed_copy(b, e, data) };
}

template<class T>
void StrVec<T>::free()
{
	if (elements != firstFree){
		for (auto p = firstFree; p != elements; /*这里写成空，注意这里的含义*/){
			alloc.destroy(--p);
		}
		alloc.deallocate(elements, last - elements);
	}
}

template<class T>
StrVec<T>::StrVec(const StrVec &s)
{
	auto newdata = allocCopy(s.begin(), s.end());
	elements = newdata.first;
	firstFree = last = newdata.second;
}

template<class T>
StrVec<T>::StrVec()
{
	free();
}

template<class>
StrVec<T> & StrVec<T>::operator=(const StrVec & rhs)
{
	auto data = allocCopy(rhs.begin(), rhs.end());
	free();
	elements = data.first;
	firstFree = last = data.second;
	return *this;
}

template<class T>
void StrVec<T>::reallocate()
{
	auto newcapacity = size() ? 2 * size() : 1;
	auto data = alloc.allocate(newcapacity);
	auto dest = data;
	auto elem = elements;
	for (size_t i = 0; i != size(); ++i){
		alloc.construct(dest++, std::move(*elem++));
	}
	free;
	elements = data;
	firstFree = dest;
	last = elements + capacity;
}