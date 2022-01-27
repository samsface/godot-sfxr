tool
extends EditorPlugin
class_name GodotSfxr

enum WAVE_SHAPES {
	SQUARE,
	SAWTOOTH,
	SINE,
	NOISE,
}

enum PRESETS {
	NONE,
	PICKUP,
	LASER,
	EXPLOSION,
	POWERUP,
	HIT,
	JUMP,
	CLICK,
	BLIP,
	SYNTH,
	RANDOM,
	TONE,
	MUTATE,
}

const OVERSAMPLING = 8

const MASTER_VOLUME = 1

func _enter_tree() -> void:
	add_custom_type(
		"SfxrStreamPlayer", "AudioStreamPlayer", load("res://addons/godot_sfxr/SfxrStreamPlayer.gd"),
		get_editor_interface().get_base_control().get_icon("AudioStreamPlayer", "EditorIcons"))

func disable_plugin() -> void:
	remove_custom_type("SfxrStreamPlayer")
