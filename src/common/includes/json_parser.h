// json_parser.h - Parser JSON simplificado para mcp_client
#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Funciones básicas para parsear respuestas JSON simples del bridge
static char* json_extract_string(const char* json, const char* key) {
    if (!json || !key) return NULL;
    
    char search_pattern[256];
    snprintf(search_pattern, sizeof(search_pattern), "\"%s\":", key);
    
    char* start = strstr(json, search_pattern);
    if (!start) return NULL;
    
    start = strchr(start, ':');
    if (!start) return NULL;
    start++;
    
    while (*start && isspace(*start)) start++;
    
    if (*start != '"') return NULL;
    start++;
    
    char* end = strchr(start, '"');
    if (!end) return NULL;
    
    size_t len = end - start;
    char* result = malloc(len + 1);
    if (!result) return NULL;
    
    strncpy(result, start, len);
    result[len] = '\0';
    
    return result;
}

static int json_extract_bool(const char* json, const char* key) {
    if (!json || !key) return 0;
    
    char search_pattern[256];
    snprintf(search_pattern, sizeof(search_pattern), "\"%s\":", key);
    
    char* start = strstr(json, search_pattern);
    if (!start) return 0;
    
    start = strchr(start, ':');
    if (!start) return 0;
    start++;
    
    while (*start && isspace(*start)) start++;
    
    if (strncmp(start, "true", 4) == 0) return 1;
    if (strncmp(start, "false", 5) == 0) return 0;
    
    return 0;
}

#endif // JSON_PARSER_H
