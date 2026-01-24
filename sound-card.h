#ifndef ALSAVOL_SOUNDCARD_H
#define ALSAVOL_SOUNDCARD_H

#include "common.h"

#define FLAG_CAPTURE  1
#define FLAG_PLAYBACK 2
#define FLAG_VOLUME   4
#define FLAG_SWITCH   8


typedef struct
{
char *ID;
char *Name;
char *DisplayName;
char *NameHint;
ListNode *Controls;
} TSoundCard;


typedef struct
{
int Idx;
int Type;
int Flags;
char *Name;
int Min;
int Max;
int ValueCount;
int Values[10];
int Masterfulness;
float dbMin;
float dbMax;
TSoundCard *Card;
} TSoundCtl;


void SoundCardDestroy(void *p_Card);
void SoundCtlDestroy(void *p_Ctl);
TSoundCard *SoundCardGet(int cardno);
ListNode *SoundCardsLoad();
int SoundControlSetValue(TSoundCard *Card, TSoundCtl *Ctl, int Value);
int SoundControlToggleValue(TSoundCard *Card, TSoundCtl *Ctl);
int SoundCtlGetPercent(TSoundCtl *Ctl);

#endif

