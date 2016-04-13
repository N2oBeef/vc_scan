/********************************************************************
	Created:	2012/06/09
	Created:	9:6:2012   21:40
	Filename: 	simple_bind.h
	File base:	simple_bind
	File ext:	h
	Author:		赵凯
	
	Purpose:	一个简化的，类似于boost里的bind的东西
*********************************************************************/

#pragma once

class ISimpleBind
{
public:
	virtual void __stdcall SimpleInvolk(void) = 0;
};


// 这一类是成员函数的bind，相对比较复杂一些
#define IMPLEMENT_DEFAULT_MF_BIND( __class )	\
	public: \
		typedef R		return_type; \
		typedef C		class_type; \
		typedef Obj		obj_type; \
		typedef MEMBER_FUNC_BIND_PROC	func_type; \
		__class(func_type pfn, Obj obj) { init(pfn, obj); } \
		__class() { init(NULL, NULL); } \
		~__class() { uninit(); } \
		void init(func_type pfn, Obj obj) { m_function = pfn; m_object = obj; } \
		void uninit() { init(NULL, NULL); } \
		void copy_default(const __class & __s) { init(__s.m_function, __s.m_object); } \
		C * raw_obj_ptr() { return &(*m_object); } \
		virtual void __stdcall SimpleInvolk(void) { Involk(); } \
	private: \
		func_type	m_function; \
		obj_type	m_object;

// 无参数
template <typename C, typename Obj, typename R>
class mf_bind_t : public ISimpleBind
{
public:
	typedef R	( C :: * MEMBER_FUNC_BIND_PROC)(void);
	IMPLEMENT_DEFAULT_MF_BIND(mf_bind_t);

public:
	mf_bind_t(const mf_bind_t & __s)
	{
		copy_default(__s);
	}

	return_type Involk() 
	{
		return (raw_obj_ptr()->*m_function)();
	}
};

template <typename C, typename Obj, typename R>
mf_bind_t< C, Obj, R > 
simple_bind( R ( C :: * func)() , Obj obj)
{
	return mf_bind_t< C, Obj, R >(func, obj);
}

// 异步的调用，分配的对象会在异步调用后删除
template <typename C, typename Obj, typename R>
mf_bind_t< C, Obj, R > *
async_simple_bind( R ( C :: * func)() , Obj obj)
{
	return new mf_bind_t< C, Obj, R >(func, obj);
}

// 1个参数
template <typename C, typename Obj, typename R, typename Arg1>
class mf_bind_t_1 : public ISimpleBind
{
public:
	typedef R	( C :: * MEMBER_FUNC_BIND_PROC)(Arg1);
	IMPLEMENT_DEFAULT_MF_BIND(mf_bind_t_1);
	
public:
	mf_bind_t_1(func_type pfn, Obj obj, Arg1 a1) 
	{
		init(pfn, obj);
		m_arg1 = a1;
	}

	mf_bind_t_1(const mf_bind_t_1 & __s) 
	{
		copy_default(__s);
		m_arg1 = __s.m_arg1;
	}

	return_type Involk() 
	{
		return (raw_obj_ptr()->*m_function)(m_arg1);
	}

	Arg1 m_arg1;
};

template <typename C, typename Obj, typename R, typename Arg1>
mf_bind_t_1< C, Obj, R, Arg1 > 
simple_bind( R ( C :: * func)(Arg1) , Obj obj, Arg1 a1)
{
	return mf_bind_t_1< C, Obj, R, Arg1 >(func, obj, a1);
}

// 异步的调用，分配的对象会在异步调用后删除
template <typename C, typename Obj, typename R, typename Arg1>
mf_bind_t_1< C, Obj, R, Arg1> *
async_simple_bind( R ( C :: * func)(Arg1) , Obj obj,Arg1 a1)
{
	return new mf_bind_t_1< C, Obj, R, Arg1>(func, obj, a1);
}

// 2个参数
template <typename C, typename Obj, typename R, typename Arg1, typename Arg2>
class mf_bind_t_2 : public ISimpleBind
{
public:
	typedef R	( C :: * MEMBER_FUNC_BIND_PROC)(Arg1, Arg2);
	IMPLEMENT_DEFAULT_MF_BIND(mf_bind_t_2);

public:
	mf_bind_t_2(func_type pfn, Obj obj, Arg1 a1, Arg2 a2) 
	{
		init(pfn, obj);
		m_arg1 = a1;
		m_arg2 = a2;
	}

	mf_bind_t_2(const mf_bind_t_2 & __s) 
	{
		copy_default(__s);
		m_arg1 = __s.m_arg1;
		m_arg2 = __s.m_arg2;
	}

	return_type Involk() 
	{
		return (raw_obj_ptr()->*m_function)(m_arg1, m_arg2);
	}

	Arg1 m_arg1;
	Arg2 m_arg2;
};

template <typename C, typename Obj, typename R, typename Arg1, typename Arg2>
mf_bind_t_2< C, Obj, R, Arg1, Arg2 > 
simple_bind( R ( C :: * func)(Arg1, Arg2) , Obj obj, Arg1 a1, Arg2 a2)
{
	return mf_bind_t_2< C, Obj, R, Arg1, Arg2 >(func, obj, a1, a2);
}


// 3个参数
template <typename C, typename Obj, typename R, typename Arg1, typename Arg2, typename Arg3>
class mf_bind_t_3 : public ISimpleBind
{
public:
	typedef R	( C :: * MEMBER_FUNC_BIND_PROC)(Arg1, Arg2, Arg3);
	IMPLEMENT_DEFAULT_MF_BIND(mf_bind_t_3);

public:
	mf_bind_t_3(func_type pfn, Obj obj, Arg1 a1, Arg2 a2, Arg3 a3) 
	{
		init(pfn, obj);
		m_arg1 = a1;
		m_arg2 = a2;
		m_arg3 = a3;
	}

	mf_bind_t_3(const mf_bind_t_3 & __s) 
	{
		copy_default(__s);
		m_arg1 = __s.m_arg1;
		m_arg2 = __s.m_arg2;
		m_arg3 = __s.m_arg3;
	}

	return_type Involk() 
	{
		return (raw_obj_ptr()->*m_function)(m_arg1, m_arg2, m_arg3);
	}

	Arg1 m_arg1;
	Arg2 m_arg2;
	Arg3 m_arg3;
};

template <typename C, typename Obj, typename R, typename Arg1, typename Arg2, typename Arg3>
mf_bind_t_3< C, Obj, R, Arg1, Arg2, Arg3 > 
simple_bind( R ( C :: * func)(Arg1, Arg2, Arg3) , Obj obj, Arg1 a1, Arg2 a2, Arg3 a3)
{
	return mf_bind_t_3< C, Obj, R, Arg1, Arg2, Arg3 >(func, obj, a1, a2, a3);
}

// 这一类是普通函数的bind，相对于成员函数的bind，就简单多了
#define IMPLEMENT_DEFAULT_F_BIND( __class )	\
	public: \
		typedef R		return_type; \
		typedef FUNC_BIND_PROC	func_type; \
		__class(func_type pfn) { init(pfn); } \
		__class() { init(NULL); } \
		~__class() { uninit(); } \
		void init(func_type pfn) { m_function = pfn; } \
		void uninit() { init(NULL); } \
		void copy_default(const __class & __s) { init(__s.m_function); } \
	private: \
		func_type	m_function; \

// 无参数
template <typename R>
class f_bind_t : public ISimpleBind
{
public:
	typedef R	(* FUNC_BIND_PROC)(void);
	IMPLEMENT_DEFAULT_F_BIND(f_bind_t);

public:
	f_bind_t(const mf_bind_t & __s)
	{
		copy_default(__s);
	}

	return_type Involk() 
	{
		return m_function();
	}
};

template <typename R>
f_bind_t< R > 
simple_bind( R (* func)() )
{
	return mf_bind_t< R >(func);
}



// 1个参数
template <typename R, typename Arg1>
class f_bind_t_1 : public ISimpleBind
{
public:
	typedef R	(* FUNC_BIND_PROC)(Arg1);
	IMPLEMENT_DEFAULT_F_BIND(f_bind_t_1);

public:
	f_bind_t_1(func_type pfn, Arg1 a1) 
	{
		init(pfn);
		m_arg1 = a1;
	}

	f_bind_t_1(const f_bind_t_1 & __s)
	{
		copy_default(__s);
		m_arg1 = __s.m_arg1;
	}

	return_type Involk() 
	{
		return m_function(m_arg1);
	}

	Arg1 m_arg1;
};

template <typename R, typename Arg1>
f_bind_t_1< R, Arg1 > 
simple_bind( R (* func)( Arg1 ), Arg1 a1 )
{
	return f_bind_t_1< R, Arg1 >(func, a1);
}

// 2个参数
template <typename R, typename Arg1, typename Arg2>
class f_bind_t_2 : public ISimpleBind
{
public:
	typedef R	(* FUNC_BIND_PROC)(Arg1, Arg2);
	IMPLEMENT_DEFAULT_F_BIND(f_bind_t_2);

public:
	f_bind_t_2(func_type pfn, Arg1 a1, Arg1 a2) 
	{
		init(pfn);
		m_arg1 = __s.m_arg1;
		m_arg2 = __s.m_arg2;
	}

	f_bind_t_2(const f_bind_t_2 & __s)
	{
		copy_default(__s);
		m_arg1 = a1;
		m_arg2 = a2;
	}

	return_type Involk() 
	{
		return m_function(m_arg1, m_arg2);
	}

	Arg1 m_arg1;
	Arg2 m_arg2;
};

template <typename R, typename Arg1, typename Arg2>
f_bind_t_2< R, Arg1, Arg2 > 
simple_bind( R (* func)( Arg1, Arg2 ), Arg1 a1, Arg2 a2 )
{
	return f_bind_t_1< R, Arg1, Arg2 >(func, a1, a2);
}
