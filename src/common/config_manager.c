#include "includes/config_manager.h"

// Move the function implementations here
void config_init(GPTConfig *config) {
    strcpy(config->model, "gpt-3.5-turbo");
    config->temperature = 0.7;
    config->max_tokens = 1000;
    strcpy(config->api_key_file, "api/config.txt");
    strcpy(config->role_file, "");
    strcpy(config->system_role, "system");
    strcpy(config->system_content, "Eres un asistente útil.");
}

int config_load_from_file(GPTConfig *config, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: No se pudo abrir el archivo de configuración %s\n", filename);
        return 0;
    }

    char line[2048];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0;
        if (line[0] == '#' || line[0] == '\0') {
            continue;
        }

        char key[64], value[2000];
        if (sscanf(line, "%63[^=]=%1999[^\r\n]", key, value) == 2) {
            char *k = key;
            while (*k && isspace((unsigned char)*k)) k++;

            char *v = value;
            while (*v && isspace((unsigned char)*v)) v++;

            char *end = k + strlen(k) - 1;
            while (end > k && isspace((unsigned char)*end)) *end-- = '\0';

            end = v + strlen(v) - 1;
            while (end > v && isspace((unsigned char)*end)) *end-- = '\0';

            if (strcmp(k, "MODEL") == 0) {
                strcpy(config->model, v);
            } else if (strcmp(k, "TEMPERATURE") == 0) {
                config->temperature = atof(v);
            } else if (strcmp(k, "MAX_TOKENS") == 0) {
                config->max_tokens = atoi(v);
            } else if (strcmp(k, "API_KEY_FILE") == 0) {
                strcpy(config->api_key_file, v);
            } else if (strcmp(k, "ROLE_FILE") == 0) {
                strcpy(config->role_file, v);
            } else if (strcmp(k, "SYSTEM_ROLE") == 0) {
                strcpy(config->system_role, v);
            } else if (strcmp(k, "SYSTEM_CONTENT") == 0) {
                strcpy(config->system_content, v);
            }
        }
    }

    fclose(file);
    return 1;
}

int config_load_role(GPTConfig *config) {
    if (strlen(config->role_file) == 0) {
        return 0;
    }

    FILE *file = fopen(config->role_file, "r");
    if (!file) {
        fprintf(stderr, "Error: No se pudo abrir el archivo de rol %s\n", config->role_file);
        return 0;
    }

    char role[50] = {0}, content[2048] = {0};
    if (fgets(role, sizeof(role), file) && fgets(content, sizeof(content), file)) {
        role[strcspn(role, "\r\n")] = 0;
        content[strcspn(content, "\r\n")] = 0;

        strcpy(config->system_role, role);
        strcpy(config->system_content, content);
    }

    fclose(file);
    return 1;
}

char* config_get_api_key(const GPTConfig *config) {
    FILE *file = fopen(config->api_key_file, "r");
    if (!file) {
        fprintf(stderr, "Error: No se pudo abrir el archivo de clave API %s\n", config->api_key_file);
        return NULL;
    }

    char line[256];
    char *api_key = NULL;
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "API_KEY=", 8) == 0) {
            api_key = strdup(line + 8);
            api_key[strcspn(api_key, "\r\n")] = 0;
            break;
        }
    }
    fclose(file);

    return api_key;
}