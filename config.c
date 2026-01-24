#include "config.h"
#include "ui.h"
#include "terminal-ui.h"
#include "help.h"

TConfig *AppConfig=NULL;


static int InitConfig()
{
    if (! AppConfig) AppConfig=(TConfig *) calloc(1, sizeof(TConfig));
    AppConfig->WindowWide=35;
    AppConfig->WindowHigh=1;
    AppConfig->DisplayType=UI_GuessDisplayType();
    AppConfig->CardIgnoreList=CopyStr(AppConfig->CardIgnoreList, "hw,oss,shm,arcam_av,sysdefault,default");
    AppConfig->CardSettings=ListCreate();
    AppConfig->TextColor=CopyStr(AppConfig->TextColor, "");
    AppConfig->GaugeBarColor=CopyStr(AppConfig->GaugeBarColor, "blue");
    AppConfig->GaugeTextColor=CopyStr(AppConfig->GaugeTextColor, "yellow");
    AppConfig->TerminalApp=CopyStr(AppConfig->TerminalApp, "urxvt,aterm,xterm,mlterm,st");
    AppConfig->ConfigFile=MCopyStr(AppConfig->ConfigFile, GetCurrUserHomeDir(), "/.config/alsavol.conf", NULL);

}


static void ParseCommandLineDisplayType(const char *Type)
{
    if (strcmp(Type, "dialog")==0) AppConfig->DisplayType=DISPLAYTYPE_DIALOG;
    else if (strcmp(Type, "x11")==0) AppConfig->DisplayType=DISPLAYTYPE_X11;
    else if (strcmp(Type, "x11term")==0) AppConfig->DisplayType=DISPLAYTYPE_X11;
    else if (strcmp(Type, "popterm")==0) AppConfig->DisplayType=DISPLAYTYPE_POPUP_TERM;
    else if (strcmp(Type, "pterm")==0) AppConfig->DisplayType=DISPLAYTYPE_POPUP_TERM;
    else if (strcmp(Type, "zenity")==0) AppConfig->DisplayType=DISPLAYTYPE_ZENITY;
    else if (strcmp(Type, "qarma")==0) AppConfig->DisplayType=DISPLAYTYPE_QARMA;
    else if (strcmp(Type, "yad")==0) AppConfig->DisplayType=DISPLAYTYPE_YAD;
    else if (strcmp(Type, "tui")==0) AppConfig->DisplayType=DISPLAYTYPE_TERMINAL;
    else if (strcmp(Type, "term")==0) AppConfig->DisplayType=DISPLAYTYPE_TERMINAL;
    else if (strcmp(Type, "terminal")==0) AppConfig->DisplayType=DISPLAYTYPE_TERMINAL;
    else if (strcmp(Type, "wish")==0) AppConfig->DisplayType=DISPLAYTYPE_WISH;
    else if (strcmp(Type, "tcl")==0) AppConfig->DisplayType=DISPLAYTYPE_WISH;
    else if (strcmp(Type, "urxvt")==0)
    {
        AppConfig->DisplayType=DISPLAYTYPE_POPUP_TERM;
        AppConfig->TerminalApp=CopyStr(AppConfig->TerminalApp, "urxvt,aterm,st,xterm,mlterm");
    }
}

static void ParseCommandLineDisplayStyle(const char *Type)
{
    if (strcmp(Type, "basic")==0) AppConfig->DisplayStyle=DISPLAYSTYLE_BASIC;
    if (strcmp(Type, "1")==0) AppConfig->DisplayStyle=DISPLAYSTYLE_1LINE;
    if (strcmp(Type, "1line")==0) AppConfig->DisplayStyle=DISPLAYSTYLE_1LINE;
    if (strcmp(Type, "2")==0) AppConfig->DisplayStyle=DISPLAYSTYLE_2LINE;
    if (strcmp(Type, "2line")==0) AppConfig->DisplayStyle=DISPLAYSTYLE_2LINE;
    if (strcmp(Type, "compact")==0) AppConfig->DisplayStyle=DISPLAYSTYLE_2LINE;
    if (strcmp(Type, "pterm")==0) AppConfig->DisplayStyle=DISPLAYSTYLE_POPUPTERM;
}


void ReadConfigFileParsePopupFlags(const char *Flags)
{
    char *Token=NULL;
    const char *ptr;

    ptr=GetToken(Flags, ",", &Token, GETTOKEN_QUOTES);
    while (ptr)
    {
        if (strcasecmp(Token, "above")==0) AppConfig->Flags |= DISPLAYFLAG_ABOVE;
        if (strcasecmp(Token, "sticky")==0) AppConfig->Flags |= DISPLAYFLAG_STICKY;
        if (strcasecmp(Token, "borderless")==0) AppConfig->Flags |= DISPLAYFLAG_BORDERLESS;
        if (strcasecmp(Token, "bl")==0) AppConfig->Flags |= DISPLAYFLAG_BORDERLESS;
        if (strcasecmp(Token, "transparent")==0) AppConfig->Flags |= DISPLAYFLAG_TRANSPARENT;
        if (strcasecmp(Token, "tr")==0) AppConfig->Flags |= DISPLAYFLAG_TRANSPARENT;
        ptr=GetToken(ptr, ",", &Token, GETTOKEN_QUOTES);
    }

    Destroy(Token);
}


static void ReadConfigFileParseSettings(ListNode *Settings)
{
    const char *p_Value;


    //This appends to existing card ignore list, so we do not need to check if it's blank or not
    AppConfig->CardIgnoreList=CatStr(AppConfig->CardIgnoreList, ParserGetValue(Settings, "IgnoreCards"));

    //As these have no default, so we don't need to check if the values was supplied in settings
    //as there is no default we are going to override
    AppConfig->CardOrder=CopyStr(AppConfig->CardOrder, ParserGetValue(Settings, "CardOrder"));
    AppConfig->LockFile=CopyStr(AppConfig->LockFile, ParserGetValue(Settings, "LockFile"));
    AppConfig->WindowClass=CopyStr(AppConfig->WindowClass, ParserGetValue(Settings, "WindowClass"));
    AppConfig->Font=CopyStr(AppConfig->Font, ParserGetValue(Settings, "Font"));
    AppConfig->PopupHotKey=CopyStr(AppConfig->PopupHotKey, ParserGetValue(Settings, "PopupHotKey"));


    p_Value=ParserGetValue(Settings, "DisplayType");
    if (StrValid(p_Value)) ParseCommandLineDisplayType(p_Value);

    p_Value=ParserGetValue(Settings, "DisplayStyle");
    if (StrValid(p_Value)) ParseCommandLineDisplayStyle(p_Value);

    p_Value=ParserGetValue(Settings, "TerminalApp");
    if (StrValid(p_Value)) AppConfig->TerminalApp=CopyStr(AppConfig->TerminalApp, p_Value);

    p_Value=ParserGetValue(Settings, "TextColor");
    if (StrValid(p_Value)) AppConfig->TextColor=CopyStr(AppConfig->TextColor, p_Value);

    p_Value=ParserGetValue(Settings, "GaugeBarColor");
    if (StrValid(p_Value)) AppConfig->GaugeBarColor=CopyStr(AppConfig->GaugeBarColor, p_Value);

    p_Value=ParserGetValue(Settings, "GaugeTextColor");
    if (StrValid(p_Value)) AppConfig->GaugeTextColor=CopyStr(AppConfig->GaugeTextColor, p_Value);

    p_Value=ParserGetValue(Settings, "PopupFlags");
    if (StrValid(p_Value)) ReadConfigFileParsePopupFlags(p_Value);


    p_Value=ParserGetValue(Settings, "WindowX");
    if (StrValid(p_Value))
    {
        AppConfig->Xpos=atoi(p_Value);
        AppConfig->Flags |= DISPLAYFLAG_POSITION;
    }

    p_Value=ParserGetValue(Settings, "WindowY");
    if (StrValid(p_Value))
    {
        AppConfig->Ypos=atoi(p_Value);
        AppConfig->Flags |= DISPLAYFLAG_POSITION;
    }

}


static void ReadConfigFileParseCardSetting(ListNode *Settings, const char *CardName, const char *SettingName)
{
    char *Tempstr=NULL;
    const char *p_Value;

    p_Value=ParserGetValue(Settings, SettingName);
    if (StrValid(p_Value))
    {
        Tempstr=MCopyStr(Tempstr, CardName, ":", SettingName, NULL);
        SetVar(AppConfig->CardSettings, Tempstr, p_Value);
    }

    Destroy(Tempstr);
}

static void ReadConfigFileParseCard(ListNode *Settings, const char *CardName)
{
    ReadConfigFileParseCardSetting(Settings, CardName, "DisplayName");
    ReadConfigFileParseCardSetting(Settings, CardName, "MasterVolume");
}


int ReadConfigFile(const char *Path)
{
    ListNode *Config=NULL, *Curr;
    char *Tempstr=NULL;
    STREAM *S;

    if (! AppConfig) InitConfig();

    S=STREAMOpen(Path, "r");
    if (S)
    {
        Tempstr=STREAMReadDocument(Tempstr, S);
        STREAMClose(S);

        Config=ParserParseDocument("ini", Tempstr);
        Curr=ListGetNext(Config);
        while (Curr)
        {
            if (CompareStr(Curr->Tag, "Settings")==0) ReadConfigFileParseSettings((ListNode *) Curr->Item);
            else ReadConfigFileParseCard((ListNode *) Curr, Curr->Tag);
            Curr=ListGetNext(Curr);
        }
    }

    Destroy(Tempstr);
}


int ParseCommandLine(int argc, char *argv[])
{
    int Act=ACT_INTERACT;
    CMDLINE *Cmd;
    const char *arg;
    int i;

    if (! AppConfig) InitConfig();

    Cmd=CommandLineParserCreate(argc, argv);
    arg=CommandLineNext(Cmd);
    while (arg != NULL)
    {
        if (strcmp(arg, "-d")==0) Act=ACT_DUMP;
        else if (strcmp(arg, "-p")==0) Act=ACT_PERSIST;
        else if (strcmp(arg, "-persist")==0) Act=ACT_PERSIST;
        else if (strcmp(arg, "-t")==0) ParseCommandLineDisplayType(CommandLineNext(Cmd));
        else if (strcmp(arg, "-s")==0) ParseCommandLineDisplayStyle(CommandLineNext(Cmd));
        else if (strcmp(arg, "-style")==0) ParseCommandLineDisplayStyle(CommandLineNext(Cmd));
        else if (strcmp(arg, "-above")==0) AppConfig->Flags |= DISPLAYFLAG_ABOVE;
        else if (strcmp(arg, "-sticky")==0) AppConfig->Flags |= DISPLAYFLAG_STICKY;
        else if (strcmp(arg, "-tr")==0) AppConfig->Flags |= DISPLAYFLAG_TRANSPARENT;
        else if (strcmp(arg, "-bl")==0) AppConfig->Flags |= DISPLAYFLAG_BORDERLESS;
        else if (strcmp(arg, "-all")==0) AppConfig->Flags |= FLAG_SET_ALL;
        else if (strcmp(arg, "-c")==0) AppConfig->TargetCard=CopyStr(AppConfig->TargetCard, CommandLineNext(Cmd));
        else if (strcmp(arg, "-card")==0) AppConfig->TargetCard=CopyStr(AppConfig->TargetCard, CommandLineNext(Cmd));
        else if (strcmp(arg, "-set")==0)
        {
            AppConfig->VolumeLevel=atoi(CommandLineNext(Cmd));
            Act=ACT_SET_VOLUME;
        }
        else if (strcmp(arg, "-x")==0)
        {
            AppConfig->Xpos = atoi(CommandLineNext(Cmd));
            AppConfig->Flags |= DISPLAYFLAG_POSITION;
        }
        else if (strcmp(arg, "-y")==0)
        {
            AppConfig->Ypos = atoi(CommandLineNext(Cmd));
            AppConfig->Flags |= DISPLAYFLAG_POSITION;
        }
        else if (strcmp(arg, "-w")==0) AppConfig->WindowWide = atoi(CommandLineNext(Cmd));
        else if (strcmp(arg, "-h")==0) AppConfig->WindowHigh = atoi(CommandLineNext(Cmd));
        else if (strcmp(arg, "-wide")==0) AppConfig->WindowWide = atoi(CommandLineNext(Cmd));
        else if (strcmp(arg, "-high")==0) AppConfig->WindowHigh = atoi(CommandLineNext(Cmd));
        else if (strcmp(arg, "-term")==0) AppConfig->TerminalApp=CopyStr(AppConfig->TerminalApp, CommandLineNext(Cmd));
        else if (strcmp(arg, "-hotkey")==0) AppConfig->PopupHotKey=CopyStr(AppConfig->PopupHotKey, CommandLineNext(Cmd));
        else if (strcmp(arg, "-fg")==0) AppConfig->TextColor=CopyStr(AppConfig->TextColor, CommandLineNext(Cmd));
        else if (strcmp(arg, "-textcolor")==0) AppConfig->TextColor=CopyStr(AppConfig->TextColor, CommandLineNext(Cmd));
        else if (strcmp(arg, "-font")==0) AppConfig->Font=CopyStr(AppConfig->Font, CommandLineNext(Cmd));
        else if (strcmp(arg, "-fn")==0) AppConfig->Font=CopyStr(AppConfig->Font, CommandLineNext(Cmd));
        else if (strcmp(arg, "-l")==0) AppConfig->LockFile=CopyStr(AppConfig->LockFile, CommandLineNext(Cmd));
        else if (strcmp(arg, "-lock")==0) AppConfig->LockFile=CopyStr(AppConfig->LockFile, CommandLineNext(Cmd));
        else if (strcmp(arg, "-class")==0) AppConfig->WindowClass=CopyStr(AppConfig->WindowClass, CommandLineNext(Cmd));
        else if (strcmp(arg, "-order")==0) AppConfig->CardOrder=CopyStr(AppConfig->CardOrder, CommandLineNext(Cmd));
        else if (strcmp(arg, "-O")==0) AppConfig->CardOrder=CopyStr(AppConfig->CardOrder, CommandLineNext(Cmd));
        else if (strcmp(arg, "-ignore")==0) AppConfig->CardIgnoreList=CopyStr(AppConfig->CardIgnoreList, CommandLineNext(Cmd));
        else if (strcmp(arg, "-I")==0) AppConfig->CardIgnoreList=CopyStr(AppConfig->CardIgnoreList, CommandLineNext(Cmd));
        else if (strcmp(arg, "-D")==0) AppConfig->Flags |= FLAG_DEBUG;
        else if (strcmp(arg, "-debug")==0) AppConfig->Flags |= FLAG_DEBUG;
        else if (strcmp(arg, "-?")==0) PrintHelp();
        else if (strcmp(arg, "-help")==0) PrintHelp();
        else if (strcmp(arg, "--help")==0) PrintHelp();
        else if (strcmp(arg, "--version")==0) PrintVersion();

        arg=CommandLineNext(Cmd);
    }

    return(Act);
}



char *GetCardSetting(char *RetStr, const char *CardName, const char *Setting)
{
    char *Tempstr=NULL;

    Tempstr=MCopyStr(Tempstr, CardName, ":", Setting, NULL);
    RetStr=CopyStr(RetStr, GetVar(AppConfig->CardSettings, Tempstr));

    Destroy(Tempstr);

    return(RetStr);
}
