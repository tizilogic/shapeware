#include "csg.h"
#include <kinc/log.h>
#include <assert.h>
#include <math.h>

static kr_vec2_t smin_cubic2(float a, float b, float k) {
	float h = fmaxf(k - fabsf(a - b), 0.0f) / k;
	float m = h * h * h * 0.5f;
	float s = m * k * (1.0f / 3.0f);
	return (a < b) ? (kr_vec2_t){a - s, m} : (kr_vec2_t){b - s, 1.0f - m};
}

static kr_vec2_t smax_cubic2(float a, float b, float k) {
	float h = fmaxf(k - fabsf(a - b), 0.0f) / k;
	float m = h * h * h * 0.5f;
	float s = m * k * (1.0f / 3.0f);
	return (a > b) ? (kr_vec2_t){a - s, m} : (kr_vec2_t){b - s, 1.0f - m};
}

static kr_vec4_t sdf_union_color(kr_vec4_t a, kr_vec4_t b) {
	return (a.w < b.w) ? a : b;
}

static kr_vec4_t sdf_subtraction_color(kr_vec4_t a, kr_vec4_t b) {
	return (-a.w > b.w) ? (kr_vec4_t){a.x, a.y, a.z, -a.w} : b;
}

static kr_vec4_t sdf_intersection_color(kr_vec4_t a, kr_vec4_t b) {
	return (a.w > b.w) ? a : b;
}

static kr_vec3_t mix(kr_vec3_t a, kr_vec3_t b, float f) {
	assert(f >= 0.0f && f <= 1.0f);
	float finv = f;
	f = 1.0f - f;
	return (kr_vec3_t){
	    .x = a.x * f + b.x * finv, .y = a.y * f + b.y * finv, .z = a.z * f + b.z * finv};
}

static kr_vec4_t sdf_smooth_union_color(kr_vec4_t a, kr_vec4_t b, float k) {
	kr_vec2_t sm = smin_cubic2(a.w, b.w, k);
	kr_vec3_t col = mix((kr_vec3_t){a.x, a.y, a.z}, (kr_vec3_t){b.x, b.y, b.z}, sm.y);
	return (kr_vec4_t){col.x, col.y, col.z, sm.x};
}

static kr_vec4_t sdf_smooth_subtraction_color(kr_vec4_t a, kr_vec4_t b, float k) {
	kr_vec2_t sm = smax_cubic2(-a.w, b.w, k);
	kr_vec3_t col = mix((kr_vec3_t){a.x, a.y, a.z}, (kr_vec3_t){b.x, b.y, b.z}, sm.y);
	return (kr_vec4_t){col.x, col.y, col.z, sm.x};
}

static kr_vec4_t sdf_smooth_intersection_color(kr_vec4_t a, kr_vec4_t b, float k) {
	kr_vec2_t sm = smax_cubic2(a.w, b.w, k);
	kr_vec3_t col = mix((kr_vec3_t){a.x, a.y, a.z}, (kr_vec3_t){b.x, b.y, b.z}, sm.y);
	return (kr_vec4_t){col.x, col.y, col.z, sm.x};
}

static float smin_cubic(float a, float b, float k) {
	float h = fmaxf(k - fabsf(a - b), 0.0f) / k;
	float m = h * h * h * 0.5f;
	float s = m * k * (1.0f / 3.0f);
	return (a < b) ? a - s : b - s;
}

static float smax_cubic(float a, float b, float k) {
	float h = fmaxf(k - fabsf(a - b), 0.0f) / k;
	float m = h * h * h * 0.5f;
	float s = m * k * (1.0f / 3.0f);
	return (a > b) ? a - s : b - s;
}

static float sdf_union(float a, float b) {
	return (a < b) ? a : b;
}

static float sdf_subtraction(float a, float b) {
	return (-a > b) ? -a : b;
}

static float sdf_intersection(float a, float b) {
	return (a > b) ? a : b;
}

static float sdf_smooth_union(float a, float b, float k) {
	return smin_cubic(a, b, k);
}

static float sdf_smooth_subtraction(float a, float b, float k) {
	return smax_cubic(-a, b, k);
}

static float sdf_smooth_intersection(float a, float b, float k) {
	return smax_cubic(a, b, k);
}

float sw_csg_evaluate(sw_type_t t, float a, float b, void *data) {
	switch (t) {
	case SW_CSG_UNION:
		return sdf_union(a, b);
	case SW_CSG_SUBTRACTION:
		return sdf_subtraction(a, b);
	case SW_CSG_INTERSECTION:
		return sdf_intersection(a, b);
	case SW_CSG_SMOOTH_UNION:
		return sdf_smooth_union(a, b, ((sw_csg_smooth_t *)data)->k);
	case SW_CSG_SMOOTH_SUBTRACTION:
		return sdf_smooth_subtraction(a, b, ((sw_csg_smooth_subtraction_t *)data)->k);
	case SW_CSG_SMOOTH_INTERSECTION:
		return sdf_smooth_intersection(a, b, ((sw_csg_smooth_t *)data)->k);

	default:
		kinc_log(KINC_LOG_LEVEL_WARNING, "Unknown CSG operation of type %d", t);
		break;
	}
	return INFINITY;
}

kr_vec4_t sw_csg_evaluate_color(sw_type_t t, kr_vec4_t a, kr_vec4_t b, void *data) {
	switch (t) {
	case SW_CSG_UNION:
		return sdf_union_color(a, b);
	case SW_CSG_SUBTRACTION:
		return sdf_subtraction_color(a, b);
	case SW_CSG_INTERSECTION:
		return sdf_intersection_color(a, b);
	case SW_CSG_SMOOTH_UNION:
		return sdf_smooth_union_color(a, b, ((sw_csg_smooth_t *)data)->k);
	case SW_CSG_SMOOTH_SUBTRACTION:
		return sdf_smooth_subtraction_color(a, b, ((sw_csg_smooth_subtraction_t *)data)->k);
	case SW_CSG_SMOOTH_INTERSECTION:
		return sdf_smooth_intersection_color(a, b, ((sw_csg_smooth_t *)data)->k);

	default:
		kinc_log(KINC_LOG_LEVEL_WARNING, "Unknown CSG operation of type %d", t);
		break;
	}
	return (kr_vec4_t){0.0f, 0.0f, 0.0f, INFINITY};
}
