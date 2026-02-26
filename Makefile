
OBJ=config.o help.o sound-card.o ui.o terminal-ui.o zenity-ui.o wmctrl.o tclwish-ui.o popup-terminal.o
LIBS=-lasound 
FLAGS=-g -g -O2 -fstack-protector-strong -fstack-clash-protection -fno-strict-overflow -fno-strict-aliasing -fno-delete-null-pointer-checks -fcf-protection=full -DPACKAGE_NAME=\"alsavol\" -DPACKAGE_TARNAME=\"alsavol\" -DPACKAGE_VERSION=\"1.2\" -DPACKAGE_STRING=\"alsavol\ 1.2\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -D_FILE_OFFSET_BITS=64 -DHAVE_LIBASOUND=1 -DUSE_LIBUSEFUL_BUNDLED=1
prefix=/usr/local
exec_prefix=${prefix}
bindir=${exec_prefix}/bin
mandir=${prefix}/share/man

all: $(OBJ) libuseful-bundled/libUseful.a
	gcc $(FLAGS) -o alsavol $(OBJ) main.c $(LIBS) libuseful-bundled/libUseful.a


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


install:
	mkdir -p $(bindir)/
	cp -f alsavol $(bindir)/
	mkdir -p $(mandir)/man1
	cp alsavol.1 $(mandir)/man1



clean:
	rm -rf alsavol *.o *.orig .*.swp */*.o */*.so */*.a
