help:
	@echo 'Available targets are:'
	@echo ' all        build all available demos'
	@echo ' f723dap    HS/FS cmsis-dap based on STM32F723 discovery kit'

.PHONY: f723dap clean all

all:
	cd f723dap && '$(MAKE)'

f723dap:
	cd f723dap && '$(MAKE)'
	cd ..
clean:
	cd f723dap && '$(MAKE)' clean
	cd ..