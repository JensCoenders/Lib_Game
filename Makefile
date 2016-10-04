include definitions.mk

# include source makefile
$(call subdir_include,src)

# include all dependency files
include $(wildcard $(ROOT_OUTDIR)/**/*.d)

all: jensgame | inc
inc: $(G_INC_HEADERS)

# rule for building libgame.dll and libgame.a
$(LIBGAME): $(G_LIBGAME_OBJS)
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	-@if not exist $(subst /,\\,$(CONFIG_BINDIR)) (mkdir $(subst /,\\,$(CONFIG_BINDIR)))
	-@if exist $(subst /,\\,$(CONFIG_BINDIR)/libgame.dll) (del $(subst /,\\,$(CONFIG_BINDIR)/libgame.dll))
	
	+g++ -o $(CONFIG_BINDIR)/libgame.dll $^ -shared -Wl,--out-implib,$@ $(addprefix -L,$(LIB_DIRS)) $(SDL_FLAGS)

$(G_LIBGAME_OBJS): | inc

# rule for generating .o files from .cpp files
$(CONFIG_OUTDIR)/%.o: $(ROOT_SRCDIR)/%.cpp
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	+g++ -c $< -o $@ $(addprefix -I,$(INC_DIRS)) $(CPPFLAGS)

clean::
	-if exist $(subst /,\\,$(BINDIR)) (rmdir /Q /S $(subst /,\\,$(BINDIR)))
	-if exist $(subst /,\\,$(INCDIR)) (rmdir /Q /S $(subst /,\\,$(INCDIR)))
	-if exist $(subst /,\\,$(LIBDIR)) (rmdir /Q /S $(subst /,\\,$(LIBDIR)))
	-if exist $(subst /,\\,$(ROOT_OUTDIR)) (rmdir /Q /S $(subst /,\\,$(ROOT_OUTDIR)))
