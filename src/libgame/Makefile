.PHONY += libgame

LIBGAME_OBJS := $(subst $(SRCDIR)/,,$(patsubst %.cpp,%.o,$(wildcard $(SRCDIR)/*.cpp)))

SDL_LIB_TARGETS := $(wildcard $(DEPDIR)/lib/*.lib)
SDL_LIB_TARGETS := $(subst $(DEPDIR)/lib,$(LIBDIR),$(SDL_LIB_TARGETS))

INC_HEADERS := $(subst $(SRCDIR)/,,$(wildcard $(SRCDIR)/*.h))
SUBDIRS := object shm

# Shorthand command line target
libgame: $(LIBGAME)
