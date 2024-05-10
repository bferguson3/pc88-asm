
// maked88.cpp
// by @retrodevdiscord, CC0
// --std=c++03, -Werror

// NOTE: Add N88-type file has been untested in the C++ version of this code. Binary files are fine.


#include <iostream>
#include <fstream>
#include <vector>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>

//#include "d88.h"

namespace d88 { 

class d88sector{
public:
	int c;
	int h;
	int r;
	int n;
	int sectors;
	int density;
	int deleted;
	int fdc;
	int bytesize;
	std::vector<unsigned char> bytes;
	d88sector(){
		c = 0;
		h = 0;
		r = 0;
		n = 0;
		sectors = 0;
		density = 0;
		deleted = 0;
		fdc = 0;
		bytesize = 0;
	}
	std::string toString(){
		std::string o = "C H R: " + std::to_string(c) + " " + std::to_string(h) + " " + std::to_string(r) + "\n";
		o += "Size byte (128 << n): " + std::to_string(n) + "\t";
		o += "Sectors this track: " + std::to_string(sectors) + "\n";
		o += "Density: " + std::to_string(density) + "\t";
		o += "Delete flag: " + std::to_string(deleted) + "\n";
		o += "FDC status: " + std::to_string(deleted) + "\t";
		o += "Total size (b): " + std::to_string(bytesize) + "\n";
		return o;
	}
};

class d88track{
public:
	std::vector<d88sector> sectors;
	int sectorcount;
	d88track(){
		sectorcount = 0;
	}
	std::string toString(){
		std::string o = "";
		int i = 0;
		while(i < sectorcount){
			o += std::to_string(i+1) + ':' + std::to_string(sectors[i].r) + ' ';
			i += 1;
		}
		return o;
	}
};

class disk{
public:
	std::vector<unsigned char> bytes;
	std::string diskname;
	int pointer;
	int writeprotect;
	int mediatype;
	int disksize;
	std::vector<int> tracktable;
	std::vector<d88track> tracks;
	disk(char* fn = nullptr, int sz = 0x55eb0){
		if(fn != nullptr){
			std::ifstream f(fn, std::ios::binary);
			char byte;
			while(f.read(&byte, sizeof(byte))){
				bytes.push_back((unsigned char)byte);
			}
			f.close();
		}
		else{
			bytes.resize(0x2b0, 0);
			bytes.resize(sz, 0xff);
		}
		diskname = "";
		pointer = 0;
		writeprotect = 0;
		mediatype = 0;
		disksize = bytes.size();
		tracktable.resize(0);
		tracks.resize(0);
		if(fn != nullptr){
			std::ifstream f(fn, std::ios::binary);
			char byte;
			while(f.read(&byte, sizeof(byte))){
				bytes.push_back((unsigned char)byte);
			}
			f.close();
			GetDiskInfo();
			GetTrackInfo();
			PopulateSectors();
		}
	}
	void PopulateSectors(){
		int f = 0;
		while(f < tracktable.size()){
			int i = tracktable[f];
			d88track tr;
			tr.sectorcount = bytes[i+4] | (bytes[i+5] << 8);
			int sn = 0;
			while(sn < tr.sectorcount){
				d88sector s;
				s.c = bytes[i];
				i += 1;
				s.h = bytes[i];
				i += 1;
				s.r = bytes[i];
				i += 1;
				s.n = bytes[i];
				i += 1;
				s.sectors = bytes[i] | (bytes[i+1] << 8);
				i += 2;
				s.density = bytes[i];
				i += 1;
				s.deleted = bytes[i];
				i += 1;
				s.fdc = bytes[i];
				i += 6;
				s.bytesize = bytes[i] + (bytes[i+1] << 8);
				i += 2;
				int j = 0;
				while(j < s.bytesize){
					s.bytes.push_back(bytes[i]);
					i += 1;
					j += 1;
				}
				tr.sectors.push_back(s);
				sn += 1;
			}
			tracks.push_back(tr);
			f += 1;
		}
	}
	void GetDiskInfo(){
		int i = 0;
		while(i < 0x10){
			if(bytes[i] == 0){
				i = 0x10;
			}
			else{
				diskname += bytes[i];
			}
			i += 1;
		}
		writeprotect = bytes[0x1a];
		mediatype = bytes[0x1b];
		disksize = bytes[0x1c] + (bytes[0x1d] << 8) + (bytes[0x1e] << 16);
	}
	void GetTrackInfo(){
		int i = 0x20;
		int trofs = -1;
		while(trofs != 0){
			trofs = bytes[i] | (bytes[i+1] << 8) | (bytes[i+2] << 16);
			tracktable.push_back(trofs);
			i += 4;
		}
		tracktable.pop_back();
	}
	void UpdateMasterBytes(){
		std::vector<unsigned char> newbytes;
		int i = 0;
		while(i < diskname.size()){
			newbytes.push_back(diskname[i]);
			i += 1;
		}
		while(i < 0x1b){
			newbytes.push_back(0);
			i += 1;
		}
		newbytes.push_back(mediatype);
		newbytes.push_back(disksize & 0xff);
		newbytes.push_back((disksize & 0xff00) >> 8);
		newbytes.push_back((disksize & 0xff0000) >> 16);
		newbytes.push_back(0);
		i = 0;
		int ofs = 0x2b0;
		while(i < tracks.size()){
			ofs = tracktable[i];
			newbytes.push_back(ofs & 0xff);
			newbytes.push_back((ofs & 0xff00) >> 8);
			newbytes.push_back((ofs & 0xff0000) >> 16);
			newbytes.push_back(0);
			i += 1;
		}
		i = newbytes.size();
		while(i < 0x2b0){
			newbytes.push_back(0);
			i += 1;
		}
		int n = 0;
		while(n < tracks.size()){
			int j = 0;
			while(j < tracks[n].sectors.size()){
				newbytes.push_back(tracks[n].sectors[j].c);
				newbytes.push_back(tracks[n].sectors[j].h);
				newbytes.push_back(tracks[n].sectors[j].r);
				newbytes.push_back(tracks[n].sectors[j].n);
				newbytes.push_back(tracks[n].sectors[j].sectors & 0xff);
				newbytes.push_back((tracks[n].sectors[j].sectors & 0xff00) >> 8);
				newbytes.push_back(tracks[n].sectors[j].density);
				newbytes.push_back(tracks[n].sectors[j].deleted);
				newbytes.push_back(tracks[n].sectors[j].fdc);
				i = 0;
				while(i < 5){
					newbytes.push_back(0);
					i += 1;
				}
				newbytes.push_back(tracks[n].sectors[j].bytesize & 0xff);
				newbytes.push_back((tracks[n].sectors[j].bytesize & 0xff00) >> 8);
				int x = 0;
				while(x < tracks[n].sectors[j].bytes.size()){
					newbytes.push_back(tracks[n].sectors[j].bytes[x]);
					x += 1;
				}
				j += 1;
			}
			n += 1;
		}
		bytes = newbytes;
	}
	disk copy(int cop = -1){
		disk newdisk;
		if(cop == -1){
			cop = newdisk.disksize;
		}
		int i = 0;
		while(i < 16 && i < diskname.size()){
			newdisk.bytes[i] = diskname[i];
			i += 1;
		}
		newdisk.bytes[0x1a] = writeprotect;
		newdisk.bytes[0x1b] = mediatype;
		newdisk.bytes[0x1c] = disksize & (0xff);
		newdisk.bytes[0x1d] = (disksize & 0xff00) >> 8;
		newdisk.bytes[0x1e] = (disksize & 0xff0000) >> 16;
		i = 0x20;
		int tn = 0;
		while(tn < tracks.size()){
			newdisk.bytes[i] = tracktable[tn] & 0xff;
			newdisk.bytes[i+1] = (tracktable[tn] & 0xff00) >> 8;
			newdisk.bytes[i+2] = (tracktable[tn] & 0xff0000) >> 16;
			i += 4;
			tn += 1;
		}
		int tt = 0;
		i = 0x2b0;
		while(tt < tracks.size()){
			int ss = tracks[tt].sectorcount;
			int si = 0;
			while(si < ss){
				newdisk.bytes[i] = tracks[tt].sectors[si].c;
				i += 1;
				newdisk.bytes[i] = tracks[tt].sectors[si].h;
				i += 1;
				newdisk.bytes[i] = tracks[tt].sectors[si].r;
				i += 1;
				newdisk.bytes[i] = tracks[tt].sectors[si].n;
				i += 1;
				newdisk.bytes[i] = tracks[tt].sectors[si].sectors & 0xff;
				i += 1;
				newdisk.bytes[i] = (tracks[tt].sectors[si].sectors & 0xff00) >> 8;
				i += 1;
				newdisk.bytes[i] = tracks[tt].sectors[si].density;
				i += 1;
				newdisk.bytes[i] = tracks[tt].sectors[si].deleted;
				i += 1;
				newdisk.bytes[i] = tracks[tt].sectors[si].fdc;
				i += 1;
				i += 5;
				newdisk.bytes[i] = tracks[tt].sectors[si].bytesize & 0xff;
				i += 1;
				newdisk.bytes[i] = (tracks[tt].sectors[si].bytesize & 0xff00) >> 8;
				i += 1;
				int x = 0;
				while(x < tracks[tt].sectors[si].bytes.size()){
					newdisk.bytes[i] = tracks[tt].sectors[si].bytes[x];
					i += 1;
					x += 1;
				}
				si += 1;
			}
			tt += 1;
		}
		newdisk.GetDiskInfo();
		newdisk.GetTrackInfo();
		newdisk.PopulateSectors();
		return newdisk;
	}
	void Format(std::string name = ""){
		int i = 0;
		while(i < diskname.size()){
			bytes[i] = diskname[i];
			i += 1;
		}
		std::cout << "Disk size: "<< disksize << '\n';
		bytes[0x1a] = 0;
		bytes[0x1b] = 0;
		bytes[0x1c] = disksize & 0xff;
		bytes[0x1d] = (disksize & 0xff00) >> 8;
		bytes[0x1e] = (disksize & 0xff0000) >> 16;
		i = 0x20;
		int ofs = 0x2b0;
		int ct = 0;
		int gap = 4352;
		while(ct < 80){
			bytes[i] = ofs & 0xff;
			bytes[i+1] = (ofs & 0xff00) >> 8;
			bytes[i+2] = (ofs & 0xff0000) >> 16;
			ofs += (256+16)*16;
			ct += 1;
			i += 4;
		}
		i = 0x2b0;
		int c = 0;
		while(c < 0x28){
			int h = 0;
			while(h < 2){
				int r = 1;
				while(r <= 16){
					bytes[i] = c;
					bytes[i+1] = h;
					bytes[i+2] = r;
					bytes[i+3] = 1;
					bytes[i+4] = 0x10;
					bytes[i+5] = 0;
					bytes[i+6] = 0;
					bytes[i+7] = 0;
					bytes[i+8] = 0;
					bytes[i+9] = 0;
					bytes[i+10] = 0;
					bytes[i+11] = 0;
					bytes[i+12] = 0;
					bytes[i+13] = 0;
					bytes[i+14] = 0;
					bytes[i+15] = 1;
					r += 1;
					i += (256+16);
				}
				h += 1;
			}
			c += 1;
		}
	}
	int WriteBytes(std::string fn){
		std::ofstream f(fn, std::ios::binary);
		int i = 0;
		while(i < bytes.size()){
			f.write((char*)&bytes[i], sizeof(bytes[i]));
			i += 1;
		}
		f.close();
		std::cout << fn << " written successfully." << std::endl;
        return 0;
	}
	int AddDirEntry(std::string fn, int ofs, int sz, std::string filetype = "ASCII"){
		if(bytes[0x28480] == 0xff && bytes[0x28481] == 0xff){
			int i = 0x28480;
			while(i < 0x28480+256){
				bytes[i] = 0;
				i += 1;
			}
		}
		int totsec = sz / 2048;
		int BAM0 = 0x28590;
		int BAM1 = 0x286a0;
		int BAM2 = 0x287b0;
		int i = 0xa0;
		bytes[BAM0 + i] = 0xae;
		bytes[BAM1 + i] = 0xae;
		bytes[BAM2 + i] = 0xae;
		i += 1;
		bytes[BAM0 + i] = 0xf0;
		bytes[BAM1 + i] = 0xf0;
		bytes[BAM2 + i] = 0xf0;
		i += 1;
		bytes[BAM0 + i] = 0xeb;
		bytes[BAM1 + i] = 0xeb;
		bytes[BAM2 + i] = 0xeb;
		i += 1;
		while(i < 256){
			bytes[BAM0 + i] = 0;
			bytes[BAM1 + i] = 0;
			bytes[BAM2 + i] = 0;
			i += 1;
		}
		if(bytes[BAM0 + ofs] != 0xff){
			std::cout << "That sector is taken!" << std::endl;
			return -1;
		}
		while(totsec > 0){
			int g = 0;
			while(bytes[BAM0 + ofs + g] != 0xff || bytes[BAM0 + ofs + g] == 0xfe){
				g += 1;
				if(ofs + g > 159){
					std::cout << "won't fit!" << std::endl;
					return -1;
				}
			}
			bytes[BAM0 + ofs] = ofs + g;
			bytes[BAM1 + ofs] = ofs + g;
			bytes[BAM2 + ofs] = ofs + g;
			totsec -= 1;
		}
		int g = 0;
		while(bytes[BAM0 + ofs + g] != 0xff || bytes[BAM0 + ofs + g] == 0xfe){
			g += 1;
			if(ofs + g > 159){
				std::cout << "won't fit!" << std::endl;
				return -1;
			}
		}
		int cval = (sz % 2048) / 256 + 1;
		bytes[BAM0 + ofs + g] = cval | 0xc0;
		bytes[BAM1 + ofs + g] = cval | 0xc0;
		bytes[BAM2 + ofs + g] = cval | 0xc0;
		if(fn.find('.') != -1){
			std::vector<std::string> parts;
			std::string part;
			for(int i = 0; i < fn.size(); i++){
				if(fn[i] == '.'){
					parts.push_back(part);
					part = "";
				}
				else{
					part += fn[i];
				}
			}
			parts.push_back(part);
			std::string newfn = "";
			for(int i = 0; i < parts[0].size(); i++){
				newfn += parts[0][i];
			}
			if(newfn.size() < 6){
				while(newfn.size() < 6){
					newfn += ' ';
				}
			}
			if(newfn.size() > 6){
				newfn = newfn.substr(0, 6);
			}
			for(int i = 0; i < parts[1].size(); i++){
				newfn += parts[1][i];
			}
			if(newfn.size() > 9){
				newfn = newfn.substr(0, 9);
			}
			int i = 0x277c9;
			while(bytes[i] != 0xff){
				i += 16;
			}
			if(filetype == "BASIC"){
				bytes[i] = 0x80;
			}
			else if(filetype == "ASCII"){
				bytes[i] = 0;
			}
			else if(filetype == "BINARY"){
				bytes[i] = 1;
			}
			i &= 0xffff0;
			int f = 0;
			while(f < newfn.size()){
				bytes[i] = newfn[f];
				i += 1;
				f += 1;
			}
			i += 1;
			bytes[i] = ofs;
			return 0;
		}
		while(fn.size() < 9){
			fn += ' ';
		}
		std::string newfn = fn.substr(0, 9);
		i = 0x277c9;
		while(bytes[i] != 0xff){
			i += 16;
		}
		if(filetype == "BASIC"){
			bytes[i] = 0x80;
		}
		else if(filetype == "ASCII"){
			bytes[i] = 0;
		}
		else if(filetype == "BINARY"){
			bytes[i] = 1;
		}
		i &= 0xffff0;
		int f = 0;
		while(f < newfn.size()){
			bytes[i] = newfn[f];
			i += 1;
			f += 1;
		}
		i += 1;
		bytes[i] = ofs;

        return 0;
	}
	int AddFile(std::string fn, int c, int h, int r, bool respectBAM = false, bool ascii = false, int loadaddr = -1, int endaddr = 0){
		std::ifstream tow(fn, std::ios::binary);
		std::vector<unsigned char> outdat;
		char byte;
		if(!tow){
			printf("%s couldn't be opened! \n", fn.c_str());
			return -1;
		}
		while(tow.read(&byte, 1)){
			outdat.push_back((unsigned char)byte);
		}
		tow.close();
		int i = 0;
		int bc = 0;
		int starttrack = (c * 2) + h;
		int startsec = r;
		int start = tracktable[starttrack] + ((startsec - 1) * (256+16)) + 16;
		if(!respectBAM){
			while(i < outdat.size()){
				bytes[start + bc] = outdat[i];
				i += 1;
				bc += 1;
				if(i % 256 == 0){
					bc += 16;
				}
			}
		}
		else{
			if(loadaddr != -1){
				bytes[start + bc] = loadaddr & 0xff;
				bytes[start + bc+1] = loadaddr >> 8;
				bytes[start + bc+2] = endaddr & 0xff;
				bytes[start + bc+3] = endaddr >> 8;
				start += 4;
				while(i < outdat.size()){
					bytes[start + bc] = outdat[i];
					i += 1;
					bc += 1;
					if((i+4) % 256 == 0){
						bc += 16;
					}
					if(start + bc == 0x28480){
						bc += 0x440;
					}
				}
			}
			else{
				while(i < outdat.size()){
					bytes[start + bc] = outdat[i];
					i += 1;
					bc += 1;
					if(i % 256 == 0){
						bc += 16;
					}
					if(start + bc == 0x28480){
						bc += 0x440;
					}
				}
			}
		}
		if(ascii){
			bytes[start + bc] = 0x1a;
			bc += 1;
		}
        int oit = 0;
		if(loadaddr != -1){
			int oit = 4;
		}
		
		while((i+oit) % 256 != 0){
			bytes[start+bc] = 0;
			i += 1;
			bc += 1;
		}
		if(bytes[start+bc] == 0xff){
			bytes[start+bc] = 0xc9;
		}
		std::cout << "File added. Last used address on disk: 0x" << std::hex << start+bc-1 << std::endl;
		std::cout << "BUG WARNING: Only add files to empty sectors, or the disk file WILL get corrupted!" << std::endl;
        // TODO: ERROR CHECKING 
        return 0;
	}
};
}


bool CheckLineEndings(const std::string& f) {
    // Ensure line endings are 0D 0A
    std::ifstream file(f, std::ios::binary);
    std::vector<char> buffer(256);
    file.read(buffer.data(), 256);
    file.close();

	std::vector<char>::iterator it;
	for(it = buffer.begin(); it!= buffer.end(); it++){
    //for (char c : buffer) {
        if (*it == 0x0d) {
            return true;
        }
    }
    return false;
}

void asrt(int a) {
    if (a == -1) {
        std::cout << "Creation failed." << std::endl;
        exit(1);
    }
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "MakeD88.py\n"
                     "Usage: $ maked88 <diskname> [<filename> <C> <H> <R>]\n"
                     "                                or     [<filename> -b <O> [<type> <L>]]\n"
                     "                                or     [<title>]\n"
                     "Where:\n"
                     "    <diskname> : is the name of the disk file.\n"
                     "      (Without further arguments this generates a blank 2D PC-88 disk.)\n"
                     "    <filename> : File to add to target CHR location\n"
                     "    <C> : Target cylinder (0-39)\n"
                     "    <H> : Target head (0 or 1)\n"
                     "    <R> : Record no (1-16)\n"
                     "    <O> : Block number (N88 format)\n"
                     "    <type> : one of ASCII, BASIC, or BINARY. Default is ASCII.\n"
                     "        * BASIC compiler supported in the future, for now use binary/ascii.\n"
                     "    <L> : Start location in hex (only required for BINARY)\n"
                     "    <title> : Changes name of disk when this is the only argument.\n"
                     "\n"
                     "e.g.\n"
                     "     $ maked88 new.d88 ipl.bin 0 0 1\n"
                     "    Adds ipl.bin to the boot sector, and makes the disk if it does not exist.\n"
                     "  -OR-\n"
                     "     $ maked88 new.d88 app.bas -b 0x48\n"
                     "    Adds app.bas in ASCII mode to new.d88 at block 72 (=CHR 12,0,1),\n"
                     "    and adds a directory entry (so it can be loaded via 'files').\n"
                     "  -OR-\n"
                     "     $ maked88 new.d88 TestName\n"
                     "    A string and no other arguments makes a new disk named TestName.\n\n";
    } else if (argc > 2) {
        // UPDATE DISK NAME ONLY
        if (argc == 3) {
            std::cout << "Changing " << argv[1] << "'s disk name to:\n" << argv[2] << std::endl;
            if (!std::ifstream(argv[1]).good()) {
                std::cout << argv[1] << " not found. Creating." << std::endl;
                d88::disk new_disk = d88::disk(nullptr, 348848);
                new_disk.Format();
                new_disk.WriteBytes(argv[1]);
            }
            std::ifstream file(argv[1], std::ios::binary);
            std::vector<char> bytes((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();
            std::ofstream out_file(argv[1], std::ios::binary);
            size_t i = 0;
            while (i < std::min(std::string(argv[2]).length(), static_cast<size_t>(16))) {
                bytes[i] = argv[2][i];
                i++;
            }
            while (i < 16) {
                bytes[i] = 0x20;
                i++;
            }
            out_file.write(bytes.data(), bytes.size());
            out_file.close();
            return 0;
        }
        // ERROR
        if (argc < 5) {
            std::cout << "Error parsing arguments. Run `maked88` to see instructions." << std::endl;
        } else {
            // Create the disk object if it doesn't exist
            d88::disk new_disk;
            if (!std::ifstream(argv[1]).good()) {
                std::cout << argv[1] << " not found. Creating." << std::endl;
                new_disk = d88::disk(nullptr, 348848);
                new_disk.Format();
                new_disk.WriteBytes(argv[1]);
            } else {
                new_disk = d88::disk(argv[1]);
            }
            new_disk.GetTrackInfo();

            // Get the file size of the input file
            std::string fn = argv[2];
            std::ifstream file(fn, std::ios::binary | std::ios::ate);
            std::streamsize fsize = file.tellg();
            file.close();
            int stloc = -1;
            if (fsize > 348848) {
                std::cout << "Won't fit! File too big for disk" << std::endl;
                return 1;
            }

            // N88 DISK BASIC FILE FORMATS
            if (std::string(argv[3]) == "-b") {
                // PARSE FILE TYPE ARGUMENTS
                std::string ftype = "ASCII";
                if (argc >= 6) {
                    std::string arg5 = argv[5];
                    std::transform(arg5.begin(), arg5.end(), arg5.begin(), ::toupper);
                    if (arg5 == "BASIC") {
                        // TODO: Add compiler
                        ftype = "BASIC";
                        std::cout << "No BASIC compiler support yet... use binary." << std::endl;
                        return 1;
                    } else if (arg5 == "BINARY") {
                        // Make sure start address is included
                        if (argc != 7) {
                            std::cout << "Please designate a start address in hex format." << std::endl;
                            return 1;
                        }
                        // Parse it and create a binary stub
                        stloc = std::stoi(argv[6], nullptr, 16);
                        int enloc = stloc + fsize;
                        if (enloc > 0xffff) { // Error check
                            std::cout << std::hex << stloc << " " << enloc << std::endl;
                            std::cout << "Won't fit!" << std::endl;
                            return 1;
                        }
                        std::cout << "Appending: " << std::hex << stloc << " " << enloc << std::endl;
                        ftype = "BINARY";
                    } else if (arg5 == "ASCII") {
                        ftype = "ASCII";
                    }
                }
                std::cout << "Adding file " << fn << " in N88 mode to " << argv[1] << "..." << std::flush;
                // Convert it from hex if necessary
                int dirofs = 0;
                if (argv[4][0] == '0' && (argv[4][1] == 'x' || argv[4][1] == 'X')) {
                    dirofs = std::stoi(argv[4], nullptr, 16);
                } else {
                    dirofs = std::stoi(argv[4]);
                }
                // n88 disk basic mode - 8 sector (2kb) fidelity.
                // Weird math, but it works out.
                int tgc = dirofs / 4;
                int tgh = (dirofs % 4) / 2;
                int tgr = ((dirofs % 2) * 8);
                int bofs = dirofs * (0x800 + 0x80) + 0x2b0;
                std::cout << std::hex << bofs << " " << dirofs << " " << tgc << " " << tgh << " " << tgr << std::endl;
                // Get the base path name
                std::string bn = fn.substr(fn.find_last_of("/\\") + 1);
                if (ftype == "ASCII") {
                    bool ok = CheckLineEndings(argv[2]);
                    if (!ok) {
                        std::cout << "Input failed! Ensure input is CRLF format." << std::endl;
                        return 1;
                    }
                }
                // Open the given disk file
                d88::disk bnew(argv[1]);
                bool atf = (ftype == "ASCII");
                // and add the file in binary mode with special flags
                int a = bnew.AddFile(fn, tgc, tgh, tgr + 1, true, atf, stloc, stloc + fsize);
                asrt(a);
                // Add the directory and BAM entry too!
                a = bnew.AddDirEntry(bn, dirofs, fsize, ftype);
                asrt(a);
                // And rewrite the bytes
                a = bnew.WriteBytes(argv[1]);
                asrt(a);
            } else {
                // NORMAL C-H-R BASED FILE ADD
                std::cout << "Adding file " << fn << " to " << argv[1] << "...\n" << std::flush;
                int a = new_disk.AddFile(fn, std::stoi(argv[3]), std::stoi(argv[4]), std::stoi(argv[5]), false);
				asrt(a);
                new_disk.WriteBytes(argv[1]);
            }
        }
    } else {
        // JUST MAKE A BLANK DISK
        std::cout << "Creating blank 2D disk " << argv[1] << "...\n" << std::endl;
        d88::disk new_disk = d88::disk(nullptr, 348848);
        new_disk.Format();
        new_disk.WriteBytes(argv[1]);
    }

    return 0;
}

// Tests 

int test_main(){
	d88::disk d;
	d.Format();
	d.WriteBytes("test.d88");
	return 0;
}


