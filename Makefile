default:
	./zasm -wuy main.z80 --dotnames --reqcolon
	python3 -B ./tools/maked88.py app.d88 ipl.rom 0 0 1 
	python3 -B ./tools/maked88.py app.d88 main.rom 0 0 2 

ipl:
	./zasm -wuy ipl.z80 --dotnames --reqcolon

run:
	~/Downloads/quasi88-port/quasi88-sdl2 ./app.d88 -debug