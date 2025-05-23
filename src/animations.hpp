#pragma once
#include <chrono>
#include <functional>
#include <map>
#include <string>
#include <cmath>

#include <imgui.h>

namespace imtweens {
using namespace std::chrono_literals;

enum class easing : uint8_t {
    LinearInterpolation,
    QuadEaseIn,
    QuadEaseOut,
    QuadEaseInOut,
    CubicEaseIn,
    CubicEaseOut,
    CubicEaseInOut,
    QuartEaseIn,
    QuartEaseOut,
    QuartEaseInOut,
    QuintEaseIn,
    QuintEaseOut,
    QuintEaseInOut,
    SineEaseIn,
    SineEaseOut,
    SineEaseInOut,
    CircEaseIn,
    CircEaseOut,
    CircEaseInOut,
    ExponentialEaseIn,
    ExponentialEaseOut,
    ExponentialEaseInOut,
    ElasticEaseIn,
    ElasticEaseOut,
    ElasticEaseInOut,
    BackEaseIn,
    BackEaseOut,
    BackEaseInOut,
    BounceEaseIn,
    BounceEaseOut,
    BounceEaseInOut
};

// Easing functions taken from
// https://github.com/warrenm/AHEasing/blob/master/AHEasing/easing.c
// Within unlicense

namespace easing_funcs {
// Modeled after the line y = x
inline float LinearInterpolation(float p)
{
    return p;
}

// Modeled after the parabola y = x^2
inline float QuadEaseIn(float p)
{
    return p * p;
}

// Modeled after the parabola y = -x^2 + 2x
inline float QuadEaseOut(float p)
{
    return -(p * (p - 2));
}

// Modeled after the piecewise quadratic
// y = (1/2)((2x)^2)             ; [0, 0.5)
// y = -(1/2)((2x-1)*(2x-3) - 1) ; [0.5, 1]
inline float QuadEaseInOut(float p)
{
    if (p < 0.5) {
        return 2 * p * p;
    }
    else {
        return (-2 * p * p) + (4 * p) - 1;
    }
}

// Modeled after the cubic y = x^3
inline float CubicEaseIn(float p)
{
    return p * p * p;
}

// Modeled after the cubic y = (x - 1)^3 + 1
inline float CubicEaseOut(float p)
{
    float f = (p - 1);
    return f * f * f + 1;
}

// Modeled after the piecewise cubic
// y = (1/2)((2x)^3)       ; [0, 0.5)
// y = (1/2)((2x-2)^3 + 2) ; [0.5, 1]
inline float CubicEaseInOut(float p)
{
    if (p < 0.5) {
        return 4 * p * p * p;
    }
    else {
        float f = ((2 * p) - 2);
        return 0.5 * f * f * f + 1;
    }
}

// Modeled after the quartic x^4
inline float QuartEaseIn(float p)
{
    return p * p * p * p;
}

// Modeled after the quartic y = 1 - (x - 1)^4
inline float QuartEaseOut(float p)
{
    float f = (p - 1);
    return f * f * f * (1 - p) + 1;
}

// Modeled after the piecewise quartic
// y = (1/2)((2x)^4)        ; [0, 0.5)
// y = -(1/2)((2x-2)^4 - 2) ; [0.5, 1]
inline float QuartEaseInOut(float p)
{
    if (p < 0.5) {
        return 8 * p * p * p * p;
    }
    else {
        float f = (p - 1);
        return -8 * f * f * f * f + 1;
    }
}

// Modeled after the quintic y = x^5
inline float QuintEaseIn(float p)
{
    return p * p * p * p * p;
}

// Modeled after the quintic y = (x - 1)^5 + 1
inline float QuintEaseOut(float p)
{
    float f = (p - 1);
    return f * f * f * f * f + 1;
}

// Modeled after the piecewise quintic
// y = (1/2)((2x)^5)       ; [0, 0.5)
// y = (1/2)((2x-2)^5 + 2) ; [0.5, 1]
inline float QuintEaseInOut(float p)
{
    if (p < 0.5) {
        return 16 * p * p * p * p * p;
    }
    else {
        float f = ((2 * p) - 2);
        return 0.5 * f * f * f * f * f + 1;
    }
}

// Modeled after quarter-cycle of sine wave
inline float SineEaseIn(float p)
{
    return sin((p - 1) * M_PI_2) + 1;
}

// Modeled after quarter-cycle of sine wave (different phase)
inline float SineEaseOut(float p)
{
    return sin(p * M_PI_2);
}

// Modeled after half sine wave
inline float SineEaseInOut(float p)
{
    return 0.5 * (1 - cos(p * M_PI));
}

// Modeled after shifted quadrant IV of unit circle
inline float CircEaseIn(float p)
{
    return 1 - sqrt(1 - (p * p));
}

// Modeled after shifted quadrant II of unit circle
inline float CircEaseOut(float p)
{
    return sqrt((2 - p) * p);
}

// Modeled after the piecewise circular function
// y = (1/2)(1 - sqrt(1 - 4x^2))           ; [0, 0.5)
// y = (1/2)(sqrt(-(2x - 3)*(2x - 1)) + 1) ; [0.5, 1]
inline float CircEaseInOut(float p)
{
    if (p < 0.5) {
        return 0.5 * (1 - sqrt(1 - 4 * (p * p)));
    }
    else {
        return 0.5 * (sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
    }
}

// Modeled after the exponential function y = 2^(10(x - 1))
inline float ExponentialEaseIn(float p)
{
    return (p == 0.0) ? p : pow(2, 10 * (p - 1));
}

// Modeled after the exponential function y = -2^(-10x) + 1
inline float ExponentialEaseOut(float p)
{
    return (p == 1.0) ? p : 1 - pow(2, -10 * p);
}

// Modeled after the piecewise exponential
// y = (1/2)2^(10(2x - 1))         ; [0,0.5)
// y = -(1/2)*2^(-10(2x - 1))) + 1 ; [0.5,1]
inline float ExponentialEaseInOut(float p)
{
    if (p == 0.0 || p == 1.0)
        return p;

    if (p < 0.5) {
        return 0.5 * pow(2, (20 * p) - 10);
    }
    else {
        return -0.5 * pow(2, (-20 * p) + 10) + 1;
    }
}

// Modeled after the damped sine wave y = sin(13pi/2*x)*pow(2, 10 * (x - 1))
inline float ElasticEaseIn(float p)
{
    return sin(13 * M_PI_2 * p) * pow(2, 10 * (p - 1));
}

// Modeled after the damped sine wave y = sin(-13pi/2*(x + 1))*pow(2, -10x) + 1
inline float ElasticEaseOut(float p)
{
    return sin(-13 * M_PI_2 * (p + 1)) * pow(2, -10 * p) + 1;
}

// Modeled after the piecewise exponentially-damped sine wave:
// y = (1/2)*sin(13pi/2*(2*x))*pow(2, 10 * ((2*x) - 1))      ; [0,0.5)
// y = (1/2)*(sin(-13pi/2*((2x-1)+1))*pow(2,-10(2*x-1)) + 2) ; [0.5, 1]
inline float ElasticEaseInOut(float p)
{
    if (p < 0.5) {
        return 0.5 * sin(13 * M_PI_2 * (2 * p)) * pow(2, 10 * ((2 * p) - 1));
    }
    else {
        return 0.5 * (sin(-13 * M_PI_2 * ((2 * p - 1) + 1)) * pow(2, -10 * (2 * p - 1)) + 2);
    }
}

// Modeled after the overshooting cubic y = x^3-x*sin(x*pi)
inline float BackEaseIn(float p)
{
    return p * p * p - p * sin(p * M_PI);
}

// Modeled after overshooting cubic y = 1-((1-x)^3-(1-x)*sin((1-x)*pi))
inline float BackEaseOut(float p)
{
    float f = (1 - p);
    return 1 - (f * f * f - f * sin(f * M_PI));
}

// Modeled after the piecewise overshooting cubic function:
// y = (1/2)*((2x)^3-(2x)*sin(2*x*pi))           ; [0, 0.5)
// y = (1/2)*(1-((1-x)^3-(1-x)*sin((1-x)*pi))+1) ; [0.5, 1]
inline float BackEaseInOut(float p)
{
    if (p < 0.5) {
        float f = 2 * p;
        return 0.5 * (f * f * f - f * sin(f * M_PI));
    }
    else {
        float f = (1 - (2 * p - 1));
        return 0.5 * (1 - (f * f * f - f * sin(f * M_PI))) + 0.5;
    }
}

inline float BounceEaseOut(float p)
{
    if (p < 4 / 11.0) {
        return (121 * p * p) / 16.0;
    }
    else if (p < 8 / 11.0) {
        return (363 / 40.0 * p * p) - (99 / 10.0 * p) + 17 / 5.0;
    }
    else if (p < 9 / 10.0) {
        return (4356 / 361.0 * p * p) - (35442 / 1805.0 * p) + 16061 / 1805.0;
    }
    else {
        return (54 / 5.0 * p * p) - (513 / 25.0 * p) + 268 / 25.0;
    }
}

inline float BounceEaseIn(float p)
{
    return 1 - BounceEaseOut(1 - p);
}

inline float BounceEaseInOut(float p)
{
    if (p < 0.5) {
        return 0.5 * BounceEaseIn(p * 2);
    }
    else {
        return 0.5 * BounceEaseOut(p * 2 - 1) + 0.5;
    }
}
}

inline std::function<float(float)> get_easing_func(easing easing)
{
    switch (easing) {
    case easing::LinearInterpolation:
        return easing_funcs::LinearInterpolation;
    case easing::QuadEaseIn:
        return easing_funcs::QuadEaseIn;
    case easing::QuadEaseOut:
        return easing_funcs::QuadEaseOut;
    case easing::QuadEaseInOut:
        return easing_funcs::QuadEaseInOut;
    case easing::CubicEaseIn:
        return easing_funcs::CubicEaseIn;
    case easing::CubicEaseOut:
        return easing_funcs::CubicEaseOut;
    case easing::CubicEaseInOut:
        return easing_funcs::CubicEaseInOut;
    case easing::QuartEaseIn:
        return easing_funcs::QuartEaseIn;
    case easing::QuartEaseOut:
        return easing_funcs::QuartEaseOut;
    case easing::QuartEaseInOut:
        return easing_funcs::QuartEaseInOut;
    case easing::QuintEaseIn:
        return easing_funcs::QuintEaseIn;
    case easing::QuintEaseOut:
        return easing_funcs::QuintEaseOut;
    case easing::QuintEaseInOut:
        return easing_funcs::QuintEaseInOut;
    case easing::SineEaseIn:
        return easing_funcs::SineEaseIn;
    case easing::SineEaseOut:
        return easing_funcs::SineEaseOut;
    case easing::SineEaseInOut:
        return easing_funcs::SineEaseInOut;
    case easing::CircEaseIn:
        return easing_funcs::CircEaseIn;
    case easing::CircEaseOut:
        return easing_funcs::CircEaseOut;
    case easing::CircEaseInOut:
        return easing_funcs::CircEaseInOut;
    case easing::ExponentialEaseIn:
        return easing_funcs::ExponentialEaseIn;
    case easing::ExponentialEaseOut:
        return easing_funcs::ExponentialEaseOut;
    case easing::ExponentialEaseInOut:
        return easing_funcs::ExponentialEaseInOut;
    case easing::ElasticEaseIn:
        return easing_funcs::ElasticEaseIn;
    case easing::ElasticEaseOut:
        return easing_funcs::ElasticEaseOut;
    case easing::ElasticEaseInOut:
        return easing_funcs::ElasticEaseInOut;
    case easing::BackEaseIn:
        return easing_funcs::BackEaseIn;
    case easing::BackEaseOut:
        return easing_funcs::BackEaseOut;
    case easing::BackEaseInOut:
        return easing_funcs::BackEaseInOut;
    case easing::BounceEaseIn:
        return easing_funcs::BounceEaseIn;
    case easing::BounceEaseOut:
        return easing_funcs::BounceEaseOut;
    case easing::BounceEaseInOut:
        return easing_funcs::BounceEaseInOut;
    default:
        return easing_funcs::LinearInterpolation;
    }
}

// For ImVec4 animations
inline ImVec4 operator*(ImVec4 const& lhs, float rhs)
{
    return ImVec4{lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs};
}
inline bool operator!=(ImColor const& lhs, ImColor const& rhs)
{
    return ImVec4(lhs) != ImVec4(rhs);
}

#include <sstream>

struct tween_id {
    tween_id(char const* name) : id{std::hash<std::string>{}(name)} {};

    template <typename T>
    tween_id(char const* name, T const& unique_value)
    {
        std::stringstream ss;
        ss << name << '#' << unique_value;
        id = std::hash<std::string>{}(ss.str());
    };
    size_t id = 0;

    bool operator<(tween_id const& other) const { return id < other.id; }
};

struct tween_cond {
public:
    tween_cond() = default;
    tween_cond(bool cond) : value{cond}, conditionable{true} {};
    operator bool() { return value; }

    bool value = false;
    bool is_empty() { return !conditionable; }

private:
    bool conditionable = false;
};

template <typename T>
struct tween_data {
    std::chrono::duration<float> duration;
    T from;
    T to;
    T last_value;
};

template <typename T>
inline auto& get_tweens()
{
    static std::map<tween_id, tween_data<T>> tweens;
    return tweens;
}

template <typename T>
inline void reset_tween(tween_id id)
{
    using Duration = std::chrono::duration<float>;

    auto& tweens = get_tweens<T>();
    tweens.erase(tweens.find(id));
}

// T requires both overloaded operator*(T, float) and operator!=(T, T) on custom types!
template <typename T>
T tween(
    tween_id id, T const& from, T const& to, easing easing = easing::LinearInterpolation,
    std::chrono::duration<float> duration = 300ms, tween_cond condition = {}
)
{
    using Duration = std::chrono::duration<float>;

    float const deltatime = ImGui::GetIO().DeltaTime;
    auto [emplaced_tween, first_try] = get_tweens<T>().try_emplace(id, tween_data<T>{Duration(0), from, to, from});
    auto& current_tween = emplaced_tween->second;
    auto& tween_duration = current_tween.duration;
    if (current_tween.to != to) {
        tween_duration = Duration(0);
    }

    if (!condition.is_empty() && !condition.value) {
        tween_duration -= static_cast<Duration>(deltatime);
        if (tween_duration.count() < 0) {
            tween_duration = Duration(0);
            return from;
        }
    }
    else {
        tween_duration += static_cast<Duration>(deltatime);
        if (tween_duration >= duration) {
            tween_duration = duration;
            return to;
        }
    }

    if (current_tween.to != to) {
        current_tween.from = current_tween.last_value;
    }
    current_tween.to = to;

    float const progress = tween_duration.count() / duration.count();
    T value = current_tween.from + (current_tween.to - current_tween.from) * (get_easing_func(easing))(progress);
    current_tween.last_value = value;
    return value;
}
}