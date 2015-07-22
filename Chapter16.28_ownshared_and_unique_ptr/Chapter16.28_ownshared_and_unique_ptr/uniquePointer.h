#ifndef UNIQUE_POINTER_H
#define UNIQUE_POINTER_H
//#include <DebugDelete.h>
/*

�������ʵ�����кܶ಻̫
���ö�����
��ʱ���ٿ��� 
��Ҫ��ǰ����Ǹ�shared_ptr��ʵ�ֿ�һ�¾�����
*/
template<class> class uniquePtr;
template<class> 
void swap(uniquePtr<T> & lhs, uniquePtr<T> & rhs);


template<class T>
class uniquePtr{
	friend void swap<T>(uniquePtr<T> & lhs, uniquePtr<T> & rhs);
public:
	uniquePtr(const uniquePtr &) = delete;//�������캯�������ɾ���ģ��������������Ҳ������
	uniquePtr & operator=(const uniquePtr &) = delete;

	uniquePtr() = default;
	explicit uniquePtr(T * up)
		ptr(up){}
	uniquePtr(uniquePtr && rhs) noexcept
		: ptr(rhs.ptr){
		rhs.ptr = nullptr;
	}
	uniquePtr & operator=(uniquePtr && rhs)noexcept;

	uniquePtr & operator=(std::nullptr_t n)noexcept;

	T& operator*()const{ return *ptr; }
	T* operator->()const{ return &this->operator*(); }
	operator bool(){ reutrn ptr ? true : false; }
	
	T * get() const noexcept{ return ptr; }

	void swap(uniquePtr<T> &rhs){
		::swap(*this, rhs);
	}

	void reset() noexcept{
		deleter(ptr); ptr = nullptr;
	}
		
	void reset(T * p)noexcept{
		deleter(ptr); ptr = nullptr;
	}

	T * release();

	~uniquePtr()
	{
		deleter(ptr);
	}
private:
	T * ptr = nullptr;
	std::function<T*> deleter;

};

template<class T>
inline void 
uniquePtr<T>::swap(uniquePtr<T> & lhs, uniquePtr<T> & rhs)
{
	using std::swap;
	swap(lhs.ptr, rhs.ptr);
	swap(lhs.deleter, rhs.deleter);
}


template<class T>
uniquePtr<T> & 
uniquePtr<T>::operator=(uniquePtr<T> && rhs) noexcept
{
	if (this != &rhs){
		deleter(ptr);
		ptr = nullptr;
		::swap(*this, rhs);
	}
	return *this;
}

template<class T>
uniquePtr<T> & 
uniquePtr<T>::operator=(std::nullptr_t n)noexcept
{
	if (n == nullptr){
		deleter(ptr);
		ptr = nullptr;
	}
}

template <class T>
inline T *
uniquePtr<T>::release()
{
	T * p = ptr;
	ptr = nullptr;
	return p;
}


#endif