
include ../config.mk

objs += escape.o
objs += array.o
objs += object.o
objs += parser.o

bins = $(objs:.o=)
deps = $(objs:.o=.d)
logs = $(objs:.o=.log)

.PHONY: all
all: $(logs)
	@for i in $(logs); do echo TEST $$i; diff -q --ignore-blank-lines --ignore-space-change $$i $$i.gold; done

.PHONY: clean
clean:
	$(RM) $(bins)
	$(RM) $(objs)
	$(RM) $(deps)
	$(RM) $(logs)

escape: escape.o ../escape.o ../error.o
array: array.o
object: object.o
parser: parser.o ../mdea.o ../error.o ../token.o ../node.o

%.log: % %.in make-always
	$(QUIET_GEN)./$< < $<.in > $@ 2>&1

.PHONY: make-always
make-always:

