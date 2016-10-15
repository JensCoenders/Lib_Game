.PHONY += jensgame

JENSGAME_OBJS := main.o testgame.o
SUBDIRS := libgame

# game components
JENSGAME_VERSION_FILE := $(SRCDIR)/testgame.h

# shorthand command line targets
jensgame: $(JENSGAME)

# rule for building the resource file
$(OUTDIR)/resources.o: $(RESDIR)/resources.rc
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	+windres $(subst /,\\,$<) -o $(subst /,\\,$@)

$(JENSGAME): $(OUTDIR)/resources.o

# rule for updating version in resource file
# $(RESDIR)/resources.rc: $(JENSGAME_VERSION_FILE)
#	+powershell.exe -File "scripts\update_version.ps1" -ResourceFile $@ -VersionFile $<
