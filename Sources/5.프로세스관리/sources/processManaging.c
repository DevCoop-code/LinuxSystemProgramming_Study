#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    int ret;

    /*
    execl
    */
    // ret = execl ("/bin/mkdir", "mkdir", "newDir", NULL);
    // if (ret == -1) {
    //     perror("execl");
    // }

    /*
    execvp
    */
    // const char* args[] = {"vi", "/Users/hangyojeong/Dev/linuxSystemProgramming/Sources/test.txt", NULL};
    // ret = execvp("/usr/bin/vi", args);
    // if (ret == -1) {
    //     perror("execvp");
    // }

    /*
    fork
    */
   pid_t pid;
   pid = fork ();
   if (pid == -1) {
       perror("fork");
   }
   // Child Process
   if (pid == 0) {
        ret = execl ("/bin/mkdir", "mkdir", "newDir", NULL);
        if (ret == -1) {
            perror("execl");
            exit(EXIT_FAILURE);
        }
        // const char* args[] = {"vi", "/Users/hangyojeong/Dev/linuxSystemProgramming/Sources/test.txt", NULL};
        // ret = execvp("/usr/bin/vi", args);
        // if (ret == -1) {
        //     perror("execvp");
        // }
   } 
//    else {
//        while (1) {

//        }
//    }

    return 0;
}