#include "filesystem.h"

void read_block(int block_number, void* buffer, size_t size) {
    FILE* file = fopen("filesystem", "rb");
    if (file == NULL) {
        printf("Failed to open file for reading.\n");
        exit(1);
    }

    int block_offset = block_number * BLOCK_SIZE;
    if (fseek(file, block_offset, SEEK_SET) != 0) {
        printf("Failed to seek to the block in the file.\n");
        fclose(file);
        exit(1);
    }

    size_t total_bytes_read = fread(buffer, 1, size, file);
    if (total_bytes_read != size) {
        if (!feof(file)) {
            printf("Failed to read the block from the file.\n");
            fclose(file);
            exit(1);
        }
    }

    fclose(file);
}

void write_block(int block_number, void* buffer, size_t size) {
    FILE* file = fopen("filesystem", "rb+");
    if (file == NULL) {
        printf("Failed to open file for writing.\n");
        exit(1);
    }

    int block_offset = block_number * BLOCK_SIZE;
    if (fseek(file, block_offset, SEEK_SET) != 0) {
        printf("Failed to seek to the block in the file.\n");
        fclose(file);
        exit(1);
    }

    size_t bytes_written = fwrite(buffer, 1, size, file);
    if (bytes_written != size) {
        if (!feof(file)) {
            printf("Failed to read the block from the file.\n");
            fclose(file);
            exit(1);
        }
    }

    fclose(file);
}

void read_inode(int inode_number, struct inode* inode) {
    struct inode inode_table[MAX_INODES];
    read_block(INODE_TABLE_NUMBER, &inode_table, sizeof(inode_table));

    *inode = inode_table[inode_number];
}

void write_inode(int inode_number, struct inode* inode) {
    struct inode inode_table[MAX_INODES];
    read_block(INODE_TABLE_NUMBER, &inode_table, sizeof(inode_table));

    inode_table[inode_number] = *inode;
    write_block(INODE_TABLE_NUMBER, inode_table, sizeof(inode_table));
}

// Function to create the filesystem
void make_fs() {
    struct superblock sb;
    sb.s_inodes_count = MAX_INODES;
    sb.s_blocks_count = MAX_BLOCKS;
    sb.s_free_blocks_count = sb.s_blocks_count;
    sb.s_free_inodes_count = sb.s_inodes_count;
    sb.s_first_data_block = 4;
    write_block(SUPERBLOCK_NUMBER, &sb, sizeof(sb));

    char inode_bitmap[INODE_BITMAP_SIZE];
    memset(inode_bitmap, 0xFF, sizeof(inode_bitmap)); // all inodes are free
    write_block(INODE_BITMAP_NUMBER, &inode_bitmap, sizeof(inode_bitmap));

    char block_bitmap[BLOCK_BITMAP_SIZE];
    memset(block_bitmap, 0xFF, sizeof(block_bitmap)); // all blocks are free
    write_block(BLOCK_BITMAP_NUMBER, &block_bitmap, sizeof(block_bitmap));

    struct inode inode_table[MAX_INODES];
    memset(inode_table, 0, sizeof(inode_table));
    write_block(INODE_TABLE_NUMBER, &inode_table, sizeof(inode_table));
}


int create_inode() {
    struct superblock superblock;
    read_block(SUPERBLOCK_NUMBER, &superblock, sizeof(superblock));

    char inode_bitmap[INODE_BITMAP_SIZE];
    read_block(INODE_BITMAP_NUMBER, inode_bitmap, sizeof(inode_bitmap));

    int inode_number;
    for (inode_number = 0; inode_number < superblock.s_inodes_count; ++inode_number) {
        int byte_index = inode_number / 8;
        int bit_index = inode_number % 8;
        char byte = inode_bitmap[byte_index];
        if (byte & (1 << bit_index)) {
            break;
        }
    }

    if (inode_number == superblock.s_inodes_count) {
        return 0;
    }

    // Mark the inode as used in the bitmap
    int byte_index = inode_number / 8;
    int bit_index = inode_number % 8;
    inode_bitmap[byte_index] &= ~(1 << bit_index);

    write_block(INODE_BITMAP_NUMBER, inode_bitmap, sizeof(inode_bitmap));

    struct inode inode;
    inode.i_size = 0;
    inode.i_blocks = 0;
    memset(inode.i_block, 0, sizeof(inode.i_block));

    write_inode(inode_number, &inode);
    
    superblock.s_free_inodes_count--;
    write_block(SUPERBLOCK_NUMBER, &superblock, sizeof(superblock));

    return inode_number;
}

void delete_inode(int inode_number) {
    struct superblock superblock;
    read_block(SUPERBLOCK_NUMBER, &superblock, sizeof(superblock));
    char inode_bitmap[INODE_BITMAP_SIZE];
    read_block(INODE_BITMAP_NUMBER, inode_bitmap, sizeof(inode_bitmap));
    char block_bitmap[BLOCK_BITMAP_SIZE];
    read_block(BLOCK_BITMAP_NUMBER, block_bitmap, sizeof(block_bitmap));

    // Mark the inode as free in the bitmap
    int byte_index = inode_number / 8;
    int bit_index = inode_number % 8;
    inode_bitmap[byte_index] |= (1 << bit_index);

    write_block(INODE_BITMAP_NUMBER, inode_bitmap, sizeof(inode_bitmap));

    struct inode inode;
    read_inode(inode_number, &inode);

    // Update the block bitmap by marking blocks as free
    for (int i = 0; i < inode.i_blocks; i++) {
        int block_number = inode.i_block[i];
        int byte_offset = block_number / 8;
        int bit_offset = block_number % 8;
        char byte = block_bitmap[byte_offset];
        char mask = 1 << bit_offset;
        block_bitmap[byte_offset] |= mask;

        superblock.s_free_blocks_count++;
    }

    write_block(BLOCK_BITMAP_NUMBER, block_bitmap, sizeof(block_bitmap));
    superblock.s_free_inodes_count++;
    write_block(SUPERBLOCK_NUMBER, &superblock, sizeof(superblock));

    memset(&inode, 0, sizeof(struct inode));
    write_inode(inode_number, &inode);
}

int allocate_block() {
    char block_bitmap[BLOCK_BITMAP_SIZE];
    read_block(BLOCK_BITMAP_NUMBER, block_bitmap, sizeof(block_bitmap));
    struct superblock superblock;
    read_block(SUPERBLOCK_NUMBER, &superblock, sizeof(superblock));

    // Find the first free block in the bitmap
    for (int i = superblock.s_first_data_block; i < MAX_BLOCKS; i++) {
        int byte_offset = i / 8;
        int bit_offset = i % 8;
        char byte = block_bitmap[byte_offset];
        char mask = 1 << bit_offset;
        if ((byte & mask) == mask) {
            // Mark the block as allocated in the bitmap
            block_bitmap[byte_offset] &= ~mask;
            write_block(BLOCK_BITMAP_NUMBER, block_bitmap, sizeof(block_bitmap));

            superblock.s_free_blocks_count--;
            write_block(SUPERBLOCK_NUMBER, &superblock, sizeof(superblock));

            return i;
        }
    }

    return -1; // No free blocks available
}

void write_file(int inode_number, const char* data, int size) {
    struct inode inode;
    read_inode(inode_number, &inode);

    // Calculate the number of blocks needed to store the data
    int num_blocks = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    if (num_blocks > N_BLOCKS) {
        printf("Not enough space in the inode to write the file.\n");
        return;
    }

    // Write the data to the blocks
    int bytes_written = 0;
    for (int i = 0; i < num_blocks; i++) {
        // Allocate a new block for the data if necessary
        if (inode.i_block[i] == 0) {
            int block_number = allocate_block();
            if (block_number == -1) {
                printf("Failed to allocate block for writing.\n");
                return;
            }
            inode.i_block[i] = block_number;
            inode.i_blocks++;
        }

        int block_offset = bytes_written % BLOCK_SIZE;
        int remaining_bytes = BLOCK_SIZE - block_offset;
        int bytes_to_write = (size - bytes_written) < remaining_bytes ? (size - bytes_written) : remaining_bytes;

        write_block(inode.i_block[i], data + bytes_written, bytes_to_write);

        bytes_written += bytes_to_write;
    }

    inode.i_size = size;
    write_inode(inode_number, &inode);
}

void read_file(int inode_number, char* buffer, size_t buffer_size) {
    struct inode inode;
    read_inode(inode_number, &inode);

    // Read the file data blocks
    size_t bytes_read = 0;
    for (int i = 0; i < inode.i_blocks; i++) {
        int block_number = inode.i_block[i];
        size_t read_size = (buffer_size - bytes_read < BLOCK_SIZE) ? buffer_size - bytes_read : BLOCK_SIZE;
        read_block(block_number, buffer + bytes_read, read_size);
        bytes_read += read_size;
        if (bytes_read >= buffer_size) {
            break; // Buffer is full, break the loop
        }
    }
}