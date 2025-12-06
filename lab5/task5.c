#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>

void print_directory_contents(const char *dir_name) {
    DIR *dir = opendir(dir_name);
    if (dir == NULL) {
        perror("Ошибка открытия каталога");
        return;
    }
    
    printf("Содержимое каталога '%s':\n", dir_name);
    printf("========================================\n");
    
    struct dirent *entry;
    int file_count = 0;
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] != '.') {
            printf("%s\n", entry->d_name);
            file_count++;
        }
    }
    
    if (file_count == 0) {
        printf("Каталог пуст\n");
    }
    
    printf("========================================\n");
    printf("Всего файлов: %d\n\n", file_count);
    
    if (closedir(dir) != 0) {
        perror("Ошибка закрытия каталога");
    }
}

int main(int argc, char *argv[]) {
    char current_dir[1024];
    if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
        perror("Ошибка получения текущего каталога");
        return 1;
    }
    
    printf("Текущий каталог: %s\n\n", current_dir);
    print_directory_contents(current_dir);
    
    if (argc >= 2) {
        print_directory_contents(argv[1]);
    } else {
        printf("Для вывода другого каталога укажите его путь как аргумент\n");
        printf("Пример: %s /home/user\n", argv[0]);
    }
    
    return 0;
}