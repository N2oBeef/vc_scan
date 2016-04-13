#pragma once

#include "IEngine.h"

namespace Engine
{
    class CFixCallbackOnUI
        : public IFixCallback
    {
    public:
        CFixCallbackOnUI();
        ~CFixCallbackOnUI();

        void SetCallback(IFixCallback *pCallback)
        {
            ATLASSERT(pCallback);
            m_pFixCallback = pCallback;
        }
    public:
        virtual void __stdcall OnFixStart(void);
        virtual void __stdcall OnFixItemStart(IItem *pItem);
        virtual void __stdcall OnFixItemEnd(IItem *pItem, BOOL bSuccess);
		virtual void __stdcall OnFixEvent(IItem *pItem, int nEvent);
        //nProgress´Ó0-100
        virtual void __stdcall OnFixProgress(IItem *pItem, int nProgress);

        virtual void __stdcall OnFixEnd(void);

    private:
        void OnFixStartOnUI(void);
        void OnFixItemStartOnUI(IItem *pItem);
        void OnFixItemEndOnUI(IItem *pItem, BOOL bSuccess);

        //nProgress´Ó0-100
        void OnFixProgressOnUI(IItem *pItem, int nProgress);

        void OnFixEndOnUI(void);
		void OnFixEventOnUI(IItem *pItem, int nEvent);

    private:
        IFixCallback *m_pFixCallback;
    };
}
