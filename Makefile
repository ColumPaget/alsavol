
OBJ=config.o help.o sound-card.o ui.o terminal-ui.o zenity-ui.o wmctrl.o tclwish-ui.o popup-terminal.o
LIBS=-lUseful-5 -lasound 
FLAGS=-g -g -O2 -DPACKAGE_NAME=\"alsavol\" -DPACKAGE_TARNAME=\"alsavol\" -DPACKAGE_VERSION=\"1.1\" -DPACKAGE_STRING=\"alsavol\ 1.1\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -DHAVE_STDIO_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_STRINGS_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_UNISTD_H=1 -DSTDC_HEADERS=1 -D_FILE_OFFSET_BITS=64 -DHAVE_LIBASOUND=1 -DHAVE_LIBUSEFUL_5_LIBUSEFUL_H=1 -DHAVE_LIBUSEFUL_5=1

all: $(OBJ) 
	gcc $(FLAGS) -o alsavol $(OBJ) main.c $(LIBS) 


libuseful-bundled/libUseful.a:
	$(MAKE) -C libuseful-bundled

config.o:config.h config.c
	gcc $(FLAGS) -c config.c

help.o:help.h help.c
	gcc $(FLAGS) -c help.c

sound-card.o:sound-card.h sound-card.c
	gcc $(FLAGS) -c sound-card.c

ui.o:ui.h ui.c
	gcc $(FLAGS) -c ui.c

terminal-ui.o:terminal-ui.h terminal-ui.c
	gcc $(FLAGS) -c terminal-ui.c

zenity-ui.o:zenity-ui.h zenity-ui.c
	gcc $(FLAGS) -c zenity-ui.c

wmctrl.o:wmctrl.h wmctrl.c
	gcc $(FLAGS) -c wmctrl.c

tclwish-ui.o:tclwish-ui.h tclwish-ui.c
	gcc $(FLAGS) -c tclwish-ui.c



popup-terminal.o:popup-terminal.h popup-terminal.c
	gcc $(FLAGS) -c popup-terminal.c




clean:
	rm -rf alsavol *.o *.orig .*.swp */*.o */*.so */*.a
