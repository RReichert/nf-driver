obj-m := falcon.o
falcon-y := driver.o

PWD := $(shell pwd)
MODULEDIR ?= /lib/modules/$(shell uname -r)/build

.PHONY: tags clean

default:
	@echo 'please select a task'

build:
	$(MAKE) -C $(MODULEDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(MODULEDIR) M=$(PWD) clean
