#include <Godot.hpp>
#include <Node.hpp>
#include <cstdint>
#include <memory>
#include <set>
#include <array>
#include <string>
#include <algorithm>

namespace godot 
{
class GodotSFXRNative : public Reference
{
    GODOT_CLASS(GodotSFXRNative, Reference)

    enum WAVE_SHAPES 
    {
        SQUARE,
        SAWTOOTH,
        SINE,
        NOISE,
    };

    std::int64_t wave_type{};

    // Envelope
    std::float_t p_env_attack{};
    std::float_t p_env_sustain{};
    std::float_t p_env_punch{};
    std::float_t p_env_decay{};

    // Tone
    std::float_t p_base_freq{};
    std::float_t p_freq_limit{};
    std::float_t p_freq_ramp{};
    std::float_t p_freq_dramp{};

    // Vibrato
    std::float_t p_vib_strength{};
    std::float_t p_vib_speed{};

    // Tonal Change
    std::float_t p_arp_mod{};
    std::float_t p_arp_speed{};

    // Square wve duty (proportion of time signal is high vs low)
    std::float_t p_duty{};
    std::float_t p_duty_ramp{};

    // Repeat
    std::float_t p_repeat_speed{};

    // Flanger
    std::float_t p_pha_offset{};
    std::float_t p_pha_ramp{};

    // Low-pass filter
    std::float_t p_lpf_freq{};
    std::float_t p_lpf_ramp{};
    std::float_t p_lpf_resonance{};

    // High-pass filter
    std::float_t p_hpf_freq{};
    std::float_t p_hpf_ramp{};

    // Sample parameters
    std::float_t sound_vol{};
    std::float_t sample_rate{};

    //
    WAVE_SHAPES wave_shape{};
    std::float_t repeat_time{};
    std::float_t elapsed_since_repeat{};
    std::float_t arpeggio_time{};
    std::float_t arpeggio_multiplier{};
    std::float_t period{};
    std::float_t period_mult{};
    std::float_t period_mult_slide{};
    std::float_t period_max{};
    bool enable_frequency_cutoff{};
    std::float_t duty_cycle{};
    std::float_t duty_cycle_slide{};
    std::float_t fltw{};
    std::float_t fltw_d{};
    std::float_t fltdmp{};
    std::float_t flthp{};
    std::float_t flthp_d{};
    bool enable_low_pass_filter{};
    std::float_t vibrato_speed{};
    std::float_t vibrato_amplitude{};
    std::array<std::float_t, 3> envelope_length{};
    std::float_t envelope_punch{};
    std::float_t flanger_offset{};
    std::float_t flanger_offset_slide{};
    std::float_t gain{};
    //std::float_t sample_rate{};

public:
    void init(Object* params)
    {
        wave_type = params->get("wave_type");
        p_env_attack = params->get("p_env_attack");
        p_env_sustain = params->get("p_env_sustain");
        p_env_punch = params->get("p_env_punch");
        p_env_decay = params->get("p_env_decay");
        p_base_freq = params->get("p_base_freq");
        p_freq_limit = params->get("p_freq_limit");
        p_freq_ramp = params->get("p_freq_ramp");
        p_freq_dramp = params->get("p_freq_dramp");
        p_vib_strength = params->get("p_vib_strength");
        p_vib_speed = params->get("p_vib_speed");
        p_arp_mod = params->get("p_arp_mod");
        p_arp_speed = params->get("p_arp_speed");
        p_duty = params->get("p_duty");
        p_duty_ramp = params->get("p_duty_ramp");
        p_repeat_speed = params->get("p_repeat_speed");
        p_pha_offset = params->get("p_pha_offset");
        p_pha_ramp = params->get("p_pha_ramp");
        p_lpf_freq = params->get("p_lpf_freq");
        p_lpf_ramp = params->get("p_lpf_ramp");
        p_lpf_resonance = params->get("p_lpf_resonance");
        p_hpf_freq = params->get("p_hpf_freq");
        p_hpf_ramp = params->get("p_hpf_ramp");
        sound_vol = params->get("sound_vol");
        sample_rate = params->get("sample_rate");

        init_for_repeat();

        // Wave shape
        wave_shape = static_cast<WAVE_SHAPES>(wave_type);

        // Filter
        fltw = std::pow(p_lpf_freq, 3.0f) * 0.1f;
        
        enable_low_pass_filter = p_lpf_freq != 1;
        fltw_d = 1.0f + p_lpf_ramp * 0.0001f;
        fltdmp = 5.0f / (1.0f + std::pow(p_lpf_resonance, 2.0f) * 20.0f) * (0.01f + fltw);
        
        if(fltdmp > 0.8f)
        {
            fltdmp = 0.8f;
        }
          
        flthp = std::pow(p_hpf_freq, 2.0f) * 0.1f;
        flthp_d = 1 + p_hpf_ramp * 0.0003f;

        // Vibrato
        vibrato_speed = std::pow(p_vib_speed, 2.0f) * 0.01f;
        vibrato_amplitude = p_vib_strength * 0.5f;

        // Envelope
        envelope_length[0] = std::floor(std::pow(p_env_attack, 2.0f) * 100000.0f);
        envelope_length[1] = std::floor(std::pow(p_env_sustain, 2.0f) * 100000.0f);
        envelope_length[2] = std::floor(std::pow(p_env_decay, 2.0f) * 100000.0f);

        envelope_punch = p_env_punch;

        // Flanger
        flanger_offset = std::pow(p_pha_offset, 2.0f) * 1020.0f;
        if(p_pha_offset < 0)
        {
            flanger_offset = -flanger_offset;
        }
        flanger_offset_slide = std::pow(p_pha_ramp, 2.0f) * 1.0f;
        if(p_pha_ramp < 0)
        {
            flanger_offset_slide = -flanger_offset_slide;
        }
        
        // Repeat
        repeat_time = std::floor(std::pow(1.0f - p_repeat_speed, 2.0f) * 20000.0f + 32.0f);
        if(p_repeat_speed == 0)
        {
            repeat_time = 0;
        }

        gain = std::exp(sound_vol) - 1.0f;
        sample_rate = sample_rate;
    }

    void init_for_repeat()
    {
        elapsed_since_repeat = 0;

        period = 100.0f / (p_base_freq * p_base_freq + 0.001f);
        period_max = 100.0f / (p_freq_limit * p_freq_limit + 0.001f);
        enable_frequency_cutoff = p_freq_limit > 0;
        period_mult = 1.0f - std::pow(p_freq_ramp, 3.0f) * 0.01f;
        period_mult_slide = - std::pow(p_freq_dramp, 3.0f) * 0.000001f;

        duty_cycle = 0.5f - p_duty * 0.5f;
        duty_cycle_slide = -p_duty_ramp * 0.00005f;

        if(p_arp_mod >= 0)
        {
            arpeggio_multiplier = 1.0f - std::pow(p_arp_mod, 2.0f) * 0.9f;
        }
        else
        {
            arpeggio_multiplier = 1.0f + std::pow(p_arp_mod, 2.0f) * 10.0f;
        }
        arpeggio_time = std::floor(std::pow(1.0f - p_arp_speed, 2.0f) * 20000.0f + 32.0f);
        if(p_arp_speed == 1)
        {
            arpeggio_time = 0;
        }
    }

    PoolVector2Array get_raw_buffer()
    {

        std::float_t fltp{};
        std::float_t fltdp{};
        std::float_t fltphp{};

        std::array<std::float_t, 32> noise_buffer;
        for(auto& i : noise_buffer)
        {
            //i = randf(); use godot or c++?>
        }

        std::size_t envelope_stage{};
        std::float_t envelope_elapsed{};

        std::float_t vibrato_phase{};

        std::float_t phase{};

        std::size_t ipp{};

        std::array<std::float_t, 1024> flanger_buffer{};

        PoolVector2Array res;

        std::float_t sample_sum{};
        std::float_t num_summed{};
        std::float_t summands{std::floor(44100 / sample_rate)};

        std::size_t t{};
        while(true)
        {
            t += 1;

            // Repeats
            elapsed_since_repeat += 1;
            if(repeat_time != 0 && elapsed_since_repeat >= repeat_time)
            {
                init_for_repeat();
            }

            // Arpeggio (single)
            if(arpeggio_time != 0 && t >= arpeggio_time)
            {
                arpeggio_time = 0;
                period *= arpeggio_multiplier;
            }

            // Frequency slide, and frequency slide slide!
            period_mult += period_mult_slide;
            period *= period_mult;
            if(period > period_max)
            {
                period = period_max;
                if(enable_frequency_cutoff)
                {
                    break;
                }
            }

            // Vibrato
            auto rfperiod = period;
            if(vibrato_amplitude > 0)
            {
                vibrato_phase += vibrato_speed;
                rfperiod = period * (1 + std::sin(vibrato_phase) * vibrato_amplitude);
            }

            auto iperiod = std::max(std::floor(rfperiod), 8.0f);//SfxrGlobals.OVERSAMPLING)
    
            // Square wave duty cycle
            duty_cycle = std::clamp(duty_cycle + duty_cycle_slide, 0.0f, 0.5f);

            // Volume envelope
            envelope_elapsed += 1;
            if(envelope_elapsed > envelope_length[envelope_stage])
            {
                envelope_elapsed = 0;
                envelope_stage += 1;
                if(envelope_stage > 2)
                {
                    break;
                }
            }
    
            if(!envelope_length[envelope_stage])
            {
                continue;
            }
            
            std::float_t env_vol{};
            auto envf = envelope_elapsed / envelope_length[envelope_stage];
            if(envelope_stage == 0) // Attack
            {
                env_vol = envf;
            }
            else if(envelope_stage == 1) // # Sustain
            {
                env_vol = 1 + (1 - envf) * 2 * envelope_punch;
            }
            else // # Decay
            {
                env_vol = 1 - envf;
            }

            // Flanger step
            
            flanger_offset += flanger_offset_slide;
            auto iphase = std::min(std::abs(std::floor(flanger_offset)), 1023.0f);

            if(flthp_d != 0)
            {
                flthp = std::clamp(flthp * flthp_d, 0.00001f, 0.1f);
            }

            // 8x oversampling
            std::float_t sample{};
            for(std::int64_t si{}; si < (8 - 1); si++)// SfxrGlobals.OVERSAMPLING - 1:
            {
                std::float_t sub_sample{};
                phase += 1;
                if(phase >= iperiod)
                {
                    phase = std::fmod(phase, iperiod);
                    if(wave_shape == WAVE_SHAPES::NOISE)
                    {
                        for(std::int64_t i{}; i < 32; i++)
                        {
                            //noise_buffer[i] = randf() * 2 - 1
                        }
                    }
                }

                // Base waveform
                auto fp = phase / iperiod;
                if(wave_shape == WAVE_SHAPES::SQUARE)
                {
                    if(fp < duty_cycle)
                    {
                        sub_sample = 0.5f;
                    }  
                    else
                    {
                        sub_sample = -0.5f;
                    }
                }
                else if(wave_shape == WAVE_SHAPES::SAWTOOTH)
                {
                    if(fp < duty_cycle)
                    {
                        sub_sample = -1.0f + 2.0f * fp / duty_cycle;
                    }    
                    else
                    {
                        sub_sample = 1.0f - 2.0f * (fp - duty_cycle) / (1.0f - duty_cycle);
                    }
                }
                else if(wave_shape == WAVE_SHAPES::SINE)
                {
                    sub_sample = std::sin(fp * 2.0f * 3.141);
                }
                else if(wave_shape == WAVE_SHAPES::NOISE)
                {
                    sub_sample = noise_buffer[std::floor(phase * 32.0f / iperiod)];
                }
                else
                {
                    //print("ERROR: Bad wave type: ", wave_shape)
                    sub_sample = 0;
                }
    
                // Low-pass filter
                auto pp = fltp;
                fltw = std::clamp(fltw * fltw_d, 0.0f, 0.1f);
                if(enable_low_pass_filter)
                {
                    fltdp += (sub_sample - fltp) * fltw;
                    fltdp -= fltdp * fltdmp;
                }
                else
                {
                    fltp = sub_sample;
                    fltdp = 0;
                }

                fltp += fltdp;

                // High-pass filter
                fltphp += fltp - pp;
                fltphp -= fltphp * flthp;
                sub_sample = fltphp;

                // Flanger
                flanger_buffer[ipp & 1023] = sub_sample;
                sub_sample += flanger_buffer[static_cast<int>(std::floor(ipp - iphase + 1024.0f)) & 1023];

                ipp = static_cast<int>(std::floor((ipp + 1))) & 1023;

                // Final accumulation and envelope application
                sample += sub_sample * env_vol;
            }


            // Accumulate samples appropriately for sample rate
            sample_sum += sample;
            num_summed += 1;
            if(num_summed >= summands)
            {
                num_summed = 0;
                sample = sample_sum / summands;
                sample_sum = 0;
            }
            else
            {
                continue;
            }

            sample = sample / 8.0f * 1.0f;//SfxrGlobals.MASTER_VOLUME;
            sample *= gain;

            res.append(Vector2(sample, sample));
        }

        return res;
    }

    static void _register_methods()
    {
        register_property("wave_type", &GodotSFXRNative::wave_type, {});
        register_property("p_env_attack", &GodotSFXRNative::p_env_attack, {});
        register_property("p_env_sustain", &GodotSFXRNative::p_env_sustain, {});
        register_property("p_env_punch", &GodotSFXRNative::p_env_punch, {});
        register_property("p_env_decay", &GodotSFXRNative::p_env_decay, {});
        register_property("p_base_freq", &GodotSFXRNative::p_base_freq, {});
        register_property("p_freq_limit", &GodotSFXRNative::p_freq_limit, {});
        register_property("p_freq_ramp", &GodotSFXRNative::p_freq_ramp, {});
        register_property("p_freq_dramp", &GodotSFXRNative::p_freq_dramp, {});
        register_property("p_vib_strength", &GodotSFXRNative::p_vib_strength, {});
        register_property("p_vib_speed", &GodotSFXRNative::p_vib_speed, {});
        register_property("p_arp_mod", &GodotSFXRNative::p_arp_mod, {});
        register_property("p_arp_speed", &GodotSFXRNative::p_arp_speed, {});
        register_property("p_duty", &GodotSFXRNative::p_duty, {});
        register_property("p_duty_ramp", &GodotSFXRNative::p_duty_ramp, {});
        register_property("p_repeat_speed", &GodotSFXRNative::p_repeat_speed, {});
        register_property("p_pha_offset", &GodotSFXRNative::p_pha_offset, {});
        register_property("p_pha_ramp", &GodotSFXRNative::p_pha_ramp, {});
        register_property("p_lpf_freq", &GodotSFXRNative::p_lpf_freq, {});
        register_property("p_lpf_ramp", &GodotSFXRNative::p_lpf_ramp, {});
        register_property("p_lpf_resonance", &GodotSFXRNative::p_lpf_resonance, {});
        register_property("p_hpf_freq", &GodotSFXRNative::p_hpf_freq, {});
        register_property("p_hpf_ramp", &GodotSFXRNative::p_hpf_ramp, {});
        register_property("sound_vol", &GodotSFXRNative::sound_vol, {});
        register_property("sample_rate", &GodotSFXRNative::sample_rate, {});

        register_method("init", &GodotSFXRNative::init);
        register_method("get_raw_buffer", &GodotSFXRNative::get_raw_buffer);
    }

    void _init()
    {
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
