
include config.mk

objs += error.o
objs += escape.o
objs += node.o
objs += null_node.o
objs += number_node.o
objs += string_node.o
objs += boolean_node.o
objs += array_node.o
objs += object_node.o
objs += parser.o
objs += file_parser.o
objs += string_parser.o
objs += file_emitter.o
objs += string_emitter.o
objs += mdea.o

deps = $(objs:.o=.d)

libs = libmdea.so libmdea.a
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
libmdea.a: $(objs)

-include $(deps)
