OD=/home/flo/workspace-mcu/openocd
OCD=$(OD)/bin/openocd

.phony: all
all:
	$(MAKE) -C Debug all

install: all
	$(OCD) -d2 -s $(OD)/share/openocd/scripts -f board/st_nucleo_f0.cfg -c "program {Debug/cube-test.elf}  verify reset; shutdown;"

#requires pip install compiledb
.phony: compile_commands.json
compile_commands.json:
	$(MAKE) -C Debug clean
	compiledb $(MAKE) -C Debug all

.phony: clean
clean:
	$(MAKE) -C Debug clean
	
