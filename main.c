#include "filesystem.c"

// Function to perform filesystem operations
void use_fs() {
    // This part isn't used I left it for testing purposes 
    struct superblock superblock;
    read_block(SUPERBLOCK_NUMBER, &superblock, sizeof(superblock));
    char inode_bitmap[INODE_BITMAP_SIZE];
    read_block(INODE_BITMAP_NUMBER, inode_bitmap, sizeof(inode_bitmap));
    char block_bitmap[BLOCK_BITMAP_SIZE];
    read_block(BLOCK_BITMAP_NUMBER, block_bitmap, sizeof(block_bitmap));
    struct inode inode_table[MAX_INODES];
    read_block(INODE_TABLE_NUMBER, &inode_table, sizeof(inode_table));

    int test_inode = create_inode();

    char data[2000];
    memset(data, 'A', 2000);
    write_file(test_inode, data, sizeof(data));
    char buffer[2000] = {0};
    read_file(test_inode, buffer, sizeof(buffer));
    printf("Contents of %d: %s\n", test_inode, buffer);
    
    delete_inode(test_inode);
}

int main() {
    make_fs();
    use_fs();

    return 0;
}
