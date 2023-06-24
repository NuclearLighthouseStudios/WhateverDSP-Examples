SUBDIRS := $(wildcard */.)

ifneq ($(MAKECMDGOALS),)
.PHONY: $(MAKECMDGOALS)
$(MAKECMDGOALS): $(SUBDIRS)
else
.PHONY: all
all: $(SUBDIRS)
endif

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)
