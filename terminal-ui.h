#ifndef ALSAVOL_TERMINAL_UI_H
#define ALSAVOL_TERMINAL_UI_H

#include "ui.h"


#define DISPLAYSTYLE_NONE      0
#define DISPLAYSTYLE_1LINE     1
#define DISPLAYSTYLE_2LINE     2
#define DISPLAYSTYLE_BASIC     3
#define DISPLAYSTYLE_COMPACT   4
#define DISPLAYSTYLE_POPUPTERM 5


void TerminalUI_Update(STREAM *Term, TSoundCard *Card, TSoundCtl *VolCtl, TSoundCtl *MuteCtl);
int TerminalUI_HandleInput(STREAM *Term, int *Value);
STREAM *TerminalUI_Display(TSoundCard *Card, TSoundCtl *VolCtl, TSoundCtl *MuteCtl);
void TerminalUI_Close(STREAM *Term);

#endif


