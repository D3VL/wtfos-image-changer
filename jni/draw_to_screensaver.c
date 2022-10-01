#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))

void *openMem(off_t target, off_t size, int write) {
    int fd = open("/dev/mem", write ? O_RDWR : O_RDONLY);
    if (fd < 0) {
        printf("Failed to open /dev/mem\n");
        return NULL;
    }

    void *memory = mmap64(NULL, size, write ? (PROT_READ | PROT_WRITE) : (PROT_READ), MAP_SHARED, fd, target);
    if (memory == MAP_FAILED) {
        printf("Failed to map memory %p\n", memory);
        return NULL;
    }

    printf("Mapped Address: %ld to %p (%ld bytes) \n", target, memory, size);

    close(fd);

    return memory;
}

int checkIfMemoryEquals(off_t target, size_t size, size_t offset, char *data) {
    
    void *memory = openMem(target, size, 0);
    if (memory == NULL) {
        return 0;
    }

    printf("Checking Memory... \n");

    int length = MIN(size, strlen(data));

    int result = memcmp(data, memory + offset, length);

    munmap(memory, size);

    if (result == 0) {
        return 1;
    } else {
        return 0;
    }
}

off_t scanMemoryForPattern(off_t start, size_t haystack_size, char *needle, size_t needle_size) {
    
    void *haystack = openMem(start, haystack_size, 0);
    if (haystack == NULL) {
        return 0;
    }
   
    // You can probably tell i had a whale of a time getting this to work
    // // print 16 bytes of the memory starting at 0x17F3D000
    // printf("\n\n16 bytes of memory where the img should be: ");
    // for (int i = 0; i < 16; i++) {
    //     printf("%02X ", ((unsigned char *)haystack)[0x1809C03B - start + i]);
    // }

    // // print 16 bytes of the data
    // printf("\n16 bytes of data from the match field:      ");
    // for (int i = 0; i < 16; i++) {
    //     printf("%02X ", ((unsigned char *)needle)[i]);
    // }
    // printf("\n\n");
    
    void *result = memmem(haystack, haystack_size, needle, needle_size);
    if (result == NULL) {
        printf("Failed to find pattern in memory\n");
        return 0;
    }

    // get the offset of the result
    off_t offset = (off_t)result - (off_t)haystack;

    printf("Haystack relative offset: %ld\n", offset);

    munmap(haystack, haystack_size);

    return offset;
}

// function that reads file into a buffer
void *readFile(char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Failed to open file %s\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(length + 1);
    if (buffer == NULL) {
        printf("Failed to allocate memory for file %s\n", filename);
        return NULL;
    }

    fread(buffer, 1, length, file);
    fclose(file);

    return buffer;
}

int writeFile(char *filename, char *data, size_t size) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Failed to open file %s\n", filename);
        return 0;
    }

    fwrite(data, 1, size, file);
    fclose(file);

    return 1;
}

// Yeah ok, WHY DOESN'T C HAVE A CONCAT... i dunno 
char* concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}


int main(int argc, char *argv[]) {
    setbuf(stdout, NULL);

    off_t ion_rsv_start = 0x13f00000;
    size_t ion_rsv_length = 0x7800000;

    off_t target = 0x17F3D000;
    off_t offset = 0x0;

    size_t size = 0x473100;
    size_t mem_size = 0x480000;
  

    char *screen = argv[1];
    char *needle;
    size_t needle_size;
    off_t needle_offset;
    int cached_offset = 0;

    if (strcmp(screen, "1") == 0) {
        target = 0x17F3D000;

        // FF 7E 7E 7E FF 7C 7C 7C FF 6C 6C 6C FF 64 64 64 FF 5D 5D 5D FF 59 59 59 FF 61 61 61 FF 65 65 65 FF 60 60 60 FF 62 62 62 FF 61 61 61 FF 62 62 62
        needle = "\xFF\x7E\x7E\x7E\xFF\x7C\x7C\x7C\xFF\x6C\x6C\x6C\xFF\x64\x64\x64\xFF\x5D\x5D\x5D\xFF\x59\x59\x59\xFF\x61\x61\x61\xFF\x65\x65\x65\xFF\x60\x60\x60\xFF\x62\x62\x62\xFF\x61\x61\x61\xFF\x62\x62\x62";
        needle_size = 0x30;
        needle_offset = 0x15F03B;

    } else if (strcmp(screen, "2") == 0) {
        target = 0x183B1000;

        // 75 61 3C FF 76 62 3D FF 77 63 3E FF 77 63 3E FF 78 64 3F FF 77 65 3E FF 77 65 3E FF 78 66 3F FF 79 67 40 FF 7A 68 41 FF 7A 68 40 FF 7A 68 40 FF
        needle = "\x75\x61\x3C\xFF\x76\x62\x3D\xFF\x77\x63\x3E\xFF\x77\x63\x3E\xFF\x78\x64\x3F\xFF\x77\x65\x3E\xFF\x77\x65\x3E\xFF\x78\x66\x3F\xFF\x79\x67\x40\xFF\x7A\x68\x41\xFF\x7A\x68\x40\xFF\x7A\x68\x40\xFF";
        needle_size = 0x30;
        needle_offset = 0x0C4970;

    } else {
        printf("Invalid screen number\n");
        return 1;
    }

    // check /tmp/screensaver_offset_0? for the offset
    void *cached_read = readFile(concat("/tmp/screensaver_offset_0", screen));
    if (cached_read != NULL) {
        needle_offset = atoi(cached_read); 
        cached_offset = 1;
    }
        
    int default_offset = checkIfMemoryEquals(target, size, needle_offset, needle);
    
    if (!default_offset && !cached_offset) {
        printf("Image is not at default address\n");                                   // 0x17F3D000

        off_t new_address = scanMemoryForPattern(ion_rsv_start, ion_rsv_length, needle, needle_size); // 0xb10c779b 
        if (new_address == 0) {
            printf("Default image not found.\n");
            return 1;
        }

        target = ((new_address - needle_offset) + ion_rsv_start);

        printf("Found image at %ld\n", ((new_address - needle_offset) + ion_rsv_start));
    } else {
        if (cached_offset) {
            printf("Using cached offset\n");
        } else {
            printf("Using default offset\n");
        }
    }

    char *file = argv[2];
    
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
    
    printf("Pre Calc Target: %ld\n", target);

    // calculate the offset of target in memory based off pagesize
    off_t page_size = getpagesize();
    off_t page_offset = target % page_size;
    target = target - page_offset;
     
    printf("Page size: %ld\n", page_size);
    printf("Page offset: %ld\n", page_offset);
    printf("Target: %ld\n", target);

    void *memory = openMem(target, mem_size, 1);

    memcpy(memory + page_offset, data, size);

    printf("Wrote Data\n");

    munmap(memory, size);

    if (!cached_offset) {
        char *cached_write = malloc(0x10);
        sprintf(cached_write, "%lx", needle_offset);
        writeFile(concat("/tmp/screensaver_offset_0", screen), cached_write, 0x10);
    }

    return 0;
}