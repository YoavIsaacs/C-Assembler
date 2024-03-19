#include "./Preprocessor/preprocessor.h"
#include <string.h>

int main(int argc, char *argv[]) {
    char name[100];
    if (argc < 2) {
        printf("Missing file name\n");
        return 1;
    }
    strcpy(name, preprocessor(argv[1]));
    return 0;
}