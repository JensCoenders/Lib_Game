.PHONY += jensgame

JENSGAME_OBJS := $(patsubst %.cpp,%.o,$(wildcard $(SRCDIR)/*.cpp))
JENSGAME_OBJS := $(subst $(SRCDIR),$(OUTDIR),$(JENSGAME_OBJS)) $(OUTDIR)/resources.o

SDL_DLL_TARGETS := $(wildcard $(RESDIR)/dll/*.dll)
SDL_DLL_TARGETS := $(subst $(RESDIR)/dll,$(CONF_BINDIR),$(SDL_DLL_TARGETS))

JENSGAME_VERSION_FILE := $(SRCDIR)/testgame.h

SUBDIRS := libgame

# Shorthand command line targets
jensgame: $(JENSGAME)

# Rule for generating game executable
$(JENSGAME): $(JENSGAME_OBJS) $(LIBGAME) $(SDL_DLL_TARGETS)
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	+g++ -o $@ $(filter %.o,$^) $(addprefix -L,$(DEFAULT_LIB_DIRS)) -lgame $(SDL_LIBS)

$(JENSGAME_OBJS): | inc

# Rule for building resource file
$(OUTDIR)/resources.o: $(RESDIR)/resources.rc
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	+windres $(subst /,\\,$<) -o $(subst /,\\,$@)

# Rule for updating version in resource file
$(RESDIR)/resources.rc: $(JENSGAME_VERSION_FILE)
#	+powershell.exe -File "scripts\update_version.ps1" -ResourceFile $@ -VersionFile $<

# Rule for copying DLL files to bin directory
$(CONF_BINDIR)/%.dll: $(RESDIR)/dll/%.dll
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	+mklink /H $(subst /,\\,$@) $(subst /,\\,$<) >NUL
	