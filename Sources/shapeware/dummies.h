#pragma once

#include "mc.h"
#include "shared.h"
#include <krink/math/vector.h>

typedef enum sw_dummies {
	SW_DUMMIES_CAMERA = SW_DUMMIES_START,
	SW_DUMMIES_COLOR,
	SW_DUMMIES_SPHERICAL_HARMONICS,
	SW_DUMMIES_RM_AND_LIGHTING,
	SW_DUMMIES_MC_SETTINGS,
} sw_dummies_t;

typedef struct sw_dummies_camera {
	float focal_length;
	kr_vec3_t pos;
	kr_vec3_t look_at;
} sw_dummies_camera_t;

typedef struct sw_dummies_color {
	kr_vec3_t bgcolor;
} sw_dummies_color_t;

typedef struct sw_dummies_spherical_harmonics {
	kr_vec3_t L00;
	kr_vec3_t L1m1;
	kr_vec3_t L10;
	kr_vec3_t L11;
	kr_vec3_t L2m2;
	kr_vec3_t L2m1;
	kr_vec3_t L20;
	kr_vec3_t L21;
	kr_vec3_t L22;
	float scaling_factor;
} sw_dummies_spherical_harmonics_t;

typedef enum sw_dummies_spherical_harmonics_presets {
	SW_SH_OLD_TOWN_SQUARE = 0,
	SW_SH_GRACE_CATHEDRAL,
	SW_SH_EUCALYPTUS_GROVE,
	SW_SH_ST_PETERS_BASILICA,
	SW_SH_UFFIZI_GALLERY,
	SW_SH_GALLILEOS_TOMB,
	SW_SH_VINE_STREET_KITCHEN,
	SW_SH_BREEZEWAY,
	SW_SH_CAMPUS_SUNSET,
	SW_SH_FUNSTON_BEACH_SUNSET,
	SW_SH_TOTAL,
} sw_dummies_spherical_harmonics_presets_t;

extern const sw_dummies_spherical_harmonics_t sw_spherical_harmonics_presets[SW_SH_TOTAL];
extern const char *sw_spherical_harmonics_names[SW_SH_TOTAL];

typedef struct sw_dummies_rm_and_lighting {
	float max_steps;
	float max_dist;
	float surf_dist;
	float shininess;
	kr_vec3_t light_dir;
} sw_dummies_rm_and_lighting_t;

typedef struct sw_dummies_mc_settings {
	float cube_sidelen;
	float cubes_per_side;
	kr_vec3_t origin;
} sw_dummies_mc_settings_t;

sw_dummies_camera_t sw_dummies_default_camera(void);
sw_dummies_color_t sw_dummies_default_color(void);
sw_dummies_spherical_harmonics_t sw_dummies_default_spherical_harmonics(void);
sw_dummies_spherical_harmonics_t
sw_dummies_get_spherical_harmonics(sw_dummies_spherical_harmonics_presets_t p);
const char *sw_dummies_get_spherical_harmonics_name(sw_dummies_spherical_harmonics_presets_t p);
sw_dummies_rm_and_lighting_t sw_dummies_default_rm_and_lighting(void);
sw_dummies_mc_settings_t sw_dummies_default_mc_settings(void);
