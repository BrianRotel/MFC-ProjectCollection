#include "stdafx.h"
#include "resource.h"

#define ARRAY_SIZE			1024
#define TRAY_ME_MENU_ID		501

const UINT WM_NOTIFY_CALLBACK_MESSAGE = ::RegisterWindowMessage(_T("WM_NOTIFY_CALLBACK_MESSAGE-{9637AF99-17D7-4aba-BC7B-B7DAF6BF07F0}"));
const UINT WM_NOTIFY_TRAY_MESSAGE = ::RegisterWindowMessage(_T("WM_NOTIFY_TRAY_MESSAGE-{66FB622B-852C-4831-A7C1-1FE457DB02E0}"));

#pragma data_seg (".SHARED")
	HHOOK g_hPreviousMouseHook = 0;
	HHOOK g_hPreviousWinProcHook = 0;
	HINSTANCE g_hInstance = 0;
	HWND g_hMinimizedWindowList[ARRAY_SIZE] = {0};
	int g_iMinimizedWindowCount = 0;
#pragma data_seg()

#pragma comment(linker, "/SECTION:.SHARED,RWS")


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

bool TrayMe(HWND hWnd)
{
	bool bReturn = false;
	try
	{
		if(g_iMinimizedWindowCount >= ARRAY_SIZE)
		{
			MessageBox(NULL, _T("Can not Tray application. \n\t Reached the Tray limit."), _T("Tray Me !"), 0);
			return bReturn;
		}

		TCHAR szModuleFileName[ARRAY_SIZE] = {0};
		DWORD dwProccessId = 0;
		
		GetWindowThreadProcessId(hWnd, &dwProccessId);
		HMODULE hModule = (HMODULE) OpenProcess(0, FALSE, dwProccessId);
		
		if(!hModule)
		{
			DebugPrintErrorMessage();
			OutputDebugString(_T("\nTrayMe:\tFailed to retrive module handle name. Not an Issue ! :D"));
		}

		GetModuleFileName(hModule, szModuleFileName, ARRAY_SIZE);
		OutputDebugString(szModuleFileName);

		TCHAR szCaption[128] = {0};

		//HICON hIcon = (HICON) SendMessage(pMouseHooksStruct->hwnd, WM_GETICON, ICON_SMALL, 0);
		SHFILEINFO shFileInfo;
		shFileInfo.hIcon = NULL;
		SHGetFileInfo(szModuleFileName, FILE_ATTRIBUTE_NORMAL, &shFileInfo, 
						sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON);

		if(!shFileInfo.hIcon)
			OutputDebugString(_T("NULL Icon handle."));

		NOTIFYICONDATA objNotifyIcon;
		objNotifyIcon.cbSize = sizeof(NOTIFYICONDATA);
		objNotifyIcon.hWnd =	hWnd;
		objNotifyIcon.uID = 1;
		objNotifyIcon.uCallbackMessage = WM_NOTIFY_CALLBACK_MESSAGE;
		objNotifyIcon.hIcon = (shFileInfo.hIcon == NULL) ? LoadIcon(NULL, IDI_QUESTION) : shFileInfo.hIcon;
		objNotifyIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;

		GetWindowText(hWnd, szCaption, 127);
		wcscpy_s(objNotifyIcon.szTip, 127, szCaption);

		Shell_NotifyIcon(NIM_ADD, &objNotifyIcon);
		ShowWindow(hWnd, SW_HIDE);
		g_hMinimizedWindowList[g_iMinimizedWindowCount++] = hWnd;

		bReturn = true;
	}
	catch(...)
	{
		DebugPrintErrorMessage(_T("Caught Exception in TrayMe !"));
		bReturn = false;
	}

	return bReturn;
}

bool UnTrayApplication(HWND hWnd)
{
	bool bReturn = false;
	try
	{
		::ShowWindow(hWnd, SW_SHOW);
		::SetForegroundWindow(hWnd);

		NOTIFYICONDATA objNotifyIcon;
		objNotifyIcon.cbSize = sizeof(NOTIFYICONDATA);
		objNotifyIcon.hWnd = hWnd;
		objNotifyIcon.uID = 1;
		Shell_NotifyIcon(NIM_DELETE, &objNotifyIcon);
		
		for(int i = 0; i < g_iMinimizedWindowCount; i++)
			if(hWnd == g_hMinimizedWindowList[i])
			{
				g_hMinimizedWindowList[i] = g_hMinimizedWindowList[--g_iMinimizedWindowCount];
				g_hMinimizedWindowList[g_iMinimizedWindowCount] = 0;

				break;
			}

		bReturn = true;
	}
	catch(...)
	{
		DebugPrintErrorMessage(_T("Caught Exception in UnTrayMe !"));
		bReturn = false;
	}

	return bReturn;
}

bool UnTrayMe(TCHAR *szWindowTitle)
{
	bool bReturn = false;
	HWND hWnd = NULL;
	try
	{
		for(int i = 0;i < g_iMinimizedWindowCount;i++)
		{
			TCHAR szWinTitle[ARRAY_SIZE] = {0};
			GetWindowText(g_hMinimizedWindowList[i], szWinTitle, ARRAY_SIZE - 1); 

			if(_wcsicmp(szWindowTitle, szWinTitle) == 0)
			{
				hWnd = g_hMinimizedWindowList[i];
				break;
			}
		}

		if(NULL == hWnd)
			return bReturn;

		::ShowWindow(hWnd, SW_SHOW);
		::SetForegroundWindow(hWnd);

		NOTIFYICONDATA objNotifyIcon;
		objNotifyIcon.cbSize = sizeof(NOTIFYICONDATA);
		objNotifyIcon.hWnd = hWnd;
		objNotifyIcon.uID = 1;
		Shell_NotifyIcon(NIM_DELETE, &objNotifyIcon);
		
		for(int i = 0; i < g_iMinimizedWindowCount; i++)
			if(hWnd == g_hMinimizedWindowList[i])
			{
				g_hMinimizedWindowList[i] = g_hMinimizedWindowList[--g_iMinimizedWindowCount];
				g_hMinimizedWindowList[g_iMinimizedWindowCount] = 0;
				break;
			}

		bReturn = true;
	}
	catch(...)
	{
		DebugPrintErrorMessage(_T("Caught Exception in UnTrayMe !"));
		bReturn = false;
	}

	return bReturn;
}

void UnTrayAll()
{
	try
	{
		while(g_iMinimizedWindowCount)
			UnTrayApplication(g_hMinimizedWindowList[0]);
	}
	catch(...)
	{
		MessageBox(NULL, _T("Failed to untray few applications."), _T("Tray Me !"), MB_ICONERROR);
		DebugPrintErrorMessage(_T("Exception Caught In UnTrayAll."));
	}
}

LRESULT CALLBACK MouseHookProcedure(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(0 > nCode)
		return CallNextHookEx(g_hPreviousMouseHook, nCode, wParam, lParam);

	//OutputDebugString(_T("Mouse Hooked Event...\n"));

	MOUSEHOOKSTRUCT *pMouseHooksStruct = (MOUSEHOOKSTRUCT *) lParam;
	switch(wParam)
	{
		case WM_NCRBUTTONUP:
		case VK_LMENU:
			{
				if(HTMINBUTTON == pMouseHooksStruct->wHitTestCode)
				{
					OutputDebugString(_T("\nMouseHookProcedure:\tMouse Clicked on Min Button...\n"));
					HMENU  hPopupMenu = ::CreatePopupMenu();
					::AppendMenu(hPopupMenu, MF_STRING | MF_ENABLED, TRAY_ME_MENU_ID, _T("&Try Me !")); 

					SetForegroundWindow(pMouseHooksStruct->hwnd);
					int iSelectedMenuId = ::TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD , pMouseHooksStruct->pt.x, pMouseHooksStruct->pt.y, 0, pMouseHooksStruct->hwnd, NULL);
					//PostMessage(WM_NULL, NULL, NULL);

					switch(iSelectedMenuId)
					{
						case TRAY_ME_MENU_ID:
							TrayMe(pMouseHooksStruct->hwnd);
							break;
					}

					DestroyMenu(hPopupMenu);
					return -1;
				}
			}
			break;
	}

	return CallNextHookEx(g_hPreviousMouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK WinProcHookProcedure(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(0 > nCode)
		return CallNextHookEx(g_hPreviousWinProcHook , nCode, wParam, lParam);

	CWPSTRUCT *pwsStruct = (CWPSTRUCT *) lParam;

	if(WM_NOTIFY_CALLBACK_MESSAGE == pwsStruct->message)
	{
		if(WM_LBUTTONDBLCLK == pwsStruct->lParam)
		{
			OutputDebugString(_T("\nWinProcHookProcedure:\tWM_NOTIFY_CALLBACK_MESSAGE WM_LBUTTONDBLCLK Fired.\n"));
			UnTrayApplication(pwsStruct->hwnd);
		}

		return -1;
	}

	if(WM_NOTIFY_TRAY_MESSAGE == pwsStruct->message)
	{
		TCHAR szWindowName[ARRAY_SIZE] = {0};
		TCHAR szMessage[ARRAY_SIZE] = {0};

		GetWindowText(pwsStruct->hwnd, szWindowName, ARRAY_SIZE);

		OutputDebugString(_T("\nWinProcHookProcedure:\tWM_NOTIFY_TRAY_MESSAGE Received.\n"));
		if(TrayMe(pwsStruct->hwnd))
			wsprintf(szMessage, _T("'%s' application minimized to tray."), szWindowName);
		else
			wsprintf(szMessage, _T("Failed to minimize '%s' application to tray."), szWindowName);

		if(_tcslen(szWindowName))
			MessageBox(NULL, szMessage, _T("Tray Me !"), MB_ICONINFORMATION);
	}

	return CallNextHookEx(g_hPreviousWinProcHook , nCode, wParam, lParam);
}

bool InstallWinProcHook(void)
{
	bool bReturn = false;
	try
	{
		OutputDebugString(_T("\nInstallWinProcHook:\tWinProc Event Hooked.\n"));
		if(NULL == g_hInstance)
			OutputDebugString(_T("\nInstallWinProcHook:\tNULL g_hInstance\n"));

		g_hPreviousWinProcHook = SetWindowsHookEx(WH_CALLWNDPROC, &WinProcHookProcedure, g_hInstance, 0);

		if(NULL == g_hPreviousWinProcHook)
		{
			TCHAR szErrorMsg[ARRAY_SIZE] = {0};
			wsprintf(szErrorMsg, _T("Failed to Hook WinProc Event.\nLast Error :%d\n"), GetLastError());
			OutputDebugString(szErrorMsg);

			bReturn = false;
		}
		else
		{
			OutputDebugString(_T("\nInstallWinProcHook:\tWinProc Event Hooked.\n"));
			bReturn = true;
		}
	}
	catch(...)
	{
		DebugPrintErrorMessage();
		bReturn = false;
	}

	return bReturn;
}

bool UnInstallWinProcHook()
{
	bool bReturn = false;

	try
	{
		OutputDebugString(_T("\nUnInstallWinProcHook:\tWinProc Event UnHooked.\n"));
		if(UnhookWindowsHookEx(g_hPreviousWinProcHook) == FALSE)
		{
			TCHAR szError[ARRAY_SIZE] = {0};
			wsprintf(szError, _T("Last Error : %d"), GetLastError());
			OutputDebugString(_T("\nUnInstallWinProcHook:\tFailed to UnHook WinProc Event.\n"));
			OutputDebugString(szError);
			bReturn = false;
		}
		else
		{
			OutputDebugString(_T("\nUnInstallWinProcHook:\tWinProc Event UnHooked.\n"));
			bReturn = true;
		}
	}
	catch(...)
	{
		DebugPrintErrorMessage();
		bReturn = false;
	}

	return bReturn;
}

bool InstallMouseHook()
{
	bool bReturn = false;

	try
	{
		OutputDebugString(_T("\nInstallMouseHook:\tMouse Event Hooked.\n"));
		g_hPreviousMouseHook = SetWindowsHookEx(WH_MOUSE, &MouseHookProcedure, g_hInstance, 0);

		if(NULL == g_hPreviousMouseHook)
		{
			TCHAR szError[ARRAY_SIZE] = {0};
			wsprintf(szError, _T("Last Error : %d"), GetLastError());
			OutputDebugString(_T("\nInstallMouseHook:\tFailed to Hook Mouse Event.\n"));
			OutputDebugString(szError);
			bReturn = false;
		}
		else
		{
			OutputDebugString(_T("\nInstallMouseHook:\tMouse Event Hooked.\n"));
			bReturn = true;
		}
	}
	catch(...)
	{
		DebugPrintErrorMessage();
		bReturn = false;
	}

	return bReturn;
}

bool UnInstallMouseHook()
{
	bool bReturn = false;

	try
	{
		OutputDebugString(_T("\nUnInstallMouseHook:\tMouse Event UnHooked.\n"));
		if(UnhookWindowsHookEx(g_hPreviousMouseHook) == FALSE)
		{
			TCHAR szError[ARRAY_SIZE] = {0};
			wsprintf(szError, _T("Last Error : %d"), GetLastError());
			OutputDebugString(_T("\nUnInstallMouseHook:\tFailed to UnHook Mouse Event.\n"));
			OutputDebugString(szError);
			bReturn = false;
		}
		else
		{
			OutputDebugString(_T("\nUnInstallMouseHook:\tMouse Event UnHooked.\n"));
			bReturn = true;
		}
	}
	catch(...)
	{
		DebugPrintErrorMessage();
		bReturn = false;
	}

	return bReturn;
}


void GetMinimizedWinodwList(TCHAR szWindowCaptions[ARRAY_SIZE][100])
{
	for(int i=0;i<g_iMinimizedWindowCount;i++)
	{
		TCHAR szWndCaption[ARRAY_SIZE] = {0};
		::GetWindowText(g_hMinimizedWindowList[i], szWndCaption, ARRAY_SIZE - 1);
		
		if(NULL != szWindowCaptions[i])
		{
			wcscpy_s(szWindowCaptions[i], _tcslen(szWndCaption) + 1, szWndCaption);
		}
	}
}

UINT GetTrayMessageID()
{
	TCHAR szTmp[ARRAY_SIZE] = {0};
	wsprintf(szTmp, _T("-------------------------------Id : %d"), WM_NOTIFY_TRAY_MESSAGE);
	OutputDebugString(szTmp);

	return WM_NOTIFY_TRAY_MESSAGE;
}