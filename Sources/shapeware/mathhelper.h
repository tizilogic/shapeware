#pragma once

#include <krink/math/vector.h>
#include <math.h>

typedef struct sw_mat2x2 {
	float m00, m01, m10, m11;
} sw_mat2x2_t;

typedef struct sw_mat3x3 {
	float m00, m01, m02, m10, m11, m12, m20, m21, m22;
} sw_mat3x3_t;

static sw_mat3x3_t sw_mat3x3_compose(kr_vec3_t c0, kr_vec3_t c1, kr_vec3_t c2) {
	sw_mat3x3_t m;
	m.m00 = c0.x;
	m.m10 = c0.y;
	m.m20 = c0.z;
	m.m01 = c1.x;
	m.m11 = c1.y;
	m.m21 = c1.z;
	m.m02 = c2.x;
	m.m12 = c2.y;
	m.m22 = c2.z;
	return m;
}

static kr_vec3_t sw_mat3x3_multvec(sw_mat3x3_t m, kr_vec3_t v) {
	return (kr_vec3_t){
	    .x = m.m00 * v.x + m.m01 * v.y + m.m02 * v.z,
	    .y = m.m10 * v.x + m.m11 * v.y + m.m12 * v.z,
	    .z = m.m20 * v.x + m.m21 * v.y + m.m22 * v.z,
	};
}

static kr_vec2_t sw_mat2x2_multvec(sw_mat2x2_t m, kr_vec2_t v) {
	return (kr_vec2_t){.x = m.m00 * v.x + m.m10 * v.y, .y = m.m01 * v.x + m.m11 * v.y};
}

static kr_vec2_t sw_vec2_abs(kr_vec2_t vlh) {
	return (kr_vec2_t){.x = fabsf(vlh.x), .y = fabsf(vlh.y)};
}

static kr_vec2_t sw_vec2_maxf(kr_vec2_t vlh, float vrh) {
	return (kr_vec2_t){.x = fmaxf(vlh.x, vrh), .y = fmaxf(vlh.y, vrh)};
}

static kr_vec2_t sw_vec2_minf(kr_vec2_t vlh, float vrh) {
	return (kr_vec2_t){.x = fminf(vlh.x, vrh), .y = fminf(vlh.y, vrh)};
}

static kr_vec3_t sw_vec3_multv(kr_vec3_t vlh, kr_vec3_t vrh) {
	return (kr_vec3_t){.x = vlh.x * vrh.x, .y = vlh.y * vrh.y, .z = vlh.z * vrh.z};
}

static kr_vec3_t sw_vec3_divv(kr_vec3_t vlh, kr_vec3_t vrh) {
	return (kr_vec3_t){.x = vlh.x / vrh.x, .y = vlh.y / vrh.y, .z = vlh.z / vrh.z};
}

static kr_vec3_t sw_vec3_abs(kr_vec3_t vlh) {
	return (kr_vec3_t){.x = fabsf(vlh.x), .y = fabsf(vlh.y), .z = fabsf(vlh.z)};
}

static kr_vec3_t sw_vec3_maxv(kr_vec3_t vlh, kr_vec3_t vrh) {
	return (kr_vec3_t){
	    .x = fmaxf(vlh.x, vrh.x), .y = fmaxf(vlh.y, vrh.y), .z = fmaxf(vlh.z, vrh.z)};
}

static kr_vec3_t sw_vec3_minv(kr_vec3_t vlh, kr_vec3_t vrh) {
	return (kr_vec3_t){
	    .x = fminf(vlh.x, vrh.x), .y = fminf(vlh.y, vrh.y), .z = fminf(vlh.z, vrh.z)};
}

static kr_vec3_t sw_vec3_maxf(kr_vec3_t vlh, float vrh) {
	return (kr_vec3_t){.x = fmaxf(vlh.x, vrh), .y = fmaxf(vlh.y, vrh), .z = fmaxf(vlh.z, vrh)};
}

static kr_vec3_t sw_vec3_minf(kr_vec3_t vlh, float vrh) {
	return (kr_vec3_t){.x = fminf(vlh.x, vrh), .y = fminf(vlh.y, vrh), .z = fminf(vlh.z, vrh)};
}

static kr_vec3_t sw_vec3_invsign(kr_vec3_t v) {
	return (kr_vec3_t){.x = -v.x, .y = -v.y, .z = -v.z};
}

static kr_vec3_t sw_vec3_clampv(kr_vec3_t x, kr_vec3_t min_val, kr_vec3_t max_val) {
	return sw_vec3_minv(sw_vec3_maxv(x, min_val), max_val);
}

static kr_vec3_t sw_dumbround(kr_vec3_t v) {
	v = kr_vec3_addf(v, 0.5f);
	return (kr_vec3_t){.x = (float)((int)v.x), .y = (float)((int)v.y), .z = (float)((int)v.z)};
}

static kr_vec3_t sw_vec3_mod(kr_vec3_t x, kr_vec3_t y) {
	return (kr_vec3_t){
	    .x = x.x - floorf(x.x / y.x), .y = x.y - floorf(x.y / y.y), .z = x.y - floorf(x.y / y.y)};
}

static float sw_clampf(float x, float min_val, float max_val) {
	return fminf(fmaxf(x, min_val), max_val);
}

static float sw_signf(float x) {
	return x == 0.0f ? 0.0f : x > 0.0f ? 1.0f : -1.0f;
}
