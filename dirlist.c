/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500

#include <dirent.h>
#include <grp.h>
#include <linux/limits.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"


typedef struct {
        char *name;
        char *username;
        char *groupname;
        char *actualPath[PATH_MAX];
        char *permString;
        struct stat fileStat;
        struct dirent *dirent_entry;
        // bool hidden;
} directory_item;

int 
compare_directory_item(const void *p1, const void *p2)
{
        char *n1, *n2;

        if (((const directory_item *) p1)->name[0] == '.')
                n1 = ((const directory_item *) p1)->name + 1;
        else
                n1 = ((const directory_item *) p1)->name;

        if (((const directory_item *) p2)->name[0] == '.')
                n2 = ((const directory_item *) p2)->name + 1;
        else
                n2 = ((const directory_item *) p2)->name;

        return strcmp(n1, n2);
}

int main(int argc, char *argv[])
{
        // printf("Program start");

        // char flags[10];
        char dir[50];

        bool showHidden = false;
        bool showColor = false;
        //printf("argv[1]: %s\n", argv[1]);
        if (argc == 2 || argc == 3) {

                if (argv[1][0] == '-') {
                        if (argc == 2)
                                strcpy(dir, "./");
                        else
                                strcpy(dir, argv[2]);

                        for (int i = 1; i < strlen(argv[1]); i++) {
                                //printf("Current arugment being parsed: %c\n", argv[1][i]);
                                switch (argv[1][i]) {
                                case 'h':
                                        showHidden = true;
                                        break;
                                case 'c':
                                        showColor = true;
                                        break;
                                default:
                                        break;
                                }
                        }

                        // if (strchr(argv[1], 'h'))
                        //       showHidden = true;
                        // if (strchr(argv[1], 'c'))
                        //        showColor = true;

                } else {
                        strcpy(dir, argv[1]);
                }
        } else {
                strcpy(dir, "./");
        }

        DIR *dp;
        dp = opendir(dir);
        int numDir = 0;
        if (dp != NULL) {
                for (; readdir(dp); numDir++)
                        ;

                directory_item *folderContents = (directory_item *) malloc(numDir * sizeof(directory_item));
                rewinddir(dp);

                int maxUserLength = 0, maxGroupLength = 0;

                for (int i = 0; i < numDir; i++) {
                        folderContents[i].dirent_entry = readdir(dp);
                        folderContents[i].name = folderContents[i].dirent_entry->d_name;
                        // folderContents[j].hidden = folderContents[j].name[0] == '.';
                        // printf("%c: %i\n", folderContents[i].name[0],
                        // folderContents[i].hidden);
                        char actualPath[PATH_MAX + 1];
                        realpath(dir, actualPath);
                        strcat(actualPath, "/");
                        strcat(actualPath, folderContents[i].dirent_entry->d_name);
                        stat(actualPath, &folderContents[i].fileStat);
                        folderContents[i].username = strdup(getpwuid(folderContents[i].fileStat.st_uid)->pw_name);

                        folderContents[i].groupname = strdup(getgrgid(folderContents[i].fileStat.st_gid)->gr_name);
                        // printf("Filename: %s, Owner: %s, Gowner: %s\n",
                        // folderContents[j].name, folderContents[j].username,
                        // folderContents[j].groupname);

                        //                 struct passwd* user =
                        //                 getpwuid(fileStat.st_uid); struct
                        //                 group* group =
                        //                 getgrgid(fileStat.st_gid);
                        if (strlen(folderContents[i].username) > maxUserLength)
                                maxUserLength = strlen(folderContents[i].username);
                        if (strlen(folderContents[i].groupname) > maxGroupLength)
                                maxGroupLength = strlen(folderContents[i].groupname);
                        // printf("Filename: %s, Owner: %s, Gowner: %s\n", folderContents[j].name,
                        //        folderContents[i].username, folderContents[i].groupname);
                        // printf("Filename:%s\n", folderContents[i].dirent_entry->d_name);
                }

                // printf("Filename: %s, Owner: %s, Gowner: %s\n",
                // folderContents[0].name, folderContents[0].username,
                // folderContents[0].groupname);

                // printf("Max user length: %d\n", maxUserLength);
                // printf("Max group length: %d\n", maxGroupLength);

                qsort(folderContents, numDir, sizeof(directory_item), compare_directory_item);

                for (int i = 0; i < numDir; i++) {
                        if (!showHidden && folderContents[i].name[0] == '.')
                                continue;

                        // printf("Filename: %s, Owner: %s, Gowner: %s\n", folderContents[i].name,
                        //        folderContents[i].username, folderContents[i].groupname);

                        printf((S_ISDIR(folderContents[i].fileStat.st_mode)) ? "d" : "-");
                        printf((folderContents[i].fileStat.st_mode & S_IRUSR) ? "r" : "-");
                        printf((folderContents[i].fileStat.st_mode & S_IWUSR) ? "w" : "-");
                        printf((folderContents[i].fileStat.st_mode & S_IXUSR) ? "x" : "-");
                        printf((folderContents[i].fileStat.st_mode & S_IRGRP) ? "r" : "-");
                        printf((folderContents[i].fileStat.st_mode & S_IWGRP) ? "w" : "-");
                        printf((folderContents[i].fileStat.st_mode & S_IXGRP) ? "x" : "-");
                        printf((folderContents[i].fileStat.st_mode & S_IROTH) ? "r" : "-");
                        printf((folderContents[i].fileStat.st_mode & S_IWOTH) ? "w" : "-");
                        printf((folderContents[i].fileStat.st_mode & S_IXOTH) ? "x" : "-");

                        printf(" %s", folderContents[i].username);

                        for (int j = strlen(folderContents[i].username); j < maxUserLength; j++)
                                printf(" ");

                        printf(" %s", folderContents[i].groupname);

                        for (int j = strlen(folderContents[i].groupname); j < maxGroupLength; j++)
                                printf(" ");



                        if (showColor)
                                switch(folderContents[i].dirent_entry->d_type)
                                {
                                        case 4:
                                                printf(ANSI_COLOR_BLUE " %s" ANSI_COLOR_RESET, folderContents[i].name);
                                                break;
                                        default:
                                                printf(" %s", folderContents[i].name);
                                                break;
                                }
                        else
                                printf(" %s", folderContents[i].name);

                        // printf(" %d", folderContents[i].dirent_entry->d_type);
                        printf("\n");
                }

                for (int i = 0; i < numDir; i++) {
                        free(folderContents[i].username);
                        free(folderContents[i].groupname);
                }
                free(folderContents);
        } else
                perror("Couldn't open the directory");
        return 0;
}
