#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

// Función para eliminar espacios en blanco al inicio y final de una cadena
char* trim(char* str);

// Función mejorada para extraer comandos bash que maneja múltiples formatos
char* extract_command_improved(const char *text, const char *language);

// Función mejorada para ejecutar comandos
char* run_command_improved(const char *cmd);

// Función para leer la clave API desde config.txt
char* read_api_key();

#endif /* COMMON_UTILS_H */