#include <vcl.h>
#include <windows.h>
#include <iostream>
#include <sstream>
#include <Registry.hpp>

//---------------------------------------------------------------------------

#include "BFresize.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
#pragma link "trayicon"
#pragma resource "*.dfm"

TfrmBF *frmBF;
//---------------------------------------------------------------------------
__fastcall TfrmBF::TfrmBF(TComponent* Owner)
        : TForm(Owner)
{

}
//---------------------------------------------------------------------------

void __fastcall TfrmBF::mnuAutoStartClick(TObject *Sender)
{
    TRegistry *reg = new TRegistry();
    try {
        reg->RootKey = HKEY_CURRENT_USER;

        if(mnuAutoStart->Checked) {
            if (reg->OpenKey("\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", true)) {
                reg->WriteString("BFbwApp", Application->ExeName);
                reg->CloseKey();
            }

            if (reg->OpenKey("\\Software\\BFbwApp", true)) {
                reg->WriteBool("AutoStart", true);
                reg->CloseKey();
            }
        } else {
            if (reg->OpenKey("\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", true)) {
                reg->DeleteValue("BFbwApp");
                reg->CloseKey();
            }

            if (reg->OpenKey("\\Software\\BFbwApp", true)) {
                reg->WriteBool("AutoStart", false);
                reg->CloseKey();
            }
        }
    } __finally {
        delete reg;
    }
    
    tmrCursorLock->Enabled = true;
}

const char* windowTitle = "BattleForge 1.2 Retail";
HWND wnd, oldwnd = NULL;
bool hadFocus = false;

RECT GetWindowSize(HWND hwnd) {
    RECT rect;
    GetWindowRect(hwnd, &rect);
    return rect;
}

RECT GetNearestScreenResolution(HWND hwnd) {
    MONITORINFO mi;
    mi.cbSize = sizeof(MONITORINFO);
    HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    GetMonitorInfo(hMonitor, &mi);
    return mi.rcMonitor;
}

void RepositionWindow(HWND hwnd) {
    RECT winRect = GetWindowSize(hwnd);
    RECT screenRect = GetNearestScreenResolution(hwnd);

    int xOffset = (winRect.right - winRect.left - (screenRect.right - screenRect.left)) / 2;
    int yOffset = (winRect.bottom - winRect.top - (screenRect.bottom - screenRect.top)) - xOffset;

    MoveWindow(hwnd, -xOffset, -yOffset, winRect.right - winRect.left, winRect.bottom - winRect.top, TRUE);
}       

void UpdateTrayIcon(bool gameFound, bool gameActive) {
    if (!gameFound) {
		frmBF->trIcon->IconIndex = 0;
        frmBF->trIcon->Hint = "No active game window";
    } else if (gameActive) {
        frmBF->trIcon->IconIndex = 2;
        frmBF->trIcon->Hint = "Game is active";
    } else {
        frmBF->trIcon->IconIndex = 1;
        frmBF->trIcon->Hint = "Active game found";
    }
}

void LockCursorToWindow(HWND hwnd) {
	RECT clientRect;
	GetClientRect(hwnd, &clientRect);

    POINT topLeft = { clientRect.left, clientRect.top };
	POINT bottomRight = { clientRect.right, clientRect.bottom };

    ClientToScreen(hwnd, &topLeft);
	ClientToScreen(hwnd, &bottomRight);

	RECT screenRect = { topLeft.x, topLeft.y, bottomRight.x, bottomRight.y };

    RECT currentRect;
    GetClipCursor(&currentRect);

    if(currentRect.left != screenRect.left ||
    	currentRect.top != screenRect.top ||
	    currentRect.right != screenRect.right ||
	    currentRect.bottom != screenRect.bottom){
    		ClipCursor(&screenRect);
			UpdateTrayIcon(true, true);
	    }

}

    /*// Remove title bar and borders
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
    SetWindowLongPtr(hwnd, GWL_STYLE, style);

    // Remove extended styles that may cause issues
    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    exStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);*/

//---------------------------------------------------------------------------

void __fastcall TfrmBF::tmrCursorLockTimer(TObject *Sender)
{
    oldwnd = wnd;
    wnd = FindWindow(NULL, windowTitle);

    if (wnd) {
        if (wnd != oldwnd) {
        	trIcon->Minimize();
            RepositionWindow(wnd);
        }

        if (GetForegroundWindow() == wnd) {
            LockCursorToWindow(wnd);
        }
    } else {
    	pbSearching->StepBy(5);
        if(pbSearching->Position == 100) pbSearching->Position = 0;
		UpdateTrayIcon(false, false);
    }
}

//---------------------------------------------------------------------------

void __fastcall TfrmBF::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caNone;
    trIcon->Minimize();
}

//---------------------------------------------------------------------------

void __fastcall TfrmBF::mnuRestoreClick(TObject *Sender)
{
	trIcon->Restore();	
}

//---------------------------------------------------------------------------

void __fastcall TfrmBF::mnuExitClick(TObject *Sender)
{
	Application->Terminate();	
}

//---------------------------------------------------------------------------

void __fastcall TfrmBF::mnuStartClick(TObject *Sender)
{
	if(mnuStart->Caption == "Start &looking"){
		tmrCursorLock->Enabled = true;
	    mnuStart->Caption = "Stop &looking";
    } else {
		tmrCursorLock->Enabled = false;
	    mnuStart->Caption = "Start &looking";
        pbSearching->Position = 0;
    }
}

//---------------------------------------------------------------------------

void __fastcall TfrmBF::FormActivate(TObject *Sender)
{
	TRegistry *reg = new TRegistry();
    try {
        reg->RootKey = HKEY_CURRENT_USER;

        if (reg->OpenKey("\\Software\\BFbwApp", false)) {
            if (reg->ValueExists("AutoStart")) {
                bool autoStart = reg->ReadBool("AutoStart");

                mnuAutoStart->Checked = autoStart;
                tmrCursorLock->Enabled = autoStart;
			    mnuStart->Caption = "Stop &looking";
                trIcon->Minimize();
            }
            reg->CloseKey();
        }
    } __finally {
        delete reg;
    }	
}

//---------------------------------------------------------------------------

