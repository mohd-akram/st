INSTALL = install
MKDIR = mkdir -p
EMCC = emcc

prefix = /usr/local
bindir = $(prefix)/bin

# Windows \
EXEEXT=.exe # \
RM=del /f # \
INSTALL=copy # \
MKDIR=mkdir # \
prefix=. # \
!if [set CL=/DSDL_MAIN_HANDLED SDL2.lib] # \
!endif # \
!if [set _CL_=/link /subsystem:windows /entry:mainCRTStartup] # \
!endif

LDLIBS = -lm -lSDL2

all: st$(EXEEXT)

wasm: html/st.js

html/st.js: st.c
	$(EMCC) -s USE_SDL=2 -o $@ st.c

clean:
	$(RM) st$(EXEEXT) html/st.js html/*.wasm

install: st$(EXEEXT) $(DESTDIR)$(bindir)
	$(INSTALL) st$(EXEEXT) "$(DESTDIR)$(bindir)"

$(DESTDIR)$(bindir):
	$(MKDIR) "$(DESTDIR)$(bindir)"
