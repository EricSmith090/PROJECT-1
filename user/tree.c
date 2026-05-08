#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void tree(char *path, int depth) {
    int fd;
    struct stat st;

    // Mo duong dan
    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "tree: cannot open %s\n", path);
        return;
    }

    // Lay thong tin
    if ((fstat(fd, &st)) < 0) {
        fprintf(2, "tree: cannot open stat %s\n", path);
        close(fd);
        return;
    }

    // In thut dau dong theo do sau
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }

    // Lay ten cuoi cung tu duong dan
    char *last = path;
    for (char *p = path; *p; p++) {
        if (*p == '/') last = p + 1;
    }
    printf("%s", last);
    if (st.type == T_DIR) {
        printf("/\n");
    } else {
        printf("\n");
        close(fd);
        return;
    }

    // Doc cac entry trong thu muc
    struct dirent de;
    char buf[512];
    while (read(fd, &de,  sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) continue;
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;

        // Tao duong dan day du cho entry
        if (strlen(path) + 1 + strlen(de.name) + 1 > sizeof(buf)) {
            fprintf(2, "tree: path too long for %s/%s\n", path, de.name);
            continue;
        }
        strcpy(buf, path);
        char *q = buf + strlen(buf);
        *q++ = '/';
        strcpy(q, de.name);

        // De quy xu ly entry
        tree(buf, depth + 1);
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    char *path;
    if (argc < 2) {
        path = ".";
    } else {
        path = argv[1];
    }
    tree(path, 0);
    exit(0);
}