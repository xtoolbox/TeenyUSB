help:
	@echo 'Available targets are:'
	@echo ' all        build all available demos'
	@echo ' bulk       custom bulk demo'
	@echo ' comp       composite demo, HID + CDC + WinUSB_Bulk + MSC'
	@echo ' rndis      rndis device demo'
	@echo ' clean      clear all generate data'

.PHONY: clean clear all bulk comp

all:
	cd custom_bulk && '$(MAKE)'
	cd composite && '$(MAKE)'
	cd rndis && '$(MAKE)'

bulk:
	cd custom_bulk && '$(MAKE)'

comp:
	cd composite && '$(MAKE)'

rndis:
	cd rndis && '$(MAKE)'

clear: clean
clean:
	cd custom_bulk && '$(MAKE)' clear
	cd composite && '$(MAKE)' clear
	cd rndis && '$(MAKE)' clear
