#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>
#include <semaphore.h>

#include "type.h"

#define SHM_NAME "/shm_name"
#define SEM_FULL "/sem_full"
#define SEM_MUTEX "/sem_mutex"
#define SEM_EMPTY "/sem_empty"
#define SEM_FULLY_READ "/fully_read"

int get_buffer_size(int argc, char* argv[]);

int open_shared_memory(int buffer_size);
char* map_shared_memory(int shm_fd, int buffer_size);
void open_semaphores(int buffer_size, sem_t** mutex, sem_t** empty, sem_t** full, sem_t** fully_read);

void clean_up(enum type_e type, int buffer_size, int shm_fd, char* ring_buffer, sem_t* mutex, sem_t* empty, sem_t* full, sem_t* fully_read);

#endif // SHARED_H


