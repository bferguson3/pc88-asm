# dehuff.py 

import sys 


class huffTree():
### Not needed as a class, just contains a list 
    def __init__(self):
        self.items = []

class node():
### if char != None, this is a leaf. otherwise its a branch.
    def __init__(self, left=None, right=None, char=None):
        self.left = left
        self.right = right
        self.char = char


curIndex = 0

def getNextBit(tb):
### requires global vars - shifts global bit field each time called
    global curIndex
    global bitcounter
    b = tb[curIndex] & bitcounter
    if(bitcounter == 1):
        bitcounter = (1 << 7)
        curIndex += 1
        if(curIndex == len(tb)):
            return -1 # EOF
    else:
        bitcounter = bitcounter >> 1
    if(b > 0):
        return 1
    else:
        return 0


def getBits(ln, tb):
### calls getNextBit n times and returns the shifted result 
    o=0
    i = 0
    while i < ln:
        o = o << 1
        o = o + getNextBit(tb)
        i += 1
    return o
    

def doNode(n, by):
### Recursive, populates node relations and assigns character data.
    b = getNextBit(by)
    if (b == 0): #node
        if(n.left == None):
            n.left = node()
            doNode(n.left, by)
        if(n.right == None): # no elif! We want to call this also each loop.
            n.right = node()
            doNode(n.right, by)
    if(b == 1):
        n.char = chr(getBits(dataLength, by))


dic = {}
def makedic(n, s):
### Make python format dictionary (not used)
    if(n.char):
        dic[int(s,2)] = n.char 
    if(n.left != None):
        makedic(n.left,s + "0")
    if(n.right != None):
        makedic(n.right,s + "1")


def decodeTree(r, d):
### Decompress the data stream.
    _b = getNextBit(d)
    last = r 
    while(_b != -1):
        if(last.char != None):
            print(last.char, end="")
            last = r 
        if(_b == 0):
            last = last.left 
        if(_b == 1):
            last = last.right 
        _b = getNextBit(d)


# load in tree and data bytes 
f = open(sys.argv[1], "rb")
tbytes = f.read()
f.close()
f = open(sys.argv[2], "rb")
dbytes = f.read()
f.close()

# new tree
tree = huffTree()

# first 6 bits are length of data field 
dataLength = tbytes[0] >> 2
# ( bit 7 is node 0 )
_lb = bitcounter = (1 << 0) # reset to 1 << 7
# start the tree with a blank
root = node()
# recurse the tree bytes and build the tree shcema
doNode(root, tbytes)

# make a dictionary (not needed)
#makedic(root, "")
# print the dict
#for k,v in dic.items():
#    print(k,v)

# reset counters
curIndex = 0
bitcounter = 0x80

# decode and print the input data stream 
decodeTree(root, dbytes)
print("")