#ifndef ESTADO_ARCH_H
#define ESTADO_ARCH_H

typedef struct {
    int particiones_hechas;
    int montaje_realizado;
    int idioma_configurado;
    int zona_horaria_configurada;
    int sistema_base_instalado;
    int grub_instalado;
} EstadoInstalacion;

void inicializar_estado();
void marcar_completado(const char* paso);
int consultar_estado(const char* paso);

#endif // ESTADO_ARCH_H
