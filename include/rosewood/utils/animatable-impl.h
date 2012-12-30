#ifndef __ROSEWOOD_ENGINE_ANIMATABLE_IMPL_H__
#define __ROSEWOOD_ENGINE_ANIMATABLE_IMPL_H__

#include "animatable.h"
#include "time.h"

#include <iostream>

namespace rosewood { namespace utils {
    
    template<typename TValue>
    Animatable<TValue>::Animatable(TValue const_value)
    : _start_value(const_value), _current_value(const_value)
    , _start_time(UINT64_MAX), _duration(0)
    , _use_target_value(false) { }
    
    template<typename TValue>
    Animatable<TValue>::Animatable(TValue start_value, TValue by_or_target_value,
                                   uint64_t start_time, uint64_t duration,
                                   bool use_target_value, std::function<float(float)> easing_function)
    : _start_value(start_value), _current_value(start_value)
    , _start_time(start_time), _duration(duration)
    , _use_target_value(use_target_value), _easing_function(easing_function)
    {
        if (_use_target_value) {
            _animation.target_value = by_or_target_value;
        }
        else {
            _animation.by_value = by_or_target_value;
        }
    }
    
    template<typename TValue>
    Animatable<TValue>::~Animatable() { }
    
    template<typename TValue>
    Animatable<TValue>::operator TValue() const {
        return _current_value;
    }
    
    template<typename TValue>
    bool Animatable<TValue>::is_alive() const {
        auto now = frame_usec_time();

        if (now < _start_time) return false;
        if (_use_target_value && now > _start_time + _duration) return false;
        return true;
    }
    
    template<typename TValue>
    void Animatable<TValue>::delay(float seconds) {
        if (!_start_time) _start_time = current_usec_time();
        
        _start_time += uint64_t(seconds * 1000000);
    }
    
    template<typename TValue>
    void Animatable<TValue>::tick() {
        auto now = frame_usec_time();
        
        if (!_start_time) {
            _start_time = now;
        }
        
        if (_start_time > now) return;

        if (_use_target_value) {
            auto t_sec = std::max(0.0f, std::min(1.0f, (now - _start_time) / (float)_duration));
            if (_easing_function) t_sec = _easing_function(t_sec);
            _current_value = _start_value + t_sec * (_animation.target_value - _start_value);
        }
        else {
            auto t_usec = std::max(0ull, now - _start_time);
            auto t_sec = t_usec / (float)kUsecPerSec;
            if (_easing_function && t_sec < 1) t_sec = _easing_function(t_sec);
            _current_value = _start_value + t_sec * _animation.by_value;
        }
    }
    
    
    template<typename TValue>
    Animatable<TValue> make_continuous_animatable(TValue start_value, TValue by_value,
                                                  std::function<float(float)> easing_function,
                                                  uint64_t start_time) {
        return Animatable<TValue>(start_value, by_value, start_time, 1, false, easing_function);
    }
    
    template<typename TValue>
    Animatable<TValue> make_durational_animatable(TValue start_value, TValue target_value,
                                                  float duration,
                                                  std::function<float(float)> easing_function,
                                                  uint64_t start_time) {
        return Animatable<TValue>(start_value, target_value, start_time, duration * kUsecPerSec, true, easing_function);
    }
    
} }

#endif

