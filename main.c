#include "common.h"
#include "sound-card.h"
#include "config.h"
#include "ui.h"

int ExitRequested=FALSE;
time_t LastEvent=0;
char *SelfPath=NULL;

void SigHandler(int Sig)
{
    if (Sig==SIGTERM) ExitRequested=TRUE;
    if (Sig==SIGINT) ExitRequested=TRUE;
}


TSoundCtl *SoundCardFindMasterControl(TSoundCard *Card, int Flags)
{
    ListNode *Curr;
    TSoundCtl *Ctl=NULL, *Master=NULL;
    char *Tempstr=NULL, *Name=NULL;

    Tempstr=MCopyStr(Tempstr, Card->Name, ":MasterVolume", NULL);
    Name=CopyStr(Name, GetVar(AppConfig->CardSettings, Tempstr));

    Curr=ListGetNext(Card->Controls);
    while (Curr)
    {
        Ctl=(TSoundCtl *) Curr->Item;
        if ((Ctl->Flags & Flags) == Flags)
        {
            if (StrValid(Name) && (strcmp(Ctl->Name, Name)==0) )
            {
                Master=Ctl;
                break;
            }

            if ((! Master) || (Ctl->Masterfulness > Master->Masterfulness)) Master=Ctl;
        }

        Curr=ListGetNext(Curr);
    }

    Destroy(Tempstr);

    return(Master);
}


TSoundCtl *SoundCardFindControl(TSoundCard *Card, const char *Name)
{
    ListNode *Node;

    Node=ListFindNamedItem(Card->Controls, Name);
    if (Node) return((TSoundCtl *) Node->Item);
    return(NULL);
}


TSoundCtl *SoundCardFindPartnerControl(TSoundCard *Card, const char *Main, const char *PartnerName)
{
    char *Type=NULL, *Direction=NULL, *Tempstr=NULL;
    const char *ptr;
    TSoundCtl *Ctl=NULL;

    ptr=GetToken(Main, "\\S", &Type, 0);
    ptr=GetToken(ptr, "\\S", &Direction, 0);
    ptr=GetToken(ptr, "\\S", &Tempstr, 0);

    Tempstr=MCopyStr(Tempstr, Type, " ", Direction, " ", PartnerName, " ", ptr, NULL);
    Ctl=SoundCardFindControl(Card, Tempstr);


    Destroy(Type);
    Destroy(Direction);
    Destroy(Tempstr);
    return(Ctl);
}




ListNode *SoundCardGetNext(ListNode *List)
{
    TSoundCtl *Ctl=NULL;
    TSoundCard *Card;
    ListNode *Curr;

    Curr=ListGetNext(List);
    while (Curr)
    {
        Card=(TSoundCard *) Curr->Item;
        Ctl=SoundCardFindMasterControl(Card, FLAG_PLAYBACK | FLAG_VOLUME);
        if (Ctl) break;
        Curr=ListGetNext(Curr);
    }

    return(Curr);
}


ListNode *SoundCardGetPrev(ListNode *List)
{
    TSoundCtl *Ctl=NULL;
    TSoundCard *Card;
    ListNode *Curr;

    Curr=ListGetPrev(List);
    while (Curr)
    {
        Card=(TSoundCard *) Curr->Item;
        Ctl=SoundCardFindMasterControl(Card, FLAG_PLAYBACK | FLAG_VOLUME);
        if (Ctl) break;
        Curr=ListGetPrev(Curr);
    }

    return(Curr);
}




void GetCurrCardAndControls(ListNode *Curr, TSoundCard **Card, TSoundCtl **VolCtl, TSoundCtl **MuteCtl)
{
    *Card=NULL;
    *VolCtl=NULL;
    *MuteCtl=NULL;

    if (Curr)
    {
        *Card=(TSoundCard *) Curr->Item;
        *VolCtl=SoundCardFindMasterControl(*Card, FLAG_PLAYBACK | FLAG_VOLUME);
        if (*VolCtl)
        {
            *MuteCtl=SoundCardFindPartnerControl(*Card, (*VolCtl)->Name, "Switch");
        }
    }

}


void SoundCardSet(ListNode *Node)
{
    TSoundCard *Card=NULL;
    TSoundCtl *VolCtl=NULL, *MuteCtl=NULL;
    int Value;

    GetCurrCardAndControls(Node, &Card, &VolCtl, &MuteCtl);
    Value=VolCtl->Max * AppConfig->VolumeLevel / 100;
    SoundControlSetValue(Card, VolCtl, Value);
    //anyting above 0 will unmute, 0 will mute
    SoundControlSetValue(Card, MuteCtl, Value);
}


void SoundCardsSet(ListNode *Cards)
{
    ListNode *Curr;

    Curr=SoundCardGetNext(Cards);
    while (Curr)
    {
        SoundCardSet(Curr);

        if (! (AppConfig->Flags & FLAG_SET_ALL)) break;
        Curr=SoundCardGetNext(Curr);
    }

}





ListNode *InteractiveProcessUIAction(ListNode *Curr, int Act, int Value, STREAM *InputS)
{
    TSoundCard *Card=NULL;
    TSoundCtl *VolCtl=NULL, *MuteCtl=NULL;
    int Change=FALSE;
    int AdjustAmount;
    ListNode *Cards;

    if (! Curr) return(NULL);

    Cards=ListGetHead(Curr);
    GetCurrCardAndControls(Curr, &Card, &VolCtl, &MuteCtl);

    switch (Act)
    {
    case UI_ACT_VALUE:
        if (VolCtl)
        {
            Value=VolCtl->Max * Value / 100;
            SoundControlSetValue(Card, VolCtl, Value);
        }
        break;

    case UI_ACT_DECR:
        AdjustAmount=(VolCtl->Max - VolCtl->Min) * AppConfig->VolumeDelta / 100;
        if (AdjustAmount < 1) AdjustAmount=1;
        if (VolCtl) SoundControlSetValue(Card, VolCtl, VolCtl->Values[0] - AdjustAmount);
        Change=TRUE;
        break;

    case UI_ACT_INCR:
        AdjustAmount=(VolCtl->Max - VolCtl->Min) * AppConfig->VolumeDelta / 100;
        if (AdjustAmount < 1) AdjustAmount=1;
        if (VolCtl) SoundControlSetValue(Card, VolCtl, VolCtl->Values[0] + AdjustAmount);
        if (MuteCtl) SoundControlSetValue(Card, MuteCtl, 1);
        Change=TRUE;
        break;

    case UI_ACT_PREV_DEVICE:
        if (Curr) Curr=SoundCardGetPrev(Curr);
        if (! Curr) Curr=SoundCardGetPrev(Cards);
        Change=TRUE;
        break;

    case UI_ACT_NEXT_DEVICE:
        if (Curr) Curr=SoundCardGetNext(Curr);
        if (! Curr) Curr=SoundCardGetNext(Cards);
        break;

    case UI_ACT_TOGGLE_MUTE:
        if (MuteCtl) SoundControlToggleValue(Card, MuteCtl);
        Change=TRUE;
        break;

    }

    return(Curr);
}


static int InteractiveExitRequired(int LastEvent)
{
 
if (ExitRequested==TRUE) return(TRUE);

if ( (AppConfig->Timeout > 0) && (LastEvent > 0) )
{
if ( (time(NULL) - LastEvent) > AppConfig->Timeout) return(TRUE);
}

return(FALSE);
}


void Interactive(ListNode *Cards)
{
    TSoundCard *Card=NULL;
    TSoundCtl *VolCtl=NULL, *MuteCtl=NULL;
    STREAM *InputS=NULL;
    ListNode *Curr, *Prev;
    int Act=UI_ACT_NONE;
    int Value;

    if (! Cards) return;

    //get first sound card
    Curr=SoundCardGetNext(Cards);
    if (! Curr) return;

    signal(SIGINT, SigHandler);
    signal(SIGTERM, SigHandler);

    GetCurrCardAndControls(Curr, &Card, &VolCtl, &MuteCtl);
		LastEvent=time(NULL);

    InputS=UI_Display(NULL, VolCtl->Card, VolCtl, MuteCtl);
    if (InputS)
    {
        while (Act != UI_ACT_QUIT)
        {
						if (InteractiveExitRequired(LastEvent)) break;
            Act=UI_HandleInput(InputS, &Value);

            if (Act != UI_ACT_NONE)
            {
								LastEvent=time(NULL);
                Prev=Curr;

                Curr=InteractiveProcessUIAction(Curr, Act, Value, InputS);
                if (Curr != Prev) //if UIAction changed which card we are displaying, then reopen display
                {
                    GetCurrCardAndControls(Curr, &Card, &VolCtl, &MuteCtl);
                    InputS=UI_Display(InputS, VolCtl->Card, VolCtl, MuteCtl);
                }
                UI_Update(InputS, VolCtl->Card, VolCtl, MuteCtl);
            }
        }

        UI_Close(InputS);
    }
    else if (AppConfig->DisplayType != DISPLAYTYPE_POPUP_TERM) printf("ERROR: cannot initialize display-type\n");
}

void DumpControls(TSoundCard *Card)
{
    ListNode *Curr;
    TSoundCtl *Ctl=NULL;

    printf("%s\n", Card->Name);
    Curr=ListGetNext(Card->Controls);
    while (Curr)
    {
        Ctl=(TSoundCtl *) Curr->Item;
        printf("   %s %d < %d > %d\n", Ctl->Name, Ctl->Min, Ctl->Values[0], Ctl->Max);
        Curr=ListGetNext(Curr);
    }
}


void DumpDevices(ListNode *Cards)
{
    ListNode *Curr;
    TSoundCard *Card=NULL;

    Curr=ListGetNext(Cards);
    while (Curr)
    {
        Card=(TSoundCard *) Curr->Item;
        DumpControls(Card);
        Curr=ListGetNext(Curr);
    }
}


void SetVolumeAction(ListNode *Cards)
{
    ListNode *Curr;

    if (StrValid(AppConfig->TargetCard))
    {
        if (isnum(AppConfig->TargetCard)) Curr=ListGetNth(Cards, atoi(AppConfig->TargetCard));
        else Curr=ListFindNamedItem(Cards, AppConfig->TargetCard);
        if (Curr) SoundCardSet(Curr);
        else fprintf(stderr, "NO SUCH CARD: [%s]\n", AppConfig->TargetCard);
    }
    else if (AppConfig->Flags & FLAG_SET_ALL)  SoundCardsSet(Cards);
}


STREAM *DoLockFile()
{
    STREAM *S;

    S=STREAMOpen(AppConfig->LockFile, "rwc");
    if (! S)
    {
        fprintf(stderr, "Can't open lockfile at: [%s]\n", AppConfig->LockFile);
        exit(1);
    }

    if (!  STREAMLock(S, LOCK_EX | LOCK_NB))
    {
        fprintf(stderr, "Can't lock lockfile at: [%s]\n", AppConfig->LockFile);
        exit(1);
    }

    return(S);
}



int main(int argc, char *argv[])
{
    ListNode *Cards;
    char *Tempstr=NULL;
    int Act;

    ProcessApplyConfig("nosu mdwe security=untrusted");

    SelfPath=CopyStr(SelfPath, argv[0]);

    ParseCommandLine(argc, argv);              //config file can be specified on the command-line, so parse args first
    ReadConfigFile(AppConfig->ConfigFile);     //now load config file..
    Act=ParseCommandLine(argc, argv);          //now parse args again, as these override config file

    if (! (AppConfig->Flags & FLAG_DEBUG)) ProcessApplyConfig("errnull");
    if (StrValid(AppConfig->LockFile)) DoLockFile();

    Cards=SoundCardsLoad();

    switch (Act)
    {
    case ACT_DUMP:
        DumpDevices(Cards);
        break;
    case ACT_SET_VOLUME:
        SetVolumeAction(Cards);
        break;
    case ACT_PERSIST:
				while (! InteractiveExitRequired(LastEvent)) Interactive(Cards);
        break;
    default:
        Interactive(Cards);
        break;
    }

    Destroy(Tempstr);

    return(0);
}
