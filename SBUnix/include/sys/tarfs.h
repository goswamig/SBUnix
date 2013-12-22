#ifndef _TARFS_H
#define _TARFS_H

extern char _binary_tarfs_start;
extern char _binary_tarfs_end;

struct posix_header_ustar {
        char name[100];
        char mode[8];
        char uid[8];
        char gid[8];
        char size[12];
        char mtime[12];
        char checksum[8];
        char typeflag[1];
        char linkname[100];
        char magic[6];
        char version[2];
        char uname[32];
        char gname[32];
        char devmajor[8];
        char devminor[8];
        char prefix[155];
        char pad[12];
};

//TARFS file system
typedef struct {
    char name[100];
    int size;
    int typeflag;
    uint64_t addr_hdr;
    int par_ind;
} tarfs_entry;

extern tarfs_entry tarfs_fs[100];

void tarfs_init();

uint64_t open_dir(char * dir);
uint64_t read_dir(uint64_t  dir);
uint64_t open(char * file);
uint64_t read_file(uint64_t file_addr, int size, uint64_t buf);
#define DIRECTORY 5
#define FILE_TYPE 0

#endif
