#include "sound-card.h"
#include <alsa/asoundlib.h>
#include "config.h"

/***** HERE BE DRAGONS *****

alsa is yet another WTF? API. There are multiple 'levels' of API and this
file uses the lowest level, since part of the motivation for doing this
was to learn how to use libalsa, not just the simplified mixer API.

ALSA supplies a barrage of functions, with functions to allocate and free
every type of structure that it uses. Hence the 'sound_ctl_info_t' structure
that holds information about a sound control (like a volume control or a
tone control or a mute switch or 'auto gain' switch) has the following
helper functions

    snd_ctl_elem_info_t *info;

    snd_ctl_elem_info_alloca(&info);

    snd_ctl_elem_info_malloc(&info);
    snd_ctl_elem_info_free(&info);

The 'alloca' version allocates the structure on the stack, so it doesn't need
an associated 'free' function, as the data is freed when the stack is unwound
at function exit. However, I'm mortally afraid of putting data on the stack, as
I grew up in the days when buffer overflows on the stack were the major source
of vulnerabilties, back before ROP chains were invented, and dinosaurs roamed
the earth. So I'll stick to using the  'malloc' and 'free' variant here.

Frequently one has to read data into a structure, like the snd_ctl_elem_info_t
structure, and one has to request the data for a specific entitiy, like a specific
sound card, or a specific sound control among many. To do this one allocates the
structure, and then sets an 'id' value within the structure which specifies which
sound card or control we want information for. Then one calls some kind of 'load'
or 'read' function which actually reads in the data for the specified entity.

For example

    snd_ctl_elem_value_t *obj;
    int value=0;

    snd_ctl_elem_value_malloc(&obj);
    snd_ctl_elem_value_set_numid(obj, id);
    snd_ctl_elem_read(ctl, obj);
    value=snd_ctl_elem_value_get_integer(obj, 0);
    snd_ctl_elem_value_free(obj);

This reads in an structure that holds data relating to the value of a control.
We first malloc the structure, then we set, within the structure,  a numeric ID
that specifies which control we're interested in. Now we call 'snd_ctl_elem_read'
to actually populate the structure. We then call 'snd_ctl_elem_value_get_integer'
to get the integer value of the control from the data we read. Then we free the
structure.


*/





void SoundCardDestroy(void *p_Card)
{
    TSoundCard *Card;

    Card=(TSoundCard *) p_Card;
    if (Card)
    {
        Destroy(Card->Name);
        Destroy(Card->NameHint);
        Destroy(Card->DisplayName);
        ListDestroy(Card->Controls, SoundCtlDestroy);
        free(Card);
    }
}


void SoundCtlDestroy(void *p_Ctl)
{
    TSoundCtl *Ctl;

    Ctl=(TSoundCtl *) p_Ctl;
    if (Ctl)
    {
        Destroy(Ctl->Name);
        free(Ctl);
    }

}


//this function tries to give a score to how likely a sound-card
//control is to be the 'master' control that effects the volume
//of all outputs on the sound card. Some cards have a 'master'
//volume, but others do not. Headphones might have
int SoundCardElementEvaluateMasterfulness(TSoundCtl *Ctl)
{
    char *Token=NULL, *Name=NULL;
    const char *ptr;

    //first find "Playback" or "Capture", everything before that
    //is the card name
    ptr=GetToken(Ctl->Name, "\\S", &Token, 0);
    while (ptr)
    {
        if (strcasecmp(Token, "Capture")==0)
        {
            Ctl->Flags |= FLAG_CAPTURE;
            break;
        }

        if (strcasecmp(Token, "Playback")==0)
        {
            Ctl->Flags |= FLAG_PLAYBACK;
            break;
        }

        if (strcasecmp(Token, "Master")==0) Ctl->Masterfulness=9;
        else if (strcasecmp(Token, "Headset")==0) Ctl->Masterfulness=8;
        else if (strcasecmp(Token, "PCM")==0) Ctl->Masterfulness=7;
        else if (strcasecmp(Token, " Headphone+LO")==0) Ctl->Masterfulness=6;
        else if (strcasecmp(Token, " Headphone")==0) Ctl->Masterfulness=5;
        else if (strcasecmp(Token, "Speaker")==0) Ctl->Masterfulness=4;
        else if (strcasecmp(Token, "Line Out")==0) Ctl->Masterfulness=3;
        else if (strcasecmp(Token, "LO")==0) Ctl->Masterfulness=3;
        else if (strcasecmp(Token, "Beep")==0) Ctl->Masterfulness=2;


        ptr=GetToken(ptr, "\\S", &Token, 0);
    }

    ptr=GetToken(ptr, "\\S", &Token, 0);
    if (strcasecmp(Token, "Switch")==0) Ctl->Flags |= FLAG_SWITCH;
    if (strcasecmp(Token, "Volume")==0) Ctl->Flags |= FLAG_VOLUME;

    Destroy(Token);
    Destroy(Name);

    return(Ctl->Masterfulness);
}


static void SoundCardElementGetdbRange(snd_ctl_t *ctl, snd_ctl_elem_id_t *value_id, TSoundCtl *Ctl)
{
    long minDB, maxDB;

    snd_ctl_get_dB_range(ctl, value_id, &minDB, &maxDB);
    Ctl->dbMin=((float) minDB) * 0.01;
    Ctl->dbMax=((float) maxDB) * 0.01;
}


static int SoundCardElementGetValue(snd_ctl_t *ctl, int type, int id, TSoundCtl *Ctl)
{
    snd_ctl_elem_value_t *obj;
    snd_ctl_elem_id_t *value_id;
    int i;

    snd_ctl_elem_value_malloc(&obj);
    snd_ctl_elem_value_set_numid(obj, id);
    snd_ctl_elem_read(ctl, obj);

    for (i=0; i < Ctl->ValueCount; i++)
    {
        if (type=='i') Ctl->Values[i]=snd_ctl_elem_value_get_integer(obj, i);
        else if (type == 'b') Ctl->Values[i]=snd_ctl_elem_value_get_boolean(obj, i);

    }

    if (type == 'i')
    {
        snd_ctl_elem_id_malloc(&value_id);
        snd_ctl_elem_value_get_id(obj, value_id);
        SoundCardElementGetdbRange(ctl, value_id, Ctl);
        snd_ctl_elem_id_free(value_id);
    }


    snd_ctl_elem_value_free(obj);

    return(Ctl->Values[0]);
}


static int SoundCardElementSetValue(snd_ctl_t *ctl, int id, int value, TSoundCtl *Ctl)
{
    snd_ctl_elem_value_t *obj;
    int result, i;

    snd_ctl_elem_value_malloc(&obj);
    snd_ctl_elem_value_set_numid(obj, id);

    for (i=0; i < Ctl->ValueCount; i++)
    {
        snd_ctl_elem_value_set_integer(obj, i, value);
    }

    result=snd_ctl_elem_write(ctl, obj);
    snd_ctl_elem_value_free(obj);

    return(result);
}




static TSoundCtl *SoundElementGet(snd_ctl_t *ctl, int id)
{
    snd_ctl_elem_info_t *info;
    TSoundCtl *Ctl;
    int type;

    snd_ctl_elem_info_malloc(&info);
    snd_ctl_elem_info_set_numid(info, id);
//    snd_ctl_elem_info_set_interface(info, SND_CTL_ELEM_IFACE_MIXER);
    snd_ctl_elem_info(ctl, info);

    Ctl=(TSoundCtl *) calloc(1, sizeof(TSoundCtl));
    Ctl->Idx=id;
    Ctl->Type='?';


    type=snd_ctl_elem_info_get_type(info);
    switch (type)
    {
    case SND_CTL_ELEM_TYPE_INTEGER:
        Ctl->Type='i';
        break;
    case SND_CTL_ELEM_TYPE_BOOLEAN:
        Ctl->Type='b';
        break;
    case SND_CTL_ELEM_TYPE_ENUMERATED:
        Ctl->Type='e';
        break;
    }

    Ctl->Name=FormatStr(Ctl->Name, "%s %d", snd_ctl_elem_info_get_name(info), snd_ctl_elem_info_get_index(info));
    if (Ctl->Type == 'i')
    {
        Ctl->Min=snd_ctl_elem_info_get_min(info);
        Ctl->Max=snd_ctl_elem_info_get_max(info);
    }

    Ctl->ValueCount=snd_ctl_elem_info_get_count(info);
    SoundCardElementGetValue(ctl, Ctl->Type, id, Ctl);

    SoundCardElementEvaluateMasterfulness(Ctl);

    snd_ctl_elem_info_free(info);

    return(Ctl);
}

static int SoundCardControlsGet(TSoundCard *Card)
{
    char *Tempstr=NULL;
    snd_ctl_t *ctl;
    snd_ctl_elem_list_t *elem_list;
    TSoundCtl *Ctl;
    int count, result, i;

    if (! Card->Controls) Card->Controls=ListCreate();
    result=snd_ctl_open(&ctl, Card->ID, 0);
    if (result == 0)
    {
        // Get list of elements
        snd_ctl_elem_list_malloc(&elem_list);
        snd_ctl_elem_list(ctl, elem_list);

        count = snd_ctl_elem_list_get_count(elem_list);
        snd_ctl_elem_list_alloc_space(elem_list, count);
        snd_ctl_elem_list(ctl, elem_list);

        for (i=0; i < count ; i++)
        {
            Ctl=SoundElementGet(ctl, snd_ctl_elem_list_get_numid(elem_list, i));
            Ctl->Card=Card;
            ListAddNamedItem(Card->Controls, Ctl->Name, Ctl);
        }
        snd_ctl_elem_list_free_space(elem_list);
        snd_ctl_elem_list_free(elem_list);

        snd_ctl_close(ctl);
    }

    Destroy(Tempstr);

return(TRUE);
}


TSoundCard *SoundCardCreate(ListNode *CardList, const char *ID, const char *Name)
{
    TSoundCard *Card=NULL;

    if (! StrValid(Name)) return(NULL);
    if (! InStringList(ID, AppConfig->CardIgnoreList, ","))
    {
        Card=(TSoundCard *) calloc(1, sizeof(TSoundCard));
        Card->ID=CopyStr(Card->ID, ID);
        Card->Name=CopyStr(Card->Name, Name);
        StripTrailingWhitespace(Card->Name);

        Card->DisplayName=GetCardSetting(Card->DisplayName, Card->Name, "DisplayName");
        if (! StrValid(Card->DisplayName)) Card->DisplayName=CopyStr(Card->DisplayName, Card->Name);


        SoundCardControlsGet(Card);

        if (AppConfig->Flags & FLAG_DEBUG) fprintf(stderr, "Card: [%s]\n", Card->Name);

        if (Card && CardList) ListAddNamedItem(CardList, Card->Name, Card);
    }
    else if (AppConfig->Flags & FLAG_DEBUG) fprintf(stderr, "ignoring card: [%s]\n", Name);

    return(Card);
}



/*
The official way of getting 'virtual' sound cards,
such as bluealsa bluetooth devices etc, is to use the
'hints' system to load a list of cardname hints.
Unfortunately this doesn't seem to actually work.
Thus we find soundcards that have been defined in
the bluealsa config and try loading each of those.
*/

void LoadConfigSoundCardControls(ListNode *CardList)
{
    const char *p_value;
    snd_config_iterator_t iter;
    snd_config_t *item, *ctl_config;
    char *ID=NULL;

    snd_config_update();
    if (snd_config_search(snd_config, "ctl", &ctl_config)==0)
    {
        iter=snd_config_iterator_first(ctl_config);
        while (iter != snd_config_iterator_end(ctl_config))
        {
            item=snd_config_iterator_entry(iter);

            if (item)
            {
                snd_config_evaluate(item, snd_config, NULL, NULL);

                snd_config_get_id(item, &p_value);
                ID=CopyStr(ID, p_value);
                SoundCardCreate(CardList, ID, ID);
            }

            iter=snd_config_iterator_next(iter);
        }
    }

    Destroy(ID);
}



void SoundCardsReorderCopyMatching(ListNode *Dest, ListNode *Src, const char *MatchPattern)
{
    ListNode *Curr;

    Curr=ListGetNext(Src);
    while (Curr)
    {

        if (
            pmatch_one(MatchPattern, Curr->Tag, StrLen(Curr->Tag), NULL, NULL, 0) &&
            (! ListFindNamedItem(Dest, Curr->Tag))
        )
            ListAddNamedItem(Dest, Curr->Tag, Curr->Item);

        Curr=ListGetNext(Curr);
    }

}


ListNode *SoundCardsReorder(ListNode *Cards)
{
    ListNode *NewList;
    char *Token=NULL;
    const char *ptr;

    if (! StrValid(AppConfig->CardOrder)) return(Cards);

    NewList=ListCreate();

    ptr=GetToken(AppConfig->CardOrder, ",", &Token, GETTOKEN_QUOTES);
    while (ptr)
    {
        SoundCardsReorderCopyMatching(NewList, Cards, Token);
        ptr=GetToken(ptr, ",", &Token, GETTOKEN_QUOTES);
    }

    ListDestroy(Cards, NULL);
    Destroy(Token);

    return(NewList);
}


ListNode *SoundCardsLoad()
{
    ListNode *CardList;
    char *ID=NULL, *Name=NULL;


//must set cardno -1 so that 'snd_card_next' returns
//sound card '0'
    int cardno=-1;

    CardList=ListCreate();


    //first we load physical sound cards
    while (snd_card_next(&cardno) == 0)
    {
        if (cardno < 0) break;
        ID=FormatStr(ID, "hw:%d", cardno);
        snd_card_get_name(cardno, &Name);
        SoundCardCreate(CardList, ID, Name);
    }

    LoadConfigSoundCardControls(CardList);

    CardList=SoundCardsReorder(CardList);


    Destroy(ID);
    Destroy(Name);

    return(CardList);
}




int SoundControlSetValue(TSoundCard *Card, TSoundCtl *Ctl, int Value)
{
    char *Tempstr=NULL;
    snd_ctl_t *ctl;
    int result;


    switch (Ctl->Type)
    {
    case 'i':
        if (Value > Ctl->Max) Value=Ctl->Max;
        if (Value < Ctl->Min) Value=Ctl->Min;
        break;

    case 'b':
        if (Value > 0) Value=1;
        if (Value < 0) Value=0;
        break;
    }

    result=snd_ctl_open(&ctl, Card->ID, 0);
    if (result == 0)
    {
        SoundCardElementSetValue(ctl, Ctl->Idx, Value, Ctl);
        SoundCardElementGetValue(ctl, Ctl->Type, Ctl->Idx, Ctl);

        snd_ctl_close(ctl);
    }

    Destroy(Tempstr);
return(TRUE);
}


int SoundControlToggleValue(TSoundCard *Card, TSoundCtl *Ctl)
{
    char *Tempstr=NULL;
    int Value, result;
    snd_ctl_t *ctl;

    result=snd_ctl_open(&ctl, Card->ID, 0);
    if (result == 0)
    {
        Value=Ctl->Values[0] ^ 1;
        SoundCardElementSetValue(ctl, Ctl->Idx, Value, Ctl);
        SoundCardElementGetValue(ctl, Ctl->Type, Ctl->Idx, Ctl);

        snd_ctl_close(ctl);
    }

    Destroy(Tempstr);

return(TRUE);
}


int SoundCtlGetPercent(TSoundCtl *Ctl)
{
    return(Ctl->Values[0] * 100 / Ctl->Max - Ctl->Min);
}

