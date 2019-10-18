help:
	@echo 'Available targets are:'
	@echo ' all        build all available demos'
	@echo ' f723dap    HS/FS cmsis-dap based on STM32F723 discovery kit'
	@echo ' bulk       custom bulk demo'
	@echo ' comp       composite demp, HID + CDC + WinUSB_Bulk + MSC'
	@echo ' clean      clear all generate data'

.PHONY: f723dap clean clear all bulk comp

all:
	cd f723dap && '$(MAKE)'
	cd custom_bulk && '$(MAKE)'
	cd composite && '$(MAKE)'

f723dap:
	cd f723dap && '$(MAKE)'

bulk:
	cd custom_bulk && '$(MAKE)'

comp:
	cd composite && '$(MAKE)'

clear: clean
clean:
	cd f723dap && '$(MAKE)' clear
	cd custom_bulk && '$(MAKE)' clear
	cd composite && '$(MAKE)' clear
