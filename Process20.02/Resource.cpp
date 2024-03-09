#pragma once
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <Tlhelp32.h>
#include <locale.h>
#include <conio.h>
#include<commctrl.h>
#include <windowsX.h>

using namespace std;
#include "resource.h"



HWND hList, hInput;
DWORD selectedProcID;





BOOL CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPTSTR lpszCmdLine, int nCmdShow)
{
   
    return DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc);
}


void ProcessList()
{
    SendMessage(hList, LB_RESETCONTENT, 0, 0);

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnapshot, &pe32))
        {
            do
            {
                SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)pe32.szExeFile);
            } while (Process32Next(hSnapshot, &pe32));
        }
        CloseHandle(hSnapshot);
    }
}



void ReProcessList()
{
    ProcessList();
}




DWORD GetProcID( TCHAR* processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
   
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnapshot, &pe32)) {
            do {
                if (_tcscmp(pe32.szExeFile, processName) == 0) {
                    CloseHandle(hSnapshot);
                    return pe32.th32ProcessID;
                }
            } while (Process32Next(hSnapshot, &pe32));
        }
        CloseHandle(hSnapshot);
    
    return 0;
}




void TerminateProc()
{
    int index = SendMessage(hList, LB_GETCURSEL, 0, 0);


    if (index != LB_ERR)
    {
        TCHAR processName[MAX_PATH];


        SendMessage(hList, LB_GETTEXT, index, (LPARAM)processName);

        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, GetProcID(processName));

        if (hProcess != NULL)
        {
            TerminateProcess(hProcess, 0);
            CloseHandle(hProcess);
            ReProcessList();

        }
    }
}






void CreateProc()
{
   
    TCHAR buffer[MAX_PATH];
    GetWindowText(hInput, buffer, MAX_PATH);

   
  
    STARTUPINFO s = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION p;

    BOOL fs = CreateProcess(NULL, buffer, NULL, NULL, FALSE, 0, NULL, NULL, &s, &p);
    
        if (fs)
        {
            CloseHandle(p.hThread);
            CloseHandle(p.hProcess);
            ReProcessList();
        }
        else
        {
       
          MessageBox(NULL, L"Failed to create process", L"Error", MB_OK | MB_ICONERROR);
        }
}



void ShowProcInfo()
{
    int index = SendMessage(hList, LB_GETCURSEL, 0, 0);

   
        TCHAR processName[MAX_PATH];
        SendMessage(hList, LB_GETTEXT, index, (LPARAM)processName);
        DWORD processId = GetProcID(processName);

        if (processId != 0)
        {
            PROCESSENTRY32 pe32;
            pe32.dwSize = sizeof(PROCESSENTRY32);

            HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if (hSnapshot != INVALID_HANDLE_VALUE)
            {
                if (Process32First(hSnapshot, &pe32))
                {
                    do
                    {
                        if (pe32.th32ProcessID == processId)
                        {
                            TCHAR infoBuffer[MAX_PATH];
                            _stprintf_s(infoBuffer, _T("ID: %4u  Threads: %5u   Priority: %2u    Name: %s\n"),
                                pe32.th32ProcessID, pe32.cntThreads, pe32.pcPriClassBase, pe32.szExeFile);
                            MessageBox(NULL, infoBuffer, _T("Process Information"), MB_OK | MB_ICONINFORMATION);
                            CloseHandle(hSnapshot);
                            return;
                        }
                    } while (Process32Next(hSnapshot, &pe32));
                }
                CloseHandle(hSnapshot);
            }
        }
        else
        {
            MessageBox(NULL, _T("No process information."), _T("Error"), MB_OK | MB_ICONERROR);
        }
    
}


BOOL CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        hList = GetDlgItem(hwnd, IDC_LIST1);
        hInput = GetDlgItem(hwnd, IDC_EDIT1);
        ProcessList();
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_TERMINATE2:
            TerminateProc();
            return TRUE;

        case IDC_UPDATE1:
            ReProcessList();
            return TRUE;

        case IDC_CREATENEW:
            CreateProc();
            return TRUE;

        case IDC_INFO3:
            ShowProcInfo();
            return TRUE;

        }
        break;

    case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
    }
    return FALSE;
}




//
//void ProcessList()
//{
//
//	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//
//	PROCESSENTRY32 pe32;
//	memset(&pe32, 0, sizeof(PROCESSENTRY32));
//	pe32.dwSize = sizeof(PROCESSENTRY32);
//	if (Process32First(hSnapShot, &pe32))
//	{
//		_tprintf(_T("ID:  %4u  Threads: %5u   priority:  %2u    name:   %s\n"), pe32.th32ProcessID, pe32.cntThreads, pe32.pcPriClassBase, pe32.szExeFile);
//
//		while (Process32Next(hSnapShot, &pe32))
//		{
//			_tprintf(_T("ID:  %4u  Threads: %5u   priority:  %2u    name:   %s\n"), pe32.th32ProcessID, pe32.cntThreads, pe32.pcPriClassBase, pe32.szExeFile);
//		}
//	}
//}


//int main()
//{
//    STARTUPINFO s = { sizeof(STARTUPINFO) };
//    PROCESS_INFORMATION p;
//    //TCHAR buffer[] = TEXT("C:\\Users\\krisc\\OneDrive - ITSTEP\\Рабочий стол\\Модуль 7. Занятие 1 многопоточность приоритеты\\Source\\Suspend_Resume_Thread\\Debug\\Suspend_Resume_Thread.exe");
//    TCHAR buffer[] = TEXT("Calc.exe");
//
//    BOOL fs = CreateProcess(NULL, buffer, NULL, NULL, FALSE, 0, NULL, NULL, &s, &p);
//
//    /*if (fs)
//    {
//        CloseHandle(p.hThread);
//        CloseHandle(p.hProcess);
//    }*/
//
//    int v;
//    cin >> v;
//    if (v == 1)
//    {
//        //ExitProcess(0);
//        cout << TerminateProcess(p.hProcess, 0) << endl;
//        //CloseHandle(p.hProcess);
//        cout << "End";
//    }
//    cout << "eakjbvjebnrjfvnke\n";
//}