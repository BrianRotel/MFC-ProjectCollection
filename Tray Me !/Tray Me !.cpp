// Tray Me !.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Tray Me !.h"
#include "Tray Me !Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef bool(*InstallHook)();
typedef bool(*UnInstallHook)();
typedef void (*UnTrayAll)();
typedef UINT (*GetTrayMessageID)();

UINT g_uiTrayMessageID = 0;
// CTrayMeApp

BEGIN_MESSAGE_MAP(CTrayMeApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

extern void DebugPrintErrorMessage(TCHAR *pszErrorString = NULL, bool bDisplayMsg = false, TCHAR *pszMsgCaption = NULL);


// CTrayMeApp construction

CTrayMeApp::CTrayMeApp()
: m_pApplicationDialog(NULL)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CTrayMeApp::~CTrayMeApp()
{
}

// The one and only CTrayMeApp object

CTrayMeApp theApp;


// CTrayMeApp initialization

BOOL CTrayMeApp::InitInstance()
{
	BOOL bReturn = FALSE;
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	//Load the Hooker dll
	HMODULE hModule = LoadLibrary(_T("Event Hooker Dll.dll"));

	try
	{
		if(NULL == hModule)
		{
			MessageBox(NULL,_T("Failed to Load Hooker Dll."),_T("Hooking Dlg"),MB_TOPMOST | MB_ICONERROR);
			throw FALSE;
		}

		//Get the function address which installs the windows procedure filter.
		InstallHook fpInstallHook = (InstallHook) GetProcAddress(hModule,"InstallWinProcHook");
		if(NULL == fpInstallHook)
		{
			MessageBox(NULL,_T("Failed to get InstallWinProcHook function address."),_T("Tray Me !"),0);
			throw FALSE;
		}

		//Install the Windows Procedure hook (Filter).
		if(!fpInstallHook())
		{
			MessageBox(NULL,_T("Failed to install hook."),_T("Tray Me !"),MB_TOPMOST | MB_ICONERROR);
			throw FALSE;
		}

		//Get the function address which installs the mouse events filter.
		InstallHook fpMouseHook = (InstallHook) GetProcAddress(hModule,"InstallMouseHook");

		if(NULL == fpMouseHook)
		{
			MessageBox(NULL,_T("Failed to get InstallMouseHook function address."),_T("Tray Me !"),MB_TOPMOST | MB_ICONERROR);
			throw FALSE;
		}

		//Install the Mouse Event Hook (Filter).
		if(!fpMouseHook())
		{
			MessageBox(NULL,_T("Failed to install Mouse Hook."),_T("Tray Me !"),MB_TOPMOST | MB_ICONERROR);
			throw FALSE;
		}

		GetTrayMessageID fpGetTrayMessageID = (GetTrayMessageID) GetProcAddress(hModule,"GetTrayMessageID");

		if(NULL == fpGetTrayMessageID)
		{
			MessageBox(NULL,_T("Failed to get GetTrayMessageID function address."),_T("Tray Me !"),MB_TOPMOST | MB_ICONERROR);
			throw FALSE;
		}

		//Install the Mouse Event Hook (Filter).
		if(0 == (g_uiTrayMessageID = fpGetTrayMessageID()))
		{
			MessageBox(NULL,_T("Failed to retrieve TrayMessage Id from dll."),_T("Tray Me !"),MB_TOPMOST | MB_ICONERROR);
			throw FALSE;
		}

		//Create a invisible modless dialog.
		m_pApplicationDialog = new CTrayMeDlg();
		m_pApplicationDialog->Create(IDD_TRAYME_DIALOG,NULL);
		m_pMainWnd = m_pApplicationDialog;
		ShowWindow(m_pMainWnd->GetSafeHwnd(),SW_HIDE);

		bReturn = TRUE;
	}
	catch(BOOL bThrownVal)
	{
		bReturn = bThrownVal;
		OutputDebugString(_T("\nCTrayMeApp::InitInstance:\tCustom Exception Caught."));
		DebugPrintErrorMessage();
	}
	catch(...)
	{
		bReturn = FALSE;
		DebugPrintErrorMessage(_T("\nCTrayMeApp::InitInstance:\tException Caught."));
	}

	if(hModule)
		FreeLibrary(hModule);

	return bReturn;

	/*CTrayMeDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();*/

	//if (nResponse == IDOK)
	//{
	//	// TODO: Place code here to handle when the dialog is
	//	//  dismissed with OK
	//}
	//else if (nResponse == IDCANCEL)
	//{
	//	// TODO: Place code here to handle when the dialog is
	//	//  dismissed with Cancel
	//}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	//return FALSE;
}

int CTrayMeApp::ExitInstance()
{
	HMODULE hModule = NULL;
	try
	{
		// TODO: Add your specialized code here and/or call the base class

		//while exiting untray all the trayed application.
		hModule = LoadLibrary(_T("Event Hooker Dll.dll"));
		
		if(NULL == hModule)
		{
			MessageBox(NULL,_T("Failed to Load Hooker Dll."),_T("Hooking Dlg"),MB_TOPMOST | MB_ICONERROR);
			throw _T("Failed to Load Hooker Dll.");
		}
		
		//Get the function address which untrays all the minimzed applications.
		UnTrayAll fpUnTrayAll = (UnTrayAll) GetProcAddress(hModule,"UnTrayAll");
		
		if(NULL == fpUnTrayAll)
		{
			MessageBox(NULL,_T("Failed to get function pointer from Hooker Dll."),_T("Hooking Dlg"),MB_TOPMOST | MB_ICONERROR);
			throw _T("Failed to get function pointer from Hooker Dll.");
		}

		//Untray the applications
		if(NULL != fpUnTrayAll)
			fpUnTrayAll();

		//Get the function address which uninstalls the windows procedure filter.
		UnInstallHook fpUnInstallHook = (UnInstallHook) GetProcAddress(hModule,"UnInstallWinProcHook");
		
		if(NULL == fpUnInstallHook)
			MessageBox(NULL,_T("Failed to get UnInstallWinProcHook function address."),_T("Tray Me !"),0);
		else if	(!fpUnInstallHook()) //Uninstall the Windows Procedure hook (Filter).
			MessageBox(NULL,_T("Failed to Uninstall hook."),_T("Tray Me !"),MB_TOPMOST | MB_ICONERROR);

		//Get the function address which uninstalls the mouse events filter.
		UnInstallHook fpMouseHook = (UnInstallHook) GetProcAddress(hModule,"UnInstallMouseHook");

		if(NULL == fpMouseHook)
			MessageBox(NULL,_T("Failed to get UnInstallMouseHook function address."),_T("Tray Me !"),MB_TOPMOST | MB_ICONERROR);
		else if(!fpMouseHook()) //unInstall the Mouse Event Hook (Filter).
			MessageBox(NULL,_T("Failed to Uninstall Mouse Hook."),_T("Tray Me !"),MB_TOPMOST | MB_ICONERROR);

	}
	catch(TCHAR *pszError)
	{
		OutputDebugString(_T("\nCTrayMeApp::ExitInstance:\tCustom Exception Cought."));
		DebugPrintErrorMessage(pszError);
	}
	catch(...)
	{
		DebugPrintErrorMessage(_T("\nCTrayMeApp::ExitInstance:\tException Cought."));
	}

	if(hModule)
		FreeLibrary(hModule);
	//Delete the object to avoid memory leak.
	if(m_pApplicationDialog)
	{
		m_pApplicationDialog->DestroyWindow();
		delete m_pApplicationDialog;
	}

	return CWinApp::ExitInstance();
}