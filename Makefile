OD=/home/flo/workspace-mcu/openocd
OCD=$(OD)/bin/openocd

.phony: all
all:
	$(MAKE) -C Debug all

.phony: nvi
nvi: compile_commands.json


install: all
	$(OCD) -d2 -s $(OD)/share/openocd/scripts -f board/st_nucleo_f0.cfg -c "program {Debug/cube-test.elf}  verify reset; shutdown;"

#requires pip install compiledb
#/workspace-mcu/arm/gcc-arm-none-eabi-9-2020-q2-update/arm-none-eabi/include
.phony: compile_commands.json
compile_commands.json:
	$(MAKE) -C Debug clean
	cd Debug
	compiledb $(MAKE) all
	sed '/.*-I..\/Core\/Inc.*/i \ \ \ "-I/home/flo/workspace-mcu/arm/gcc-arm-none-eabi-9-2020-q2-update/arm-none-eabi/include",' $@ > ../$@

.phony: clean
clean:
	$(MAKE) -C Debug clean
	rm -f compile_commands.json
	
