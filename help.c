#include "help.h"

void PrintVersion()
{
printf("alsavol version: %s\n", PACKAGE_VERSION);
exit(1);
}

void PrintHelp()
{
    printf("-d                    print a list of available devices and their controls\n");
    printf("-c                    specify target sound card\n");
    printf("-card                 specify target sound card\n");
    printf("-D                    debug mode, output some errors etc\n");
    printf("-debug                debug mode, output some errors etc\n");
    printf("-set <level>          set volume level\n");
    printf("-delta <value>        APPROXIMATE percentage to increase/decrease volume with up/down buttons\n");
    printf("-all                  apply volume change to all sound cards\n");
		printf("-ignore <list>        comma separated list of card names to NOT display\n");
		printf("-I <list>             comma separated list of card names to NOT display\n");
		printf("-order <list>         comma separated list of card names to display, in the specified order\n");
		printf("-O <list>             comma separated list of card names to display, in the specified order\n");
		printf("-I <list>             comma separated list of card names to NOT display\n");
    printf("-t <type>             display type, one of 'term', 'dialog', 'zenity', 'qarma', 'yad', 'wish', 'pterm'\n");
    printf("-T <apps>             force 'pterm' display type, 'apps' is a list of terminal apps to search for/use\n");
    printf("-term <apps>          force 'pterm' display type, 'apps' is a list of terminal apps to search for/use\n");
    printf("-s <name>             display style, one of 'basic', '1line', '2line', 'compact' and 'pterm'\n");
    printf("-style <name>         display style, one of 'basic', '1line', '2line', 'compact' and 'pterm'\n");
    printf("-x <pos>              x-position of display window\n");
    printf("-y <pos>              y-position of display window\n");
    printf("-w <value>            width in character cells for pterm windows\n");
    printf("-wide <value>         width in character cells for pterm windows\n");
    printf("-h <value>            height in character cells for pterm windows\n");
    printf("-high <value>         height in character cells for pterm windows\n");
    printf("-fg <color>           foreground text color\n");
    printf("-textcolor <color>    foreground text color\n");
    printf("-font <name>          font to use with pterm displays\n");
    printf("-fn <name>            font to use with pterm displays\n");
    printf("-class <name>         window class (used to configure window-related settings in window-manager)\n");
    printf("-tr                   transparent background: only for 'pterm' displays using urxvt, st, mlterm or aterm\n");
    printf("-bl                   borderless: only for 'pterm' displays using st, mlterm or aterm\n");
    printf("-above                set window to be above other windows\n");
    printf("-sticky               set window to display on all desktops\n");
    printf("-p                    persist. Respawn if window is closed, mostly for use with pterm display type\n");
    printf("-persist              persist. Respawn if window is closed, mostly for use with pterm display type\n");
    printf("-timeout <seconds>    timeout after which app shuts down if no user input. (only for terminal or pterm mode)\n");
    printf("-hotkey <key>         popup hotkey for use with the 'urxvt' display type\n");
    printf("-l <path>             path to lockfile\n");
    printf("-lock <path>          path to lockfile\n");
    printf("--version             print program version\n");
    printf("-?                    this help\n");
    printf("-help                 this help\n");
    printf("--help                this help\n");


    exit(1);
}




