#include <stdlib.h>
#include <string.h>

#include "filesystem.c"

#define MAX_BUFFER_SIZE 1024

void clear_output() {
    system("clear");
}

void print_inodes() {
    int is_once_view = 0;
    printf("\n--- Inodes ---\n");
    for (int inode_number = 0; inode_number < MAX_INODES; inode_number++) {
        struct inode inode;
        read_inode(inode_number, &inode);
        if (inode.i_size != 0) {
            is_once_view = 1;
            printf("\x1b[32mInode %d\x1b[0m: Size = %d bytes, Blocks = %d\n", inode_number, inode.i_size, inode.i_blocks);
        }
    }
    if (!is_once_view)
        printf("No Inodes with data\n");
}

void print_menu() {
    printf("\n\x1b[32mFilesystem Menu:\n");
    printf("1. Initialize Filesystem\n");
    printf("2. Delete Inode\n");
    printf("3. Write to File\n");
    printf("4. Read from File\n");
    printf("5. Print Inodes\n");
    printf("0. Exit\x1b[0m\n");
    printf("Enter your choice: ");
}

int main() {
    int choice;
    int inode_number;
    int err = 0;

    while (1) {
        clear_output();
        print_menu();
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                make_fs();
                printf("Filesystem initialized.\n");
                break;
            case 2:
                printf("Enter the inode number to delete: ");
                scanf("%d", &inode_number);
                if (inode_number >= MAX_INODES|| inode_number < 0) {
                    printf("\x1b[31mEnter another inode number\x1b[0m\n");
                    break;
                }
                delete_inode(inode_number);
                printf("Inode %d deleted.\n", inode_number);
                break;
            case 3:
                printf("Enter the inode number: ");
                scanf("%d", &inode_number);
                if (inode_number >= MAX_INODES|| inode_number < 0) {
                    printf("\x1b[31mEnter another inode number\x1b[0m\n");
                    break;
                }
                printf("Enter the data to write: ");
                char data[MAX_BUFFER_SIZE];
                getchar(); // Clear the newline character
                fgets(data, sizeof(data), stdin);
                int size = strlen(data);
                write_file(inode_number, data, size);
                printf("Data written to the file.\n");
                break;
            case 4:
                printf("Enter the inode number: ");
                scanf("%d", &inode_number);
                if (inode_number >= MAX_INODES || inode_number < 0) {
                    printf("\x1b[31mEnter another inode number\x1b[0m\n");
                    break;
                }
                printf("Enter the buffer size: ");
                size_t buffer_size;
                scanf("%zu", &buffer_size);
                if (buffer_size > MAX_BUFFER_SIZE) {
                    printf("\x1b[31mBuffer size exceeds the maximum limit.\x1b[0m\n");
                    break;
                }
                char buffer[MAX_BUFFER_SIZE];
                if (read_file(inode_number, buffer, buffer_size)) {
                    printf("\x1b[31mThis inode doesn't exist\x1b[0m\n");
                    break;
                }
                printf("Data read from the file: %.*s\n", (int)buffer_size, buffer);
                break;
            case 5:
                print_inodes();
                break;
            case 0:
                printf("Exiting...\n");
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }

        printf("Press Enter to continue...");
        getchar(); // Clear the newline character
        getchar(); // Wait for user input before clearing the output
    }

    return 0;
}
