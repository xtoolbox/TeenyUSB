# TeenyDT format descriptor
TEENYDT = drd_desc.lua
USBMEM ?= 512
USBEP  ?= 7

all: desc

desc: teeny_usb_init.h teeny_usb_desc.c

PWD = $(shell pwd)
teeny_usb_init.h teeny_usb_desc.c:$(TEENYDT)
	@echo generate USB descriptor
	cd $(PWD)/../../TeenyDT && \
	lua gen_descriptor.lua $(PWD)/$< -maxmem=$(USBMEM) -maxep=$(USBEP) && \
	cd $(PWD)

.PHONY: all
