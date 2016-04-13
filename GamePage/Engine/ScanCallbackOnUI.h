#pragma once

#include "IEngine.h"

namespace Engine
{
	//这个类会封装一个继承IScanCallback，并且有虚函数，可以实现回调操作继承IScanCallback的类
	class CScanCallbackOnUI
		: public IScanCallback
	{
	public:
		CScanCallbackOnUI();
		~CScanCallbackOnUI();
	
		void SetCallback(IScanCallback *pCallback)
		{
			ATLASSERT(pCallback);
			m_pScanCallback = pCallback;
		}
	public:
		virtual void __stdcall OnScanStart(void);
		virtual void __stdcall OnScanItemStart(IItem *pItem);
		virtual void __stdcall OnScanItemEnd(IItem *pItem);

		//nProgress从0-100
		virtual void __stdcall OnScanProgress(int nProgress);

		virtual void __stdcall OnScanEnd(void);

	private:
		void OnScanStartOnUI(void);
		void OnScanItemStartOnUI(IItem *pItem);
		void OnScanItemEndOnUI(IItem *pItem);

		//nProgress从0-100
		void OnScanProgressOnUI(int nProgress);

		void OnScanEndOnUI(void);

	private:
		IScanCallback *m_pScanCallback;
	};
}
