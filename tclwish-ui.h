#ifndef ALSAVOL_TCLWISH_UI_H
#define ALSAVOL_TCLWISH_UI_H

#include "ui.h"


#include "tclwish-ui.h"

STREAM *TCLWishUI_Display(TSoundCard *Card, TSoundCtl *VolCtl, TSoundCtl *MuteCtl);
STREAM *TCLWishUI_Update(STREAM *S, TSoundCard *Card, TSoundCtl *VolCtl, TSoundCtl *MuteCtl);
int TCLWishUI_HandleInput(STREAM *S, int *Value);
void TCLWishUI_Close(STREAM *S);

#endif
