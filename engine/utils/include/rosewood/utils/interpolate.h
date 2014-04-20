#ifndef __ROSEWOOD_MATH_INTERPOLATE_H__
#define __ROSEWOOD_MATH_INTERPOLATE_H__

namespace rosewood { namespace utils {
    
    template<typename T>
    T lerp(float mix, T from, T to) {
        return from*(1-mix) + to*mix;
    }

    template<typename T>
    T slerp(float mix, T from, T to) {
        float cosHalfTheta = dot(from, to);
        if (cosHalfTheta < 0) {
            to = -to;
            cosHalfTheta = -cosHalfTheta;
        }
        
        if (std::abs(cosHalfTheta) >= 1.0f) {
            return from;
        }
        
        float halfTheta = acosf(cosHalfTheta);
        float sinHalfTheta = sqrtf(1.0f - cosHalfTheta*cosHalfTheta);
        
        if (std::abs(sinHalfTheta) < 0.001) {
            return 0.5f * (from + to);
        }
        
        float ratioA = sinf((1 - mix) * halfTheta) / sinHalfTheta;
        float ratioB = sinf(     mix  * halfTheta) / sinHalfTheta;
        
        return ratioA * from + ratioB * to;
    }
    
    inline float cubic_ease_in(float mix) {
        return mix*mix*mix;
    }
    
    inline float cubic_ease_out(float mix) {
        mix -= 1;
        return mix*mix*mix + 1;
    }
    
    inline float cubic_ease_in_out(float mix) {
        if (mix < 0.5f) {
            return cubic_ease_in(mix * 2) * 0.5f;
        }
        return 0.5f + cubic_ease_out(2*mix - 1) * 0.5f;
    }

    
} }

#endif
