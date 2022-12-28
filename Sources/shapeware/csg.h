#pragma once

#include "graph.h"
#include "shared.h"

#include <krink/math/vector.h>

typedef enum sw_csg_op {
	SW_CSG_UNION = SW_CSG_START,
	SW_CSG_SUBTRACTION,
	SW_CSG_INTERSECTION,
	SW_CSG_SMOOTH_UNION,
	SW_CSG_SMOOTH_SUBTRACTION,
	SW_CSG_SMOOTH_INTERSECTION,
} sw_csg_op_t;

typedef struct sw_csg_subtraction {
	int subtractor_id;
} sw_csg_subtraction_t;

typedef struct sw_csg_smooth {
	float k;
} sw_csg_smooth_t;

typedef struct sw_csg_smooth_subtraction {
	float k;
	int subtractor_id;
} sw_csg_smooth_subtraction_t;

float sw_csg_evaluate(sw_type_t t, float a, float b, void *data);
kr_vec4_t sw_csg_evaluate_color(sw_type_t t, kr_vec4_t a, kr_vec4_t b, void *data);
