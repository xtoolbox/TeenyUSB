help:
	@echo 'Available targets are:'
	@echo ' all        build all available demos'
	@echo ' f723dap    HS/FS cmsis-dap based on STM32F723 discovery kit'
	@echo ' bulk       custom bulk demo'
	@echo ' clean      clear all generate data'

.PHONY: f723dap clean clear all bulk

all:
	cd f723dap && '$(MAKE)'
	cd custom_bulk && '$(MAKE)'

f723dap:
	cd f723dap && '$(MAKE)'
	cd ..
bulk:
	cd custom_bulk && '$(MAKE)'
	cd ..

clear: clean
clean:
	cd f723dap && '$(MAKE)' clear
	cd custom_bulk && '$(MAKE)' clear
	cd ..