#include "mcp_client.h"
#include "src/common/includes/json_parser.h"
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>

MCPClient* mcp_create_client() {
    MCPClient* client = malloc(sizeof(MCPClient));
    if (!client) return NULL;
    
    int to_bridge[2], from_bridge[2];
    
    if (pipe(to_bridge) == -1 || pipe(from_bridge) == -1) {
        free(client);
        return NULL;
    }
    
    pid_t pid = fork();
    if (pid == -1) {
        close(to_bridge[0]); close(to_bridge[1]);
        close(from_bridge[0]); close(from_bridge[1]);
        free(client);
        return NULL;
    }
    
    if (pid == 0) {
        // Proceso hijo - ejecutar el bridge
        close(to_bridge[1]);
        close(from_bridge[0]);
        
        dup2(to_bridge[0], STDIN_FILENO);
        dup2(from_bridge[1], STDOUT_FILENO);
        
        close(to_bridge[0]);
        close(from_bridge[1]);
        
        // Ejecutar el bridge nativo desde out/
        execl("./out/MCPBridge_native", "MCPBridge_native", NULL);
        exit(1);
    }
    
    // Proceso padre
    close(to_bridge[0]);
    close(from_bridge[1]);
    
    client->bridge_in = fdopen(to_bridge[1], "w");
    client->bridge_out = fdopen(from_bridge[0], "r");
    client->bridge_pid = pid;
    
    if (!client->bridge_in || !client->bridge_out) {
        mcp_cleanup(client);
        return NULL;
    }
    
    return client;
}

void mcp_cleanup(MCPClient* client) {
    if (!client) return;
    
    if (client->bridge_in) {
        fprintf(client->bridge_in, "EXIT\n");
        fflush(client->bridge_in);
        fclose(client->bridge_in);
    }
    
    if (client->bridge_out) {
        fclose(client->bridge_out);
    }
    
    if (client->bridge_pid > 0) {
        kill(client->bridge_pid, SIGTERM);
        waitpid(client->bridge_pid, NULL, 0);
    }
    
    free(client);
}

MCPResponse* mcp_send_command(MCPClient* client, const char* action, const char* data) {
    if (!client || !action) return NULL;
    
    // Construir JSON para el comando
    fprintf(client->bridge_in, "{\"Action\":\"%s\"", action);
    if (data) {
        fprintf(client->bridge_in, ",\"Data\":\"");
        for (const char* p = data; *p; p++) {
            if (*p == '"' || *p == '\\') {
                fputc('\\', client->bridge_in);
            }
            fputc(*p, client->bridge_in);
        }
        fprintf(client->bridge_in, "\"");
    }
    fprintf(client->bridge_in, "}\n");
    fflush(client->bridge_in);
    
    // Leer respuesta
    char buffer[8192];
    if (!fgets(buffer, sizeof(buffer), client->bridge_out)) {
        return NULL;
    }
    
    // Crear respuesta usando nuestro parser simple
    MCPResponse* response = calloc(1, sizeof(MCPResponse));
    if (!response) return NULL;
    
    // Usar nuestras funciones simples de json_parser.h
    response->success = json_extract_bool(buffer, "Success");
    
    char* result = json_extract_string(buffer, "Result");
    if (result) {
        response->result = result;
    }
    
    char* error = json_extract_string(buffer, "Error");
    if (error) {
        response->error = error;
    }
    
    return response;
}

MCPResponse* mcp_execute_command(MCPClient* client, const char* command) {
    return mcp_send_command(client, "execute_command", command);
}

MCPResponse* mcp_analyze_text(MCPClient* client, const char* text) {
    return mcp_send_command(client, "analyze_text", text);
}

MCPResponse* mcp_get_system_info(MCPClient* client) {
    return mcp_send_command(client, "get_system_info", NULL);
}

MCPResponse* mcp_arch_diagnostics(MCPClient* client) {
    return mcp_send_command(client, "arch_diagnostics", NULL);
}

void mcp_free_response(MCPResponse* response) {
    if (!response) return;
    
    if (response->result) free(response->result);
    if (response->error) free(response->error);
    free(response);
}

int is_user_command(const char* text) {
    if (!text) return 0;
    
    // Saltar espacios iniciales
    while (isspace(*text)) text++;
    
    // Lista de comandos comunes
    const char* commands[] = {
        "ls", "dir", "pwd", "cd", "cat", "grep", "find", "ps", "top", 
        "df", "du", "free", "uname", "which", "whereis", "locate",
        "pacman", "yay", "makepkg", "systemctl", "journalctl",
        "sudo", "su", "chmod", "chown", "mkdir", "rmdir", "rm",
        "cp", "mv", "ln", "tar", "gzip", "unzip", "wget", "curl",
        "lsblk", "fdisk", "cfdisk", "mount", "umount",
        NULL
    };
    
    for (int i = 0; commands[i]; i++) {
        size_t len = strlen(commands[i]);
        if (strncmp(text, commands[i], len) == 0) {
            if (text[len] == '\0' || isspace(text[len]) || text[len] == '-') {
                return 1;
            }
        }
    }
    
    return 0;
}
