#include "rosewood/remote-control/event_loop.h"

#include <iostream>
#include <vector>
#include <memory>
#include <string>

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "rosewood/core/memory.h"
#include "rosewood/core/logging.h"

#include "rosewood/remote-control/command_dispatch.h"

namespace rosewood { namespace remote_control {
    
    
    class ServerDispatch {
    public:
        ServerDispatch();
        
        ServerDispatch(const ServerDispatch&) = delete;
        ServerDispatch &operator=(const ServerDispatch&) = delete;
        
        ev_io *io() { return &_io; }
        int fd() const { return _fd; }
        bool is_okay() const { return _fd != -1; }
        
        void client_disconnected(Client *client);
        
    private:
        ev_io _io;
        int _fd;
        sockaddr_in _socket;
        int _socket_len;
        std::vector<std::unique_ptr<Client>> _clients;
        
        static void read_ready(EV_P_ ev_io *io, int read_events);
        void read_ready(EV_P_ int read_events);
    };
    
    Client::Client(ServerDispatch *server, int fd) : _fd(fd), _server(server) {
        int flags = fcntl(_fd, F_GETFL);
        if (flags == -1) {
            LOG(ERROR, "Could not get socket file flags");
            _fd = -1;
            return;
        }
        
        flags |= O_NONBLOCK;
        
        if (fcntl(_fd, F_SETFL, flags) == -1) {
            LOG(ERROR, "Could not set socket file flags");
            _fd = -1;
            return;
        }
        
        ev_io_init(&_io, event_arrived, _fd, EV_READ | EV_WRITE);
    }
    
    void Client::enqueue_command(const msgpack::sbuffer &buffer) {
        size_t start = _send_buffer.size();
        _send_buffer.resize(start + buffer.size());
        memcpy(&_send_buffer[start], buffer.data(), buffer.size());
    }
    
    void Client::event_arrived(EV_P_ ev_io *io, int events) {
        auto client = reinterpret_cast<Client*>(io);
        client->event_arrived(EV_A_ events);
    }
    
    void Client::event_arrived(EV_P_ int events) {
        if (events & EV_READ) {
            _unpacker.reserve_buffer(65536);
            
            auto n = recv(_fd, _unpacker.buffer(), _unpacker.buffer_capacity(), 0);
            if (n == -1) {
                if (errno == EAGAIN) {
                    LOG(ERROR, "Could not read from client");
                }
                return;
            }
            else if (n == 0) {
                LOG(ERROR, "Client disconnected");
                ev_io_stop(EV_A_ io());
                close(_fd);
                _server->client_disconnected(this);
                return;
            }
            
            _unpacker.buffer_consumed(n);
            
            msgpack::unpacked obj;
            while (_unpacker.next(&obj)) {
                dispatch_command(*this, obj.get());
            }
        }

        if ((events & EV_WRITE) && !_send_buffer.empty()) {
            auto n = send(_fd, _send_buffer.data(), _send_buffer.size(), 0);
            _send_buffer.erase(_send_buffer.begin(), _send_buffer.begin() + n);
        }
    }
    
    ServerDispatch::ServerDispatch() : _fd(-1) {
        addrinfo hints, *res;
        
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        
        if (getaddrinfo(nullptr, "7768", &hints, &res) != 0) {
            LOG(ERROR, "Could not get address info");
            return;
        }
        
        _socket.sin_addr.s_addr = INADDR_ANY;
        _socket.sin_family = AF_INET;
        _socket.sin_port = htons(7768);
        std::fill_n(_socket.sin_zero, 8, '\0');
        _socket_len = sizeof _socket;
        
        _fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (_fd == -1) {
            LOG(ERROR, "Could not create socket");
            return;
        }
        
        int flags = fcntl(_fd, F_GETFL);
        if (flags == -1) {
            LOG(ERROR, "Could not get socket file flags");
            _fd = -1;
            return;
        }
        
        flags |= O_NONBLOCK;
        
        if (fcntl(_fd, F_SETFL, flags) == -1) {
            LOG(ERROR, "Could not set socket file flags");
            _fd = -1;
            return;
        }
        
        if (bind(_fd, res->ai_addr, res->ai_addrlen) == -1 ) {
            LOG(ERROR, "Could not bind socket");
            _fd = -1;
            return;
        }
        
        if (listen(_fd, 5) == -1) {
            LOG(ERROR, "Could not listen on socket");
            _fd = -1;
            return;
        }

        ev_io_init(&_io, read_ready, _fd, EV_READ);
    }
    
    void ServerDispatch::client_disconnected(Client *client) {
        auto client_it = std::find_if(begin(_clients), end(_clients),
                                      [=](const std::unique_ptr<Client> &c) {
                                          return c.get() == client;
                                      });

        if (client_it != end(_clients)) {
            _clients.erase(client_it);
        }
    }
    
    void ServerDispatch::read_ready(EV_P_ ev_io *io, int read_events) {
        ServerDispatch *server = reinterpret_cast<ServerDispatch*>(io);
        server->read_ready(EV_A_ read_events);
    }
    
    void ServerDispatch::read_ready(EV_P_ int) {
        while (true) {
            int client_fd = accept(_fd, nullptr, nullptr);
            if (client_fd == -1) {
                if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    LOG(ERROR, "accept() failed");
                }
                break;
            }

            LOG(INFO, "Accepted a client");
            
            auto client = make_unique<Client>(this, client_fd);
            ev_io_start(EV_A_ client->io());
            _clients.push_back(std::move(client));
        }
    }
    
    static ServerDispatch *gServer = nullptr;
    
    void init() {
        init_command_dispatch();
        EV_P = ev_default_loop(0);
        
        gServer = new ServerDispatch();
        
        if (!gServer->is_okay()) {
            LOG(WARNING, "No loop inited");
            delete gServer;
            gServer = nullptr;
            return;
        }
        
        ev_io_start(EV_A_ gServer->io());
    }
    
    void tick() {
        EV_P = ev_default_loop(0);
        ev_run(EV_A_ EVRUN_NOWAIT);
    }
    
} }
