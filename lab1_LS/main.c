// #include <stdio.h>
// #include <stdlib.h>
// #include <dirent.h>
// #include <sys/stat.h>
// #include <unistd.h>
// #include <pwd.h>
// #include <grp.h>
// #include <time.h>
// #include <string.h>
// #include <getopt.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

void print_ls(char* path, char flags) 
{
    

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
    char flags = 0;

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
                _exit(-1);
        }
    }

    char* path = (optarg != NULL) ? optarg : "."; 
    print_ls(path, flags);

    // if(optarg) {
    //     path = optarg;
    // }

    // printf("%s\n", path);

    return 0;
}