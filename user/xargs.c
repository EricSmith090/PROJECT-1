#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: xargs command...\n");
        exit(1);
    }

    char *xargv[MAXARG];
    int xargc = 0;
    for (int i = 1; i < argc; i++) {
        xargv[xargc++] = argv[i];
    }

    char buf[512];
    int buf_idx = 0;
    char c;

    while (read(0, &c, 1) == 1) {
        if (c == '\n') {
            buf[buf_idx] = '\0';
            xargv[xargc] = buf;
            xargv[xargc + 1] = 0;
            if (fork() == 0) {
                exec(xargv[0], xargv);
                fprintf(2, "exec %s failed\n", xargv[0]);
                exit(1);
            } else {
                wait(0);
            }

            buf_idx = 0;
        } else {
            buf[buf_idx++] = c;
        }
    }

    exit(0);
}
