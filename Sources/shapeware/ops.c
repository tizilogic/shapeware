#include "ops.h"

#include "mathhelper.h"
#include <kinc/log.h>
#include <math.h>

kr_vec3_t sw_ops_evaluate_pos(sw_type_t t, kr_vec3_t pos, void *data) {
	switch (t) {
	case SW_OPS_MIRROR: {
		sw_ops_mirror_t *op = (sw_ops_mirror_t *)data;
		if ((op->mirror_flags & SW_MIRROR_X) > 0) pos.x = fabsf(pos.x);
		if ((op->mirror_flags & SW_MIRROR_Y) > 0) pos.y = fabsf(pos.y);
		if ((op->mirror_flags & SW_MIRROR_Z) > 0) pos.z = fabsf(pos.z);
	} break;
	case SW_OPS_ELONGATE: {
		sw_ops_elongate_t *op = (sw_ops_elongate_t *)data;
		kr_vec3_t h = (kr_vec3_t){op->x, op->y, op->z};
		pos = kr_vec3_subv(pos, sw_vec3_clampv(pos, sw_vec3_invsign(h), h));
	} break;
	case SW_OPS_BEND: {
		sw_ops_bend_t *op = (sw_ops_bend_t *)data;
		float c = cosf(*op * pos.x);
		float s = sinf(*op * pos.x);
		sw_mat2x2_t m = (sw_mat2x2_t){c, -s, s, c};
		kr_vec2_t r = sw_mat2x2_multvec(m, (kr_vec2_t){pos.x, pos.y});
		pos = (kr_vec3_t){r.x, r.y, pos.z};
	} break;
	case SW_OPS_REPEAT: {
		sw_ops_repeat_t *op = (sw_ops_repeat_t *)data;
		pos = kr_vec3_subv(
		    pos, sw_vec3_multv(op->c, sw_vec3_clampv(sw_dumbround(sw_vec3_divv(pos, op->c)),
		                                             sw_vec3_invsign(op->l), op->l)));
	} break;
	case SW_OPS_REPEAT_INF: {
		sw_ops_repeat_inf_t *op = (sw_ops_repeat_inf_t *)data;
		pos = kr_vec3_subv(sw_vec3_mod(kr_vec3_addv(pos, kr_vec3_mult(*op, 0.5f)), *op),
		                   kr_vec3_mult(*op, 0.5f));
	} break;
	case SW_OPS_TWIST: {
		sw_ops_twist_t *op = (sw_ops_twist_t *)data;
		float c = cosf(*op * pos.y);
		float s = sinf(*op * pos.y);
		sw_mat2x2_t m = (sw_mat2x2_t){c, -s, s, c};
		kr_vec2_t t = sw_mat2x2_multvec(m, (kr_vec2_t){pos.x, pos.z});
		pos = (kr_vec3_t){t.x, pos.y, t.y};
	} break;
	// No pos change
	case SW_OPS_ROUND:
	case SW_OPS_ONION:
	case SW_OPS_STEP_REDUCTION:
	case SW_OPS_SIN_DISPLACEMENT:
		break;

	default:
		kinc_log(KINC_LOG_LEVEL_WARNING, "Unknown OP of type %d", t);
		break;
	}
	return pos;
}

float sw_ops_evaluate_dist(sw_type_t t, float a, kr_vec3_t pos, void *data) {
	switch (t) {
	case SW_OPS_ROUND: {
		sw_ops_round_t *op = (sw_ops_round_t *)data;
		a -= *op;
	} break;
	case SW_OPS_ONION: {
		sw_ops_onion_t *op = (sw_ops_onion_t *)data;
		a = fabsf(a) - *op;
	} break;
	case SW_OPS_STEP_REDUCTION: {
		sw_ops_step_reduction_t *op = (sw_ops_step_reduction_t *)data;
		a *= *op;
	} break;
	case SW_OPS_SIN_DISPLACEMENT: {
		sw_ops_sin_displacement_t *op = (sw_ops_sin_displacement_t *)data;
		kr_vec3_t f = (kr_vec3_t){sinf(op->frequency.x * pos.x), sinf(op->frequency.y * pos.y),
		                          sinf(op->frequency.z * pos.z)};
		a += (f.x * f.y * f.z) * op->amplitude;
	} break;
	// No dist change
	case SW_OPS_MIRROR:
	case SW_OPS_ELONGATE:
	case SW_OPS_BEND:
	case SW_OPS_REPEAT:
	case SW_OPS_REPEAT_INF:
	case SW_OPS_TWIST:
		break;

	default:
		kinc_log(KINC_LOG_LEVEL_WARNING, "Unknown OP of type %d", t);
		break;
	}
	return a;
}

sw_ops_mirror_t sw_ops_default_mirror(void) {
	return (sw_ops_mirror_t){.mirror_flags = SW_MIRROR_X};
}

sw_ops_round_t sw_ops_default_round(void) {
	return 0.2f;
}

sw_ops_onion_t sw_ops_default_onion(void) {
	return 0.2f;
}

sw_ops_elongate_t sw_ops_default_elongate(void) {
	return (sw_ops_elongate_t){.x = 0.5f, .y = 0.25f, .z = 0.0f};
}

sw_ops_bend_t sw_ops_default_bend(void) {
	return 0.2f;
}

sw_ops_repeat_t sw_ops_default_repeat(void) {
	return (sw_ops_repeat_t){.c = sw_ops_default_repeat_inf(),
	                         .l = {.x = 5.0f, .y = 2.0f, .z = 1.0f}};
}

sw_ops_repeat_inf_t sw_ops_default_repeat_inf(void) {
	return (sw_ops_repeat_inf_t){.x = 3.0f, .y = 3.0f, .z = 3.0f};
}

sw_ops_step_reduction_t sw_ops_default_step_reduction(void) {
	return 0.5f;
}

sw_ops_twist_t sw_ops_default_twist(void) {
	return 3.0f;
}

sw_ops_sin_displacement_t sw_ops_default_sin_displacement(void) {
	return (sw_ops_sin_displacement_t){.frequency = {.x = 20.0f, .y = 20.0f, .z = 20.0f},
	                                   .amplitude = 0.03f};
}
