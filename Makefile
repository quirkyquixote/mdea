
include config.mk

objs += error.o
objs += escape.o
objs += token.o
objs += mdea.o

deps = $(objs:.o=.d)

libs = libmdea.so
install_libs = $(addprefix $(libdir)/,$(libs))

.PHONY: all
all: $(libs)

.PHONY: clean
clean:
	$(RM) $(objs)
	$(RM) $(libs)
	$(RM) $(deps)

.PHONY: install
install: all $(install_libs)

.PHONY: examples
examples: all
	@make -C examples all

.PHONY: clean-examples
clean-examples:
	@make -C examples clean

.PHONY: install-examples
install-examples: install
	@make -C examples install

.PHONY: test
test: all
	@make -C tests all

.PHONY: clean-test
clean-test:
	@make -C tests clean

.PHONY: realclean
realclean:
	git clean -fdx

libmdea.so: $(objs)

-include $(deps)
