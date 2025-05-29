#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "api/openai.h"
#include "common/includes/utils.h"
#include "common/includes/context.h"
#include "common/includes/config_manager.h"
#include "mcp_client.h"

// Definiciones específicas para cada módulo
#ifdef MODO_ARCH_MCP
#include "modulos/arch_mcp/executor.h"
#define MODULE_NAME "🚀 Asistente Arch Linux MCP"
#define CONFIG_FILE "modulos/arch_mcp/config.ini"
#define extract_command extract_command_arch_mcp
#define run_command run_command_arch_mcp
#endif

// Definición predeterminada
#ifndef CONFIG_FILE
#define CONFIG_FILE "modulos/arch_mcp/config.ini"
#endif

#ifndef MODULE_NAME
#define MODULE_NAME "🚀 Asistente GPT con MCP"
#endif

#ifndef extract_command
#define extract_command extract_command_improved
#endif

#ifndef run_command
#define run_command run_command_improved
#endif

// Función para mostrar ayuda con comandos disponibles
void show_help() {
    printf("\n=== 🔧 Comandos disponibles ===\n");
    printf("• Comandos directos: ls, pwd, cd, df, free, ps, etc.\n");
    printf("• Comandos Arch: pacman, systemctl, journalctl\n");
    printf("• /help - Mostrar esta ayuda\n");
    printf("• /clear - Limpiar contexto\n");
    printf("• /status - Estado del sistema\n");
    printf("• /diag - Diagnóstico completo Arch Linux\n");
    printf("• /mcp - Información del bridge MCP\n");
    printf("• salir/exit/quit - Terminar\n");
    printf("• O simplemente pregunta algo...\n\n");
}

// Función para procesar comandos especiales
int process_special_command(const char* input, MCPClient* mcp_client) {
    if (strcmp(input, "/help") == 0) {
        show_help();
        return 1;
    }
    
    if (strcmp(input, "/clear") == 0) {
        system("rm -f context.txt");
        load_context();
        printf("✅ Contexto limpiado.\n\n");
        return 1;
    }
    
    if (strcmp(input, "/status") == 0) {
        if (mcp_client) {
            printf("🔍 Obteniendo información del sistema...\n");
            MCPResponse* response = mcp_get_system_info(mcp_client);
            if (response && response->success && response->result) {
                printf("=== 📊 Estado del Sistema ===\n%s\n\n", response->result);
            } else {
                printf("❌ Error al obtener información del sistema.\n\n");
            }
            mcp_free_response(response);
        } else {
            printf("⚠️  MCP no disponible. Información básica:\n");
            system("uname -a && df -h . && free -h");
        }
        return 1;
    }
    
    if (strcmp(input, "/diag") == 0) {
        if (mcp_client) {
            printf("🔍 Ejecutando diagnóstico completo de Arch Linux...\n");
            MCPResponse* response = mcp_arch_diagnostics(mcp_client);
            if (response && response->success && response->result) {
                printf("=== 🩺 Diagnóstico Arch Linux ===\n%s\n", response->result);
            } else {
                printf("❌ Error en el diagnóstico.\n");
            }
            mcp_free_response(response);
        } else {
            printf("⚠️  MCP no disponible. Diagnóstico básico:\n");
            system("lsblk && df -h && free -h");
        }
        return 1;
    }
    
    if (strcmp(input, "/mcp") == 0) {
        if (mcp_client) {
            printf("✅ Bridge MCP: Conectado y funcional\n");
            printf("📡 Funciones disponibles:\n");
            printf("  - Ejecución de comandos del sistema\n");
            printf("  - Análisis de texto para detectar comandos\n");
            printf("  - Diagnósticos de Arch Linux\n");
            printf("  - Información del sistema\n\n");
        } else {
            printf("❌ Bridge MCP: No disponible\n");
            printf("   El sistema funciona en modo básico\n\n");
        }
        return 1;
    }
    
    return 0; // No es un comando especial
}

// Función mejorada para manejar comandos del usuario
void handle_user_command(const char* command, MCPClient* mcp_client) {
    printf("\n🔧 Ejecutando: %s\n", command);
    printf("--- Resultado ---\n");
    
    if (mcp_client) {
        // Usar MCP para ejecutar el comando
        MCPResponse* response = mcp_execute_command(mcp_client, command);
        if (response) {
            if (response->success && response->result) {
                printf("%s\n", response->result);
            } else if (response->error) {
                printf("❌ Error: %s\n", response->error);
            } else {
                printf("❌ Error desconocido en la ejecución\n");
            }
            mcp_free_response(response);
        } else {
            printf("❌ Error: No se pudo comunicar con el bridge MCP\n");
        }
    } else {
        // Fallback al método original
        printf("⚠️  Usando modo básico (sin MCP):\n");
        char* result = run_command(command);
        printf("%s\n", result);
        free(result);
    }
    
    printf("--- Fin ---\n\n");
}

// Función principal
int main(int __attribute__((unused)) argc, char __attribute__((unused)) *argv[]) {
    // Inicializar el contexto
    load_context();
    
    // Crear cliente MCP
    printf("🔌 Inicializando cliente MCP...\n");
    MCPClient* mcp_client = mcp_create_client();
    if (!mcp_client) {
        printf("⚠️  No se pudo inicializar MCP. Continuando en modo básico.\n");
        printf("   (Asegúrate de que MCPBridge esté en el directorio actual)\n");
    } else {
        printf("✅ Cliente MCP inicializado correctamente.\n");
    }
    
    printf("\n=== %s ===\n", MODULE_NAME);
    printf("💡 Escribe comandos directos (ls, pwd, etc.) o pregunta algo.\n");
    printf("   Usa /help para ver todos los comandos disponibles.\n\n");
    
    char input[2048];
    
    while (1) {
        printf("🤖 > ");
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }
        
        // Eliminar el salto de línea final
        input[strcspn(input, "\n")] = 0;
        
        // Verificar si se debe salir
        if (strcmp(input, "salir") == 0 || 
            strcmp(input, "exit") == 0 || 
            strcmp(input, "quit") == 0) {
            break;
        }
        
        // Si está vacío, continuar
        if (strlen(input) == 0) {
            continue;
        }
        
        // Procesar comandos especiales
        if (process_special_command(input, mcp_client)) {
            continue;
        }
        
        // Detectar si es un comando del usuario
        if (is_user_command(input)) {
            handle_user_command(input, mcp_client);
            
            // Agregar al contexto para que GPT sepa qué se ejecutó
            FILE *ctx = fopen("context.txt", "a");
            if (ctx) {
                fprintf(ctx, "system\t✅ Comando ejecutado: %s\n", input);
                fclose(ctx);
            }
            
            continue;
        }
        
        // Si no es un comando directo, enviar a GPT
        printf("🤖 Procesando con GPT...\n");
        char* respuesta = send_prompt(input, CONFIG_FILE);
        
        // Mostrar la respuesta
        printf("\n--- 💬 Respuesta GPT ---\n%s\n\n", respuesta);
        
        // Verificar si GPT sugiere ejecutar comandos
        char* comando_sugerido = extract_command(respuesta);
        if (comando_sugerido) {
            printf("💡 GPT sugiere ejecutar: %s\n", comando_sugerido);
            printf("¿Deseas ejecutarlo? [s/N]: ");
            
            char confirmar[10] = {0};
            fgets(confirmar, sizeof(confirmar), stdin);
            confirmar[strcspn(confirmar, "\n")] = 0;
            
            if (confirmar[0] == 's' || confirmar[0] == 'S') {
                handle_user_command(comando_sugerido, mcp_client);
                
                // Agregar resultado al contexto
                FILE *ctx = fopen("context.txt", "a");
                if (ctx) {
                    fprintf(ctx, "system\t💡 GPT sugirió y se ejecutó: %s\n", comando_sugerido);
                    fclose(ctx);
                }
            }
            
            free(comando_sugerido);
        }
        
        free(respuesta);
    }
    
    // Limpiar
    if (mcp_client) {
        mcp_cleanup(mcp_client);
        printf("🔌 Cliente MCP desconectado.\n");
    }
    
    printf("¡Hasta pronto! 👋\n");
    return 0;
}