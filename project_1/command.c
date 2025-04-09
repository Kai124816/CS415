#define _GNU_SOURCE
#include <unistd.h>           // for syscall(), close()
#include <sys/syscall.h>      // for SYS_getdents
#include <fcntl.h>            // for open(), O_RDONLY, O_DIRECTORY
#include <stdio.h>            // for printf()
#include <stdlib.h>           // for exit()
#include <linux/dirent.h>     // for struct linux_dirent

int main() {
    // Open the current directory (".") as a file descriptor
    // O_RDONLY: open for reading
    // O_DIRECTORY: ensure it's a directory, not a regular file
    int fd = open(".", O_RDONLY | O_DIRECTORY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Create a buffer to store the directory entries
    char buf[1024];

    // Make the syscall to get directory entries
    // SYS_getdents is the syscall number for getdents
    // fd: the directory file descriptor
    // buf: the buffer to fill with entries
    // sizeof(buf): max bytes to read
    int nread = syscall(SYS_getdents, fd, buf, sizeof(buf));
    if (nread == -1) {
        perror("getdents");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Cast the buffer to linux_dirent* and walk through it
    struct linux_dirent *d;
    int bpos = 0;

    // Iterate through each directory entry in the buffer
    while (bpos < nread) {
        // Move pointer to the next directory entry
        d = (struct linux_dirent *) (buf + bpos);

        // Print the name of the entry (null-terminated string)
        printf("%s\n", d->d_name);

        // Move to the next entry using the length of the current one
        bpos += d->d_reclen;
    }

    // Close the directory file descriptor
    close(fd);

    return 0;
}
