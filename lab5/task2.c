#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Ошибка: Используйте: %s <имя_файла>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "w");
    if (file == NULL) {
        perror("Ошибка открытия файла");
        return 1;
    }
    
    printf("Введите текст (для выхода нажмите Ctrl+D):\n");
    
    int ch;
    int has_error = 0;
    
    while ((ch = getchar()) != EOF) {
        if (fputc(ch, file) == EOF) {
            perror("Ошибка записи в файл");
            has_error = 1;
            break;
        }
    }

    if (fclose(file) != 0) {
        perror("Ошибка закрытия файла");
        has_error = 1;
    }
    
    if (!has_error) {
        printf("Данные успешно записаны в файл '%s'\n", argv[1]);
    }
    
    return has_error;
}