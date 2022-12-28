#pragma once

#include "graph.h"
#include "shared.h"

#include <krink/math/vector.h>

typedef enum sw_csw_op {
	SW_CSW_UNION = SW_CSW_START,
	SW_CSW_SUBTRACTION,
	SW_CSW_INTERSECTION,
	SW_CSW_SMOOTH_UNION,
	SW_CSW_SMOOTH_SUBTRACTION,
	SW_CSW_SMOOTH_INTERSECTION,
} sw_csw_op_t;

typedef struct sw_csw_subtraction {
	int subtractor_id;
} sw_csw_subtraction_t;

typedef struct sw_csw_smooth {
	float k;
} sw_csw_smooth_t;

typedef struct sw_csw_smooth_subtraction {
	float k;
	int subtractor_id;
} sw_csw_smooth_subtraction_t;

float sw_csw_evaluate(sw_type_t t, float a, float b, void *data);
kr_vec4_t sw_csw_evaluate_color(sw_type_t t, kr_vec4_t a, kr_vec4_t b, void *data);
