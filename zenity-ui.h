#ifndef ALSAVOL_ZENITY_UI_H
#define ALSAVOL_ZENITY_UI_H

#include "ui.h"

STREAM *ZenityUI_Display(TSoundCard *Card, TSoundCtl *VolCtl, TSoundCtl *MuteCtl, const char *Type);
int ZenityUI_HandleInput(STREAM *S, int *Value);

#endif


