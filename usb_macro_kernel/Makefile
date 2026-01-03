APP := app
MOD := usb_macro

obj-m := $(MOD).o

ARCH ?= arm
CROSS_COMPILE ?= arm-linux-gnueabihf-
CC := $(CROSS_COMPILE)gcc

KDIR := /home/ubuntu/pi_bsp/kernel/linux
PWD  := $(shell pwd)

.PHONY: all modules app clean

all: app modules

modules:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

app: $(APP).c
	$(CC) -O2 -Wall -o $(APP) $(APP).c

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f $(APP)
