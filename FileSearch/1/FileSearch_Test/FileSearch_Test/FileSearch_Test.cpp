// FileSearch_Test.cpp : 定义控制台应用程序的入口点。
//
//#pragma warning(disable:4996)
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "FileSearch.h"
#include <iostream>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
#if 1
	 char rootPath[10] = {0}, driveType[21] = {0};  
     UINT nType;  
 
     for(char a = 'A'; a <= 'Z'; a++)  
     {  
         sprintf_s(rootPath, "%c:\\", a);  
         nType = GetDriveType(rootPath);  
         if(nType != DRIVE_NO_ROOT_DIR)                  // DRIVE_NO_ROOT_DIR: 路径无效  
         {  
             switch(nType)  
             {  
             case DRIVE_FIXED:  
                 strcpy_s(driveType, "硬盘");  
                 break;  
             case DRIVE_REMOVABLE:  
				 strcpy_s(driveType, "移动硬盘");
                 break;  
             case DRIVE_CDROM:  
				 strcpy_s(driveType, "光盘");
                 break;  
             case DRIVE_RAMDISK:  
				 strcpy_s(driveType, "RAM盘");
                 break;  
             case DRIVE_REMOTE:  
				 strcpy_s(driveType, "Remote(Network) drive 网络磁盘");
                 break;  
             case DRIVE_UNKNOWN:  
             default:  
				 strcpy_s(driveType, "未知盘");
                 break;  
             }  
             cout<<rootPath<<"\t"<<driveType<<endl;  
			 SearchFile(rootPath);
         }  
     } 
#endif
	// SearchFile("D:\\");

	system("pause");
	return 0;
}

