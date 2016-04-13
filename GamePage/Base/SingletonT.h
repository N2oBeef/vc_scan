#pragma once
/********************************************************************
	created:	2012/12/07
	created:	7:12:2012   14:46
	filename: 	SingletonT.h
	file path:	FunctionModule
	file base:	SingletonT
	file ext:	h
	author:		Pan.M
	
	purpose:	简单的单价模式基类，可以派生出单件，
				不过需要声明基类是子类的friend, SINLGETON(ChildClass)可实现
				还需要把子类构造函数声明为private
*********************************************************************/

template<typename T>
class CSingletonT
{
public:
	static T * GetInstance(void)
	{
		if (s_pSinglton == NULL)
		{
			s_pSinglton = new T();
		}
		return s_pSinglton;
	}

	// must to be protected
protected:
	CSingletonT(){};

	// no implementation
private:
 	CSingletonT(const T &);
 	CSingletonT & operator= (const T & );

private:
	static T * s_pSinglton;
};

template<typename T>
T * CSingletonT<T>::s_pSinglton = NULL;

// 方便子类声明友元
#define SINGLETON(DrivedSingleton) 	friend class CSingletonT<DrivedSingleton>
