#include "ui.h"
#include "terminal-ui.h"
#include "zenity-ui.h"
#include "tclwish-ui.h"
#include "popup-terminal.h"


int UI_GuessDisplayType()
{
    char *Exec=NULL;

    if (! StrValid(getenv("DISPLAY"))) return(DISPLAYTYPE_TERMINAL);

    if (! isatty(0))
    {

        Exec=FindFileInPath(Exec, "wish", getenv("PATH"));
        if (StrValid(Exec))
        {
            Destroy(Exec);
            return(DISPLAYTYPE_WISH);
        }


        Exec=FindFileInPath(Exec, "qarma", getenv("PATH"));
        if (! StrValid(Exec)) Exec=FindFileInPath(Exec, "zenity", getenv("PATH"));
        if (! StrValid(Exec)) Exec=FindFileInPath(Exec, "yad", getenv("PATH"));
        if (StrValid(Exec))
        {
            Destroy(Exec);
            return(DISPLAYTYPE_DIALOG);
        }

    }

    Destroy(Exec);

    return(DISPLAYTYPE_TERMINAL);
}




STREAM *UI_Display(STREAM *S, TSoundCard *Card, TSoundCtl *VolCtl, TSoundCtl *MuteCtl)
{

    switch (AppConfig->DisplayType)
    {
    case DISPLAYTYPE_POPUP_TERM:
        return(PopupTerminal_Display(AppConfig->TerminalApp));
        break;

    case DISPLAYTYPE_DIALOG:
        return(ZenityUI_Display(Card, VolCtl, MuteCtl, ""));
        break;

    case DISPLAYTYPE_QARMA:
        return(ZenityUI_Display(Card, VolCtl, MuteCtl, "qarma"));
        break;


    case DISPLAYTYPE_YAD:
        return(ZenityUI_Display(Card, VolCtl, MuteCtl, "yad"));
        break;


    case DISPLAYTYPE_ZENITY:
        return(ZenityUI_Display(Card, VolCtl, MuteCtl, "zenity"));
        break;

    case DISPLAYTYPE_WISH:
        if (S) return(TCLWishUI_Update(S, Card, VolCtl, MuteCtl));
        return(TCLWishUI_Display(Card, VolCtl, MuteCtl));
        break;

    default:
        if (S) return(S);
        return(TerminalUI_Display(Card, VolCtl, MuteCtl));
        break;
    }

    return(NULL);
}


void UI_Update(STREAM *Term, TSoundCard *Card, TSoundCtl *VolCtl, TSoundCtl *MuteCtl)
{

    switch (AppConfig->DisplayType)
    {
    case DISPLAYTYPE_DIALOG:
    case DISPLAYTYPE_ZENITY:
    case DISPLAYTYPE_QARMA:
    case DISPLAYTYPE_YAD:
        break;

    case DISPLAYTYPE_WISH:
        TCLWishUI_Update(Term, Card, VolCtl, MuteCtl);
        break;

    default:
        TerminalUI_Update(Term, Card, VolCtl, MuteCtl);
        break;
    }

}


int UI_HandleInput(STREAM *S, int *Value)
{
    int result;

    STREAMSetTimeout(S, 25);
    result=STREAMWaitForBytes(S);

    if (result != 0)
    {
        switch (AppConfig->DisplayType)
        {
        case DISPLAYTYPE_DIALOG:
        case DISPLAYTYPE_ZENITY:
        case DISPLAYTYPE_QARMA:
        case DISPLAYTYPE_YAD:
            return(ZenityUI_HandleInput(S, Value));
            break;

        case DISPLAYTYPE_WISH:
            return(TCLWishUI_HandleInput(S, Value));
            break;

        default:
            return(TerminalUI_HandleInput(S, Value));
            break;
        }
    }

    return(UI_ACT_NONE);
}


void UI_Close(STREAM *Input)
{
    switch (AppConfig->DisplayType)
    {
    case DISPLAYTYPE_WISH:
        TCLWishUI_Close(Input);
        break;

    default:
        TerminalUI_Close(Input);
        break;
    }

}

