// Tray Me !.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "Tray Me !Dlg.h"

// CTrayMeApp:
// See Tray Me !.cpp for the implementation of this class
//

class CTrayMeApp : public CWinApp
{
public:
	CTrayMeApp();
	~CTrayMeApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
private:
	CTrayMeDlg * m_pApplicationDialog;
};

extern CTrayMeApp theApp;