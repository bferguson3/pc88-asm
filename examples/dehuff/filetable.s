; file list 

.include "../../pc88.z80"

    .org 0
             ;//123456789
FileTableEntry "treetest ", 2, 1, 41, 0x1000
FileTableEntry "bintest  ", 2, 2, 572, 0x1fff-572
