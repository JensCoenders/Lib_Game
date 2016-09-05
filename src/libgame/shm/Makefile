.PHONY += shm_gen

SHM_SRCDIR := $(SRCDIR)

INC_HEADERS := shm_generated.h
LIBGAME_OBJS := shm_generated.o

# Shorthand command line target
shm_gen: $(SRCDIR)/shm_generated.h

# Rule for generating shm_generated.h and shm_generated.cpp
$(SRCDIR)/shm_generated.h $(SRCDIR)/shm_generated.cpp: $(SRCDIR)/shm.dat
	$(DIRECTORY_GUARD)
	$(TARGET_GUARD)
	+java -jar $(SCRIPTSDIR)/ShmGenerator.jar --input $< --output-prefix shm_generated \
		--output-dir $(dir $@)

clean::
	-if exist $(subst /,\\,$(SHM_SRCDIR)/shm_generated.h) (del /Q $(subst /,\\,$(SHM_SRCDIR)/shm_generated.h))
	-if exist $(subst /,\\,$(SHM_SRCDIR)/shm_generated.cpp) (del /Q $(subst /,\\,$(SHM_SRCDIR)/shm_generated.cpp))