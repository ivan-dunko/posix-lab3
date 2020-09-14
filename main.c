#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_LEN 256
#define ERROR_CODE -1
#define SUCCESS_CODE 0

typedef struct Context{
    size_t line_cnt;
    char **lines;
} Context;

void exitWithFailure(const char *msg, int errcode){
    errno = errcode;
    fprintf(stderr, "%.256s:%.256s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}

void *routine(void *data){
    Context *cntx = (Context*)data;
    
    for (size_t i = 0; i < cntx->line_cnt; ++i){
        int err = printf("%.256s\n", cntx->lines[i]);
        if (err < 0)
            exitWithFailure("routine", errno);
    }

	return SUCCESS_CODE;
}

int main(int argc, char **argv){

    if (argc == 1){
        printf("Format: [number of lines] [lines] and so on...\n");
        exit(EXIT_SUCCESS);
    }

    pthread_t pid[4];
    Context cntx[4];
    int ind = 1;
    for (int i = 0; i < 4; ++i){
        if (ind > argc)
            exitWithFailure("main", EINVAL);

        errno = 0;
        long int line_cnt = strtol(argv[ind], NULL, 10);
        ++ind;
        if (errno != SUCCESS_CODE || line_cnt < 0)
            exitWithFailure("main", EINVAL);

        cntx[i].line_cnt = (size_t)line_cnt;
        cntx[i].lines = argv + ind;
        ind += line_cnt;
        
    }

    if (ind > argc)
        exitWithFailure("main", EINVAL);

    for (int i = 0; i < 4; ++i){
        int err = pthread_create(&pid[i], NULL, routine, (void*)(&cntx[i]));
        if (err == ERROR_CODE)
            exitWithFailure("main", errno);
    }

    for (int i = 0; i < 4; ++i){
        int err = pthread_join(pid[i], NULL);
        if (err == ERROR_CODE)
            exitWithFailure("main", errno);
    }

    return 0;
}
