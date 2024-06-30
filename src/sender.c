#include "shared.h"

void send_termination_character(char* buffer, int idx, sem_t* mutex, sem_t* empty, sem_t* full) {
    sem_wait(empty);
    sem_wait(mutex);

    buffer[idx] = '\0';

    sem_post(mutex);
    sem_post(full);
}

void write_to_buffer(char* buffer, int buffer_size, sem_t* mutex, sem_t* empty, sem_t* full) {
    int character;
    int idx = 0;

    while ((character = getchar()) != EOF) {
        sem_wait(empty);
        sem_wait(mutex);

        buffer[idx] = (char)character;
        idx = (idx + 1) % buffer_size;

        sem_post(mutex);
        sem_post(full);
    }

    send_termination_character(buffer, idx, mutex, empty, full);
}

int main(int argc, char *argv[]) {
    int buffer_size = get_buffer_size(argc, argv);

    int shm_fd = open_shared_memory(buffer_size);

    char *buffer = map_shared_memory(shm_fd, buffer_size);

    sem_t *mutex, *empty, *full, *fully_read;
    open_semaphores(buffer_size, &mutex, &empty, &full, &fully_read);

    write_to_buffer(buffer, buffer_size, mutex, empty, full);

    clean_up(SENDER, buffer_size, shm_fd, buffer, mutex, empty, full, fully_read);

    return EXIT_SUCCESS;
}
