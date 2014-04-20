#ifndef __ROSEWOOD_REMOTE_DEVICE_EVENT_LOOP_H__
#define __ROSEWOOD_REMOTE_DEVICE_EVENT_LOOP_H__

#ifndef __OBJC__

#include "rosewood/core/clang_msgpack.h"

#include <ev.h>
#include <vector>

#endif

namespace rosewood { namespace remote_control {

    void init();
    void tick();

#ifndef __OBJC__
    class ServerDispatch;
    
    class Client {
    public:
        Client(ServerDispatch *server, int fd);
        
        void enqueue_command(const msgpack::sbuffer &command);
        
        Client(const Client&) = delete;
        Client &operator=(const Client&) = delete;
        
        ev_io *io() { return &_io; }
        
    private:
        ev_io _io;
        int _fd;
        ServerDispatch *_server;
        msgpack::unpacker _unpacker;
        std::vector<char> _send_buffer;
        
        static void event_arrived(EV_P_ ev_io *io, int events);
        void event_arrived(EV_P_ int events);
    };
#endif

} }

#endif
