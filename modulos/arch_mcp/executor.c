#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../common/includes/utils.h"

// Función específica para extraer comandos en modo Arch MCP
char* extract_command_arch_mcp(const char *text) {
    // Usar la función mejorada con parámetro específico para bash
    return extract_command_improved(text, "bash");
}

// Función específica para ejecutar comandos en modo Arch MCP
char* run_command_arch_mcp(const char *cmd) {
    // Usar la función mejorada para ejecutar comandos
    return run_command_improved(cmd);
}
