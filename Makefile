# Windows \
EXEEXT=.exe # \
RM=del /f # \
!if [set CL=/DSDL_MAIN_HANDLED SDL2.lib] # \
!endif # \
!if [set _CL_=/link /subsystem:windows /entry:mainCRTStartup] # \
!endif

LDLIBS = -lm -lSDL2
all: st$(EXEEXT)
clean:
	$(RM) st$(EXEEXT)
