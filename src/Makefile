.PHONY += jensgame

SUBDIRS := libgame

# target and objects definition
TARGET := $(JENSGAME)
JENSGAME_OBJS := $(patsubst %.cpp,%.o,$(wildcard $(SRCDIR)/*.cpp)) $(OUTDIR)/resources.o
SDL_DLLS := $(wildcard $(RESDIR)/dll/*.dll)

# variable conversion
JENSGAME_OBJS := $(subst $(SRCDIR),$(OUTDIR),$(JENSGAME_OBJS)) 
SDL_DLLS := $(subst $(RESDIR)/dll,$(CONFIG_BINDIR),$(SDL_DLLS))

JENSGAME_VERSION_FILE := $(SRCDIR)/testgame.h

ifeq ($(CONFIG),release)
RELEASE_LIBS := -static-libgcc -static-libstdc++
endif

# shorthand command line targets
jensgame: $(JENSGAME)

# rule for generating the game executable
$(JENSGAME): $(JENSGAME_OBJS) $(LIBGAME) $(SDL_DLLS)
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	+g++ -o $@ $(filter %.o,$^) $(addprefix -L,$(LIB_DIRS)) -L$(CONFIG_LIBDIR) -lgame $(SDL_FLAGS)

# rule for building the resource file
$(OUTDIR)/resources.o: $(RESDIR)/resources.rc
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	+windres $(subst /,\\,$<) -o $(subst /,\\,$@)

# rule for copying dependency dll files to bin directory
$(CONFIG_BINDIR)/%.dll: $(RESDIR)/dll/%.dll
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	+mklink /H $(subst /,\\,$@) $(subst /,\\,$<) >NUL

# rule for updating version in resource file
# $(RESDIR)/resources.rc: $(JENSGAME_VERSION_FILE)
#	+powershell.exe -File "scripts\update_version.ps1" -ResourceFile $@ -VersionFile $<

$(JENSGAME_OBJS): | inc
