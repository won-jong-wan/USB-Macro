APP := app
MOD := usb_macro

obj-m := $(MOD).o

# Build against the *running kernel* (fixes vermagic mismatch / invalid module format)
KDIR ?= /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

# User-space app compiler (native on target Ubuntu)
CC ?= gcc
CFLAGS_APP ?= -O2 -Wall -Wextra

.PHONY: all modules app clean

all: app modules

modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

app: $(APP).c
	$(CC) $(CFLAGS_APP) -o $(APP) $(APP).c

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f $(APP)
