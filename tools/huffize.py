# huffize.py 

#inputstr = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Lorem ipsum dolor sit amet consectetur. Nisl rhoncus mattis rhoncus urna neque. Sit amet porttitor eget dolor. Vitae nunc sed velit dignissim sodales ut eu sem. Volutpat blandit aliquam etiam erat. Id diam vel quam elementum pulvinar etiam non quam lacus. Faucibus et molestie ac feugiat sed lectus vestibulum. Diam quis enim lobortis scelerisque fermentum. Lacus suspendisse faucibus interdum posuere lorem ipsum dolor sit. Elementum pulvinar etiam non quam lacus suspendisse. Et magnis dis parturient montes nascetur ridiculus mus mauris vitae. Est ullamcorper eget nulla facilisi etiam dignissim diam quis enim. Hac habitasse platea dictumst vestibulum rhoncus est pellentesque. Dictum fusce ut placerat orci nulla. Bibendum neque egestas congue quisque egestas. Eget nulla facilisi etiam dignissim diam quis. Tristique nulla aliquet enim tortor at. Massa tincidunt nunc pulvinar sapien et ligula. Sagittis id consectetur purus ut faucibus pulvinar elementum."
inputstr = "AAAAABBBCCCCDDDDDDDDDz"
# huffman tree class 
class freqTree():
    def __init__(self):
        self.items = []
        self.highestFreq = 0
        
    def addItem(self, i):
    ### Increases frequency instead if already exists
        n = 0
        found = False
        while n < len(self.items):
            if (self.items[n].char == i.char):
                found = True
                self.items[n].freq += 1
                break
            n += 1
        if (found == False):
            t = treeItem(i.char)
            self.items.append(t)

    def getHighestF(self):
    ### Finds and assigns highest freq item
        i = 0 
        hifr = 0
        while i < len(self.items):
            if (self.items[i].freq > hifr):
                hifr = self.items[i].freq 
            i +=1
        self.highestFreq = hifr 
        return self.highestFreq 

    def sort(self): # Memhog Creates a new array. 
    ### descending by frequency
        sorts = []
        f = self.getHighestF()
        while f > 0:
            i = 0
            while i < len(self.items):
                if (self.items[i].freq == f):
                    sorts.append(self.items[i])
                i += 1
            f -= 1
        self.items = sorts 
    
    def poplast(self):
        return self.items.pop()
        

class _node():
    ### if char is null, its a tree. otherwise, its a leaf. 
    def __init__(self, char = '', freq = 0, left = None, right = None, ix = 0):
        self.char = char 
        self.freq = freq 
        self.left = left 
        self.right = right 
        self.index = ix

def treeItem(char):
    return _node(char=char, freq=1)
def fork(left, right):
    return _node(char=None, freq=left.freq+right.freq, left=left, right=right)
def node(char, freq):
    return _node(char=char, freq=freq)

# DANGER! RECURSION!!
def traverse(node, bitstr, dic):
    ### Populates the dictionary
    if(node.char != None): # am I a leaf?
        dic[node.char] = bitstr
        return 
    if (node.left != None):
        traverse(node.left, bitstr+"0", dic)
    if(node.right != None):
        traverse(node.right, bitstr+"1", dic)

def trav_output(node):
    ### Writes to global output binary string  
    global output
    global maxsz 
    if(node.char != None):
        output = output + "1" # leaf 
        s = "{0:08b}".format(ord(node.char))
        output = output + s[-maxsz:]
        print(s[-maxsz:])
        return
    else:
        output = output + "0" # node
        trav_output(node.left)
        trav_output(node.right)
        

def str2bin(t):
    ### converts binary string to bytes object
    out = []
    i = 0
    while i < len(t):
        n = 0
        b = ''
        while n < 8:
            b = b + t[i] 
            n += 1
            i += 1
        out.append(bytes([int(b,2)]))
    return out

def encode(instr, dict):
    ### converts the input string with the given dictionary
    i = 0 
    out = ''
    while i < len(instr):
        out = out + dict[instr[i]]
        #print(huffdict[instr[i]])
        i += 1
    return out


def sortByVal(t, dic):
#"""This is useless. """
    out = {}
    lowest = 0
    i = 0
    while i < 128:
        for c in dic:
            if(int(dic[c],2) == i):
                out[c] = dic[c]
                print(c, out[c])
        i += 1
    return out 
##
## script
##

tree = freqTree()       # make new tree queue

i = 0 
while i < len(inputstr):    # parse in data to compress 
    a = treeItem(inputstr[i])
    n = node(a.char, 1)     # make it a new node object
    tree.addItem(n)         # and add it (or increase freq) to the tree
    i += 1

tree.sort()    # sort from highest frequency to lowest so we can pop

final = []      # make final frequency tree 
fi = 0          # final index counter


# recurse while there are nodes to pair 
while len(tree.items) > 1:
    n = tree.items.pop()  # get lowest two frequencies
    n2 = tree.items.pop()
    new = fork(n, n2)       # make a new parent node
    new.index = fi          # assign it a new index
    fi += 1 
    final.append(new)       # and include it in the final list
    tree.items.append(new)  # add it back to the queue
    tree.sort()

# assign each node a binary string 
huffdict = {}
bitstr = ""

#for c in huffdict:
#    print(c, huffdict[c])

newdic = sortByVal(final, huffdict)
# build the huffdict object from the local tree struct 
traverse(final[len(final)-1], "", newdic)

## Write the binary tree data:
output = ""
# FROM THE LEFT!
# first 6 bits = length in bits of each VAL, 1-127. 7 = 128 values, 8 = 256 values...
## WAIT - first lets get the actual size of the data.
maxsz = 0
for c in newdic: 
    if (ord(c) > maxsz):
        maxsz = ord(c)
#output += "{0:06b}".format(maxsz) 
g = 1 
while (g << 1) < maxsz:
    g = g << 1
print("Data length in bits:", len("{:b}".format(g)))
maxsz = len("{:b}".format(g))
output += "{0:06b}".format(maxsz)
# 7th bit is always 0 (root is a node)
#dont output it. 
#output += "0"

# build the binary encoded huffman tree
trav_output(final[len(final)-1])

# buffer end of string to 8 bits 
while(len(output) % 8 != 0):
    output = output + "0"
# convert to bytes 
outby = str2bin(output)
print("tree is",len(output),"bits.")
print(output)
# write to file 
f = open("tree.bin", "wb")
for a in outby:
    f.write(a)
f.close()

## finally, encode the data itself

# encode the string for output
encodedstr = encode(inputstr, newdic)
# buffer it to 8 bits 
while(len(encodedstr) % 8 != 0):
    encodedstr = encodedstr + "0"
print(encodedstr)
# convert to bytes 
encodedbytes = str2bin(encodedstr)
# write bytes to file 
f = open("data.bin", "wb")
for a in encodedbytes:
    f.write(a)
f.close()

#
print("length tree {} + length encoded str {} = {}".format( len(outby), len(encodedbytes), len(outby)+len(encodedbytes)), "bytes")
print("length of uncompressed string:",len(inputstr))


# 000111.0.11000100.0.11000001.0.0.11111010.11000010.11000011000000
# 0 - D 
# 1 0 - A 
# 1 1 1 - Z 
# 1 1 0 0 - B 
# 1 1 0 1 - C 
"""
111.111.111 
if 1 : 
    go up 1 bytes (2 nodes)
if 0 :
    go up 0 bytes (1 node)

start at 0x250
0x250 + 1 = 2
0x250 + (2*2) = 0x254
0x254 + 1 = 4
0x250 + (4*2) = 0x258
0x258 + 1 = 6
0x250 + (12) = 0x25c
0x25c = z 
test:
0: 1 2 
1: D n 
2: 3 4 
3: A n 
4: 5 6 
5: 6 7
6: C n 
7: B n
8: z n

            0 
    1D               2
            3A               4
                        5          6z 
                    C7       B8
                 
               

1 2 
D n 
3 4 
A n 
5 6 
C n 
B n 
z n 

"""

1 2 
D n 
