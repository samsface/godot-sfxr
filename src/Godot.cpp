#include <Godot.hpp>
#include <Node.hpp>
#include <cstdint>
#include <memory>
#include <set>
#include <array>
#include <string>

namespace godot 
{
class GodotSFXRNative : public Reference
{
    GODOT_CLASS(GodotSFXRNative, Reference)

public:
    void build_buffer()
    {
    }

    static void _register_methods()
    {
    }

    void _init()
    {
        register_method("build_buffer", &GodotSFXRNative::build_buffer);
    }
};

extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *o) 
{
    godot::Godot::gdnative_init(o);
}

extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *o) 
{
    godot::Godot::gdnative_terminate(o);
}

extern "C" void GDN_EXPORT godot_nativescript_init(void *handle) 
{
    godot::Godot::nativescript_init(handle);
    godot::register_class<godot::GodotSFXRNative>();
}
}
