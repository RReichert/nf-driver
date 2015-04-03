obj-m    := falcon.o
falcon-y := src/driver.o

ccflags-y := -DDEBUG
#ccflags-y := -I$(src)/include

PWD       := $(shell pwd)
MODULEDIR := /lib/modules/$(shell uname -r)/build

.PHONE: build clean

default:
	@echo 'please select a task'

all:
	$(MAKE) -C $(MODULEDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(MODULEDIR) M=$(PWD) clean
