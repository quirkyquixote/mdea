
include ../config.mk

ALL_CFLAGS += -I../src

objs += escape.o
objs += array.o
objs += object.o
objs += file.o
objs += string.o
objs += file_to_node_to_file.o

bins = $(objs:.o=)
deps = $(objs:.o=.d)
logs = $(objs:.o=.log)

.PHONY: all
all: $(logs)
	@for i in $(bins); do echo TEST $$i; diff -q --ignore-blank-lines --ignore-space-change $$i.log $$i.gold; done

.PHONY: clean
clean:
	$(RM) $(bins)
	$(RM) $(objs)
	$(RM) $(deps)
	$(RM) $(logs)

%.log: % %.in make-always
	$(QUIET_GEN)./$< < $<.in > $@ 2>&1

escape: escape.o ../src/libmdea.a
array: array.o ../src/libmdea.a
object: object.o ../src/libmdea.a
file: file.o ../src/libmdea.a
string: string.o ../src/libmdea.a
file_to_node_to_file: file_to_node_to_file.o ../src/libmdea.a

.PHONY: make-always
make-always:

