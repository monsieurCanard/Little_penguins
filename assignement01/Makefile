obj-m += hello_world.o

KDIR := /lib/modules/$(shell uname -r)/extra/
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean