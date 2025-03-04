 #include <stdio.h>
#include <stdlib.h> 
#include <string.h> // strcmp, memset
#include <fcntl.h> // open, O_WRONLY, O_CREAT, O_TRUNC
#include <unistd.h> //write, close
#include <stdbool.h> //true, false
#include <time.h> // time, CLOCK_PER_SEC, 


void print_help(const char *prog_name) {
    printf("Usage: %s <output_file> <num_blocks can add suffix: [K, M, G]> <block_size in B; can add suffix: [K, M, G]> <data_type: [0, 1, block_rand | br, all_rand | r, all_urand | u]>\n", prog_name);
}

int parse_block_size(const char *arg) {
    char *endptr;
    long value = strtol(arg, &endptr, 10);
    
    if (*endptr == '\0') {
        return value;
    } else if (*endptr == 'K' || *endptr == 'k') {
        return value * 1024;
    } else if (*endptr == 'M' || *endptr == 'm') {
        return value * 1024 * 1024;
    } else if (*endptr == 'G' || *endptr == 'g') {
        return value * 1024 * 1024 * 1024;
    } else {
        printf("Error: Invalid block size suffix: %s\n", arg);
        exit(EXIT_FAILURE);
    }
}

void print_result(int all_num_blocks, int num_blocks, int block_size, const char *filename, clock_t time_start) {
    clock_t end = clock();
    double time_taken = (double)(end - time_start) / CLOCKS_PER_SEC;
    char total_bytes_unit;
    long total_bytes = (long) num_blocks * block_size;
    double total_bytes_in_unit = (double) total_bytes;
    if (total_bytes_in_unit < 1024) {
        total_bytes_unit = ' ';
    } else if (total_bytes_in_unit < 1024*1024) {
        total_bytes_in_unit /= 1024;
        total_bytes_unit = 'K';
    } else if (total_bytes_in_unit < 1024*1024*1024) {
        total_bytes_in_unit /= (1024*1024);
        total_bytes_unit = 'M';
    } else {
        total_bytes_in_unit /= (1024*1024*1024);
        total_bytes_unit = 'G';
    }

    if (num_blocks == all_num_blocks) {
        printf("Success: ");
    }
    else {
        printf("Error: ");
    }
    printf(
        "Wrote %d blocks of %dB, saving a total of %.2f%cB (%ldB) to %s in %.6fs.\n", 
        num_blocks, block_size, total_bytes_in_unit, total_bytes_unit, total_bytes, filename, time_taken
    );
}

int main(int argc, char *argv[]) {
    clock_t start;
    start = clock();
    if (argc != 5) {
        print_help(argv[0]);
        return EXIT_FAILURE;
    }

    srand(time(NULL));
    const char *filename = argv[1];
    int num_blocks = parse_block_size(argv[2]);
    int block_size = parse_block_size(argv[3]);
    char *data_type = argv[4];

    if (num_blocks <= 0 || block_size <= 0) {
        printf("Error: num_blocks and block_size must be >0\n");
        return EXIT_FAILURE;
    }
    int fd_urandom;
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
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
    bool is_all_urandom_mode = false;

    if (strcmp(data_type, "0") == 0) {
        memset(buffer, 0, block_size);
    } else if (strcmp(data_type, "1") == 0) {
        memset(buffer, 0xFF, block_size);
    } else if (strcmp(data_type, "block_rand") == 0 || strcmp(data_type, "br") == 0) {
        for (int i = 0; i < block_size; i++) {
            buffer[i] = rand() % 256;
        }
    } else if (strcmp(data_type, "all_rand") == 0 || strcmp(data_type, "r") == 0) {
        is_all_random_mode = true;
    } else if (strcmp(data_type, "all_urand") == 0 || strcmp(data_type, "u") == 0) {
        is_all_urandom_mode = true;
    } else {
        printf("Error: Invalid data_type. Use '0', '1', or 'rand'.\n");
        free(buffer);
        close(fd);
        return EXIT_FAILURE;
    }

    if (is_all_urandom_mode) {
        fd_urandom = open("/dev/urandom", O_RDONLY);
        if (fd_urandom == -1) {
            printf("Error: error while opening /dev/urandom");
            free(buffer);
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_blocks; i++) {
        if (is_all_random_mode) {
            for (int j = 0; j < block_size; j++) {
                buffer[j] = rand() % 256;
            }
        } else if (is_all_urandom_mode) {
            int bytes_read = read(fd_urandom, buffer, block_size);
            if (bytes_read != block_size) {
                printf("Error: reading from /dev/urandom");
                close(fd_urandom);
                free(buffer);
                close(fd);
                exit(EXIT_FAILURE);
            }
        }

        if (write(fd, buffer, block_size) != block_size) {
            printf("Error: error while writing to file");
            print_result(num_blocks, i, block_size, filename, start);
            free(buffer);
            close(fd);
            if(is_all_urandom_mode) close(fd_urandom);
            return EXIT_FAILURE;
        }
    }

    free(buffer);
    close(fd);
    if(is_all_urandom_mode) close(fd_urandom);
    print_result(num_blocks, num_blocks, block_size, filename, start);
    return EXIT_SUCCESS;
}
