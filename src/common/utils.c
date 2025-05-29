#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include "includes/utils.h"

// Función para eliminar espacios en blanco al inicio y final de una cadena
char* trim(char* str) {
    if (!str) return NULL;
    
    // Eliminar espacios al final
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }
    *(end + 1) = '\0';
    
    // Eliminar espacios al inicio
    char* start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }
    
    // Si hubo espacios al inicio, mover la cadena
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
    
    return str;
}

// Función mejorada para extraer comandos bash que maneja múltiples formatos
char* extract_command_improved(const char *text, const char *language) {
    if (!text || !language) return NULL;
    
    // Preparar los marcadores a buscar
    char start_marker[20];
    snprintf(start_marker, sizeof(start_marker), "```%s", language);
    
    // Buscar cualquier formato de bloque de código
    const char *patterns[] = {
        start_marker,      // ```bash
        "```shell",        // ```shell
        "```sh",           // ```sh
        "```console",      // ```console
        "```terminal",     // ```terminal
        "```",             // Bloque de código sin especificar
        "$",               // Línea que comienza con $
        NULL
    };
    
    char *result = NULL;
    const char *start = NULL;
    const char *end = NULL;
    int pattern_index = 0;
    
    // Intentar cada patrón
    while (patterns[pattern_index] != NULL && !start) {
        start = strstr(text, patterns[pattern_index]);
        
        // Si encontramos un patrón de inicio de bloque de código
        if (start && (strncmp(patterns[pattern_index], "```", 3) == 0)) {
            // Saltar al final del marcador
            start = strchr(start, '\n');
            if (start) start++;
            
            // Buscar el cierre del bloque
            end = strstr(start, "```");
            
            // Si no encontramos cierre, buscar hasta el final del texto
            if (!end) end = text + strlen(text);
        }
        // Si encontramos un patrón de línea con $
        else if (start && (strcmp(patterns[pattern_index], "$") == 0)) {
            // Saltar el $ y espacios
            start++;
            while (*start && isspace((unsigned char)*start)) start++;
            
            // Buscar el final de la línea
            end = strchr(start, '\n');
            if (!end) end = text + strlen(text);
        }
        
        pattern_index++;
    }
    
    // Si encontramos un comando
    if (start && start < end) {
        size_t len = end - start;
        result = malloc(len + 1);
        if (result) {
            strncpy(result, start, len);
            result[len] = '\0';
            
            // Limpiar espacios en blanco
            trim(result);
            
            // Si es vacío, liberar y devolver NULL
            if (strlen(result) == 0) {
                free(result);
                result = NULL;
            }
        }
    }
    
    return result;
}

// Versión mejorada de run_command que registra salida estándar y errores
char* run_command_improved(const char *cmd) {
    if (!cmd) return strdup("Error: Comando vacío");
    
    // Crear un comando que capture tanto stdout como stderr
    char actual_cmd[4096];
    snprintf(actual_cmd, sizeof(actual_cmd), "{ %s; } 2>&1", cmd);
    
    FILE *fp = popen(actual_cmd, "r");
    if (!fp) return strdup("Error: No se pudo ejecutar el comando");
    
    // Leer la salida
    char *output = malloc(8192);
    if (!output) {
        pclose(fp);
        return strdup("Error: No se pudo asignar memoria para la salida");
    }
    
    output[0] = '\0';
    char buffer[1024];
    size_t total_read = 0;
    const size_t max_size = 8192 - 1; // Reservar espacio para el terminador nulo
    
    while (fgets(buffer, sizeof(buffer), fp) && total_read < max_size) {
        size_t len = strlen(buffer);
        if (total_read + len > max_size) {
            len = max_size - total_read;
            buffer[len] = '\0';
        }
        
        strcat(output, buffer);
        total_read += len;
    }
    
    int status = pclose(fp);
    
    // Añadir información sobre el estado de salida si hubo error
    if (status != 0) {
        char status_info[100];
        snprintf(status_info, sizeof(status_info), 
                 "\n[Código de salida: %d]", WEXITSTATUS(status));
        
        if (total_read + strlen(status_info) < max_size) {
            strcat(output, status_info);
        }
    }
    
    return output;
}

char* read_api_key() {
    FILE *config = fopen("config.txt", "r");
    if (!config) {
        fprintf(stderr, "Error: No se pudo abrir el archivo config.txt\n");
        return NULL;
    }

    char line[256];
    while (fgets(line, sizeof(line), config)) {
        // Buscar la línea que comienza con "API_KEY="
        if (strncmp(line, "API_KEY=", 8) == 0) {
            char *api_key = strdup(line + 8); // Copiar el valor después de "API_KEY="
            api_key[strcspn(api_key, "\r\n")] = 0; // Eliminar saltos de línea
            fclose(config);
            return api_key;
        }
    }

    fclose(config);
    fprintf(stderr, "Error: No se encontró la clave API en config.txt\n");
    return NULL;
}