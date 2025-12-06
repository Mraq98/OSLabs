#include <stdio.h>
#include <stdlib.h>

void wait_for_key() {
    printf("\nНажмите любую клавишу для продолжения...\n");
    getchar();
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Ошибка: Используйте: %s <имя_файла> <N>\n", argv[0]);
        fprintf(stderr, "N = 0 - вывод без пауз\n");
        fprintf(stderr, "N > 0 - вывод по N строк с паузой\n");
        return 1;
    }
    
    char *filename = argv[1];
    int N = atoi(argv[2]);
    
    if (N < 0) {
        fprintf(stderr, "Ошибка: N не может быть отрицательным\n");
        return 1;
    }
    
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Ошибка открытия файла");
        return 1;
    }
    
    printf("Содержимое файла '%s':\n", filename);
    printf("========================================\n");
    
    int ch;
    int line_count = 0;
    
    while ((ch = fgetc(fp)) != EOF) {
        putchar(ch);
        
        if (ch == '\n') {
            line_count++;
        }
        
        if (N > 0 && line_count == N) {
            wait_for_key();
            line_count = 0;
        }
    }
    
    if (ferror(fp)) {
        perror("Ошибка чтения файла");
        fclose(fp);
        return 1;
    }
    
    if (fclose(fp) == EOF) {
        perror("Ошибка закрытия файла");
        return 1;
    }
    
    printf("\n========================================\n");
    printf("Вывод файла завершен\n");
    
    return 0;
}