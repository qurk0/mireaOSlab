#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <sys/dir.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <getopt.h>

#define RESET_COLOR "\033[0m"
#define BLUE_COLOR "\033[34m"
#define GREEN_COLOR "\033[32m"
#define CYAN_COLOR "\033[36m"

int compare_alphabetically(const struct dirent **a, const struct dirent **b) {
    return strcmp((*a)->d_name, (*b)->d_name);
}

void printFileColor(const char* path, const struct stat *fileStat)
{
    if(S_ISDIR(fileStat->st_mode))
    {
        printf(BLUE_COLOR);
    }
    else if (S_ISLNK(fileStat->st_mode))
    {
        printf(CYAN_COLOR);
    }
    else if(fileStat->st_mode & S_IXUSR)
    {
        printf(GREEN_COLOR);
    }
}

void printFileInfo(const char *name, const char *path, const struct stat *fileStat)
{
    char modes[11] = "----------";
    struct passwd *pw = getpwuid(fileStat->st_uid);
    struct group *gr = getgrgid(fileStat->st_gid);
    char timebuf[80];

    modes[0] = S_ISDIR(fileStat->st_mode) ? 'd' :
              S_ISLNK(fileStat->st_mode) ? 'l' : '-';
    modes[1] = (fileStat->st_mode & S_IRUSR) ? 'r' : '-';
    modes[2] = (fileStat->st_mode & S_IWUSR) ? 'w' : '-';
    modes[3] = (fileStat->st_mode & S_IXUSR) ? 'r' : '-';
    modes[4] = (fileStat->st_mode & S_IRGRP) ? 'r' : '-';
    modes[5] = (fileStat->st_mode & S_IWGRP) ? 'w' : '-';
    modes[6] = (fileStat->st_mode & S_IXGRP) ? 'x' : '-';
    modes[7] = (fileStat->st_mode & S_IROTH) ? 'r' : '-';
    modes[8] = (fileStat->st_mode & S_IWOTH) ? 'w' : '-';
    modes[9] = (fileStat->st_mode & S_IXOTH) ? 'x' : '-';

    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&fileStat->st_mtime));
    printf("%s %lu %s %s %5ld %s", modes, fileStat->st_nlink, pw->pw_name, gr->gr_name, fileStat->st_size, timebuf);
    printFileColor(path, fileStat);
    printf(" %s\n", name);
    printf(RESET_COLOR);
}

void print_ls(char* path, unsigned char flags) 
{
    DIR* directory = opendir(path);

    if(!directory) {
        perror ("Open directory error");
        return;
    }
    
    struct dirent **entry;
    int filesCount = scandir(path, &entry, NULL, compare_alphabetically);
    if(filesCount < 0) 
    {
        perror("Directory scanning error"); return;
    }

    for (int i = 0; i < filesCount; i++) {
        if(entry[i]->d_name[0] == '.' && !(flags & 2)) {
            free(entry[i]); continue;
        }

    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%s/%s", path, entry[i]->d_name);

    struct stat fileStat;
    if(lstat(buffer, &fileStat) < 0)
    {
        perror("Check status error");
        free(entry[i]); continue;
    }

    if(flags & 1) 
    {
        printFileInfo(entry[i]->d_name, buffer, &fileStat);
    }
    else
    {
        printFileColor(buffer, &fileStat);
        printf("%s\n", entry[i]->d_name);
        printf(RESET_COLOR);
    }

    }

    return;    
}

int main(int argc, char** argv)
{
    /*
    Переменная flags занимает 8 бит в памяти, что можно использовать для отслеживания 8 флагов.
    При инициализации флагов 0 мы имеем следущую картину:
    
    << 0 0 0 0 0 0 0 0 >>

    Допустим мы возьмем флаг "-l" = 1 и "-a" = 2; тогда

    << 0 0 0 0 0 0 0 1 >>
                     ^
                     Отвечает за флаг "-l";
    
    << 0 0 0 0 0 0 1 0 >>
                   ^
                   Отвечает за флаг "-a";

    А 0 + 1 + 2 будет выглядеть как: ... 0 0 + ... 0 1 + ... 1 0, и тогда

    << 0 0 0 0 0 0 1 1 >>
                   ^ ^
                   В конце flags (при условии, что flags = 3) будут две единицы, говорящие о том,
                   что на вход мы получили оба флага.

    Потом проверяем флаги масками и битовым И

    (
    Пример:
        flags = 0 0 0 0 0 0 1 0
        mask  = 0 0 0 0 0 0 0 1

        flags & mask = 0, так как биты не совпадают

        а если 
        mask = 0 0 0 0 0 0 1 0

        то flags & mask = 1, биты совпали, значит была подана какая-то опция и мы ее будем учитывать.
    )

    Для экономии памяти можно на бумаге взять маску "0 0 0 0 0 0 0 1" и потом побитово сдвигать вправо, проверяя на 1 из 8 флагов
    
    */
    unsigned char flags = 0;

    // Наша опция
    char c;

    while((c = getopt(argc, argv, "l::a::")) != -1) {
        switch(c) {
            case 'l':
                flags += 1;
                break;
            case 'a':
                flags += 2;
                break;
            default:
                perror("Undefined options. Use -l or -a, plzzzz\n");
                _exit(EXIT_FAILURE);
        }
    }

    char* path = (optarg != NULL) ? optarg : "."; 
    print_ls(path, flags);
    return 0;
}
