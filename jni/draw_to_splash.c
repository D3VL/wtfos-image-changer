#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>

// get 0x12FB9F bytes from /dev/urandom
// then write it to memory at 0x2E000100

int main(int argc, char *argv[]) {

    off_t target = 0x2E000000;  // 0x2E000100;
    size_t size = 0x1C79FF;
    size_t mem_size = 0x1D0000; // 0x1C79FF

    // get the file from arguments
    char *file = argv[1];
    
    char *data = malloc(size);
    FILE *input = fopen(file, "r");
    if (input == NULL) {
        printf("Error opening file: %s", strerror(errno));
        
        memset(data, 0x00, size);
    } else {
        fread(data, size, 1, input);

        fclose(input);
    }

    printf("Got Data\n");

    int fd = open("/dev/mem", O_RDWR);
    if (fd < 0) {
        printf("Failed to open /dev/mem\n");
        return 1;
    }

    void *mapped_address = mmap64(NULL, mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, target);
    if (mapped_address == MAP_FAILED) {
        printf("Failed to map memory %p\n", mapped_address);
        return 1;
    }

    printf("Mapped Address: %ld to %p\n", target, mapped_address);

    memcpy(mapped_address + 0x100, data, size);

    printf("Wrote Data\n");

    munmap(mapped_address, size);

    close(fd);

    return 0;
}