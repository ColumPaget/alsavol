#include "popup-terminal.h"
#include "terminal-ui.h"
#include "wmctrl.h"

static char *PopupTerminalFormatGeometry(char *RetStr, int x, int y, int wide, int high)
{
    char *Tempstr=NULL;

    RetStr=FormatStr(RetStr, " -g %dx%d", wide, high);
    if (AppConfig->Flags & DISPLAYFLAG_POSITION)
    {
        if (x > 0) Tempstr=FormatStr(Tempstr, "+%d", x);
        else Tempstr=FormatStr(Tempstr, "%d", x);
        RetStr=CatStr(RetStr, Tempstr);

        if (y > 0) Tempstr=FormatStr(Tempstr, "+%d", y);
        else Tempstr=FormatStr(Tempstr, "%d", y);
        RetStr=CatStr(RetStr, Tempstr);
    }

    Destroy(Tempstr);
    return(RetStr);
}



static char *PopupTerminal_Add_URXVT_Options(char *RetStr)
{
    RetStr=CatStr(RetStr, " +sb");
    if (AppConfig->Flags & DISPLAYFLAG_TRANSPARENT) RetStr=CatStr(RetStr, " -tr");
    if (AppConfig->Flags & DISPLAYFLAG_BORDERLESS) RetStr=CatStr(RetStr, " -bl");
    if (StrValid(AppConfig->Font)) RetStr=MCatStr(RetStr, " -fn '", AppConfig->Font, "'", NULL);
    if (StrValid(AppConfig->PopupHotKey)) RetStr=MCatStr(RetStr, " -kuake-hotkey ", AppConfig->PopupHotKey, NULL);

    return(RetStr);
}

static char *PopupTerminal_Add_ATerm_Options(char *RetStr)
{
    RetStr=CatStr(RetStr, " +sb");
    if (AppConfig->Flags & DISPLAYFLAG_TRANSPARENT) RetStr=CatStr(RetStr, " -tr");
    if (AppConfig->Flags & DISPLAYFLAG_BORDERLESS) RetStr=CatStr(RetStr, " -bl");
    if (StrValid(AppConfig->Font)) RetStr=MCatStr(RetStr, " -fn '", AppConfig->Font, "'", NULL);

    return(RetStr);
}

static char *PopupTerminal_Add_XTerm_Options(char *RetStr)
{
    RetStr=CatStr(RetStr, " +sb");
    if (StrValid(AppConfig->WindowClass)) RetStr=MCatStr(RetStr, " -class '", AppConfig->WindowClass, "'", NULL);
    if (StrValid(AppConfig->Font)) RetStr=MCatStr(RetStr, " -fn '", AppConfig->Font, "'", NULL);

    return(RetStr);
}

static char *PopupTerminal_Add_ST_Options(char *RetStr)
{
    if (StrValid(AppConfig->WindowClass)) RetStr=MCatStr(RetStr, " -class '", AppConfig->WindowClass, "'", NULL);
    if (StrValid(AppConfig->Font)) RetStr=MCatStr(RetStr, " -f '", AppConfig->Font, "'", NULL);

    return(RetStr);
}

static char *PopupTerminal_Add_MLTerm_Options(char *RetStr)
{
    RetStr=CatStr(RetStr, " --sb=false");
    RetStr=CatStr(RetStr, " --fg=white");
    if (AppConfig->Flags & DISPLAYFLAG_TRANSPARENT) RetStr=CatStr(RetStr, " --transbg=true");
    if (AppConfig->Flags & DISPLAYFLAG_BORDERLESS) RetStr=CatStr(RetStr, " --borderless=true");
    if (StrValid(AppConfig->Font)) RetStr=MCatStr(RetStr, " --deffont='", AppConfig->Font, "'", NULL);

    return(RetStr);
}


STREAM *PopupTerminal_Display(const char *TerminalApps)
{
    char *Tempstr=NULL, *TermExec=NULL, *Cmd=NULL, *Token=NULL;
    const char *ptr;
		STREAM *S;
    int i;

    ptr=GetToken(TerminalApps, ",", &Token, 0);
    while (ptr)
    {
        TermExec=FindFileInPath(TermExec, Token, getenv("PATH"));
        if (StrValid(TermExec)) break;
        ptr=GetToken(ptr, ",", &Token, 0);
    }

    if (StrValid(TermExec))
    {
        Cmd=MCopyStr(Cmd, TermExec, " ", NULL);

        if (strcmp(GetBasename(TermExec), "urxvt")==0) Cmd=PopupTerminal_Add_URXVT_Options(Cmd);
        if (strcmp(GetBasename(TermExec), "mlterm")==0) Cmd=PopupTerminal_Add_MLTerm_Options(Cmd);
        if (strcmp(GetBasename(TermExec), "aterm")==0) Cmd=PopupTerminal_Add_ATerm_Options(Cmd);
        if (strcmp(GetBasename(TermExec), "xterm")==0) Cmd=PopupTerminal_Add_XTerm_Options(Cmd);
        if (strcmp(GetBasename(TermExec), "st")==0) Cmd=PopupTerminal_Add_ST_Options(Cmd);

        Tempstr=PopupTerminalFormatGeometry(Tempstr, AppConfig->Xpos, AppConfig->Ypos, AppConfig->WindowWide, AppConfig->WindowHigh);
        Cmd=MCatStr(Cmd, " ", Tempstr, " -e ", SelfPath, " -t x11term ", NULL);
        if (StrValid(AppConfig->TextColor)) Cmd=MCatStr(Cmd, " -textcolor '", AppConfig->TextColor, "'", NULL);
        if (AppConfig->Flags & DISPLAYFLAG_BORDERLESS) Cmd=CatStr(Cmd, " -style 1line");


        printf("%s\n", Cmd);
        S=STREAMSpawnCommand(Cmd, "wr nostderr");

				if (S)
				{
        WMCtrlReconfigureWindow(STREAMGetValue(S, "PeerPID"), AppConfig->Flags & ~DISPLAYFLAG_POSITION);
				}
    }

    Destroy(Tempstr);
    Destroy(TermExec);
    Destroy(Token);
    Destroy(Cmd);

    return(S);
}

