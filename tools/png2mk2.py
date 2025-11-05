#!/usr/bin/python3
# png2mk2.py

import sys, os
from PIL import Image

# load in image pixels
def LoadImg(inf):
	f = Image.open(inf)
	px = f.load()
	if (type(px[0,0]) == tuple):
	    print('not indexed image. quitting...')
	    f.close()
	    sys.exit()
	# save img size
	imsize = f.size
	f.close()
	return imsize,px

# init output
def InitImgOutput(inf, imsize, px):
	bn = os.path.basename(inf)
	bn = bn.split('.')[0]
	ob = [] # out byte array
	iy = 0
	while iy < imsize[1]:
		ix = 0
		while ix < imsize[0]:
			pind = px[ix,iy]
			if(pind > 7):
				print('more than 8 colors. quitting...')
				sys.exit()
			ob.append(px[ix,iy]<<4|px[ix+1,iy])
			ix += 2
		iy += 1
	return bn, ob

imgsize, pix = LoadImg(sys.argv[1])
basen, uncompr = InitImgOutput(sys.argv[1], imgsize, pix)	
# basen = base name without extension 
# uncompr = raw 4bpp bytes 
f = open(basen + ".bin", "wb")
f.write(bytes(uncompr))
f.close()

print(basen + ".bin written successfully.")