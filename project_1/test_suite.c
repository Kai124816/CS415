#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>

#include "command.h"  // your function declarations

void test_make_and_change_dir() {
    char *test_dir = "test_temp_dir";
    makeDir(test_dir);
    struct stat st;
    assert(stat(test_dir, &st) == 0 && S_ISDIR(st.st_mode));

    changeDir(test_dir);
    char cwd[256];
    getcwd(cwd, sizeof(cwd));
    assert(strstr(cwd, test_dir) != NULL);

    chdir("..");
    rmdir(test_dir);
    printf("✅ makeDir and changeDir passed.\n");
}

void test_listDir() {
    int fd = open("test_ls_out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int saved_stdout = dup(1);
    dup2(fd, 1);

    listDir();

    fflush(stdout);
    dup2(saved_stdout, 1);
    close(fd);

    FILE *f = fopen("test_ls_out.txt", "r");
    assert(f != NULL);

    char content[1024];
    fread(content, 1, sizeof(content), f);
    assert(strstr(content, "test_ls_out.txt") != NULL);

    fclose(f);
    remove("test_ls_out.txt");
    printf("✅ listDir passed.\n");
}

void test_copy_and_display_file() {
    char *src = "test_input.txt";
    char *dest = "test_output.txt";
    FILE *f = fopen(src, "w");
    fprintf(f, "hello test file\n");
    fclose(f);

    copyFile(src, dest);

    FILE *f2 = fopen(dest, "r");
    char line[256];
    fgets(line, sizeof(line), f2);
    assert(strcmp(line, "hello test file\n") == 0);
    fclose(f2);

    // test displayFile()
    int fd = open("test_cat_out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int saved_stdout = dup(1);
    dup2(fd, 1);

    displayFile(dest);

    fflush(stdout);
    dup2(saved_stdout, 1);
    close(fd);

    FILE *out = fopen("test_cat_out.txt", "r");
    fgets(line, sizeof(line), out);
    assert(strcmp(line, "hello test file\n") == 0);
    fclose(out);

    remove(src);
    remove(dest);
    remove("test_cat_out.txt");

    printf("✅ copyFile and displayFile passed.\n");
}

void test_move_file() {
    FILE *f = fopen("move_src.txt", "w");
    fprintf(f, "data");
    fclose(f);

    makeDir("mvtest");
    moveFile("move_src.txt", "mvtest/move_dest.txt");

    assert(access("move_src.txt", F_OK) != 0); // should be deleted
    assert(access("mvtest/move_dest.txt", F_OK) == 0);

    remove("mvtest/move_dest.txt");
    rmdir("mvtest");
    printf("✅ moveFile passed.\n");
}

void test_delete_file() {
    FILE *f = fopen("delete_me.txt", "w");
    fprintf(f, "bye");
    fclose(f);

    deleteFile("delete_me.txt");

    assert(access("delete_me.txt", F_OK) != 0);
    printf("✅ deleteFile passed.\n");
}

int main() {
    test_make_and_change_dir();
    test_listDir();
    test_copy_and_display_file();
    test_move_file();
    test_delete_file();

    printf("\n🎉 All command tests passed!\n");
    return 0;
}
