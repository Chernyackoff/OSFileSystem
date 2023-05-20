#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 1024
#define N_BLOCKS 15
#define MAX_BLOCKS 128
#define MAX_INODES 32
#define BLOCK_BITMAP_SIZE (MAX_BLOCKS / 8)
#define INODE_BITMAP_SIZE (MAX_INODES / 8)

struct superblock {
    int s_inodes_count;          // Total number of inodes in the filesystem
    int s_blocks_count;          // Total number of blocks in the filesystem
    int s_free_blocks_count;     // Total number of free blocks
    int s_free_inodes_count;     // Total number of free inodes
    int s_first_data_block;      // Block number of the first data block
};

struct inode {
    int i_size;                // Size of the file in bytes
    int i_blocks;              // Number of blocks allocated to the file
    int i_block[N_BLOCKS];     // Array of block pointers
};

void read_block(int block_number, void* buffer, size_t size);
void write_block(int block_number, const void* buffer, size_t size);
void read_inode(int inode_number, struct inode* inode);
void write_inode(int inode_number, struct inode* inode);
void make_fs();
int create_inode();
void delete_inode(int inode_number);
int allocate_block();
void write_file(int inode_number, const char* data, int size);
void read_file(int inode_number, char* buffer, size_t buffer_size);

#endif /* FILESYSTEM_H */
