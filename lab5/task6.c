#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_FILES 1000
#define MAX_PATH 1024

typedef struct {
    char path[MAX_PATH];
    struct stat file_info;
    unsigned char *content;
    size_t content_size;
} FileData;

FileData dir1_files[MAX_FILES];
FileData dir2_files[MAX_FILES];
int dir1_count = 0;
int dir2_count = 0;

void read_directory(const char *base_path, FileData *files_array, int *count) {
    char path[MAX_PATH];
    struct dirent *entry;
    DIR *dir = opendir(base_path);
    
    if (dir == NULL) {
        perror("Ошибка открытия каталога");
        return;
    }
    
    while ((entry = readdir(dir)) != NULL && *count < MAX_FILES) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        snprintf(path, sizeof(path), "%s/%s", base_path, entry->d_name);
        
        struct stat path_stat;
        if (stat(path, &path_stat) != 0) {
            perror("Ошибка stat");
            continue;
        }
        
        if (S_ISDIR(path_stat.st_mode)) {
            read_directory(path, files_array, count);
        } 
        else if (S_ISREG(path_stat.st_mode)) {
            FileData *file = &files_array[*count];
            strncpy(file->path, path, MAX_PATH - 1);
            file->path[MAX_PATH - 1] = '\0';
            file->file_info = path_stat;
            
            FILE *fp = fopen(path, "rb");
            if (fp != NULL) {
                fseek(fp, 0, SEEK_END);
                file->content_size = ftell(fp);
                fseek(fp, 0, SEEK_SET);
                
                file->content = malloc(file->content_size);
                if (file->content != NULL) {
                    fread(file->content, 1, file->content_size, fp);
                }
                fclose(fp);
            }
            
            (*count)++;
        }
    }
    
    closedir(dir);
}

int compare_files(const FileData *file1, const FileData *file2) {
    if (file1->file_info.st_size != file2->file_info.st_size) {
        return 0;
    }
    
    if (file1->content != NULL && file2->content != NULL) {
        return memcmp(file1->content, file2->content, file1->file_info.st_size) == 0;
    }
    
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Ошибка: Используйте: %s <каталог1> <каталог2> <выходной_файл>\n", argv[0]);
        return 1;
    }
    
    printf("Чтение файлов из каталога: %s\n", argv[1]);
    read_directory(argv[1], dir1_files, &dir1_count);
    
    printf("Чтение файлов из каталога: %s\n", argv[2]);
    read_directory(argv[2], dir2_files, &dir2_count);
    
    printf("Найдено файлов: в каталоге1 - %d, в каталоге2 - %d\n", 
           dir1_count, dir2_count);
    
    FILE *output_file = fopen(argv[3], "w");
    if (output_file == NULL) {
        perror("Ошибка открытия выходного файла");
        return 1;
    }
    
    fprintf(output_file, "Отчет о совпадающих файлах\n");
    fprintf(output_file, "Каталог 1: %s\n", argv[1]);
    fprintf(output_file, "Каталог 2: %s\n\n", argv[2]);
    
    printf("\nСовпадающие файлы:\n");
    printf("================================================================================\n");
    int match_count = 0;
    
    for (int i = 0; i < dir1_count; i++) {
        for (int j = 0; j < dir2_count; j++) {
            if (compare_files(&dir1_files[i], &dir2_files[j])) {
                match_count++;
                
                printf("Совпадение %d:\n", match_count);
                printf("  Файл 1: %s\n", dir1_files[i].path);
                printf("  Файл 2: %s\n", dir2_files[j].path);
                printf("  Размер: %ld байт\n", dir1_files[i].file_info.st_size);
                printf("  Индексный дескриптор: %ld (ф1), %ld (ф2)\n", 
                       dir1_files[i].file_info.st_ino, dir2_files[j].file_info.st_ino);
                printf("  Права доступа: %o\n", dir1_files[i].file_info.st_mode & 0777);
                printf("  Дата создания: %ld\n", dir1_files[i].file_info.st_ctime);
                printf("\n");
                
                fprintf(output_file, "Совпадение %d:\n", match_count);
                fprintf(output_file, "  Файл 1: %s\n", dir1_files[i].path);
                fprintf(output_file, "  Файл 2: %s\n", dir2_files[j].path);
                fprintf(output_file, "  Размер: %ld байт\n", dir1_files[i].file_info.st_size);
                fprintf(output_file, "  Индексный дескриптор: %ld (ф1), %ld (ф2)\n", 
                        dir1_files[i].file_info.st_ino, dir2_files[j].file_info.st_ino);
                fprintf(output_file, "  Права доступа: %o\n", dir1_files[i].file_info.st_mode & 0777);
                fprintf(output_file, "  Дата создания: %ld\n", dir1_files[i].file_info.st_ctime);
                fprintf(output_file, "\n");
            }
        }
    }
    
    printf("================================================================================\n");
    printf("Всего найдено совпадений: %d\n", match_count);
    printf("Подробный отчет сохранен в файл: %s\n", argv[3]);
    
    fprintf(output_file, "Итого: найдено %d совпадений\n", match_count);
    
    for (int i = 0; i < dir1_count; i++) {
        if (dir1_files[i].content != NULL) {
            free(dir1_files[i].content);
        }
    }
    
    for (int i = 0; i < dir2_count; i++) {
        if (dir2_files[i].content != NULL) {
            free(dir2_files[i].content);
        }
    }
    
    fclose(output_file);
    return 0;
}