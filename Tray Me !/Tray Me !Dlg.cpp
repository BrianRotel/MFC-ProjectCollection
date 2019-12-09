// Tray Me !Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "Tray Me !.h"
#include "Tray Me !Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef void (*UnTrayAll)();
typedef void (*GetMinimizedWinodwList)(TCHAR szParam[ARRAY_SIZE][100]);
typedef bool (*UnTrayMe)(TCHAR *szTitile);

extern UINT g_uiTrayMessageID;

void DebugPrintErrorMessage(TCHAR *pszErrorString = NULL, bool bDisplayMsg = false, TCHAR *pszMsgCaption = NULL)
{
	DWORD iErrorNo = GetLastError();

	if(!iErrorNo)
	{
		//OutputDebugString(_T("\n\n====================Zero Error Number."));
		if(pszErrorString)
			OutputDebugString(pszErrorString);
		return;
	}


	TCHAR szFormattedErrorString[1024] = {0};

	{
		LPVOID lpBuff = NULL;

		FormatMessage(	FORMAT_MESSAGE_ALLOCATE_BUFFER |
						FORMAT_MESSAGE_FROM_SYSTEM | 
						FORMAT_MESSAGE_IGNORE_INSERTS, 
						NULL, 
						iErrorNo, 
						0, // Default language
						(LPTSTR) &lpBuff, 
						0, 
						NULL 
					);

		wsprintf(szFormattedErrorString, _T("\nError Number :\t%d\nCusrom Error Description:\t%s\nSystem Error Description:\t%s"), iErrorNo, pszErrorString, (LPTSTR) lpBuff);
		LocalFree(lpBuff);
	}

	OutputDebugString(szFormattedErrorString);

	if(bDisplayMsg)
		MessageBox(NULL, szFormattedErrorString, pszMsgCaption, MB_ICONERROR);
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{ 
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CTrayMeDlg dialog




CTrayMeDlg::CTrayMeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTrayMeDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTrayMeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTrayMeDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_REGISTERED_MESSAGE(WM_TRAY_ICON_NOTIFY_MESSAGE, OnTrayNotify)
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CTrayMeDlg message handlers

BOOL CTrayMeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	//Add a Tray Notification Icon for the application.
	NOTIFYICONDATA niData;
	niData.cbSize = sizeof(NOTIFYICONDATA);
	niData.hWnd = m_hWnd;
	niData.uID = 1;
	niData.uCallbackMessage = WM_TRAY_ICON_NOTIFY_MESSAGE;
	niData.hIcon = m_hIcon;
	niData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	wcscpy_s(niData.szTip, 127, _T("Tray Me !"));

	Shell_NotifyIcon(NIM_ADD, &niData);
	RegisterApplicationHotKeys();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTrayMeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model, 
//  this is automatically done for you by the framework.

void CTrayMeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTrayMeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTrayMeDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnClose();
	PostQuitMessage(0);
}

LRESULT CTrayMeDlg::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	UINT uMsg = (UINT) lParam;
	//Handle the tray notification messages here.
	HMODULE hModule = NULL;

	try
	{
		//Entertain only Right click event.
		if(WM_RBUTTONDOWN == uMsg || WM_CONTEXTMENU == uMsg)
		{
			//Build a context menu with minimised application captios as menu items and pop it up.
			CPoint pt;
			GetCursorPos(&pt);

			hModule = LoadLibrary(_T("Event Hooker Dll.dll"));

			if(NULL == hModule)
			{
				MessageBox(_T("Failed to Load Hooker Dll."), _T("Hooking Dlg"), 0);
				throw _T("Failed to Load Hooker Dll.");
			}

			GetMinimizedWinodwList fpGetMinimizedWinodwList = (GetMinimizedWinodwList) GetProcAddress(hModule, "GetMinimizedWinodwList");

			if(NULL == fpGetMinimizedWinodwList)
			{
				MessageBox(_T("Failed to Get GetMinimizedWinodwList Function pointer from Hooker Dll."), _T("Hooking Dlg"), 0);
				throw _T("Failed to Get GetMinimizedWinodwList Function pointer from Hooker Dll.");
			}

			//Retrive the list of caption of the minimized windows
			TCHAR szWindowCaptions[ARRAY_SIZE][100] = {0};
			fpGetMinimizedWinodwList(szWindowCaptions);

			HMENU hContextMenu = CreatePopupMenu();
			HMENU hSubMenu = CreatePopupMenu();

			int iMinimizedAppCount = 0;

			//Add the list items into the context menu.
			for(iMinimizedAppCount = 0;iMinimizedAppCount < 1024; iMinimizedAppCount++, OutputDebugString(_T("\n")))
			{
				if(wcslen(szWindowCaptions[iMinimizedAppCount]))
				{
					::AppendMenu(hSubMenu, MF_STRING, MENUID_APPNAMES + iMinimizedAppCount , szWindowCaptions[iMinimizedAppCount]);
					OutputDebugString(szWindowCaptions[iMinimizedAppCount]);
				}
				else
					break;
			}

			long lMenuProperties = MF_STRING ;

			if(0 == iMinimizedAppCount)
				lMenuProperties |= MF_GRAYED;

			::AppendMenu(hContextMenu, lMenuProperties | MF_POPUP, (UINT_PTR) hSubMenu , _T("UnTray"));
			::AppendMenu(hContextMenu, lMenuProperties, MENUID_UNTRAY_ALL, _T("UnTray All"));
			::AppendMenu(hContextMenu, MF_SEPARATOR, MENUID_SEPARATOR, _T("Tray Me Separator"));
			::AppendMenu(hContextMenu, MF_STRING, MENUID_ABOUT_DLG, _T("About Tray Me !"));
			::AppendMenu(hContextMenu, MF_SEPARATOR, MENUID_SEPARATOR, _T("Tray Me Separator"));
			::AppendMenu(hContextMenu, MF_STRING, MENUID_EXIT, _T("Exit"));

			SetForegroundWindow();
			//Popup the context menu.
			BOOL iSelectedIndex = TrackPopupMenu(hContextMenu, TPM_LEFTALIGN | TPM_HORPOSANIMATION | TPM_RETURNCMD | TPM_LEFTBUTTON, pt.x, pt.y, 0, m_hWnd, NULL);
			//PostMessage(WM_NULL, NULL, NULL);

			//Perform the user selected task.
			switch(iSelectedIndex)
			{
			case MENUID_EXIT:
				//Exit the application.
				PostQuitMessage(0);
				break;
			case MENUID_UNTRAY_ALL:
				{
					//Untray all the applications.
					UnTrayAll fpUnTrayAll = (UnTrayAll) GetProcAddress(hModule, "UnTrayAll");

					if(NULL == fpUnTrayAll)
					{
						MessageBox(_T("Failed to Get UnTrayAll Function pointer from Hooker Dll."), _T("Hooking Dlg"), 0);
						throw _T("Failed to Get UnTrayAll Function pointer from Hooker Dll.");
					}

					fpUnTrayAll();
				}
				break;
			case MENUID_ABOUT_DLG:
				{
					//Show about dlg.
					CAboutDlg dlgAbout;
					dlgAbout.DoModal();
				}
				break;
			default :
				if(iSelectedIndex >= MENUID_APPNAMES)
				{
					//Untray the selected application only.
					UnTrayMe fpUnTrayMe = (UnTrayMe) GetProcAddress(hModule, "UnTrayMe");

					if(NULL == fpUnTrayMe)
						MessageBox(_T("Failed to Get UnTrayMe Function pointer from Hooker Dll."), _T("Hooking Dlg"), 0);

					OutputDebugString(szWindowCaptions[iSelectedIndex - MENUID_APPNAMES]);
					fpUnTrayMe(szWindowCaptions[iSelectedIndex - MENUID_APPNAMES]);
				}
				break;
			}
		}
	}
	catch(TCHAR *pszError)
	{
		OutputDebugString(_T("\nCTrayMeDlg::OnTrayNotify:\tCustom Exception Caught."));
		DebugPrintErrorMessage(pszError);
	}
	catch(...)
	{
		DebugPrintErrorMessage(_T("\nCTrayMeDlg::OnTrayNotify:\tException Caught."));
	}

	if(hModule)
		FreeLibrary(hModule);

	return LRESULT();
}

void CTrayMeDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here

	//Delete the tray icon while exiting the application.
	NOTIFYICONDATA niData;
	niData.cbSize = sizeof(NOTIFYICONDATA);
	niData.hWnd = m_hWnd;
	niData.uID = 1;

	Shell_NotifyIcon(NIM_DELETE, &niData);
	UnRegisterApplicationHotKeys();
}


LRESULT CTrayMeDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	OutputDebugString(_T("HotKey Pressed."));
	try
	{
		HWND hForegroundWindow = ::GetForegroundWindow();

		if(NULL != hForegroundWindow)
		{
			TCHAR szClassName[ARRAY_SIZE] = {0};
			//::GetWindowText(hForegroundWindow, szTmp, ARRAY_SIZE);
			//MessageBox(szTmp, _T("Caption Tray Me !"), MB_ICONINFORMATION);

			::GetClassName(hForegroundWindow, szClassName, ARRAY_SIZE);
			//MessageBox(szTmp, _T("Class Tray Me !"), MB_ICONINFORMATION);

			if(!::IsWindowVisible(hForegroundWindow))
			{
				MessageBox(_T("Can not minimize invisible window."), _T("Tray Me !"), MB_ICONINFORMATION);
				return -1;
			}

			if(::GetDesktopWindow() == hForegroundWindow	|| 
				!_tcsicmp(szClassName, _T("Shell_TrayWnd"))
			  )
			{
				MessageBox(_T("Miniming Desktop/ShellWindow to tray is not allowed."), _T("Tray Me !"), MB_ICONINFORMATION);
				return -1;
			}


			OutputDebugString(_T("\nCTrayMeDlg::OnHotKey:\tPosting message to forground window."));
			::SendMessage(hForegroundWindow, g_uiTrayMessageID, NULL, NULL);
		}
		else
			OutputDebugString(_T("\nCTrayMeDlg::OnHotKey:\tnull forground window handle."));

	}
	catch(TCHAR *pszErrorString)
	{
		OutputDebugString(_T("\nCTrayMeDlg::OnHotKey:\tCustom Exception caught."));
		DebugPrintErrorMessage(pszErrorString);
	}
	catch(...)
	{
		OutputDebugString(_T("\nCTrayMeDlg::OnHotKey:\tException caught."));
		DebugPrintErrorMessage();
	}

	return 1;
}


bool CTrayMeDlg::RegisterApplicationHotKeys(void)
{
	bool bReturn = false;

	try
	{
		if(!RegisterHotKey(m_hWnd, HOT_KEY_ID, MOD_CONTROL | MOD_SHIFT, VK_DOWN))
			throw _T("\nHot Key Registration Failed.");
		else
			OutputDebugString(_T("\nCTrayMeDlg::RegisterApplicationHotKeys:\tHot Key Registered Successfully."));

		bReturn = true;
	}
	catch(TCHAR *pszErrorString)
	{
		OutputDebugString(_T("\nCTrayMeDlg::RegisterApplicationHotKeys:\tCustom Exception caught."));
		DebugPrintErrorMessage(pszErrorString);
		bReturn = false;
	}
	catch(...)
	{
		OutputDebugString(_T("\nCTrayMeDlg::RegisterApplicationHotKeys:\tException caught."));
		bReturn = false;
		DebugPrintErrorMessage();
	}

	return bReturn;
}

bool CTrayMeDlg::UnRegisterApplicationHotKeys(void)
{
	bool bReturn = false;
	
	try
	{
		if(!UnregisterHotKey(m_hWnd, HOT_KEY_ID))
			throw _T("\nHot Key Registration Failed.");
		else
			OutputDebugString(_T("\nCTrayMeDlg::UnRegisterApplicationHotKeys:\tHot Key UnRegistered Successfully."));

		bReturn = true;
	}
	catch(TCHAR *pszErrorString)
	{
		int i = GetLastError();
		OutputDebugString(_T("\nCTrayMeDlg::UnRegisterApplicationHotKeys:\tCustom Exception caught."));
		DebugPrintErrorMessage(pszErrorString);
		bReturn = false;
	}
	catch(...)
	{
		DebugPrintErrorMessage();
		OutputDebugString(_T("\nCTrayMeDlg::UnRegisterApplicationHotKeys:\tException caught."));
		bReturn = false;
	}

	return bReturn;
}