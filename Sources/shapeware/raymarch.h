#pragma once

#include "sdf.h"
#include <krink/math/vector.h>
#include <stdbool.h>

kr_vec3_t sw_raymarch_ray_direction(kr_vec3_t origin, kr_vec3_t look_at, kr_vec2_t frag_pos,
                                    float focal_length);
kr_vec3_t sw_raymarch_surface_pos(sw_sdf_t *sdf, kr_vec3_t origin, kr_vec3_t direction,
                                  int max_steps, float surf_dist, float max_dist, bool *hit);
kr_vec3_t sw_raymarch_surface_normal(sw_sdf_t *sdf, kr_vec3_t pos);
