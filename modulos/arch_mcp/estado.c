#include "estado.h"
#include <string.h>
#include <stdio.h>

static EstadoInstalacion estado;

void inicializar_estado() {
    memset(&estado, 0, sizeof(EstadoInstalacion));
}

void marcar_completado(const char* paso) {
    if (strcmp(paso, "particiones") == 0) estado.particiones_hechas = 1;
    else if (strcmp(paso, "montaje") == 0) estado.montaje_realizado = 1;
    else if (strcmp(paso, "idioma") == 0) estado.idioma_configurado = 1;
    else if (strcmp(paso, "zona") == 0) estado.zona_horaria_configurada = 1;
    else if (strcmp(paso, "base") == 0) estado.sistema_base_instalado = 1;
    else if (strcmp(paso, "grub") == 0) estado.grub_instalado = 1;
}

int consultar_estado(const char* paso) {
    if (strcmp(paso, "particiones") == 0) return estado.particiones_hechas;
    else if (strcmp(paso, "montaje") == 0) return estado.montaje_realizado;
    else if (strcmp(paso, "idioma") == 0) return estado.idioma_configurado;
    else if (strcmp(paso, "zona") == 0) return estado.zona_horaria_configurada;
    else if (strcmp(paso, "base") == 0) return estado.sistema_base_instalado;
    else if (strcmp(paso, "grub") == 0) return estado.grub_instalado;
    return 0;
}
