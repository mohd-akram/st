INSTALL = install
MKDIR = mkdir -p

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
clean:
	$(RM) st$(EXEEXT)
install: st$(EXEEXT) $(DESTDIR)$(bindir)
	$(INSTALL) st$(EXEEXT) "$(DESTDIR)$(bindir)"

$(DESTDIR)$(bindir):
	$(MKDIR) "$(DESTDIR)$(bindir)"
