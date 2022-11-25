# parsevgm.py 

import sys 

f = open(sys.argv[1], "rb")
inby = f.read()
f.close()

outby = []
i = 0 
# skip header if exists 
# header version      # header size 
    # 1.0              48(36) bytes 
    # 1.01             48(40) bytes 
    # 1.10             48(43) bytes 
    # 1.50             64(56) bytes
    # 1.51             128 b
    # 1.60             128 b
    # 1.61             192(184) b
    # 1.70             192 b
    # 1.71             256 bytes
vgmheaders = {
    "0x00": 48,
    "0x01": 48,
    "0x10": 48,
    "0x50": 64,
    "0x51": 128,
    "0x60": 128,
    "0x61": 192,
    "0x70": 192,
    "0x71": 256
}

if(inby[0] == ord('V')):
    if(inby[1] == ord('g')):
        if(inby[2] == ord('m')):
            if(inby[9] != 1):
                print("VGM version less than 1.0. Quitting...")
                sys.exit()
            print("VGM header found. Version 1.", end="")
            print(hex(inby[8])[2:])
            try:
                i = vgmheaders[hex(inby[8])]
            except:
                print("VGM format unrecognized. Quitting...")
                sys.exit()
            print("Skipping",i,"bytes.")

while i < len(inby):
    if(inby[i] == 0x62)or(inby[i] == 0x63): # wait frame ntsc/pal
        p = inby[i]
        while( (inby[i] == inby[i+4]) 
          and inby[i+1] == inby[i+4+1]
          and inby[i+2] == inby[i+4+2]
          and inby[i+3] == inby[i+4+3]):
            outby.append(p)
            i += 4
    outby.append(inby[i])
    i += 1

print(len(outby),"vs",len(inby))

f = open("parsed.vgm", "wb")
i = 0
while i < len(outby):
    f.write(bytes([outby[i]]))
    #print(chr(outby[i]))
    i += 1
f.close()