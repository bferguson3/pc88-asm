; file list 

.include "../../pc88.z80"

    .org 0
             ;//123456789
FileTableEntry "treetest ", 2, 1, 41, 0x1000
FileTableEntry "bintest  ", 2, 2, 572, 0x1fff-572

; Made for loading onto disks. 
; File names are 9 characters, null-term
; Track and sector are the numbers sent to the disk SS to load the file 
; Size is in bytes, (size >> 8) + 1 is number of sectors on disk 
; Addr is (recommended) load address. This can be ignored.