#include <sys/select.h>

namespace CMacroWrapper
{
void FdSet(int fd, fd_set * set);

bool FdIsSet(int fd, fd_set * set);
}
