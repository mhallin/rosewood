#ifndef __ROSEWOOD_ENGINE_ANIMATABLE_H__
#define __ROSEWOOD_ENGINE_ANIMATABLE_H__

#include <functional>

namespace rosewood { namespace utils {
    
    void tick_all_animations();
    
    class BaseAnimation {
    public:
        BaseAnimation();
        virtual ~BaseAnimation();
        
        virtual void tick() = 0;
    };
    
    template<typename TValue>
    class Animatable : public BaseAnimation {
    public:
        Animatable(TValue const_value);
        Animatable(TValue start_value, TValue by_or_target_value,
                   uint64_t start_time, uint64_t duration, bool use_target_value,
                   std::function<float(float)> easing_function);
        virtual ~Animatable();
        
        operator TValue() const;
        
        bool is_alive() const;
        void delay(float seconds);
        
        virtual void tick() override;
        
    private:
        TValue _start_value;
        TValue _current_value;
        union {
            TValue by_value;
            TValue target_value;
        } _animation;
        
        uint64_t _start_time;
        uint64_t _duration;
        bool _use_target_value;
        std::function<float(float)> _easing_function;
    };
    
    template<typename TValue>
    Animatable<TValue> make_continuous_animatable(TValue start_value, TValue by_value,
                                                  std::function<float(float)> easing_function = nullptr,
                                                  uint64_t start_time = 0);
    
    template<typename TValue>
    Animatable<TValue> make_durational_animatable(TValue start_value, TValue target_value,
                                                  float duration,
                                                  std::function<float(float)> easing_function = nullptr,
                                                  uint64_t start_time = 0);

} }

#endif
