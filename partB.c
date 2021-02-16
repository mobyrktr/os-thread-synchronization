#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h> 
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

const int MEMSIZE = 40000; // 10.000 değer için
const char* SHM_NAME = "ABO";

int readFromFile(FILE* fp) {
    int line;

    if(fscanf(fp, "%d\n", &line) != EOF){
        return line;
    }

    return -1; // EOF
}

void replace(int* ptr, int N) {
    int tmp;
    for(int i = 0; i < N - 1; i++) {
        if(*(ptr + i) > *(ptr + i + 1)) {
            tmp = *(ptr + i);
            *(ptr + i) = *(ptr + i + 1);
            *(ptr + i + 1) = tmp;
        }
    }
}

void createProcesses(int N, int K, int* ptr, char filenames[][20], sem_t* sem) {
    for(int i = 0; i < N; i++) {
        if(fork() == 0) {
            FILE *fp = fopen(filenames[i], "r");
            int line;
            do {
                line = readFromFile(fp);
                sem_wait(sem);
                if(line > *ptr) {
                    *ptr = line;
                    replace(ptr, K);
                }
                sem_post(sem);
            }

            while(line != -1);
            fclose(fp);
            exit(0);
        }
    }

    for(int i = 0; i < N; i++)
        wait(NULL);
}

void writeDescending(int *ptr, int K, FILE* fp) {
    for(int i = K - 1; i >= 0; i--) {
        fprintf(fp, "%d\n", *(ptr + i));
    }
}


void main(int argc, char* argv[]) {

    int K; // node 100 - 10000
    int N; // thread 1-10

    sscanf(argv[1], "%d", &K);
    sscanf(argv[2], "%d", &N);

    char filenames[N][20], outfile[20];

    for (int i = 3; i < argc - 1; i++)
    {
        strcpy(filenames[i - 3], argv[i]);
    }

    strcpy(outfile, argv[argc - 1]);

    shm_unlink(SHM_NAME);
    sem_unlink("ABOSEM");

    sem_t *sem = sem_open("ABOSEM", O_CREAT | O_EXCL, 0644, 1);
    sem_init(sem, 1, 1);
    
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0644);

    ftruncate(shm_fd, MEMSIZE);
    int* ptr = mmap(0, MEMSIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    createProcesses(N, K, ptr, filenames, sem);
    ptr = mmap(0, MEMSIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    
    FILE* out = fopen(outfile, "w");
    writeDescending(ptr, K, out);
    
    sem_destroy(sem);
    sem_unlink("ABOSEM");
}

// ./a.out 150 5 infile1.txt infile2.txt infile3.txt infile4.txt infile5.txt outfile_sem.txt
// gcc partB.c -lrt -lpthread