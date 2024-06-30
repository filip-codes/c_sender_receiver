#include "shared.h"

void read_from_buffer(const char* buffer, int buffer_size, sem_t* mutex, sem_t* empty, sem_t* full) {
    char character;
    int idx = 0;

    while (1) {
        sem_wait(full);
        sem_wait(mutex);

        character = buffer[idx];
        idx = (idx + 1) % buffer_size;

        sem_post(mutex);
        sem_post(empty);

        if (character == '\0')
            break;

        putchar(character);
        fflush(stdout);
    }
}

int main(int argc, char* argv[]) {
    int buffer_size = get_buffer_size(argc, argv);

    int shm_fd = open_shared_memory(buffer_size);

    char *ring_buffer = map_shared_memory(shm_fd, buffer_size);

    sem_t *mutex, *empty, *full, *fully_read;
    open_semaphores(buffer_size, &mutex, &empty, &full, &fully_read);

    read_from_buffer(ring_buffer, buffer_size, mutex, empty, full);

    clean_up(RECEIVER, buffer_size, shm_fd, ring_buffer, mutex, empty, full, fully_read);

    return EXIT_SUCCESS;
}
