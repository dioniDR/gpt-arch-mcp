#include "diagnostico.h"
#include <stdio.h>
#include <stdlib.h>

char* diagnosticar_estado_general() {
    system("echo '[Diagnóstico del sistema]'");
    system("lsblk -f");
    system("findmnt /mnt");
    system("cat /etc/locale.conf 2>/dev/null || echo 'Idioma no configurado'");
    system("timedatectl");
    return "Diagnóstico ejecutado. Verifica los resultados.";
}
