
include ../config.mk

objs += error.o
objs += escape.o
objs += node.o
objs += null_node.o
objs += number_node.o
objs += string_node.o
objs += boolean_node.o
objs += array_node.o
objs += object_node.o
objs += token.o
objs += json_parser.o
objs += node_parser.o
objs += emitter.o
objs += json_emitter.o
objs += node_emitter.o
objs += string_input.o
objs += file_input.o
objs += string_output.o
objs += file_output.o
objs += mdea.o

deps = $(objs:.o=.d)

libs = libmdea.so libmdea.a
install_libs = $(addprefix $(libdir)/,$(libs))

bins = mdea-json
install_bins = $(addprefix $(bindir)/,$(bins))

.PHONY: all
all: $(libs) $(bins)

.PHONY: clean
clean:
	$(RM) $(objs)
	$(RM) $(libs)
	$(RM) $(deps)
	$(RM) $(bins)

.PHONY: install
install: all $(install_libs)

libmdea.so: $(objs)
libmdea.a: $(objs)
mdea-json: mdea-json.o libmdea.a

-include $(deps)
