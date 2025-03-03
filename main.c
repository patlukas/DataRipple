#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>

void print_help(const char *prog_name) {
    printf("Usage: %s <output_file> <num_blocks> <block_size> <data_type: [0, 1, block_rand, all_rand]>\n", prog_name);
}

int main(int argc, char *argv[]) {
    clock_t start, end;
    start = clock();
    if (argc != 5) {
        print_help(argv[0]);
        return EXIT_FAILURE;
    }

    srand(time(NULL));
    const char *filename = argv[1];
    int num_blocks = atoi(argv[2]);
    int block_size = atoi(argv[3]);
    char *data_type = argv[4];

    if (num_blocks <= 0 || block_size <= 0) {
        printf("Error: num_blocks and block_size must be >0\n");
        return EXIT_FAILURE;
    }

    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC);
    if (fd == -1) {
        printf("Error: Can't open file");
        return EXIT_FAILURE;
    }

    unsigned char *buffer = malloc(block_size);
    if (!buffer) {
        printf("Error: Memory allocation error");
        close(fd);
        return EXIT_FAILURE;
    }

    bool is_all_random_mode = false;

    if (strcmp(data_type, "0") == 0) {
        memset(buffer, 0, block_size);
    } else if (strcmp(data_type, "1") == 0) {
        memset(buffer, 0xFF, block_size);
    } else if (strcmp(data_type, "block_rand") == 0) {
        for (int i = 0; i < block_size; i++) {
            buffer[i] = rand() % 256;
        }
    } else if (strcmp(data_type, "all_rand") == 0) {
        is_all_random_mode = true;
    }else {
        printf("Error: Invalid data_type. Use '0', '1', or 'rand'.\n");
        free(buffer);
        close(fd);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < num_blocks; i++) {
        if (is_all_random_mode) {
            for (int j = 0; j < block_size; j++) {
                buffer[j] = rand() % 256;
            }
        }

        if (write(fd, buffer, block_size) != block_size) {
            printf("Error: error while writing to file");
            free(buffer);
            close(fd);
            return EXIT_FAILURE;
        }
    }

    free(buffer);
    close(fd);
    end = clock();
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Successfully wrote %d blocks of %d bytes to %s in %.6fs.\n", num_blocks, block_size, filename, time_taken);
    return EXIT_SUCCESS;
}
