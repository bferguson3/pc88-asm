# kanjiaddr.py

import sys

filemode = False

if(len(sys.argv) < 2):
    print("Usage: \t   python3 kanjiaddr.py <Japanese string>\n\t\
OR python3 kanjiaddr.py -f <filename>\n\
\n\tA string will output the JIS code and ROM address to the console.\
\n\t-f will convert the text to its 2-byte ROM address and save as 'output.bin'.")
    sys.exit()
else:
    for n in sys.argv:
        if n[0] == '-':
            if n == '-f':
                filemode = True
        else:
            inputstr = n

if (filemode):
    f = open(inputstr, "r")
    inb = f.read()
    f.close()
    inputstr = inb

# http://www.maroon.dti.ne.jp/youkan/pc88/kanjirom.html

def qtr_addr(a):
#"""0100 to 01ff"""
    return (((a & 0xf) << 2) | (1 << 11))

def halfwidth_addr(a):
#"""0020 to 00ff"""
    return ((a & 0xf) << 3)

def nonkanji_addr(a):
#"""2120h to 277fh"""
    return ( ((a & 0x1f) << 4) | ((a & 0b01100000) << 7) | ((a & 0x700) << 1 ) )

def kanji_std_addr(a):
#"""3020h to 4f5fh"""
    return ( ((a & 0x1f) << 4) | ((a & 0b01100000) << 9) | ((a & 0x1f00) << 1 ))

def kanji_std2_addr(a):
#""5020h to 6f7fh"""
    return ( ((a & 0b1100000)<<9) | ((a & 0x2000)) | ((a & 0xf00)<<1) | ((a & 0x1f) <<4) )

def kanji_std2_addr_ex(a):
#""7020h-705fh"""
    return ( ((a & 0x700)<<1) | ((a & 0b1100000)<<7) | ((a & 0x1f)<<4) ) 

def sjis_to_jis(a): # a => 2-byte array
    b = [0, 0]
    # high byte: set offset to 0x2f00 and increment based on half of high byte difference
    b[0] += int((a[0] - 0x88) * 2) + 0x2f
    # low byte 
    if ( (a[1] >= 0x40) and (a[1] <= 0x9e)):
        b[1] = a[1] - 0x1f # 9e > 7e
    elif ( (a[1] >= 0x9f) and (a[1] <= 0xfc) ): # (sjis skips pages a0 to df)
        b[0] += 1       # higher offset - second page in JIS
        b[1] = a[1] - 0x7e # 9f > 21
    return bytes(b)

f2 = None

if not filemode:
    print("sample conversion of:", inputstr)
else:
    f2 = open("output.bin", "wb")

for r in inputstr:
    print(r.encode("shift_jis"))
    b = sjis_to_jis(r.encode("shift_jis"))
    if not filemode:
        print(r,"jis code",hex(b[0])+hex(b[1])[2:])
        if( (b[0] < 1) ):
            print("addr in kanji rom:",hex(halfwidth_addr(b[1])))
        elif ( (b[0] > 0) and (b[0] < 2)):
            print("addr in kanji rom:",hex(qtr_addr(b[0] << 8 | b[1])))
        elif((b[0] > 0x20) and (b[0] < 0x28)): #non-kanji
            print("addr in kanji rom:",hex(nonkanji_addr(b[0] << 8 | b[1])))
        elif((b[0] > 0x29) and (b[0] < 0x50)): # standard kanji 1
            print("addr in kanji rom:",hex(kanji_std_addr(b[0] << 8 | b[1])))
        elif( (b[0] >= 0x50) and (b[0] <= 0x6f) ):
            print("addr in kanji rom:",hex(kanji_std2_addr(b[0] << 8 | b[1])))
        elif( (b[0] == 0x70) ):
            print("addr in kanji rom:",hex(kanji_std2_addr_ex(b[0] << 8 | b[1])))
        else:
            print(hex(b[0]),"out of range")
            continue
    else: # write to file
        if( (b[0] < 1) ):
            a = halfwidth_addr(b[1])
            f2.write(bytes([ a & 0xff ]))
            f2.write(bytes([ a >> 8 ]))
        elif ( (b[0] > 0) and (b[0] < 2)):
            a = qtr_addr(b[0] << 8 | b[1])
            f2.write(bytes([ a & 0xff ]))
            f2.write(bytes([ a >> 8 ]))
        elif((b[0] > 0x20) and (b[0] < 0x28)): #non-kanji
            a = nonkanji_addr(b[0] << 8 | b[1])
            f2.write(bytes([ a & 0xff ]))
            f2.write(bytes([ a >> 8 ]))
        elif((b[0] > 0x29) and (b[0] < 0x50)): # standard kanji 1
            a = kanji_std_addr(b[0] << 8 | b[1])
            f2.write(bytes([ a & 0xff ]))
            f2.write(bytes([ a >> 8 ]))
        elif( (b[0] >= 0x50) and (b[0] <= 0x6f) ):
            a = kanji_std2_addr(b[0] << 8 | b[1])
            f2.write(bytes([ a & 0xff ]))
            f2.write(bytes([ a >> 8 ]))
        elif( (b[0] == 0x70) ):
            a = kanji_std2_addr_ex(b[0] << 8 | b[1])
            f2.write(bytes([ a & 0xff ]))
            f2.write(bytes([ a >> 8 ]))
if(f2):
    f2.close()
