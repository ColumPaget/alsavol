#include "tclwish-ui.h"
#include "wmctrl.h"

static char *TCLWishFormatPosition(char *RetStr, int x, int y)
{
    char *Tempstr=NULL;

    if (AppConfig->Flags & DISPLAYFLAG_POSITION)
    {
    		RetStr=CatStr(RetStr, " -geometry ");
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



STREAM *TCLWishUI_Display(TSoundCard *Card, TSoundCtl *VolCtl, TSoundCtl *MuteCtl)
{
    STREAM *S;
    char *Exec=NULL, *Tempstr=NULL, *Cmd=NULL;
    int percent;

    Exec=FindFileInPath(Exec, "wish", getenv("PATH"));

    if (StrValid(Exec))
    {
				Tempstr=FormatStr(Tempstr, "%s -name 'alsavol ppid=%lu'", Exec, (unsigned long) getpid());
				Tempstr=TCLWishFormatPosition(Tempstr, AppConfig->Xpos, AppConfig->Ypos);

        S=STREAMSpawnCommand(Tempstr, "rw nostderr");
        if (S)
        {
            Cmd=CopyStr(Cmd, "proc OutputScale {vol} {puts stdout \"volume=$vol\"}\n");
            Cmd=CatStr(Cmd, "proc OutputMute {} {puts stdout \"mute\"}\n");
            Cmd=CatStr(Cmd, "proc OutputNext {} {puts stdout \"next\"}\n");
            Cmd=CatStr(Cmd, "proc OutputDone {} {puts stdout \"done\"}\n");
            Cmd=MCatStr(Cmd, "set card_name \"", Card->Name, "\"\n", NULL);
        		percent=VolCtl->Values[0] * 100 / VolCtl->Max;
            Tempstr=FormatStr(Tempstr, "set volume %d\n", percent);
            Cmd=CatStr(Cmd, Tempstr);
            Cmd=CatStr(Cmd, "label .card_name -text \"$card_name\"\n");
            Cmd=CatStr(Cmd, "scale .volume_scale -orient horizontal -from 0 -to 100 -showvalue true -variable volume -command OutputScale\n");
            Cmd=CatStr(Cmd, "button .mute -text Mute -command OutputMute\n");
            Cmd=CatStr(Cmd, "button .next -text Next -command OutputNext\n");
            Cmd=CatStr(Cmd, "button .done -text Done -command OutputDone\n");
            Cmd=CatStr(Cmd, "pack .card_name\npack .volume_scale\npack .mute .next .done -side left\n");
            STREAMWriteLine(Cmd, S);
            TCLWishUI_Update(S, Card, VolCtl, MuteCtl);

            STREAMFlush(S);

    		    WMCtrlReconfigureWindow(STREAMGetValue(S, "PeerPID"), AppConfig->Flags & ~DISPLAYFLAG_POSITION);
        }
    }

    Destroy(Tempstr);
    Destroy(Exec);
    Destroy(Cmd);

    return(S);
}


STREAM *TCLWishUI_Update(STREAM *S, TSoundCard *Card, TSoundCtl *VolCtl, TSoundCtl *MuteCtl)
{
    char *Tempstr=NULL;

    Tempstr=MCopyStr(Tempstr, ".card_name configure -text \"", Card->Name, "\"\n", NULL);
    STREAMWriteLine(Tempstr, S);

    if (! MuteCtl->Values[0]) Tempstr=CopyStr(Tempstr, ".mute configure -foreground red\n");
    else Tempstr=CopyStr(Tempstr, ".mute configure -foreground black\n");

    STREAMWriteLine(Tempstr, S);
    STREAMFlush(S);

    Destroy(Tempstr);

    return(S);
}


int TCLWishUI_HandleInput(STREAM *S, int *Value)
{
    char *Tempstr=NULL;
    int RetVal=UI_ACT_NONE;

    Tempstr=STREAMReadLine(Tempstr, S);
    if (! Tempstr) return(UI_ACT_QUIT);

    StripTrailingWhitespace(Tempstr);

    if (strcmp(Tempstr, "mute")==0) RetVal=UI_ACT_TOGGLE_MUTE;
    else if (strcmp(Tempstr, "next")==0) RetVal=UI_ACT_NEXT_DEVICE;
    else if (strcmp(Tempstr, "done")==0) RetVal=UI_ACT_QUIT;
    else if (strncmp(Tempstr, "volume=", 7) == 0)
    {
        *Value=atoi(Tempstr+7);
        RetVal=UI_ACT_VALUE;
    }

    Destroy(Tempstr);

    return(RetVal);
}


void TCLWishUI_Close(STREAM *S)
{
    char *Tempstr=NULL;

    STREAMWriteLine("exit\n", S);
    Tempstr=STREAMReadLine(Tempstr, S);
    while (Tempstr)
    {
        Tempstr=STREAMReadLine(Tempstr, S);
    }
    STREAMClose(S);

    Destroy(Tempstr);
}
