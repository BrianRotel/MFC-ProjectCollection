// Event Hooker Dll.h : main header file for the Event Hooker Dll DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CEventHookerDllApp
// See Event Hooker Dll.cpp for the implementation of this class
//

class CEventHookerDllApp : public CWinApp
{
public:
	CEventHookerDllApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
