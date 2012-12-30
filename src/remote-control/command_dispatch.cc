#include "rosewood/remote-control/command_dispatch.h"

#include <sys/time.h>

#include <string>
#include <iostream>
#include <unordered_map>

#include "rosewood/core/resource_manager.h"
#include "rosewood/core/memory.h"

#include "rosewood/remote-control/event_loop.h"

namespace rosewood { namespace remote_control {
    
    class StringResourceLoader : public core::IResourceLoader {
    public:
        struct Entry {
            std::string file_contents;
            struct timespec mtime;
        };
        
        virtual ~StringResourceLoader() { }
        
        virtual core::FileInfo find_file(std::string path) const override {
            core::FileInfo i;

            if (_entries.find(path) == _entries.end()) {
                i.exists = false;
                return i;
            }
            
            auto &entry = _entries.at(path);
            i.exists = true;
            i.mtime = entry.mtime;
            
            return i;
        }
        
        virtual std::string read_file(std::string path) const override {
            return _entries.at(path).file_contents;
        }
        
        void update_file(const std::string &path, const std::string &file_contents) {
            Entry e;
            struct timeval tv;
            gettimeofday(&tv, nullptr);
            
            e.mtime.tv_sec = tv.tv_sec;
            e.mtime.tv_nsec = tv.tv_usec * 1000;
            e.file_contents = file_contents;
            
            _entries[path] = e;

            core::notify_file_changed(path);
        }
        
    private:
        std::unordered_map<std::string, Entry> _entries;
    };
    
    static StringResourceLoader *gStringResourceLoader = nullptr;
    static std::unordered_map<std::string, std::function<void(Client&, const msgpack::object_array&)>> gCommands;
    
    static void reload_asset_command(const Client&, const msgpack::object_array &args) {
        if (args.size != 3) {
            std::cerr << "Wrong number of arguments, reload_asset expects 2\n";
            return;
        }
        
        gStringResourceLoader->update_file(args.ptr[1].as<std::string>(),
                                           args.ptr[2].as<std::string>());
    }
    
    static void get_asset_command(Client &client, const msgpack::object_array &args) {
        if (args.size != 2) {
            std::cerr << "Wrong number of arguments, get_asset expects 1\n";
            return;
        }
        
        auto asset = core::get_resource(args.ptr[1].as<std::string>());
        std::vector<std::string> result { "get_asset", args.ptr[1].as<std::string>(), asset->str() };
        
        msgpack::sbuffer sbuf;
        msgpack::pack(sbuf, result);
        
        client.enqueue_command(sbuf);
    }
    
    void init_command_dispatch() {
        auto loader = make_unique<StringResourceLoader>();
        gStringResourceLoader = loader.get();
        core::add_resource_loader(std::move(loader));
        
        gCommands["reload_asset"] = reload_asset_command;
        gCommands["get_asset"] = get_asset_command;
    }
    
    void dispatch_command(Client &client, msgpack::object obj) {
        if (obj.type != MSGPACK_OBJECT_ARRAY) {
            std::cerr << "Command object is not an array\n";
            return;
        }

        auto array = obj.via.array;
        if (array.size == 0) {
            std::cerr << "Empty command array\n";
            return;
        }
        
        auto cmd_obj = array.ptr[0];
        if (cmd_obj.type != MSGPACK_OBJECT_RAW) {
            std::cerr << "Invalid command name object\n";
            return;
        }

        auto cmd_name = array.ptr[0].as<std::string>();
        if (gCommands.find(cmd_name) == end(gCommands)) {
            std::cerr << "Unknown command: " << cmd_name << "\n";
        }
        else {
            gCommands.at(cmd_name)(client, array);
        }
    }
    
} }
