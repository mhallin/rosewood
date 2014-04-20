#ifndef __ROSEWOOD_CORE_EVENT_H__
#define __ROSEWOOD_CORE_EVENT_H__

#include <functional>
#include <unordered_map>
#include <tuple>
#include <vector>
#include <algorithm>

namespace std {
    template<typename T1, typename T2> struct hash<tuple<T1, T2>> {
        size_t operator()(const tuple<T1, T2> &t) const {
            return hash<T1>()(get<0>(t)) ^ hash<T2>()(get<1>(t));
        }
    };
}

namespace rosewood { namespace core {

    typedef unsigned int EventTypeCode;

    struct BaseEvent {
    protected:
        static EventTypeCode _type_code_counter;
    };

    template<typename Derived>
    struct Event : public BaseEvent {
        static EventTypeCode type_code() {
            static auto tc = _type_code_counter++;
            return tc;
        }
    };

    template<typename TSender>
    class EventManager {
    public:
        template<typename TEvent, typename TListener, typename THandler>
        void add_listener(TSender sender, TListener *listener, const THandler &handler);

        template<typename TEvent, typename TListener, typename THandler>
        void add_listener(TListener *listener, const THandler &handler);

        template<typename TEvent>
        void send_event(TSender sender, const TEvent &event);

        template<typename TListener>
        void remove_listener(const TListener *listener);

        typedef std::tuple<TSender, EventTypeCode> SenderEventTuple;
        typedef std::function<void(TSender, void*, const BaseEvent*)> GenericHandler;
        typedef std::tuple<void*, GenericHandler> ListenerHandlerTuple;

    private:
        // Mapping from (sender, event) -> [ (listener, handler) ]
        std::unordered_map<SenderEventTuple, std::vector<ListenerHandlerTuple>> _handlers;

        // Mapping from event -> [ (global listener, handler) ]
        std::unordered_map<EventTypeCode, std::vector<ListenerHandlerTuple>> _global_handlers;
    };

    template<typename TSender>
    template<typename TEvent, typename TListener, typename THandler>
    void EventManager<TSender>::add_listener(TSender sender, TListener *listener,
                                             const THandler &handler) {
        typedef std::function<void(TListener*, TSender, const TEvent*)> THandlerFn;

        THandlerFn handler_fn = handler;
        auto gen_handler = [handler_fn](TSender sender, void *listener, const BaseEvent *event) {
            handler_fn(static_cast<TListener*>(listener),
                       static_cast<TSender>(sender),
                       static_cast<const TEvent*>(event));
        };

        auto se_tuple = SenderEventTuple(sender, TEvent::type_code());
        auto lh_tuple = ListenerHandlerTuple(listener, gen_handler);

        _handlers[se_tuple].emplace_back(lh_tuple);
    }

    template<typename TSender>
    template<typename TEvent, typename TListener, typename THandler>
    void EventManager<TSender>::add_listener(TListener *listener,
                                             const THandler &handler) {
        typedef std::function<void(TListener*, TSender, const TEvent*)> THandlerFn;

        THandlerFn handler_fn = handler;
        auto gen_handler = [handler_fn](TSender sender, void *listener, const BaseEvent *event) {
            handler_fn(static_cast<TListener*>(listener),
                       static_cast<TSender>(sender),
                       static_cast<const TEvent*>(event));
        };

        auto lh_tuple = ListenerHandlerTuple(listener, gen_handler);

        _global_handlers[TEvent::type_code()].emplace_back(lh_tuple);
    }

    template<typename TSender>
    template<typename TEvent>
    void EventManager<TSender>::send_event(TSender sender, const TEvent &event) {
        auto se_tuple = SenderEventTuple(sender, TEvent::type_code());
        for (auto &lh_tuple : _handlers[se_tuple]) {
            auto listener = std::get<0>(lh_tuple);
            auto &handler = std::get<1>(lh_tuple);
            handler(sender, listener, &event);
        }

        for (auto &lh_tuple : _global_handlers[TEvent::type_code()]) {
            auto listener = std::get<0>(lh_tuple);
            auto &handler = std::get<1>(lh_tuple);
            handler(sender, listener, &event);
        }
    }

    template<typename TSender>
    template<typename TListener>
    void EventManager<TSender>::remove_listener(const TListener *listener) {
        for (auto &pair : _handlers) {
            auto new_begin = std::remove_if(begin(pair.second), end(pair.second),
                                            [listener](ListenerHandlerTuple &lh_tuple) {
                                                return std::get<0>(lh_tuple) == listener;
                                            });
            pair.second.erase(new_begin, end(pair.second));
        }

        for (auto &pair : _global_handlers) {
            auto new_begin = std::remove_if(begin(pair.second), end(pair.second),
                                            [listener](ListenerHandlerTuple &lh_tuple) {
                                                return std::get<0>(lh_tuple) == listener;
                                            });
            pair.second.erase(new_begin, end(pair.second));
        }
    }

} }

#endif
