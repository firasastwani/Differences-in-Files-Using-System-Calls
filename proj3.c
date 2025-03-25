#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdbool.h>

// Function declarations
void step1(const char *file1, const char *file2);
void step2(const char *file1, const char *file2);

double getTimeDiff(struct timeval start, struct timeval end);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: proj3.out <file1> <file2>\n");
        exit(0);
    }

    struct timeval start;
    struct timeval end;

    double step1Time;
    double step2Time;

    gettimeofday(&start, NULL);
    step1(argv[1], argv[2]);
    gettimeofday(&end, NULL);
    step1Time = getTimeDiff(start, end);

    gettimeofday(&start, NULL);
    step2(argv[1], argv[2]);
    gettimeofday(&end, NULL);
    step2Time = getTimeDiff(start, end);

    printf("Step 1 took %.6f seconds\n", step1Time);
    printf("Step 2 took %.6f seconds\n", step2Time);

    return 0;
}

double getTimeDiff(struct timeval start, struct timeval end)
{
    double diff = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 100000.0;
    return diff;
}

void step1(const char *file1, const char *file2)
{
    int fd1;
    int fd2;
    int fd_out1;

    char buf1[2];
    char buf2[2];

    size_t read1;
    size_t read2;

    fd1 = open(file1, O_RDONLY);
    fd2 = open(file2, O_RDONLY);

    if (fd1 == -1 || fd2 == -1)
    {
        printf("There was an error reading a file.\n");

        if (fd1 != -1)
            close(fd1);
        if (fd2 != -1)
            close(fd2);

        exit(0);
    }

    fd_out1 = open("differencesFoundInFile1.txt", O_WRONLY | O_CREAT | O_TRUNC, 0600);

    if (fd_out1 == -1)
    {
        printf("There was in error writing a file.\n");
        close(fd1);
        close(fd2);
        exit(0);
    }

    bool cont = true;

    while (cont)
    {
        read1 = read(fd1, buf1, 1);
        read2 = read(fd2, buf2, 1);

        if (read1 <= 0)
        {
            cont = false;
        }
        else if (read2 <= 0)
        {
            if (write(fd_out1, buf1, 1) == -1)
            {
                printf("There was an error writing to a file.\n");
                close(fd1);
                close(fd2);
                close(fd_out1);
                exit(0);
            }

            while ((read1 = read(fd1, buf1, 1)) > 0)
            {
                if (write(fd_out1, buf1, 1) == -1)
                {
                    printf("There was an error writing to a file.\n");
                    close(fd1);
                    close(fd2);
                    close(fd_out1);
                    exit(0);
                }
            }
            cont = false;
        }
        else if (buf1[0] != buf2[0])
        {
            if (write(fd_out1, buf1, 1) == -1)
            {
                printf("There was an error writing to a file\n");
                close(fd1);
                close(fd2);
                close(fd_out1);
                exit(0);
            }
        }
    }

    close(fd1);
    close(fd2);
    close(fd_out1);
}

void step2(const char *file1, const char *file2)
{
    int fd1;
    int fd2;
    int fdOut2;

    struct stat st1;
    struct stat st2;

    char *buff1;
    char *buff2;
    char *diffBuff;

    int read1;
    int read2;

    int fileSize1;
    int fileSize2;

    if (stat(file1, &st1) == -1 || stat(file2, &st2) == -1)
    {
        printf("There was an error reading the file\n");
        exit(0);
    }

    fileSize1 = st1.st_size;
    fileSize2 = st2.st_size;

    buff1 = malloc(fileSize1);
    buff2 = malloc(fileSize2);

    diffBuff = malloc(fileSize2);

    if (buff1 == NULL || buff2 == NULL || diffBuff == NULL)
    {
        printf("Memory allocation error.\n");
        free(buff1);
        free(buff2);
        free(diffBuff);
        exit(0);
    }

    fd1 = open(file1, O_RDONLY);
    fd2 = open(file2, O_RDONLY);

    if (fd1 == -1 || fd2 == -1)
    {
        printf("There was an error reading a file.\n");
        free(buff1);
        free(buff2);
        free(diffBuff);

        if (fd1 != -1)
            close(fd1);
        if (fd2 != -1)
            close(fd2);

        exit(0);
    }

    read1 = read(fd1, buff1, fileSize1);
    read2 = read(fd2, buff2, fileSize2);

    if (read1 == -1 || read2 == -1)
    {
        printf("There was an error reading a file.\n");
        free(buff1);
        free(buff2);
        free(diffBuff);

        close(fd1);
        close(fd2);

        exit(0);
    }

    int diffSize = 0;

    int minSize;

    if (fileSize1 > fileSize2)
    {
        minSize = fileSize2;
    }
    else
    {
        minSize = fileSize1;
    }

    for (int i = 0; i < minSize; i++)
    {
        if (*(buff2 + i) != *(buff1 + i))
        {
            diffBuff[diffSize++] = *(buff2 + i);
        }
    }

    for (int i = minSize; i < fileSize2; i++)
    {
        diffBuff[diffSize++] = *(buff2 + i);
    }

    fdOut2 = open("differencesFoundInFile2.txt", O_WRONLY | O_CREAT | O_TRUNC, 0600);

    if (fdOut2 == -1)
    {
        printf("There was an error writing to a file.\n");
        free(buff1);
        free(buff2);
        free(diffBuff);

        close(fd1);
        close(fd2);

        exit(0);
    }

    if (diffSize > 0)
    {
        if (write(fdOut2, diffBuff, diffSize) == -1)
        {
            printf("There was an error writing to a file.\n");
            free(buff1);
            free(buff2);
            free(diffBuff);
            close(fd1);
            close(fd2);
            close(fdOut2);
            exit(0);
        }
    }

    free(buff1);
    free(buff2);
    free(diffBuff);

    close(fd1);
    close(fd2);
    close(fdOut2);
}
