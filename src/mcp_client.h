#ifndef MCP_CLIENT_H
#define MCP_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

typedef struct {
    FILE* bridge_in;
    FILE* bridge_out;
    pid_t bridge_pid;
} MCPClient;

typedef struct {
    int success;
    char* result;
    char* error;
} MCPResponse;

// Funciones del cliente MCP
MCPClient* mcp_create_client();
void mcp_cleanup(MCPClient* client);

MCPResponse* mcp_send_command(MCPClient* client, const char* action, const char* data);
MCPResponse* mcp_execute_command(MCPClient* client, const char* command);
MCPResponse* mcp_analyze_text(MCPClient* client, const char* text);
MCPResponse* mcp_get_system_info(MCPClient* client);
MCPResponse* mcp_arch_diagnostics(MCPClient* client);

void mcp_free_response(MCPResponse* response);

// Función para detectar si el texto del usuario es un comando
int is_user_command(const char* text);

#endif