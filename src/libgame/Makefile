.PHONY += libgame

INC_HEADERS := $(subst $(SRCDIR)/,,$(wildcard $(SRCDIR)/*.h))
SUBDIRS := object shm

# target and objects definition
TARGET := $(LIBGAME)
LIBGAME_OBJS := $(subst $(SRCDIR)/,,$(patsubst %.cpp,%.o,$(wildcard $(SRCDIR)/*.cpp)))

libgame: $(LIBGAME)
