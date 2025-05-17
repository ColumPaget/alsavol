#include "zenity-ui.h"
#include "wmctrl.h"
#include <wait.h>

#define EXEC_TYPE_UNKNOWN -1

const char *ExecCandidates[]= {"yad", "zenity","qarma", NULL};
typedef enum {EXEC_TYPE_YAD, EXEC_TYPE_ZENITY, EXEC_TYPE_QARMA} E_ExecTypes;



static int ExecType(const char *Path)
{
    const char *ptr;

    if (StrValid(Path))
    {
        ptr=GetBasename(Path);
        if (StrValid(ptr)) return(MatchTokenFromList(ptr, ExecCandidates, 0));
    }

    return(EXEC_TYPE_UNKNOWN);
}





char *ZenityUI_AppendYADOptions(char *RetStr)
{
    char *Tempstr=NULL;

    RetStr=CatStr(RetStr, " --button=Mute:2 --button=Next:0 --button=Done:1");
    if (AppConfig->Flags & DISPLAYFLAG_ABOVE) RetStr=CatStr(RetStr, " --on-top");
    if (AppConfig->Flags & DISPLAYFLAG_STICKY) RetStr=CatStr(RetStr, " --sticky");
    if (AppConfig->Flags & DISPLAYFLAG_POSITION)
    {
        Tempstr=FormatStr(Tempstr, " --posx=%d --posy=%d", AppConfig->Xpos, AppConfig->Xpos);
        RetStr=CatStr(RetStr, Tempstr);
    }

    Destroy(Tempstr);

    return(RetStr);
}


static char *ZenityUI_FindExec(char *RetStr, const char *Type)
{
    int i;

    RetStr=CopyStr(RetStr, "");
    if (StrValid(Type))
    {
        RetStr=FindFileInPath(RetStr, Type, getenv("PATH"));
    }
    else
    {
        for (i=0; ExecCandidates[i] != NULL; i++)
        {
            RetStr=FindFileInPath(RetStr, ExecCandidates[i], getenv("PATH"));
            if (StrValid(RetStr)) break;
        }
    }

    return(RetStr);
}



STREAM *ZenityUI_Display(TSoundCard *Card, TSoundCtl *VolCtl, TSoundCtl *MuteCtl, const char *Type)
{
    char *Exec=NULL, *Tempstr=NULL, *WMCtrl=NULL, *Title=NULL;
    STREAM *S=NULL;
    int i;


    Exec=ZenityUI_FindExec(Exec, Type);

    if (StrValid(Exec))
    {
        Title=CopyStr(Title, Card->Name);
        if (MuteCtl && (MuteCtl->Values[0] == 0)) Title=CatStr(Title, "[MUTED]");


        Tempstr=FormatStr(Tempstr, "%s \"--title=alsavol\" \"--text=%s\" --scale --print-partial --ok-label=next --cancel-label=done --min-value=0 --max-value=100 --value=%d", Exec, Title, SoundCtlGetPercent(VolCtl));
        if (ExecType(Exec)==EXEC_TYPE_YAD)  Tempstr=ZenityUI_AppendYADOptions(Tempstr);

        S=STREAMSpawnCommand(Tempstr, "rw pty nostderr noshell");
				if (S)
				{
        Tempstr=STREAMReadLine(Tempstr, S);
        if (ExecType(Exec) != EXEC_TYPE_YAD) WMCtrlReconfigureWindow(STREAMGetValue(S, "PeerPID"), AppConfig->Flags);
				}
    }

    Destroy(Tempstr);
    Destroy(WMCtrl);
    Destroy(Title);
    Destroy(Exec);

    return(S);
}


int ZenityUI_HandleInput(STREAM *S, int *Value)
{
    char *Tempstr=NULL;
    int RetVal=UI_ACT_NONE;
    pid_t Pid;
    int status;

    Tempstr=STREAMReadLine(Tempstr, S);
    if (! Tempstr)
    {
        Pid=atoi(STREAMGetValue(S, "PeerPID"));
        waitpid(Pid, &status, 0);
        if (WEXITSTATUS(status) == 2) return(UI_ACT_MUTE);
        if (WEXITSTATUS(status) == 0) return(UI_ACT_NEXT_DEVICE);
        return(UI_ACT_QUIT);
    }
    StripTrailingWhitespace(Tempstr);

    if (isnum(Tempstr))
    {
        *Value=atoi(Tempstr);
        RetVal=UI_ACT_VALUE;
    }

    Destroy(Tempstr);

    return(RetVal);
}


