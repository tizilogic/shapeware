#pragma once

#include "shared.h"
#include <krink/math/vector.h>

typedef enum sw_transform {
	SW_TRANSFORM_TRANSLATION = SW_TRANSFOM_START,
	SW_TRANSFORM_ROTATION,
} sw_transform_t;

typedef kr_vec3_t sw_transform_translation_t;
typedef kr_vec3_t sw_transform_rotation_t;

sw_transform_translation_t sw_transform_default_translation(void);
sw_transform_rotation_t sw_transform_default_rotation(void);
