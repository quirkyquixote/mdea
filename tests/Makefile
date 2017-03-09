
include ../config.mk

objs += escape.o
objs += array.o
objs += object.o
objs += file.o
objs += string.o

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

%.log: % %.in make-always
	$(QUIET_GEN)./$< < $<.in > $@ 2>&1

escape: escape.o ../libmdea.a
array: array.o ../libmdea.a
object: object.o ../libmdea.a
file: file.o ../libmdea.a
string: string.o ../libmdea.a

.PHONY: make-always
make-always:

