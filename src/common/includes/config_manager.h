/*
 * config_manager.h - Gestor de configuración para GPT Terminal Assistant
 * Este archivo define una interfaz para manejar configuraciones de diferentes módulos
 */

 #ifndef CONFIG_MANAGER_H
 #define CONFIG_MANAGER_H
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <ctype.h>
 
 // Estructura para manejar configuración
 typedef struct {
     char model[50];              // Modelo de GPT a utilizar
     float temperature;           // Temperatura para la generación
     int max_tokens;              // Máximo de tokens a generar
     char api_key_file[256];      // Ruta al archivo de la API key
     char role_file[256];         // Ruta al archivo del rol
     char system_role[50];        // Rol del sistema (system, user, assistant)
     char system_content[2048];   // Contenido del mensaje del sistema
 } GPTConfig;
 
 // Inicializa la configuración con valores predeterminados
 void config_init(GPTConfig *config);
 
 // Carga la configuración desde un archivo
 int config_load_from_file(GPTConfig *config, const char *filename);
 
 // Carga el rol desde un archivo específico
 int config_load_role(GPTConfig *config);
 
 // Lee la clave API desde el archivo configurado
 char* config_get_api_key(const GPTConfig *config);
 
 #endif /* CONFIG_MANAGER_H */