#include "wmctrl.h"
#include "config.h"


//wmctrl -d produces list of desktops and their available space
//0  * DG: 1920x1080  VP: 0,0  WA: 0,0 1920x1059  1

static int WMCtrlGetDesktopAvailable(const char *WMCtrlPath, int *wide, int *high)
{
    char *Tempstr=NULL, *Token=NULL, *Found=NULL;
    const char *ptr;
    int RetVal=FALSE;
    STREAM *S;

    Tempstr=MCopyStr(Tempstr, WMCtrlPath, " -d", NULL);
    S=STREAMSpawnCommand(Tempstr, "rw noshell nostderr");
    if (S)
    {
        Tempstr=STREAMReadLine(Tempstr, S);
        while (Tempstr)
        {
            StripTrailingWhitespace(Tempstr);
            ptr=GetToken(Tempstr, "\\S", &Token, 0); //desktop number

            ptr=GetToken(ptr, "\\S", &Token, 0); //is current desktop
            if (CompareStr(Token, "*")==0)
            {
                while (ptr)
                {
                    if (CompareStr(Token, "WA:")==0)
                    {
                        ptr=GetToken(ptr, "\\S", &Token, 0);
                        ptr=GetToken(ptr, "\\S", &Found, 0);
                    }
                    ptr=GetToken(ptr, "\\S", &Token, 0);
                }
            }
            Tempstr=STREAMReadLine(Tempstr, S);
        }
        STREAMClose(S);
    }

    if (StrValid(Found))
    {
        ptr=GetToken(Found, "x", &Token, 0);
        *wide=atoi(Token);
        ptr=GetToken(ptr, "x", &Token, 0);
        *high=atoi(Token);
        RetVal=TRUE;
    }

    Destroy(Tempstr);
    Destroy(Found);
    Destroy(Token);

    return(RetVal);
}


static char *WMCtrlGetWindowID(char *RetStr, const char *WMCtrlPath, const char *PeerPID)
{
    char *Tempstr=NULL, *Token=NULL, *ID=NULL, *Name=NULL;
    const char *ptr;
    STREAM *S;

    RetStr=CopyStr(RetStr, "");
    Name=FormatStr(Name, "alsavol ppid=%lu", (unsigned long)  getpid());

    Tempstr=MCopyStr(Tempstr, WMCtrlPath, " -l -p", NULL);
    S=STREAMSpawnCommand(Tempstr, "rw noshell nostderr");
    if (S)
    {
        Tempstr=STREAMReadLine(Tempstr, S);
        while (Tempstr)
        {
            StripTrailingWhitespace(Tempstr);

            ptr=GetToken(Tempstr, "\\S", &ID, 0); //window id
            ptr=GetToken(ptr, "\\S", &Token, 0);  //something, I know not what
            ptr=GetToken(ptr, "\\S", &Token, 0);  // pid
            if (atoi(Token) == atoi(PeerPID)) RetStr=CopyStr(RetStr, ID);

            ptr=GetToken(ptr, "\\S", &Token, 0); //hostname
            if (CompareStr(Name, ptr)==0)  RetStr=CopyStr(RetStr, ID);

            if (StrValid(RetStr)) break;

            Tempstr=STREAMReadLine(Tempstr, S);
        }
        STREAMClose(S);
    }

    Destroy(Tempstr);
    Destroy(Token);
    Destroy(Name);
    Destroy(ID);

    return(RetStr);
}


void WMCtrlReconfigureWindow(const char *PeerPID, int Flags)
{
    char *WMCtrlPath=NULL, *WinID=NULL, *Tempstr=NULL, *Cmd=NULL;
    int i, x, y, wide, high, deskwide, deskhigh;

    if (! Flags) return;

    WMCtrlPath=FindFileInPath(WMCtrlPath, "wmctrl", getenv("PATH"));
    if (StrValid(WMCtrlPath))
    {

        for (i=0; i < 10; i++)
        {
            usleep(100000);
            WinID=WMCtrlGetWindowID(WinID, WMCtrlPath, PeerPID);
            if (StrValid(WinID)) break;
        }

        Cmd=MCopyStr(Cmd, WMCtrlPath, " -i -r ", WinID, " ", NULL);
        if (Flags & (DISPLAYFLAG_ABOVE | DISPLAYFLAG_STICKY))
        {
            Cmd=CatStr(Cmd, "-b add");
            if (Flags & DISPLAYFLAG_ABOVE) Cmd=CatStr(Cmd, ",above");
            if (Flags & DISPLAYFLAG_STICKY) Cmd=CatStr(Cmd, ",sticky");
        }

        if (Flags & DISPLAYFLAG_POSITION)
        {
            WMCtrlGetDesktopAvailable(WMCtrlPath, &deskwide, &deskhigh);

            x=AppConfig->Xpos;
            wide=300;
            if (x < 0) x=deskwide - wide - x - 4;

            y=AppConfig->Ypos;
            high=100;
            if (y < 0) y=deskhigh - high - y - 4;

            Tempstr=FormatStr(Tempstr, " -e 0,%d,%d,300,100", x, y);
            Cmd=CatStr(Cmd, Tempstr);
        }

        system(Cmd);

    }

    Destroy(WMCtrlPath);
    Destroy(Tempstr);
    Destroy(WinID);
    Destroy(Cmd);
}

