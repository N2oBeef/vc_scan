#pragma once

/********************************************************************
	created:	2012/12/07
	created:	7:12:2012   14:59
	filename: 	ContainerT.h
	file path:	FunctionModule
	file base:	ContainerT
	file ext:	h
	author:		Pan.M
	
	purpose:	简单的容器包装类， 提供添加删除之类的操作
*********************************************************************/

#include <vector>
#include <algorithm>
using namespace std;

template<typename T>
class CContainerT
{	
public:
	BOOL AddItem(const T & item)
	{	
		if (IsItemExist(item)) return FALSE;
		
		m_Container.push_back(item);

		return TRUE;
	}

	BOOL RemoveItem(const T & item)
	{
		ItemItor it = find(m_Container.begin(), m_Container.end(), item);
		if (it != m_Container.end())
		{
			m_Container.erase(it);

			return TRUE;
		}

		return FALSE;
	}

	BOOL IsItemExist(const T & item)
	{
		ItemItor itFind;
		ItemItor itEnd = m_Container.end();
		itFind = find(m_Container.begin(), m_Container.end(), item);

		return itFind != itEnd;
	}

	int GetCount(void)
	{
		return static_cast<int>(m_Container.size());
	}

	T GetItem(int nIndex)
	{
		int nCount = GetCount();
		if (nIndex < 0 || nIndex >= nCount) return NULL;
		
		return *(m_Container.begin() + nIndex);
	}
	
	void Clear(void)
	{
		m_Container.clear();
	}

private:
	typedef typename vector<T> _Container;
	typedef typename _Container::iterator ItemItor;
	typedef typename _Container::value_type ValueType;

	_Container  m_Container;
};

