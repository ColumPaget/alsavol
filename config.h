#ifndef ALSAVOL_CONFIG_H
#define ALSAVOL_CONFIG_H

#include "common.h"

#define ACT_INTERACT   0
#define ACT_SET_VOLUME 1
#define ACT_DUMP       2
#define ACT_PERSIST    3



typedef enum {DISPLAYTYPE_TERMINAL, DISPLAYTYPE_DIALOG, DISPLAYTYPE_X11, DISPLAYTYPE_POPUP_TERM, DISPLAYTYPE_ZENITY, DISPLAYTYPE_QARMA, DISPLAYTYPE_YAD, DISPLAYTYPE_WISH} EDisplayType;

#define DISPLAYFLAG_ABOVE        1
#define DISPLAYFLAG_STICKY       2
#define DISPLAYFLAG_POSITION     4
#define DISPLAYFLAG_AT_MOUSE     8
#define DISPLAYFLAG_TRANSPARENT 16
#define DISPLAYFLAG_BORDERLESS  32

#define FLAG_SET_ALL    8192
#define FLAG_DEBUG     16384

typedef struct
{
int VolumeLevel;
int DisplayType;
int DisplayStyle;
int Flags;
int Xpos;
int Ypos;
int WindowWide;
int WindowHigh;
char *ConfigFile;
char *TargetCard;
char *CardIgnoreList;
char *CardOrder;
char *TerminalApp;
char *WindowClass;
char *TextColor;
char *GaugeBarColor;
char *GaugeTextColor;
char *BackgroundTint;
char *Font;
char *PopupHotKey;
char *LockFile;
ListNode *CardSettings;
} TConfig;

extern TConfig *AppConfig;

const char *ConfigGetValue(const char *Section, const char *Value);
int ReadConfigFile(const char *Path);
int ParseCommandLine(int argc, char *argv[]);
char *GetCardSetting(char *RetStr, const char *CardName, const char *Setting);

#endif
