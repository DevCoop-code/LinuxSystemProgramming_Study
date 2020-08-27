#include <stdio.h>

int main(int argc, char* argv[]) {
    int ret;

    ret = execl ("/bin/mkdir", "mkdir", "newDir", NULL);
    if (ret == -1) {
        perror("execl");
    }

    return 0;
}