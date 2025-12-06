#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Необходимо указать два файла: исходный и целевой\n");
        printf("Пример: %s файл1.txt файл2.txt\n", argv[0]);
        return 1;
    }

    FILE *f_input = fopen(argv[1], "rb");
    if (f_input == NULL) {
        printf("Не удалось открыть файл для чтения: %s\n", argv[1]);
        return 1;
    }

    FILE *f_output = fopen(argv[2], "wb");
    if (f_output == NULL) {
        printf("Не удалось открыть файл для записи: %s\n", argv[2]);
        fclose(f_input);
        return 1;
    }

    int symbol;
    long counter = 0;
    
    while ((symbol = fgetc(f_input)) != EOF) {
        if (fputc(symbol, f_output) == EOF) {
            printf("Ошибка при записи в файл\n");
            fclose(f_input);
            fclose(f_output);
            return 1;
        }
        counter++;
    }

    if (ferror(f_input)) {
        printf("Произошла ошибка при чтении файла\n");
        fclose(f_input);
        fclose(f_output);
        return 1;
    }

    fclose(f_input);
    
    if (fclose(f_output) != 0) {
        printf("Ошибка при закрытии файла\n");
        return 1;
    }

    struct stat file_stats;
    if (stat(argv[1], &file_stats) == 0) {
        if (chmod(argv[2], file_stats.st_mode) != 0) {
            printf("Не удалось скопировать права доступа\n");
        } else {
            printf("Права доступа успешно скопированы\n");
        }
    } else {
        printf("Не удалось получить информацию об исходном файле\n");
    }

    printf("Файл успешно скопирован\n");
    printf("Исходный файл: %s\n", argv[1]);
    printf("Целевой файл: %s\n", argv[2]);
    printf("Скопировано символов: %ld\n", counter);

    return 0;
}