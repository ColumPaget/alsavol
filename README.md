WHAT
====

**alsavol** is a simple ALSA volume control ('simple' as in a single slider). It works either at the terminal, or using 'dialog' apps like zenity, qarma and yad, or using the TCL/Tk 'wish' shell. It can also popup an x11 terminal containing itself using urxvt, xterm, mlterm, or st. It can be configured to know which sound-card on the system is relevant, or just to change volume of all cards simultaneously.

WHO
===

**alsavol** is Copyright (C) 2025 Colum Paget. It's released under the GPLv3 so you may do anything with them that the GPLv3 allows.

Email: colums.projects@gmail.com


WHY
===

I wanted a volume knob. Not a mixer, an equalizer, or a homage to the medbay diagnostics screen from Start Trek TOS, just a volume knob. I wanted this volume knob to work in a terminal so I could use it even on gui-less systems. I wanted it not to use the function keys, as I have many keyboards that lack those without playing chords on the keyboard. I wanted it to work with bluealsa so I could control bluetooth headphones. I don't care about capture volume or stereo balance, so for the initial release alsavol concens itself only with playback volume. I wanted it to present me with the minimum possible information needed to set the playback volume.

I also thought it would be a good idea to get my hands dirty with some libALSA programming. I realize now that was a mistake. 

Achiving these goals is more of a campaign than one might expect. ALSA has no idea which control on a device is the "master" volume control. Hence most volume control apps for ALSA present ALL the controls a card supports, creating a confusing multiple choice display. Also ALSA has no idea what order cards should be handled in. It's quite possible, for instance, that USB sound devices will be recognized as 'card zero' over internal devices on the PCI bus. Thus, when the USB device is unplugged, there is no 'card zero' any more, and some ALSA software will crash or error out if there is no card zero.

Hence alsavol has a config file, perhaps surprisingly for a simple volume-setter app, but this config file allows specifying which subcontrol of a sound device is the one we want to treat as the volume, changing the displayed name of sound devices (the default is not always informative), configuring display style, etc, etc.


HOW
===


**alsavol** supports several display types. These are:

term 
: display in terminal
dialog 
: display using whichever of zenity, qarma or yad is found on the system
zenity 
: display using the 'zenity' dialog application
qarma
: display using the 'qarma' dialog application
yad
: display using the 'yad' dialog application
wish
: display using the tcl 'wish' shell
pterm
: display using a popup terminal using one of urxvt, xterm, aterm, mlterm or st
urxvt
: display using a 'urxvt' as a popup window


alsavol's default behavior is to display a slider bar in the terminal/console. This slider can be moved using keys or the mousewheel. 

If 'dialog'. 'zenity', 'qarma', 'yad' or 'wish' modes are used then a popup window is created using one of those applications. Keystrokes listed below in 'TERMINAL KEYS' are not honored in these modes, and the resulting display is entirely controlled by the dialog system. 

'pterm' and 'urxvt' are modes where alsavol displays itself within a dedicated terminal window. In this mode 'TERMINAL KEYS' are honored, and there are some unique options, like urxvt's 'kuake hotkey' feature.



TERMINAL KEYS
=============


decrease volume:
  * left arrow
  * <
  * ,
  * -
  * mousewheel 

increase volume:
  * right arrow
  * >
  * .
  * +
  * mousewheel 

next sound card:
  * tab
  * home
  * down arrow
  * ]
  * n

prev sound card:
  * up arrow
  * [
  * p

mute:
  * m

exit app:
	* q
  * escape
  * delete
  * backspace
  * end


The 'volume up', 'volume down' and 'mute' media keys may also be honored, but this is a whole other kettle of fish, especially as these keys are usually grabbed by other applications.


COMMAND LINE OPTIONS
====================

```
-d             print a list of available devices and their controls
-f <path>      config file
-config <path> config file
-c             specify target sound card (see 'Card Order' below)
-card          specify target sound card (see 'Card Order' below)
-D             debug mode, output some errors etc
-debug         debug mode, output some errors etc
-set <level>   set volume level
-all           apply volume change to all sound cards
-t <type>      display type, one of 'term', 'dialog', 'zenity', 'qarma', 'yad', 'wish', 'pterm'
-term <type>   terminal app to use with 'pterm' display type
-s <name>      display style, one of 'basic', '1line', '2line', 'compact' and 'pterm'
-style <name>  display style, one of 'basic', '1line', '2line', 'compact' and 'pterm'
-x <pos>       x-position of display window for pterm, wish and yad windows
-y <pos>       y-position of display window for pterm, wish and yad windows
-w <value>     width in character cells for pterm windows
-wide <value>  width in character cells for pterm windows
-h <value>     height in character cells for pterm windows
-high <value>  height in character cells for pterm windows
-tr            transparent background: only for 'pterm' displays using urxvt, st, mlterm or aterm
-bl            borderless: only for 'pterm' displays using st, mlterm or aterm
-above         set window to be above other windows
-p             persist. Respawn if window is closed, mostly for use with pterm display type
-persist       persist. Respawn if window is closed, mostly for use with pterm display type
-hotkey <key>  popup hotkey for use with the 'urxvt' display type
-font <name>   font to use with pterm displays
-fn <name>     font to use with pterm displays
-class <name>  window class (used to configure window-related settings in window-manager)
-l <path>      path to lockfile, used to ensure only one alsavol is active at a time
-lock <path>   path to lockfile, used to ensure only one alsavol is active at a time
-fg <color>    foreground text color
-textcolor <color>    foreground text color
-?             this help
-help          this help
--help         this help
```


CONFIG FILE
===========

The config file is essentially a dos/windows 'ini' file (yes, I know, I know, I just felt it was a good fit for this application). It has a main stanza called 'settings' and contains global configurations. Every other stanza has the name of a sound device, and contains settings relevant to that device.

For example:

```
[Settings]
DisplayType=wish
PopupFlags=above,sticky
WindowX=-1
WindowY=-1

[HDA Intel PCH]
DisplayName=Intel HDA

[Corsair VOID ELITE USB Gaming H]
DisplayName=Corsair Void Elite
MasterVolume=Headset Playback Volume 1
```


This config file will create a popup control window at the bottom right corner of the screen using the 'wish' TCL/TK system. Two sound devices are recognized, both given tidier names than their default ALSA name, and the Corsair Void Elite headphones have their master volume declared to be 'Headset Playback Volume 1' (there is also 'Headset Playback Volume 0', which as far as I can tell has no effect on playback volume, and perhaps controls sidetone or some such thing).


## Options in the 'Settings' Stanza of the config file

IgnoreCards=<list>         
: do not display any sound cards in comma-separated list <list> (see 'Card Order' below)

CardOrder=<list>         
: display sound cards in comma-separated list <list>, in the order they are listed (see 'Card Order' below)

DisplayType=<name>
: type of GUI display to use, values are: term,pterm,dialog,wish,zenity,qarma,yad,urxvt

DisplayStyle=<name>        
: style of display to use for 'term' and 'pterm' display types, values are: 'basic', '1line', '2line', 'compact' and 'pterm'

WindowX=<pos>
: x position of window for pterm, wish, dialog, etc display types

WindowY=<pos>
: y position of window for pterm, wish, dialog, etc display types

WindowClass=<class>
: 'class' of X11 window. Used to hint to window manager so that it can treat the window in specific ways.

LockFile=<path>            
: <path> to lock file used to prevent multiple copies of alsavol being launched. Useful with popup display types where the window is already displayed.

TerminalApp=<name>
: x11 terminal app to use with DisplayType pterm, this can be a comma-separated list of apps, and the first one found will be used

PopupHotKey=<key> 
: hotkey to be used with urxvt 'kuake' mode. urxvt does not support modifiers, so this can only be a keyname like 'F1'

PopupFlags=<list>
: comma-separated list of the following strings: 'above', 'sticky', 'transparent', 'tr', 'borderless', 'bl'. See 'window settings' below.

Font=<name>
: font to use with 'pterm' display type

TextColor=<color> 
: color for text section of terminal output: this can be a 16-color name like 'red', 'blue' or a 'tilde identifier' like ~r, ~b. 

GaugeBarColor=<color> 
: color for gauge/slider bar part of terminal output: this can be a 16-color name like 'red', 'blue' or a 'tilde identifier' like ~r, ~b. 

GaugeTextColor=<color>
: color for text overlaid over the gauge/slider bar part of terminal output: this can be a 16-color name like 'red', 'blue' or a 'tilde identifier' like ~r, ~b. 



## Options in a 'Sound card' Stanza of the config file

DisplayName=<string>
: Display <string> as the name of this sound card
MasterVolume=<name>
: The name of the 'real' volume control on this sound card is 'name' ("alsavol -d" gives a list of card and control names)




CARD ORDER
==========

There are a number of config-file and command-line settings that control which sound-cards are displayed, and in what order. These are applied a different stages of processing the sound-card list.

`IgnoreCards` or `-ignore` or `-I`
: This is a comma-separated list of cards to ignore. It's applied when we gather a list of cards from the ALSA API. **alsavol** already has an internal list of card names to ignore, consisting of "hw,oss,shm,arcam_av,sysdefault,default" and the user-supplied names are added to this ignore list. 

`CardOrder` or `-order` or `-O`
: This is a comma-separated list of shell-style pattern matches that allows setting the order in which cards are displayed to the user. If a card is not listed in this order then it is not displayed, which is another way to ignore a card. And Example of use might be:

```
CardOrder=*Corsair VOID*,*Headset,*
```

Which would display cards in the order: Card with "Corsair VOID" in it's name, all cards ending in "Headset", then anything else

N.B. if the `*` is missed from the end of the card order, then the remaining cards will not be displayed.


There is also the `-c` and `-card` command-line options. These are exclusively used with the '-set' option to set the value of a device's volume level without displaying a user interface. They take either a full ALSA card name, or a card index ('0' for the first card).



WINDOW SETTINGS
===============

Different display types support windows that 'stick to desktop' or are transparent, or borderless. Some of these features are natively supported by the applications, while others are set using the 'wmctrl' command-line application, and will not work if that's not installed. These features are activated using 'PopupFlags' in the config file, or by individual options on the command-line. The available features are:

above
: keep window above all other windows, supported using `wmctrl` for all display types (except obviously terminal) 

sticky
: keep window above all other windows, supported using `wmctrl` for all display types (except obviously terminal) 

transparent
: give window a pseudo-transparent background. Supported for 'pterm' display type with terminals mlterm, aterm, and urxvt

tr
: short name for 'transparent'

borderless
: display window without the usual window-manager title bar and border. Supported for 'pterm' display type with terminals aterm, mlterm but not urxvt due to a bug in urxvt that makes borderless windows 'no input'

bl
: short name for 'borderless'



WINDOW STYLES
=============

The '-style' command-line option and 'WindowStyle' config setting select a display style for the 'term' and 'pterm' display types. Available styles are:


basic
: a 2-line display style where the control slider is displayed under the card name

2line
: another name for 'basic' style

1line
: a compact style where everything is displayed on 1 line, with the card name before the control slider

compact
: a compact style where everything is displayed on 1 line, with the card name overlaid on the control slider

pterm
: a style where the card name is not displayed, usually used for 'pterm' display types, where the card name is instead set as the window title



INSTALL
=======

The usual invocation:

```
./configure
make
make install
```

should work


