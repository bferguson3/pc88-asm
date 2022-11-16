# pc88-asm
<br>
This is a continuation of [PC88-C](https://github.com/bferguson3/pc88-c), meant to be a more universal, and faster, method of developing games in pure z80 assembly for the PC8801+ series of Japanese computers. 
<b>
<br>PC88-C has been relegated to ON HIATUS thanks to the nonstop changes and bugs introduced into SDCC post version 4.0.</b><br>
<br>
To use:<br>
** Requires zasm (https://github.com/Megatokio/zasm) to be on the path or root folder.<br>
To build hello world:<br>

```
make PROJECT=examples/hw
```

PROJECT should always point towards the folder of your current project, which has a main.z80 file. <br>
To add files to a recently built project without invoking Python to use maked88.py, you can instead use:<br>
```
make add ARGS="examples/tilemap/map1.bin 0 4 1"
```
This is identical to: "python3 ./tools/maked88.py app.d88 ./examples/tilemap/map1.bin 0 4 1"
<br>
<br>
References:<br>
https://github.com/RetroDevDaemon/pc88-c/blob/main/src/pc88-c.h<br>
http://mydocuments.g2.xrea.com/<br>
http://www.maroon.dti.ne.jp/youkan/pc88/index.html<br>
