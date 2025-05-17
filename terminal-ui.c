#include "terminal-ui.h"


#define COLOR_FOREGROUND 0
#define COLOR_BACKGROUND 1

char *Terminal_UI_FormatColor(char *RetStr, int ForeOrBack, const char *Config)
{
    RetStr=CopyStr(RetStr, "");

    if (strcasecmp(Config, "red")==0) RetStr=CopyStr(RetStr, "~r");
    else if (strcasecmp(Config, "magenta")==0) RetStr=CopyStr(RetStr, "~m");
    else if (strcasecmp(Config, "yellow")==0) RetStr=CopyStr(RetStr, "~y");
    else if (strcasecmp(Config, "blue")==0) RetStr=CopyStr(RetStr, "~b");
    else if (strcasecmp(Config, "cyan")==0) RetStr=CopyStr(RetStr, "~c");
    else if (strcasecmp(Config, "green")==0) RetStr=CopyStr(RetStr, "~g");
    else if (strcasecmp(Config, "black")==0) RetStr=CopyStr(RetStr, "~n");
    else if (strcasecmp(Config, "white")==0) RetStr=CopyStr(RetStr, "~w");
    else if (strcasecmp(Config, "inverse")==0) RetStr=CopyStr(RetStr, "~i");
    else if (strcasecmp(Config, "reverse")==0) RetStr=CopyStr(RetStr, "~i");
    else RetStr=CopyStr(RetStr, Config);

    if (ForeOrBack == COLOR_BACKGROUND) strupr(RetStr);
    else strlwr(RetStr);


    return(RetStr);
}



static int TerminalUI_ChooseDisplayStyle(STREAM *Term, int wide, int high)
{
    if (AppConfig->DisplayStyle > DISPLAYSTYLE_NONE) return(AppConfig->DisplayStyle);

    if (AppConfig->DisplayType == DISPLAYTYPE_X11)
    {
        if (high > 1) return(DISPLAYSTYLE_2LINE);
        else return(DISPLAYSTYLE_POPUPTERM);
    }
    return(DISPLAYSTYLE_1LINE);
}



static char *TerminalUI_StyleTitles(char *ProgressBarSetup, STREAM *Term, int style, TSoundCard *Card, int TermWide, const char *DisplayName)
{
    char *Title=NULL, *Tempstr=NULL;


    if (AppConfig->DisplayType == DISPLAYTYPE_X11)
    {
        Title=MCopyStr(Title, "volume: ", DisplayName, NULL);
        XtermSetTitle(Term, Title);
        TerminalClear(Term);
    }

    if (TermWide > AppConfig->WindowWide) TermWide=AppConfig->WindowWide;

    if (style == DISPLAYSTYLE_COMPACT)
    {
        ProgressBarSetup=MCatStr(ProgressBarSetup, " innertext='", DisplayName, "'", NULL);
        Tempstr=FormatStr(Tempstr, " width=%d", TermWide - 4);
        ProgressBarSetup=CatStr(ProgressBarSetup, Tempstr);
    }
    else if (style == DISPLAYSTYLE_POPUPTERM)
    {
        ProgressBarSetup=MCatStr(ProgressBarSetup, " innertext='$(percent)%'", NULL);
        Tempstr=FormatStr(Tempstr, " width=%d", TermWide - 4);
        ProgressBarSetup=CatStr(ProgressBarSetup, Tempstr);
    }
    else if (style == DISPLAYSTYLE_1LINE)
    {
        ProgressBarSetup=MCatStr(ProgressBarSetup, " prompt='", AppConfig->TextColor, DisplayName, ":~0' innertext='$(percent)%'", NULL);
        Tempstr=FormatStr(Tempstr, " width=%d", TermWide - StrLen(DisplayName) -4);
        ProgressBarSetup=CatStr(ProgressBarSetup, Tempstr);
    }
    else
    {
        ProgressBarSetup=MCatStr(ProgressBarSetup, " innertext='$(percent)%'", NULL);
        Tempstr=FormatStr(Tempstr, " width=%d", TermWide - 4);
        ProgressBarSetup=CatStr(ProgressBarSetup, Tempstr);
        Title=MCopyStr(Title, "\n", DisplayName, "\n", NULL);
        TerminalPutStr(Title, Term);
    }

    Destroy(Title);
    Destroy(Tempstr);

    return(ProgressBarSetup);
}


void TerminalUI_Update(STREAM *Term, TSoundCard *Card, TSoundCtl *VolCtl, TSoundCtl *MuteCtl)
{
    char *Tempstr=NULL, *Title=NULL, *Config=NULL, *DisplayName=NULL;
    TERMPROGRESS *TP;
    int wide, high;
    int val, style;

    TerminalGeometry(Term, &wide, &high);
    style=TerminalUI_ChooseDisplayStyle(Term, wide, high);

		DisplayName=GetCardSetting(DisplayName, Card->Name, "DisplayName");
		if (! StrValid(DisplayName)) DisplayName=CopyStr(DisplayName, Card->Name);


    if ((! MuteCtl) || (MuteCtl->Values[0] > 0))
    {
        Config=CopyStr(Config, " remain=' ' progress=' ' attribs=~0");
        if (StrValid(AppConfig->GaugeBarColor) || (AppConfig->GaugeTextColor))
        {
            Config=CatStr(Config, " progress-attribs=");
            if (StrValid(AppConfig->GaugeBarColor))
            {
                Tempstr=Terminal_UI_FormatColor(Tempstr, COLOR_BACKGROUND, AppConfig->GaugeBarColor);
                Config=CatStr(Config, Tempstr);
            }

            if (StrValid(AppConfig->GaugeTextColor))
            {
                Tempstr=Terminal_UI_FormatColor(Tempstr, COLOR_FOREGROUND,  AppConfig->GaugeTextColor);
                Config=CatStr(Config, Tempstr);
            }
        }

        Config=TerminalUI_StyleTitles(Config, Term, style, Card, wide, DisplayName);

        TP=TerminalProgressCreate(Term, Config);
        val=VolCtl->Values[0] * 100 / VolCtl->Max;
        Tempstr=FormatStr(Tempstr, "level: %d", VolCtl->Values[0]);
        TerminalProgressUpdate(TP, VolCtl->Values[0], VolCtl->Max, "");
        TerminalProgressDestroy(TP);
        TerminalPutStr("~>", Term);
    }
    else
    {
        Tempstr=FormatStr(Tempstr, "\r%s: ~rMUTED~0~>", DisplayName);
        TerminalPutStr(Tempstr, Term);
    }

    STREAMFlush(Term);

    Destroy(DisplayName);
    Destroy(Tempstr);
    Destroy(Config);
    Destroy(Title);
}



int TerminalUI_HandleInput(STREAM *Term, int *Value)
{
    int inchar, RetVal=UI_ACT_NONE;

    inchar=TerminalReadChar(Term);
    switch (inchar)
    {
		case -1:
    case TKEY_ESCAPE:
    case TKEY_DELETE:
    case TKEY_BACKSPACE:
    case TKEY_END:
    case 'q':
        RetVal=UI_ACT_QUIT;
        break;

    case TKEY_VOL_DOWN:
    case TKEY_LEFT:
    case TKEY_PGDN:
    case MOUSE_BTN_5:
    case ',':
    case '-':
        RetVal=UI_ACT_DECR;
        break;


    case TKEY_VOL_UP:
    case TKEY_RIGHT:
    case MOUSE_BTN_4:
    case TKEY_PGUP:
    case '.':
    case '+':
        RetVal=UI_ACT_INCR;
        break;

    case TKEY_UP:
    case '[':
    case 'p':
        RetVal=UI_ACT_PREV_DEVICE;
        break;


    case TKEY_DOWN:
    case TKEY_TAB:
    case TKEY_ENTER:
    case TKEY_HOME:
    case ']':
    case 'n':
        RetVal=UI_ACT_NEXT_DEVICE;
        break;

    case 'm':
    case TKEY_MEDIA_MUTE:
        RetVal=UI_ACT_TOGGLE_MUTE;
        break;
    }

    return(RetVal);
}


void TerminalUI_Close(STREAM *Term)
{
    TerminalPutStr("\r\n", Term);
    TerminalReset(Term);
    STREAMFlush(Term);
}


STREAM *TerminalUI_Display(TSoundCard *Card, TSoundCtl *VolCtl, TSoundCtl *MuteCtl)
{
    STREAM *Term;

    Term=STREAMFromDualFD(0,1);
    STREAMSetTimeout(Term, 10);
    TerminalInit(Term, TERM_RAWKEYS|TERM_WHEELMOUSE|TERM_SAVEATTRIBS);
    TerminalUI_Update(Term, Card, VolCtl, MuteCtl);

    return(Term);
}

