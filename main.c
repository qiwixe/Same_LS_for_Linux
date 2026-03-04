#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>

typedef struct {
    char permissions[11];
    long links;
    char owner[32];
    char group[32];
    long size;
    char date[32];
    char name[256];
} FileInfo;

void header() {
    printf("%-11s %3s %-8s %-8s %8s %-12s %s\n",
           "ПРАВА",
           "ССЫЛКИ",
           "ВЛАДЕЛЕЦ",
           "ГРУППА",
           "РАЗМЕР",
           "ДАТА",
           "ИМЯ ФАЙЛА");

    printf("-------------------------------------------------------------------------------\n");
}

//-h
void size() {

}

void generate_permissions(char *perm, mode_t mode) {
    perm[0] = S_ISDIR(mode) ? 'd' : '-';
    perm[1] = (mode & S_IRUSR) ? 'r' : '-';
    perm[2] = (mode & S_IWUSR) ? 'w' : '-';
    perm[3] = (mode & S_IXUSR) ? 'x' : '-';
    perm[4] = (mode & S_IRGRP) ? 'r' : '-';
    perm[5] = (mode & S_IWGRP) ? 'w' : '-';
    perm[6] = (mode & S_IXGRP) ? 'x' : '-';
    perm[7] = (mode & S_IROTH) ? 'r' : '-';
    perm[8] = (mode & S_IWOTH) ? 'w' : '-';
    perm[9] = (mode & S_IXOTH) ? 'x' : '-';
    perm[10] = '\0';
}

//-l
int get_info(const int lflag,const char *path, FileInfo file[] ) {
    int count = 0;

    DIR *dir = opendir(path);

    if (!dir) {
        perror("opendir");
        return 0;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        strcpy(file[count].name, entry->d_name);
        if (lflag) {

            char fullpath[1024];
            struct stat file_stat;

            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
            if (stat(fullpath, &file_stat) != 0) {
                perror("stat");
                continue;
            }

            file[count].links = file_stat.st_nlink;
            file[count].size  = file_stat.st_size;

            struct passwd *pw = getpwuid(file_stat.st_uid);
            struct group  *gr = getgrgid(file_stat.st_gid);
            strcpy(file[count].owner, pw ? pw->pw_name : "unknown");
            strcpy(file[count].group, gr ? gr->gr_name : "unknown");

            strftime(file[count].date, sizeof(file[count].date),
                     "%b %d %H:%M",
                     localtime(&file_stat.st_mtime));

            generate_permissions(file[count].permissions, file_stat.st_mode);
    }
    count++;
    }
    closedir(dir);
    return count;
}

void print_file_info(FileInfo file, int lflag) {
    if (lflag) {
        printf("%-11s %3ld %-8s %-8s %8ld %-12s %s\n",
                    file.permissions,
                    file.links,
                    file.owner,
                    file.group,
                    file.size,
                    file.date,
                    file.name);
    } else {
        printf("%s\n", file.name);
    }
}

int main(int argc, char *argv[]) {

    int lflag = 0;
    int hflag = 0;
    int rflag = 0;
    const char *path = ".";

    FileInfo file[1024];
    int count = 0;
    int opt;

    //Обработка аргументов
    while ((opt = getopt(argc, argv, "lhr")) != -1) {
        switch (opt) {
            case 'l': lflag = 1; break;
            case 'h': hflag = 1; break;
            case 'r': rflag = 1; break;
            default:
                fprintf(stderr, "Неизвестный аргумент: -%c\n", optopt);
                return 1;
        }
    }

    //Обработка пути из аргументов (Берем последний, остальные игнорируются)
    if (optind < argc) {
        path = argv[argc - 1];
    }

    count = get_info(lflag, path, file);

    //print
    for (int i = (rflag ? count-1 : 0);
     (rflag ? i >= 0 : i < count);
     (rflag ? i-- : i++))
        print_file_info(file[i],lflag);

    return 0;
}