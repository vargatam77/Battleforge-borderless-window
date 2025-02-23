//---------------------------------------------------------------------------

#ifndef BFresizeH
#define BFresizeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "trayicon.h"
#include <ImgList.hpp>
#include <Menus.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmBF : public TForm
{
__published:	// IDE-managed Components
    TTimer *tmrCursorLock;
	TTrayIcon *trIcon;
	TImageList *ilIcons;
	TPopupMenu *mnuTray;
	TMenuItem *mnuRestore;
	TMenuItem *mnuExit;
	TMainMenu *mnuMain;
	TMenuItem *mnuLook;
	TMenuItem *mnuClose;
	TMenuItem *mnuAutoStart;
	TMenuItem *mnuStart;
	TProgressBar *pbSearching;
    void __fastcall tmrCursorLockTimer(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall mnuRestoreClick(TObject *Sender);
	void __fastcall mnuExitClick(TObject *Sender);
	void __fastcall mnuStartClick(TObject *Sender);
	void __fastcall mnuAutoStartClick(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TfrmBF(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmBF *frmBF;
//---------------------------------------------------------------------------
#endif
