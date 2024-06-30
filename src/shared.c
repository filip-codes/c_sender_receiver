#include "shared.h"

int get_buffer_size(int argc, char* argv[]) {
    int opt, buffer_size = -1;

    while ((opt = getopt(argc, argv, "m:")) != -1) {
        switch (opt) {
            case 'm':
                buffer_size = atoi(optarg);

                if (buffer_size <= 0) {
                    fprintf(stderr, "Error: Invalid buffer size\n");
                    exit(EXIT_FAILURE);
                }

                break;
            default:
                fprintf(stderr, "Usage: %s -m <elements>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    return buffer_size;
}

int open_shared_memory(int size) {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);

    if (shm_fd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, sizeof(char) * size) == -1) {
        perror("ftruncate failed");
        exit(EXIT_FAILURE);
    }

    return shm_fd;
}

char* map_shared_memory(int shm_fd, int buffer_size) {
    char* buffer = mmap(0, sizeof(char) * buffer_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (buffer == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    return buffer;
}

void open_semaphores(int buffer_size, sem_t** mutex, sem_t** empty, sem_t** full, sem_t** fully_read) {
    *mutex = sem_open(SEM_MUTEX, O_CREAT, 0666, 1);
    *full = sem_open(SEM_FULL, O_CREAT, 0666, 0);
    *empty = sem_open(SEM_EMPTY, O_CREAT, 0666, buffer_size);
    *fully_read = sem_open(SEM_FULLY_READ, O_CREAT, 0666, 0);

    if (*mutex == SEM_FAILED || *full == SEM_FAILED || *empty == SEM_FAILED || *fully_read == SEM_FAILED) {
        perror("sem_open failed");
        exit(EXIT_FAILURE);
    }
}

void clean_up(enum type_e type, int buffer_size, int shm_fd, char* ring_buffer, sem_t* mutex, sem_t* empty, sem_t* full, sem_t* fully_read) {
    munmap(ring_buffer, sizeof(char) * buffer_size);
    close(shm_fd);

    sem_close(mutex);
    sem_close(empty);
    sem_close(full);

    if (type == SENDER) {
        sem_wait(fully_read);
        sem_close(fully_read);
        sem_unlink(SEM_FULLY_READ);
        sem_unlink(SEM_MUTEX);
        sem_unlink(SEM_EMPTY);
        sem_unlink(SEM_FULL);
        shm_unlink(SHM_NAME);
    }

    if (type == RECEIVER) {
        sem_post(fully_read);
        sem_close(fully_read);
    }
}
