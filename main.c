/******************************************************************************
 * @file    main.c
 * @author  Qiwixe
 * @brief   Утилита клон стандартной ls
 *
 * @details
 * Реализованы флаги
 *  -l  Расширенный формат представления
 *  -h  Человеко-читаемый формат размеров файлов
 *  -r  Обратный порядок вывода данных
 *
 * Данные файлов получает функция stat() и после формируется массив для вывода
 *
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>

#define BUFSIZE 1024

/**
 * @brief Структура хранения данных файлов
 */
typedef struct {
    char permissions[11];  ///< Права доступа
    long links;            ///< Количество жёстких ссылок
    char owner[32];        ///< Имя владельца
    char group[32];        ///< Имя группы
    long size;             ///< Размер файла в байтах
    char date[32];         ///< Дата последнего изменения
    char name[256];        ///< Имя файла
} FileInfo;

/**
 * @brief Вывод заголовка таблицы для расширенного формата (-l).
 *
 * Функция печатает названия столбцов таблицы:
 *  - PERM  : права доступа к файлу
 *  - LINK  : количество жёстких ссылок
 *  - OWNER : владелец файла
 *  - GROUP : группа файла
 *  - SIZE  : размер файла
 *  - DATE  : дата последнего изменения
 *  - FILE NAME : имя файла
 *
 * @note
 * Используется при выводе списка файлов в расширенном формате.
 */
void header() {
    printf("%-10s %-4s %-8s %-8s %-10s %-12s %s\n",
               "PERM",
               "LINK",
               "OWNER",
               "GROUP",
               "SIZE",
               "DATE",
               "FILE NAME");
    printf("-------------------------------------------------------------------------------\n");
}

/**
 * @brief Вывод размера файла в человеко-читаемом формате.
 *
 * Преобразует размер файла из байтов в удобный формат (B, K, M, G, T)
 *
 * Например:
 *  1536   -> 1.5K
 *  1048576 -> 1.0M
 *
 * @param size Размер файла в байтах.
 */
void print_size(long size) {
    const char *units[] = {"B", "K", "M", "G", "T"};
    int i = 0;
    double sz = (double)size;

    while (sz >= 1024 && i < 4) {
        sz /= 1024;
        i++;
    }
    printf("%6.1f%1s", sz, units[i]);
}

/**
 * @brief Создает строку прав файла.
 *
 * @param perm  Буфер для вывода строки разрешений
 * @param mode  Данные о правах
 *
 * @details
 * Преобразует данные в строку
 * Пример: drwxr-xr-x
 */
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

/**
 * @brief Получение информации о файлах в каталоге.
 *
 * Функция открывает указанный каталог и считывает информацию обо всех
 * файлах внутри него. Для каждого файла заполняется структура FileInfo.
 *
 * Если установлен флаг lflag, дополнительно собирается расширенная
 * информация о файле:
 *  - права доступа
 *  - количество ссылок
 *  - владелец
 *  - группа
 *  - размер файла
 *  - дата последнего изменения
 *
 * @param lflag Флаг длинного вывода (аналог опции -l в команде ls).
 *              Если равен 1 — собирается полная информация о файлах.
 * @param path  Путь к каталогу, содержимое которого необходимо прочитать.
 * @param file  Массив структур FileInfo, в который записывается информация
 *              о найденных файлах.
 *
 * @return Количество файлов, успешно прочитанных из каталога.
 *
 * @note Если каталог невозможно открыть, функция выводит сообщение
 *       об ошибке через perror() и возвращает 0.
 */
int get_info(const int lflag,const char *path, FileInfo file[] ) {
    int count = 0;

    DIR *dir = opendir(path);

    if (!dir) {
        perror("opendir");
        return 0;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {

        if (count >= BUFSIZE) {
            fprintf(stderr, "Выход за пределы (Ограничение 1024)\n");
            break;
        }

        strcpy(file[count].name, entry->d_name);
        if (lflag) {

            char fullpath[BUFSIZE];
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

/**
 * @brief Вывод информации о файле.
 *
 * Функция печатает информацию о файле в зависимости от установленных флагов.
 *
 * Если установлен флаг lflag, выводится расширенная информация:
 *  - права доступа
 *  - количество ссылок
 *  - владелец
 *  - группа
 *  - размер файла
 *  - дата изменения
 *  - имя файла
 *
 * Если установлен флаг hflag, размер файла выводится в
 * человекочитаемом формате (K, M, G).
 *
 * Если lflag не установлен, выводится только имя файла.
 *
 * @param file  Структура FileInfo, содержащая информацию о файле.
 * @param lflag Флаг длинного формата вывода (-l).
 * @param hflag Флаг человекочитаемого размера (-h).
 */
void print_file_info(const FileInfo file, int lflag, int hflag) {
    if (lflag) {
        printf("%-11s %3ld %-8s %-8s ",
            file.permissions,
            file.links,
            file.owner,
            file.group);
        if (hflag) {
            print_size(file.size);
        }else {
            printf("%8ld",file.size );
        }
        printf(" %-13s %s\n",
            file.date,
            file.name);
    } else {
        printf("%s\n", file.name);
    }
}

/**
 * @brief Точка входа программы.
 *
 * Поддерживаемые аргументы командной строки:
 *  -l  длинный формат вывода (подробная информация о файле)
 *  -h  человекочитаемый размер файла
 *  -r  вывод в обратном порядке
 *
 * Если путь к каталогу не указан, используется текущий каталог (.).
 *
 * Алгоритм работы:
 * 1. Обработка аргументов командной строки с помощью getopt().
 * 2. Определение каталога для чтения.
 * 3. Получение информации о файлах через функцию get_info().
 * 4. Вывод списка файлов (в прямом или обратном порядке).
 *
 * @param argc Количество аргументов командной строки.
 * @param argv Массив аргументов командной строки.
 *
 * @return 0 при успешном завершении программы.
 */
int main(int argc, char *argv[]) {

    int lflag = 0;
    int hflag = 0;
    int rflag = 0;
    const char *path = ".";

    FileInfo file[BUFSIZE];
    int count = 0;
    int opt;

    //Обработка аргументов
    while ((opt = getopt(argc, argv, "lhr")) != -1) {
        switch (opt) {
            case 'l': lflag = 1; header(); break;
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
        print_file_info(file[i],lflag,hflag);

    return 0;
}