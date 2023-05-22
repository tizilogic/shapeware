#include "dummies.h"

sw_dummies_camera_t sw_dummies_default_camera(void) {
	return (sw_dummies_camera_t){.focal_length = 4.5f,
	                             .look_at = {.x = 0.0f, .y = 0.0f, .z = 0.0f},
	                             .pos = {.x = 0.0f, .y = 0.0f, .z = -12.0f}};
}

sw_dummies_color_t sw_dummies_default_color(void) {
	return (sw_dummies_color_t){.bgcolor = {.x = 0.02f, .y = 0.02f, .z = 0.02f}};
}

#define DEFAULT_SCALING_FACTOR 0.2f

const sw_dummies_spherical_harmonics_t sw_spherical_harmonics_presets[SW_SH_TOTAL] = {
    // Old Town Square
    {.L00 = {.x = 0.87f, .y = 0.88f, .z = 0.86f},
     .L1m1 = {.x = 0.18f, .y = 0.25f, .z = 0.31f},
     .L10 = {.x = 0.03f, .y = 0.04f, .z = 0.04f},
     .L11 = {.x = -0.0f, .y = -0.03f, .z = -0.05f},
     .L2m2 = {.x = -0.12f, .y = -0.12f, .z = -0.12f},
     .L2m1 = {.x = 0.0f, .y = 0.0f, .z = 0.01f},
     .L20 = {.x = -0.03f, .y = -0.02f, .z = -0.02f},
     .L21 = {.x = -0.08f, .y = -0.09f, .z = -0.09f},
     .L22 = {.x = -0.16f, .y = -0.19f, .z = -0.22f},
     .scaling_factor = DEFAULT_SCALING_FACTOR},
    // Grace Cathedral
    {.L00 = {.x = 0.79f, .y = 0.44f, .z = 0.54f},
     .L1m1 = {.x = 0.39f, .y = 0.35f, .z = 0.60f},
     .L10 = {.x = -0.34f, .y = -0.18f, .z = -0.27f},
     .L11 = {.x = -0.29f, .y = -0.06f, .z = 0.01f},
     .L2m2 = {.x = -0.11f, .y = -0.05f, .z = -0.12f},
     .L2m1 = {.x = -0.26f, .y = -0.22f, .z = -0.47f},
     .L20 = {.x = -0.16f, .y = -0.09f, .z = -0.15f},
     .L21 = {.x = 0.56f, .y = 0.21f, .z = 0.14f},
     .L22 = {.x = 0.21f, .y = -0.05f, .z = -0.30f},
     .scaling_factor = DEFAULT_SCALING_FACTOR},
    // Eucalyptus Grove
    {.L00 = {.x = 0.38f, .y = 0.43f, .z = 0.45f},
     .L1m1 = {.x = 0.29f, .y = 0.36f, .z = 0.41f},
     .L10 = {.x = 0.04f, .y = 0.03f, .z = 0.01f},
     .L11 = {.x = -0.10f, .y = -0.10f, .z = -0.09f},
     .L2m2 = {.x = -0.06f, .y = -0.06f, .z = -0.04f},
     .L2m1 = {.x = 0.01f, .y = -0.01f, .z = -0.05f},
     .L20 = {.x = -0.09f, .y = -0.13f, .z = -0.15f},
     .L21 = {.x = -0.06f, .y = -0.05f, .z = -0.04f},
     .L22 = {.x = 0.02f, .y = 0.0f, .z = -0.05f},
     .scaling_factor = DEFAULT_SCALING_FACTOR},
    // St. Peters Basilica
    {.L00 = {.x = 0.36f, .y = 0.26f, .z = 0.23f},
     .L1m1 = {.x = 0.18f, .y = 0.14f, .z = 0.13f},
     .L10 = {.x = -0.02f, .y = -0.01f, .z = 0.0f},
     .L11 = {.x = 0.03f, .y = 0.02f, .z = 0.0f},
     .L2m2 = {.x = 0.02f, .y = 0.01f, .z = 0.0f},
     .L2m1 = {.x = -0.05f, .y = -0.03f, .z = -0.01f},
     .L20 = {.x = -0.09f, .y = -0.08f, .z = -0.07f},
     .L21 = {.x = 0.01f, .y = 0.0f, .z = 0.0f},
     .L22 = {.x = -0.08f, .y = -0.03f, .z = 0.0f},
     .scaling_factor = DEFAULT_SCALING_FACTOR},
    // Uffizi Gallery
    {.L00 = {.x = 0.32f, .y = 0.31f, .z = 0.35f},
     .L1m1 = {.x = 0.37f, .y = 0.37f, .z = 0.43f},
     .L10 = {.x = 0.0f, .y = 0.0f, .z = 0.0f},
     .L11 = {.x = -0.01f, .y = -0.01f, .z = -0.01f},
     .L2m2 = {.x = -0.02f, .y = -0.02f, .z = -0.03f},
     .L2m1 = {.x = -0.01f, .y = -0.01f, .z = -0.01f},
     .L20 = {.x = -0.28f, .y = -0.28f, .z = -0.32f},
     .L21 = {.x = 0.0f, .y = 0.0f, .z = 0.0f},
     .L22 = {.x = -0.24f, .y = -0.24f, .z = -0.28f},
     .scaling_factor = DEFAULT_SCALING_FACTOR},
    // Gallileos Tomb
    {.L00 = {.x = 1.04f, .y = 0.76f, .z = 0.71f},
     .L1m1 = {.x = 0.44f, .y = 0.34f, .z = 0.34f},
     .L10 = {.x = -0.22f, .y = -0.18f, .z = -0.17f},
     .L11 = {.x = 0.71f, .y = 0.54f, .z = 0.56f},
     .L2m2 = {.x = 0.64f, .y = 0.50f, .z = 0.52f},
     .L2m1 = {.x = -0.12f, .y = -0.09f, .z = -0.08f},
     .L20 = {.x = -0.37f, .y = -0.28f, .z = -0.29f},
     .L21 = {.x = -0.17f, .y = -0.13f, .z = -0.13f},
     .L22 = {.x = 0.55f, .y = 0.42f, .z = 0.42f},
     .scaling_factor = DEFAULT_SCALING_FACTOR},
    // Vine Street Kitchen
    {.L00 = {.x = 0.64f, .y = 0.67f, .z = 0.73f},
     .L1m1 = {.x = 0.28f, .y = 0.32f, .z = 0.33f},
     .L10 = {.x = 0.42f, .y = 0.60f, .z = 0.77f},
     .L11 = {.x = -0.05f, .y = -0.04f, .z = -0.02f},
     .L2m2 = {.x = -0.10f, .y = -0.08f, .z = -0.05f},
     .L2m1 = {.x = 0.25f, .y = 0.39f, .z = 0.53f},
     .L20 = {.x = 0.38f, .y = 0.54f, .z = 0.71f},
     .L21 = {.x = 0.06f, .y = 0.01f, .z = -0.02f},
     .L22 = {.x = -0.03f, .y = -0.02f, .z = -0.03f},
     .scaling_factor = DEFAULT_SCALING_FACTOR},
    // Breezeway
    {.L00 = {.x = 0.32f, .y = 0.36f, .z = 0.38f},
     .L1m1 = {.x = 0.37f, .y = 0.41f, .z = 0.45f},
     .L10 = {.x = -0.01f, .y = -0.01f, .z = -0.01f},
     .L11 = {.x = -0.10f, .y = -0.12f, .z = -0.12f},
     .L2m2 = {.x = -0.13f, .y = -0.15f, .z = -0.17f},
     .L2m1 = {.x = -0.01f, .y = -0.02f, .z = 0.02f},
     .L20 = {.x = -0.07f, .y = -0.08f, .z = -0.09f},
     .L21 = {.x = 0.02f, .y = 0.03f, .z = 0.03f},
     .L22 = {.x = -0.29f, .y = -0.32f, .z = -0.36f},
     .scaling_factor = DEFAULT_SCALING_FACTOR},
    // Campus Sunset
    {.L00 = {.x = 0.79f, .y = 0.94f, .z = 0.98f},
     .L1m1 = {.x = 0.44f, .y = 0.56f, .z = 0.70f},
     .L10 = {.x = -0.10f, .y = -0.18f, .z = -0.27f},
     .L11 = {.x = 0.45f, .y = 0.38f, .z = 0.20f},
     .L2m2 = {.x = 0.18f, .y = 0.14f, .z = 0.05f},
     .L2m1 = {.x = -0.14f, .y = -0.22f, .z = -0.31f},
     .L20 = {.x = -0.39f, .y = -0.40f, .z = -0.36f},
     .L21 = {.x = 0.09f, .y = 0.07f, .z = 0.04f},
     .L22 = {.x = 0.67f, .y = 0.67f, .z = 0.52f},
     .scaling_factor = DEFAULT_SCALING_FACTOR},
    // Funston Beach Sunset
    {.L00 = {.x = 0.68f, .y = 0.69f, .z = 0.70f},
     .L1m1 = {.x = 0.32f, .y = 0.37f, .z = 0.44f},
     .L10 = {.x = -0.17f, .y = -0.17f, .z = -0.17f},
     .L11 = {.x = -0.45f, .y = -0.42f, .z = -0.34f},
     .L2m2 = {.x = -0.17f, .y = -0.17f, .z = -0.15f},
     .L2m1 = {.x = -0.08f, .y = -0.09f, .z = -0.10f},
     .L20 = {.x = -0.03f, .y = -0.02f, .z = -0.01f},
     .L21 = {.x = 0.16f, .y = 0.14f, .z = 0.10f},
     .L22 = {.x = 0.37f, .y = 0.31f, .z = 0.20f},
     .scaling_factor = DEFAULT_SCALING_FACTOR},
};

const char *sw_spherical_harmonics_names[SW_SH_TOTAL] = {
    "Old Town Square", "Grace Cathedral",      "Eucalyptus Grove",    "St. Peter's Basilica",
    "Uffizi Gallery",  "Galileo's Tomb",       "Vine Street Kitchen", "Breezeway",
    "Campus Sunset",   "Funston Beach Sunset",
};

sw_dummies_spherical_harmonics_t sw_dummies_default_spherical_harmonics(void) {
	return sw_dummies_get_spherical_harmonics(SW_SH_OLD_TOWN_SQUARE);
}

sw_dummies_spherical_harmonics_t
sw_dummies_get_spherical_harmonics(sw_dummies_spherical_harmonics_presets_t p) {
	return sw_spherical_harmonics_presets[p];
}

const char *sw_dummies_get_spherical_harmonics_name(sw_dummies_spherical_harmonics_presets_t p) {
	return sw_spherical_harmonics_names[p];
}

sw_dummies_rm_and_lighting_t sw_dummies_default_rm_and_lighting(void) {
	return (sw_dummies_rm_and_lighting_t){.max_steps = 150.0f,
	                                      .max_dist = 100.0f,
	                                      .surf_dist = 0.001f,
	                                      .shininess = 10.5f,
	                                      .light_dir = {.x = 0.0f, .y = 0.5f, .z = -0.5f}};
}

sw_dummies_mc_settings_t sw_dummies_default_mc_settings(void) {
    return (sw_dummies_mc_settings_t){.cube_sidelen = 0.1f, .cubes_per_side = 10, .origin = {0}};
}
