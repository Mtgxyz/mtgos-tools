#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include "mtgcompat.h"
#include <stdint.h>
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
struct FIRM_sect {
    uint32_t offset;
    uint32_t physical;
    uint32_t size;
    bool arm11;
    uint8_t SHA256[0x20]; //Implement later.
}__attribute__((packed));
struct FIRM_header {
    char magic[4];
    int reserved1;
    uint32_t arm11Entry;
    uint32_t arm9Entry;
    uint8_t reserved2[0x30];
    FIRM_sect sections[4];
    uint8_t RSA2048[0x100]; //I'd need to find out big N's private key to do that!
}__attribute__((packed));
int main(int argc, char** argv) {
    std::vector<std::string> infiles;
    std::vector<uint32_t> offsets;
    std::string outfile="firm.bin";
    FIRM_header header;
    //Populate header
    char * tmp="FIRM";
    memcpy(header.magic,tmp,4);
    header.reserved1=0;
    memset(header.reserved2,0,sizeof(header.reserved2));
    memset(header.RSA2048,0,sizeof(header.RSA2048));
    header.arm11Entry=0x1FF80000;
    header.arm9Entry=0x08000000;
    char opt = (char)getopt(argc, argv, "O:o:e:E:"); //O = load point of segment; o= output file; e=ARM9 entry; E=ARM11 entry
    while (opt!=-1) {
        switch(opt) {
            case 'O': {
                std::stringstream tmp;
                tmp<<std::hex << optarg;
                uint32_t entry;
                tmp >> entry;
                offsets.push_back(entry);
                break; }
            case 'o': {
                outfile=optarg;
                break; }
            case 'e': {
                std::stringstream tmp;
                tmp<<std::hex << optarg;
                uint32_t entry;
                tmp >> entry;
                header.arm9Entry=entry;
                break; }
            case 'E': {
                std::stringstream tmp;
                tmp<<std::hex << optarg;
                uint32_t entry;
                tmp >> entry;
                header.arm11Entry=entry;
                break;}
            case '?': {
                std::cerr << "Usage: " <<argv[0] << " -O offset1 [-O offset2 [-O offset3 [-O offset4]]] [-o outfile] [-e ARM9-entry] [-E ARM11-entry] file1 [file2 [file3 [file4]]]" << std::endl; 
                break; }
        }
        opt=(char)getopt(argc, argv, "O:o:e:E:");
    }
    char** inputFiles = argv + optind;
    int numInputFiles = argc - optind;
    while(numInputFiles--) {
        infiles.push_back(*(inputFiles++));
    }
    std::cout << "Found " << infiles.size() << " input files..." << std::endl;
    std::ifstream files[4];
    int foffset=sizeof(FIRM_header);
    for(int i=0;i<MIN(MIN(infiles.size(),offsets.size()),4);i++) {
        header.sections[i].offset=foffset;
        header.sections[i].physical=offsets[i];
        FILE *p_file = NULL;
        p_file = fopen(infiles[i].c_str(),"rb");
        fseek(p_file,0,SEEK_END);
        int size = ftell(p_file);
        foffset+=size;
        header.sections[i].size=size;
        fclose(p_file);
    }
    std::ofstream out(outfile.c_str(),std::ios::binary);
    out.write(reinterpret_cast<char*>(&header),sizeof header);
    for(int i=0;i<MIN(MIN(infiles.size(),offsets.size()),4);i++) {
	std::ifstream file(infiles[i].c_str(),std::ios::binary);
        char *buf=new char[header.sections[i].size];
	file.read(buf,header.sections[i].size);
	out.write(buf,header.sections[i].size);
    }
    return 0;
}