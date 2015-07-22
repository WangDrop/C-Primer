#include <iostream>
#include <vector>
#include <string>
#include <initializer_list>
#include <algorithm>
#include <memory>
#include <stdexcept>
using namespace std;
template<class T>
class Blob{
public:
	typedef T value_type;
	typedef typename vector<T>::size_type size_type;
	Blob();
	Blob(initializer_list<T> il);
	size_type size()const{
		return data->size();
	}
	bool empty()const;
	void push_back(const T & t){
		data->push_back(t);
	}
	void push_back(T && t){
		data->push_back(std::move(t));
	}
	void pop_back(){
		data->pop_back();
	}
	T &back();
	T back()const;
	T &operator[](size_type i);
	T operator[](size_type i)const;
private:
	shared_ptr<vector<T>> data;
	void check(size_type i, const string & msg)const;

};

template<class T>
Blob<T>::Blob()
	:data(make_shared<vector<T>>()){}

template<class T>
Blob<T>::Blob(initializer_list<T> il)
	: data(make_shared<vector<T>>(il)){}

template<class T>
void Blob<T>::check(size_type i, const string & msg)const
{
	if (i >= data->size())
		throw std::out_of_range(msg);
}

template<class T>
T & Blob<T>::operator[](size_type i)
{
	check(i, "sub out of range");
	return (*data)[i];
}

template<class T>
T Blob<T>::operator[](size_type i)const
{
	check(i, "sub out of range");
	return (*data)[i];
}

template<class T>
T &Blob<T>::back()
{
	check(0, "back on  empty Blob");
	return data->back();
}

template<class T>
T Blob<T>::back()const
{
	check(0, "back on empty Blob");
	return data->back();
}


int main()
{
	Blob<int> squares = {0,1,2,3,4,5,6,7,8,9};
	for (size_t i = 0; i < squares.size(); ++i){
		squares[i] = i * i;
	}
	cin.get();
	return 0;
}