/* mingw_compat.c — Windows (msvcrt) compatibility shims for the Demo build ONLY.
 * Repo sources are NOT modified; this file is compiled only into the
 * cross-built Windows demo binaries in Demo/windows-bin/.
 *
 * open_memstream (POSIX) does not exist on msvcrt. The single caller
 * (dhad_backend.c pass-1 sizing) writes to the stream, closes it and frees
 * the buffer WITHOUT EVER READING its content (only side-effect counters
 * are used). A tmpfile-backed stream is therefore behavior-identical here.
 */
#include <stdio.h>
#include <stddef.h>

FILE *open_memstream(char **bufp, size_t *sizep) {
    if (bufp) *bufp = NULL;   /* free(NULL) is safe for the caller */
    if (sizep) *sizep = 0;
    return tmpfile();
}
