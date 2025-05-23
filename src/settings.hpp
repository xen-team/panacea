#pragma once

namespace panacea {
struct Settings {
    float master_volume = 1.f;
    float ambient_volume = 0.25f;
    bool enable_health_bar = true;
};
inline Settings& get_settings()
{
    static Settings settings;
    return settings;
}
}