obj-m += lab4.o

CFLAGS += -msoft-float -msse -msse2 -msse3 -msse4 -msse4

EXTRA_CFLAGS := -msoft-float -msse -msse2 -msse3 -msse4

PWD := $(CURDIR)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean