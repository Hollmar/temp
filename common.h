#ifndef _COMMON_H_
#define _COMMON_H_

#include <pb.h>

pb_ostream_t pb_ostream_from_socket(int fd);
pb_istream_t pb_istream_from_socket(int fd);

#endif
