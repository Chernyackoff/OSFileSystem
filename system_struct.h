#ifndef __SYSTEM_STRUCT_H__
#define __SYSTEM_STRUCT_H__

#include <inttypes.h>

#define BUF_SIZE 256

struct attr_s {
    char name[BUF_SIZE];

    uint64_t date;
    uint16_t capacity;
} attr;

struct inode_s {
    struct attr_s attr;
    uint32_t block_ptr[18];
};

struct super_block_s {
    uint32_t block_count;
    uint32_t inode_count;

    uint32_t free_block_count;
    uint32_t free_inode_count;

    uint8_t flags;

    uint32_t first_free_block;
    uint32_t first_free_inode;

    uint8_t inode_block;


};


#endif //__SYSTEM_STRUCT_H__
