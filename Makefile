obj-m := falcon.o
falcon-y := src/driver.o

PWD := $(shell pwd)
KERNELDIR ?= /usr/src/linux-headers-$(shell uname -r | grep -o '.*[0-9]')
MODULEDIR ?= /lib/modules/$(shell uname -r)/build
CSCOPEFILE := $(shell mktemp)

.PHONY: clean

all:
	$(MAKE) -C $(MODULEDIR) M=$(PWD) modules

tags: 
	if [ -f $(KERNELDIR)/cscope.files ] ; then tail -n +3 $(KERNELDIR)/cscope.files | sed "s|^|$(KERNELDIR)/|g" >> $(CSCOPEFILE) ; fi
	find . -name \*.[ch] >> $(CSCOPEFILE)
	cscope -b -q -k -i $(CSCOPEFILE)
	ctags -L $(CSCOPEFILE)
	rm $(CSCOPEFILE)

clean:
	$(MAKE) -C $(MODULEDIR) M=$(PWD) clean
	rm -f cscope.* tags
