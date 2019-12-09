#include "stdafx.h"

#define ARRAY_SIZE				1024

//All the menu ids
#define MENUID_SEPARATOR		100
#define MENUID_UNTRAY_ALL		101
#define MENUID_ABOUT_DLG		102
#define MENUID_EXIT				103

#define MENUID_APPNAMES			200
#define HOT_KEY_ID				46000
//Register a window message.
const UINT WM_TRAY_ICON_NOTIFY_MESSAGE = ::RegisterWindowMessage(_T("WM_TRAY_ICON_NOTIFY_MESSAGE-{648DC384-565B-4d39-9E1D-D5A5652714B4}"));

