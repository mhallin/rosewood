#ifndef __ROSEWOOD_CORE_STATS_H__
#define __ROSEWOOD_CORE_STATS_H__

namespace rosewood { namespace core {

    namespace stats {

        template<typename T>
        class Counter {
        public:
            T read() const { return _count; }
            void increment() { _count += T(1); }
            void increment(T amount) { _count += amount; }
            void reset() { _count = T(0); }

        private:
            T _count;
        };

        extern Counter<int> draw_calls;
        extern Counter<int> triangle_count;
        extern Counter<int> shader_change_count;
    }

} }

#endif
