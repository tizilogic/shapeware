#pragma once

#include "graph.h"
#include "shared.h"
#include <krink/math/vector.h>
#include <stdint.h>

typedef enum sw_op_type {
	SW_OPS_MIRROR = SW_OPS_START,
	SW_OPS_ROUND,
	SW_OPS_ONION,
	SW_OPS_ELONGATE,
	SW_OPS_BEND,
	SW_OPS_REPEAT,
	SW_OPS_REPEAT_INF,
	SW_OPS_STEP_REDUCTION,
	SW_OPS_TWIST,
	SW_OPS_SIN_DISPLACEMENT,
} sw_op_type_t;

typedef enum sw_ops_mirror_flag {
	SW_MIRROR_X = 1,
	SW_MIRROR_Y = 2,
	SW_MIRROR_Z = 4,
} sw_ops_mirror_flag_t;

typedef struct sw_mirror {
	uint32_t mirror_flags;
} sw_ops_mirror_t;

typedef float sw_ops_round_t;

typedef float sw_ops_onion_t;

typedef kr_vec3_t sw_ops_elongate_t;

typedef float sw_ops_bend_t;

typedef struct sw_ops_repeat {
	kr_vec3_t c;
	kr_vec3_t l;
} sw_ops_repeat_t;

typedef kr_vec3_t sw_ops_repeat_inf_t;

typedef float sw_ops_step_reduction_t;

typedef float sw_ops_twist_t;

typedef struct sw_ops_sin_displacement {
	kr_vec3_t frequency;
	float amplitude;
} sw_ops_sin_displacement_t;

kr_vec3_t sw_ops_evaluate_pos(sw_type_t t, kr_vec3_t pos, void *data);
float sw_ops_evaluate_dist(sw_type_t t, float a, kr_vec3_t pos, void *data);
sw_ops_mirror_t sw_ops_default_mirror(void);
sw_ops_round_t sw_ops_default_round(void);
sw_ops_onion_t sw_ops_default_onion(void);
sw_ops_elongate_t sw_ops_default_elongate(void);
sw_ops_bend_t sw_ops_default_bend(void);
sw_ops_repeat_t sw_ops_default_repeat(void);
sw_ops_repeat_inf_t sw_ops_default_repeat_inf(void);
sw_ops_step_reduction_t sw_ops_default_step_reduction(void);
sw_ops_twist_t sw_ops_default_twist(void);
sw_ops_sin_displacement_t sw_ops_default_sin_displacement(void);
