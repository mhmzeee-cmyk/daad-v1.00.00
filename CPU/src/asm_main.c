/* ═══════════════════════════════════════════════════════════════════════════
   dhad_asm — الدال筲 المستقل للمجمّع
   ═══════════════════════════════════════════════════════════════════════════ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dhad_asm.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: dhad_asm <file.ضasm> [output.bin]\n");
        return 1;
    }
    DhadAsm a;
    memset(&a, 0, sizeof(a));
    int rc = dhad_asm(&a, argv[1]);
    if (rc != 0) {
        fprintf(stderr, "Assembly failed: %d errors\n", a.error_count);
        return 1;
    }
    const char *out = (argc > 2) ? argv[2] : "output.bin";
    FILE *f = fopen(out, "wb");
    if (!f) { perror(out); return 1; }
    fwrite(a.program, 1, a.prog_size, f);
    fclose(f);
    printf("Assembled: %s (%d bytes)\n", argv[1], a.prog_size);
    return 0;
}
