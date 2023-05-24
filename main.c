#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filesystem.c"

#define MAX_BUFFER_SIZE 1024

void clear_output() {
    system("clear");
}

void print_menu() {
    printf("\nFilesystem Menu:\n");
    printf("1. Initialize Filesystem\n");
    printf("2. Create Inode\n");
    printf("3. Delete Inode\n");
    printf("4. Allocate Block\n");
    printf("5. Write to File\n");
    printf("6. Read from File\n");
    printf("7. Print Inodes\n");
    printf("0. Exit\n");
    printf("Enter your choice: ");
}

void print_inodes() {
    printf("\n--- Inodes ---\n");
    for (int inode_number = 0; inode_number < MAX_INODES; inode_number++) {
        struct inode inode;
        read_inode(inode_number, &inode);
        printf("Inode %d: Size = %d bytes, Blocks = %d\n", inode_number, inode.i_size, inode.i_blocks);
    }
}

int main() {
    int choice;
    int inode_number;

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
                inode_number = create_inode();
                if (inode_number != -1) {
                    printf("Inode created successfully. Inode number: %d\n", inode_number);
                } else {
                    printf("No free inodes available.\n");
                }
                break;
            case 3:
                printf("Enter the inode number to delete: ");
                scanf("%d", &inode_number);
                delete_inode(inode_number);
                printf("Inode %d deleted.\n", inode_number);
                break;
            case 4:
                inode_number = allocate_block();
                if (inode_number != -1) {
                    printf("Block allocated successfully. Block number: %d\n", inode_number);
                } else {
                    printf("No free blocks available.\n");
                }
                break;
            case 5:
                printf("Enter the inode number: ");
                scanf("%d", &inode_number);
                printf("Enter the data to write: ");
                char data[MAX_BUFFER_SIZE];
                getchar(); // Clear the newline character
                fgets(data, sizeof(data), stdin);
                int size = strlen(data);
                write_file(inode_number, data, size);
                printf("Data written to the file.\n");
                break;
            case 6:
                printf("Enter the inode number: ");
                scanf("%d", &inode_number);
                printf("Enter the buffer size: ");
                size_t buffer_size;
                scanf("%zu", &buffer_size);
                if (buffer_size > MAX_BUFFER_SIZE) {
                    printf("Buffer size exceeds the maximum limit.\n");
                    break;
                }
                char buffer[MAX_BUFFER_SIZE];
                read_file(inode_number, buffer, buffer_size);
                printf("Data read from the file: %.*s\n", (int)buffer_size, buffer);
                break;
            case 7:
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
