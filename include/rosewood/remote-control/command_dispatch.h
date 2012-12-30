#ifndef __ROSEWOOD_REMOTE_DEVICE_COMMAND_DISPATCH_H__
#define __ROSEWOOD_REMOTE_DEVICE_COMMAND_DISPATCH_H__

#include "rosewood/core/clang_msgpack.h"

namespace rosewood { namespace remote_control {
    
    class Client;
    
    void init_command_dispatch();
    void dispatch_command(Client &client, msgpack::object object);
    
} }

#endif
