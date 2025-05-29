#include <stdio.h>
#include <stdlib.h>

void append_to_context(const char* cmd, const char* output) {
    FILE *f = fopen("context.txt", "a");
    fprintf(f, "user\t%s\n", cmd);
    fprintf(f, "assistant\t%s\n", output);
    fclose(f);
}

void load_context() {
    // Verificar si context.txt existe, si no, crearlo
    FILE *f = fopen("context.txt", "r");
    if (!f) {
        // El archivo no existe, crearlo
        f = fopen("context.txt", "w");
        if (f) fclose(f);
    } else {
        fclose(f);
    }
}
