PROJECT = 0
ARGS = 0

default:
ifeq ($(PROJECT),0)
	$(info PROJECT var not set! Try e.g. 'make PROJECT=examples/hw')
	@$(exit)
else
	./zasm -wuy $(PROJECT)/main.z80 --dotnames --reqcolon
	python3 -B ./tools/maked88.py app.d88 ipl.rom 0 0 1 
	python3 -B ./tools/maked88.py app.d88 $(PROJECT)/main.rom 0 0 2 
#	python3 -B ./tools/maked88.py app.d88 $(PROJECT)/map1.bin 0 4 1 
endif 

add:
ifeq ($(ARGS),0)
	$(info ARGS var not set! Try e.g. 'make add ARGS="file/to/add C H R"')
	@$(exit)
else 
	python3 -B ./tools/maked88.py app.d88 $(FILE)
endif 

ipl:
	./zasm -wuy ipl.z80 --dotnames --reqcolon

run:
	~/Downloads/quasi88-port/quasi88.sdl2 ./app.d88 -debug