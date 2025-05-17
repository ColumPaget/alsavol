#ifndef ALSAVOL_UI_H
#define ALSAVOL_UI_H


typedef enum{UI_ACT_NONE, UI_ACT_QUIT, UI_ACT_INCR, UI_ACT_DECR, UI_ACT_VALUE, UI_ACT_TOGGLE_MUTE, UI_ACT_MUTE, UI_ACT_UNMUTE, UI_ACT_PREV_DEVICE , UI_ACT_NEXT_DEVICE} E_UI_Actions;


#include "common.h"
#include "sound-card.h"
#include "config.h"

int UI_GuessDisplayType();
STREAM *UI_Display(STREAM *S, TSoundCard *Card, TSoundCtl *VolCtl, TSoundCtl *MuteCtl);
void UI_Update(STREAM *Term, TSoundCard *Card, TSoundCtl *VolCtl, TSoundCtl *MuteCtl);
int UI_HandleInput(STREAM *Term, int *Value);
void UI_Close(STREAM *Input);

#endif
