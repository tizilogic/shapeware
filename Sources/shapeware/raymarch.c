#include "raymarch.h"

#include "mathhelper.h"
#include <math.h>

kr_vec3_t sw_raymarch_ray_direction(kr_vec3_t origin, kr_vec3_t look_at, kr_vec2_t frag_pos,
                                    float focal_length) {
	kr_vec3_t cw = kr_vec3_normalized(kr_vec3_subv(look_at, origin));
	kr_vec3_t cp = (kr_vec3_t){sinf(0.0f), cosf(0.0f), 0.0f};
	kr_vec3_t cu = kr_vec3_normalized(kr_vec3_cross(cw, cp));
	kr_vec3_t cv = kr_vec3_cross(cu, cw);
	sw_mat3x3_t ca = sw_mat3x3_compose(cu, cv, cw);
	return sw_mat3x3_multvec(ca,
	                         kr_vec3_normalized((kr_vec3_t){frag_pos.x, frag_pos.y, focal_length}));
}

kr_vec3_t sw_raymarch_surface_pos(sw_sdf_t *sdf, sw_sdf_stack_frame_t *stack, kr_vec3_t origin,
                                  kr_vec3_t direction, int max_steps, float surf_dist,
                                  float max_dist, bool *hit) {
	float offset = 0.0f;
	*hit = true;
	for (int i = 0; i < max_steps; i++) {
		kr_vec3_t p = kr_vec3_addv(origin, kr_vec3_mult(direction, offset));
		float dist = sw_sdf_compute(sdf, p, stack);
		offset += dist;
		if (fabsf(dist) < surf_dist) return p;
		if (offset > max_dist) break;
	}
	*hit = false;
	return (kr_vec3_t){0.0f};
}

kr_vec3_t sw_raymarch_surface_normal(sw_sdf_t *sdf, sw_sdf_stack_frame_t *stack, kr_vec3_t pos) {
	const float h = 0.001f;
	const kr_vec3_t xyy = (kr_vec3_t){.x = 1.0f, .y = -1.0f, .z = -1.0f};
	const kr_vec3_t yyx = (kr_vec3_t){.x = -1.0f, .y = -1.0f, .z = 1.0f};
	const kr_vec3_t yxy = (kr_vec3_t){.x = -1.0f, .y = 1.0f, .z = -1.0f};
	const kr_vec3_t xxx = (kr_vec3_t){.x = 1.0f, .y = 1.0f, .z = 1.0f};

	return kr_vec3_normalized(kr_vec3_addv(
	    kr_vec3_addv(
	        kr_vec3_addv(
	            kr_vec3_mult(xyy,
	                         sw_sdf_compute(sdf, kr_vec3_addv(pos, kr_vec3_mult(xyy, h)), stack)),
	            kr_vec3_mult(yyx,
	                         sw_sdf_compute(sdf, kr_vec3_addv(pos, kr_vec3_mult(yyx, h)), stack))),
	        kr_vec3_mult(yxy, sw_sdf_compute(sdf, kr_vec3_addv(pos, kr_vec3_mult(yxy, h)), stack))),
	    kr_vec3_mult(xxx, sw_sdf_compute(sdf, kr_vec3_addv(pos, kr_vec3_mult(xxx, h)), stack))));
}
