#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

pthread_mutex_t lock;

typedef struct ABO
{
    int data;
    struct ABO* next;
} ABO;

ABO root;

void insertNode(ABO* root) {
    if(root->next == NULL) {
        ABO* new = malloc(sizeof(ABO));
        new->data = 0;
        new->next = NULL;
        root->next = new;
    }

    else {
        insertNode(root->next);
    }
}

void createNodes(ABO* root, int K) {
    for(int i = 0; i < K - 1; i++) {
        insertNode(root);
    }
}

void swapNodes(ABO* n1, ABO* n2) {
    int tmp = n1 -> data;
    n1 -> data = n2 -> data;
    n2 -> data = tmp;
}

void replaceNode(ABO* root, int new_data) { // 8 5 6 7 9
    if(root -> next == NULL || (root -> next) -> data > new_data) {
        return;
    }

    else {
        swapNodes(root, root -> next);
        replaceNode(root -> next, new_data);
    }
}

int readFromFile(FILE* fp) {
    int line;

    if(fscanf(fp, "%d\n", &line) != EOF){
        return line;
    }

    return -1; // EOF
}

void printTree(ABO* root) {
    if(root->next == NULL) {
        printf("%d\n", root->data);
    }

    else {
        printf("%d\n", root->data);
        printTree(root->next);
    }
}

void* fillNodes(void *arg) {
    FILE *fp = (FILE*) arg;
    int curr_number;
    do {
        curr_number = readFromFile(fp);
        pthread_mutex_lock(&lock);
        if(curr_number > root.data) {
            root.data = curr_number;
            replaceNode(&root, curr_number);
        }
        pthread_mutex_unlock(&lock);
    }

    while(curr_number != -1);
    
    return NULL;
}

void writeDescending(ABO* root, FILE* fp) {
    if(root->next == NULL) {
        fprintf(fp, "%d\n", root->data);
        return;
    }

    writeDescending(root->next, fp);
    fprintf(fp, "%d\n", root->data);
}

void main(int argc, char* argv[]) {
    pthread_mutex_init(&lock, NULL);
    
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

    pthread_t pths[N];
    FILE *fps[N];

    root.data = 0;
    root.next = NULL;
    
    createNodes(&root, K);

    for(int i = 0; i < N; i++) {
        fps[i] = fopen(filenames[i], "r");
        pthread_create(&pths[i], NULL, &fillNodes, fps[i]);
    }

    for(int j = 0; j < N; j++) {
        pthread_join(pths[j], NULL);
        fclose(fps[j]);
    }

    pthread_mutex_destroy(&lock);

    FILE* out_fp = fopen(outfile, "w");
    writeDescending(&root, out_fp);
    fclose(out_fp);
}

// ./a.out 150 5 infile1.txt infile2.txt infile3.txt infile4.txt infile5.txt outfile.txt
// gcc partA.c -lpthread