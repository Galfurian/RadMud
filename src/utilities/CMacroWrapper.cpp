#include "utilities/CMacroWrapper.hpp"

#pragma GCC diagnostic ignored "-Wsign-conversion"

void CMacroWrapper::FdSet(int fd, fd_set * set)
{
    FD_SET(fd, set);
}

bool CMacroWrapper::FdIsSet(int fd, fd_set * set)
{
    return FD_ISSET(fd, set);
}
