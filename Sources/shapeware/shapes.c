#include "shapes.h"
#include "mathhelper.h"
#include <kinc/log.h>

kr_vec4_t sw_shapes_evaluate_color(sw_type_t t, void *data, kr_vec3_t pos) {
	sw_material_t *color = NULL;
	float distance = INFINITY;
	switch (t) {
	case SW_SHAPE_SPHERE: {
		sw_shapes_sphere_t *s = (sw_shapes_sphere_t *)data;
		color = &s->m;
		distance = kr_vec3_length(pos) - s->r;
	} break;
	case SW_SHAPE_ELLIPSOID: {
		sw_shapes_ellipsoid_t *s = (sw_shapes_ellipsoid_t *)data;
		color = &s->m;
		float k0 = kr_vec3_length(sw_vec3_divv(pos, s->r));
		float k1 = kr_vec3_length(sw_vec3_divv(pos, sw_vec3_multv(s->r, s->r)));
		distance = k0 * (k0 - 1.0f) / k1;
	} break;
	case SW_SHAPE_BOX: {
		sw_shapes_box_t *s = (sw_shapes_box_t *)data;
		color = &s->m;
		kr_vec3_t q = kr_vec3_subv(sw_vec3_abs(pos), s->b);
		distance = kr_vec3_length(sw_vec3_maxf(q, 0.0f)) + fminf(fmaxf(q.x, fmaxf(q.y, q.z)), 0.0f);
	} break;
	case SW_SHAPE_BOX_FRAME: {
		sw_shapes_box_frame_t *s = (sw_shapes_box_frame_t *)data;
		color = &s->m;
		pos = kr_vec3_subv(sw_vec3_abs(pos), s->b);
		kr_vec3_t q = kr_vec3_subf(sw_vec3_abs(kr_vec3_addf(pos, s->t)), s->t);
		distance = fminf(fminf(kr_vec3_length(sw_vec3_maxf((kr_vec3_t){pos.x, q.y, q.z}, 0.0f)) +
		                           fminf(fmaxf(pos.x, fmaxf(q.y, q.z)), 0.0f),
		                       kr_vec3_length(sw_vec3_maxf((kr_vec3_t){q.x, pos.y, q.z}, 0.0f)) +
		                           fminf(fmaxf(q.x, fmaxf(pos.y, q.z)), 0.0f)),
		                 kr_vec3_length(sw_vec3_maxf((kr_vec3_t){q.x, q.y, pos.z}, 0.0f)) +
		                     fminf(fmaxf(q.x, fmaxf(q.y, pos.z)), 0.0f));
	} break;
	case SW_SHAPE_TORUS: {
		sw_shapes_torus_t *s = (sw_shapes_torus_t *)data;
		color = &s->m;
		kr_vec2_t q = (kr_vec2_t){kr_vec2_length((kr_vec2_t){pos.x, pos.z}) - s->t.x, pos.y};
		distance = kr_vec2_length(q) - s->t.y;
	} break;
	case SW_SHAPE_CAPPED_TORUS: {
		sw_shapes_capped_torus_t *s = (sw_shapes_capped_torus_t *)data;
		color = &s->m;
		pos.x = fabsf(pos.x);
		float k = (s->r.y * pos.x > s->r.x * pos.y) ? kr_vec2_dot((kr_vec2_t){pos.x, pos.y}, s->r)
		                                            : kr_vec2_length((kr_vec2_t){pos.x, pos.y});
		distance = sqrtf(kr_vec3_dot(pos, pos) + s->t.x * s->t.x - 2.0f * s->t.x * k) - s->t.y;
	} break;
	case SW_SHAPE_LINK: {
		sw_shapes_link_t *s = (sw_shapes_link_t *)data;
		color = &s->m;
		kr_vec3_t q = (kr_vec3_t){pos.x, fmaxf(fabsf(pos.y) - s->le, 0.0f), pos.z};
		distance =
		    kr_vec2_length((kr_vec2_t){kr_vec2_length((kr_vec2_t){q.x, q.y}) - s->r1, q.z}) - s->r2;
	} break;
	case SW_SHAPE_PLANE: {
		sw_shapes_plane_t *s = (sw_shapes_plane_t *)data;
		color = &s->m;
		distance = kr_vec3_dot(pos, s->n) + s->h;
	} break;
	case SW_SHAPE_HEX_PRISM: {
		sw_shapes_hex_prism_t *s = (sw_shapes_hex_prism_t *)data;
		color = &s->m;
		const kr_vec3_t k = (kr_vec3_t){-0.8660254f, 0.5f, 0.57735f};
		pos = sw_vec3_abs(pos);
		float mindotxy = fminf(kr_vec2_dot((kr_vec2_t){k.x, k.y}, (kr_vec2_t){pos.x, pos.y}), 0.0f);
		pos.x -= 2.0f * mindotxy * k.x;
		pos.y -= 2.0f * mindotxy * k.y;
		kr_vec2_t d = (kr_vec2_t){
		    kr_vec2_length(kr_vec2_subv(
		        (kr_vec2_t){pos.x, pos.y},
		        kr_vec2_mult((kr_vec2_t){sw_clampf(pos.x, -k.z * s->h.x, k.z * s->h.x), s->h.x},
		                     sw_signf(pos.y - s->h.x)))),
		    pos.z - s->h.y};
		distance = fminf(fmaxf(d.x, d.y), 0.0f) + kr_vec2_length(sw_vec2_maxf(d, 0.0f));
	} break;
	case SW_SHAPE_TRI_PRISM: {
		sw_shapes_tri_prism_t *s = (sw_shapes_tri_prism_t *)data;
		color = &s->m;
		kr_vec3_t q = sw_vec3_abs(pos);
		distance =
		    fmaxf(q.z - s->h.y, fmaxf(q.x * 0.866025f + pos.y * 0.5f, -pos.y) - s->h.x * 0.5f);
	} break;
	case SW_SHAPE_CAPSULE: {
		sw_shapes_capsule_t *s = (sw_shapes_capsule_t *)data;
		color = &s->m;
		kr_vec3_t pa = kr_vec3_subv(pos, s->a);
		kr_vec3_t ba = kr_vec3_subv(s->b, s->a);
		float h = sw_clampf(kr_vec3_dot(pa, ba) / kr_vec3_dot(ba, ba), 0.0f, 1.0f);
		distance = kr_vec3_length(kr_vec3_subv(pa, kr_vec3_mult(ba, h))) - s->r;
	} break;
	case SW_SHAPE_CAPPED_CYLINDER: {
		sw_shapes_capped_cylinder_t *s = (sw_shapes_capped_cylinder_t *)data;
		color = &s->m;
		kr_vec2_t d =
		    kr_vec2_subv(sw_vec2_abs((kr_vec2_t){kr_vec2_length((kr_vec2_t){pos.x, pos.z}), pos.y}),
		                 (kr_vec2_t){s->r, s->h});
		distance = fmin(fmax(d.x, d.y), 0.0f) + kr_vec2_length(sw_vec2_maxf(d, 0.0f));
	} break;
	case SW_SHAPE_CAPPED_CONE: {
		// Used another method to compute the SDF, unsure why this works..
		sw_shapes_capped_cone_t *s = (sw_shapes_capped_cone_t *)data;
		color = &s->m;
		float ra = s->r2;
		float rb = s->r1;
		kr_vec3_t a = (kr_vec3_t){.x = 0, .y = s->h, .z = 0};
		kr_vec3_t b = (kr_vec3_t){.x = 0, .y = -s->h, .z = 0};
		float rba = rb - ra;
		float baba = kr_vec3_dot(kr_vec3_subv(b, a), kr_vec3_subv(b, a));
		float papa = kr_vec3_dot(kr_vec3_subv(pos, a), kr_vec3_subv(pos, a));
		float paba = kr_vec3_dot(kr_vec3_subv(pos, a), kr_vec3_subv(b, a)) / baba;
		float x = sqrtf(papa - paba * paba * baba);
		float cax = fmaxf(0, x - ((paba < 0.5f) ? ra : rb));
		float cay = fabsf(paba - 0.5f) - 0.5f;
		float k = rba * rba + baba;
		float f = sw_clampf((rba * (x - ra) + paba * baba) / k, 0, 1);
		float cbx = x - ra - f * rba;
		float cby = paba - f;
		float ss = (cbx < 0.0 && cay < 0.0) ? -1.0 : 1.0;
		distance = ss * sqrtf(fminf(cax * cax + cay * cay * baba, cbx * cbx + cby * cby * baba));
	} break;
	case SW_SHAPE_SOLID_ANGLE: {
		sw_shapes_solid_angle_t *s = (sw_shapes_solid_angle_t *)data;
		color = &s->m;
		kr_vec2_t q = (kr_vec2_t){kr_vec2_length((kr_vec2_t){pos.x, pos.z}), pos.y};
		float l = kr_vec2_length(q) - s->r;
		float m = kr_vec2_length(
		    kr_vec2_subv(q, kr_vec2_mult(s->sc, sw_clampf(kr_vec2_dot(q, s->sc), 0.0f, s->r))));
		distance = fmaxf(l, m * sw_signf(s->sc.y * q.x - s->sc.x * q.y));
	} break;
	case SW_SHAPE_CUT_SPHERE: {
		sw_shapes_cut_sphere_t *s = (sw_shapes_cut_sphere_t *)data;
		color = &s->m;
		float w = sqrtf(s->r * s->r - s->h * s->h);
		kr_vec2_t q = (kr_vec2_t){kr_vec2_length((kr_vec2_t){pos.x, pos.z}), pos.y};
		float ss = fmaxf((s->h - s->r) * q.x * q.x + w * w * (s->h + s->r - 2.0f * q.y),
		                 s->h * q.x - w * q.y);
		distance = (ss < 0.0)  ? kr_vec2_length(q) - s->r
		           : (q.x < w) ? s->h - q.y
		                       : kr_vec2_length(kr_vec2_subv(q, (kr_vec2_t){w, s->h}));
	} break;
	case SW_SHAPE_CUT_HOLLOW_SPHERE: {
		sw_shapes_cut_hollow_sphere_t *s = (sw_shapes_cut_hollow_sphere_t *)data;
		color = &s->m;
		float w = sqrtf(s->r * s->r - s->h * s->h);
		kr_vec2_t q = (kr_vec2_t){kr_vec2_length((kr_vec2_t){pos.x, pos.z}), pos.y};
		distance = ((s->h * q.x < w * q.y) ? kr_vec2_length(kr_vec2_subv(q, (kr_vec2_t){w, s->h}))
		                                   : fabsf(kr_vec2_length(q) - s->r)) -
		           s->t;
	} break;
	case SW_SHAPE_DEATH_STAR: {
		sw_shapes_death_star_t *s = (sw_shapes_death_star_t *)data;
		color = &s->m;
		float a = (s->ra * s->ra - s->rb * s->rb + s->d * s->d) / (2.0f * s->d);
		float b = sqrtf(fmaxf(s->ra * s->ra - a * a, 0.0f));
		kr_vec2_t p = (kr_vec2_t){pos.x, kr_vec2_length((kr_vec2_t){pos.y, pos.z})};
		if (p.x * b - p.y * a > s->d * fmaxf(b - p.y, 0.0f))
			distance = kr_vec2_length(kr_vec2_subv(p, (kr_vec2_t){a, b}));
		else
			distance = fmaxf((kr_vec2_length(p) - s->ra),
			                 -(kr_vec2_length(kr_vec2_subv(p, (kr_vec2_t){s->d, 0.0f})) - s->rb));
	} break;
	case SW_SHAPE_ROUND_CONE: {
		sw_shapes_round_cone_t *s = (sw_shapes_round_cone_t *)data;
		color = &s->m;
		float b = (s->r1 - s->r2) / s->h;
		float a = sqrtf(1.0f - b * b);
		kr_vec2_t q = (kr_vec2_t){kr_vec2_length((kr_vec2_t){pos.x, pos.z}), pos.y};
		float k = kr_vec2_dot(q, (kr_vec2_t){-b, a});
		if (k < 0.0f)
			distance = kr_vec2_length(q) - s->r1;
		else if (k > a * s->h)
			distance = kr_vec2_length(kr_vec2_subv(q, (kr_vec2_t){0.0f, s->h})) - s->r2;
		else
			distance = kr_vec2_dot(q, (kr_vec2_t){a, b}) - s->r1;
	} break;
	case SW_SHAPE_OCTAHEDRON: {
		sw_shapes_octahedron_t *s = (sw_shapes_octahedron_t *)data;
		color = &s->m;
		pos = sw_vec3_abs(pos);
		float m = pos.x + pos.y + pos.z - s->s;
		kr_vec3_t q;
		if (3.0f * pos.x < m)
			q = pos;
		else if (3.0f * pos.y < m)
			q = (kr_vec3_t){pos.y, pos.z, pos.x};
		else if (3.0f * pos.z < m)
			q = (kr_vec3_t){pos.z, pos.x, pos.y};
		else
			return (kr_vec4_t){s->m.r, s->m.g, s->m.b, m * 0.57735027f};

		float k = sw_clampf(0.5f * (q.z - q.y + s->s), 0.0f, s->s);
		distance = kr_vec3_length((kr_vec3_t){q.x, q.y - s->s + k, q.z - k});
	} break;

	default: {
		kinc_log(KINC_LOG_LEVEL_WARNING, "Unknown shape of type %d", t);
		return (kr_vec4_t){0.0f, 0.0f, 0.0f, INFINITY};
	} break;
	}
	return (kr_vec4_t){color->r, color->g, color->b, distance};
}

float sw_shapes_evaluate(sw_type_t t, void *data, kr_vec3_t pos) {
	return sw_shapes_evaluate_color(t, data, pos).w;
}

sw_shapes_sphere_t sw_shapes_default_sphere(void) {
	return (sw_shapes_sphere_t){.m = {.r = 1.0f, .g = 1.0f, .b = 1.0f}, .r = 1.0f};
}

sw_shapes_ellipsoid_t sw_shapes_default_ellipsoid(void) {
	return (sw_shapes_ellipsoid_t){.m = {.r = 1.0f, .g = 1.0f, .b = 1.0f},
	                               .r = {.x = 1.0f, .y = 1.0f, .z = 1.0f}};
}

sw_shapes_box_t sw_shapes_default_box(void) {
	return (sw_shapes_box_t){.m = {.r = 1.0f, .g = 1.0f, .b = 1.0f},
	                         .b = {.x = 1.0f, .y = 1.0f, .z = 1.0f}};
}

sw_shapes_box_frame_t sw_shapes_default_box_frame(void) {
	return (sw_shapes_box_frame_t){
	    .m = {.r = 1.0f, .g = 1.0f, .b = 1.0f}, .t = 0.1f, .b = {.x = 1.0f, .y = 1.0f, .z = 1.0f}};
}

sw_shapes_torus_t sw_shapes_default_torus(void) {
	return (sw_shapes_torus_t){.m = {.r = 1.0f, .g = 1.0f, .b = 1.0f},
	                           .t = {.x = 1.0f, .y = 0.25f}};
}

sw_shapes_capped_torus_t sw_shapes_default_capped_torus(void) {
	return (sw_shapes_capped_torus_t){.m = {.r = 1.0f, .g = 1.0f, .b = 1.0f},
	                                  .t = {.x = 1.0f, .y = 0.25f},
	                                  .r = {.x = 0.866025f, .y = -0.5f}};
}

sw_shapes_link_t sw_shapes_default_link(void) {
	return (sw_shapes_link_t){
	    .m = {.r = 1.0f, .g = 1.0f, .b = 1.0f}, .le = 1.0f, .r1 = 0.5f, .r2 = 0.25f};
}

sw_shapes_plane_t sw_shapes_default_plane(void) {
	return (sw_shapes_plane_t){
	    .m = {.r = 1.0f, .g = 1.0f, .b = 1.0f}, .n = {.x = 0.0f, .y = 1.0f, .z = 0.0f}, .h = 15.0f};
}

sw_shapes_hex_prism_t sw_shapes_default_hex_prism(void) {
	return (sw_shapes_hex_prism_t){.m = {.r = 1.0f, .g = 1.0f, .b = 1.0f},
	                               .h = {.x = 1.0f, .y = 0.25f}};
}

sw_shapes_tri_prism_t sw_shapes_default_tri_prism(void) {
	return sw_shapes_default_hex_prism();
}

sw_shapes_capsule_t sw_shapes_default_capsule(void) {
	return (sw_shapes_capsule_t){.m = {.r = 1.0f, .g = 1.0f, .b = 1.0f},
	                             .a = {.x = -1.0f, .y = 0.0f, .z = 0.0f},
	                             .b = {.x = 1.0f, .y = 0.0f, .z = 0.0f},
	                             .r = 0.25f};
}

sw_shapes_capped_cylinder_t sw_shapes_default_capped_cylinder(void) {
	return (sw_shapes_capped_cylinder_t){
	    .m = {.r = 1.0f, .g = 1.0f, .b = 1.0f}, .h = 1.0f, .r = 0.25f};
}

sw_shapes_capped_cone_t sw_shapes_default_capped_cone(void) {
	return (sw_shapes_capped_cone_t){
	    .m = {.r = 1.0f, .g = 1.0f, .b = 1.0f}, .h = 1.0f, .r1 = 0.3f, .r2 = 0.05f};
}

sw_shapes_solid_angle_t sw_shapes_default_solid_angle(void) {
	return (sw_shapes_solid_angle_t){
	    .m = {.r = 1.0f, .g = 1.0f, .b = 1.0f}, .sc = {.x = 0.5f, .y = 0.866025f}, .r = 1.0f};
}

sw_shapes_cut_sphere_t sw_shapes_default_cut_sphere(void) {
	return (sw_shapes_cut_sphere_t){.m = {.r = 1.0f, .g = 1.0f, .b = 1.0f}, .r = 1.0f, .h = -0.2f};
}

sw_shapes_cut_hollow_sphere_t sw_shapes_default_cut_hollow_sphere(void) {
	return (sw_shapes_cut_hollow_sphere_t){
	    .m = {.r = 1.0f, .g = 1.0f, .b = 1.0f}, .r = 1.0f, .h = -0.2f, .t = 0.1f};
}

sw_shapes_death_star_t sw_shapes_default_death_star(void) {
	return (sw_shapes_death_star_t){
	    .m = {.r = 1.0f, .g = 1.0f, .b = 1.0f}, .ra = 1.0f, .rb = 0.7f, .d = 0.5f};
}

sw_shapes_round_cone_t sw_shapes_default_round_cone(void) {
	return (sw_shapes_round_cone_t){
	    .m = {.r = 1.0f, .g = 1.0f, .b = 1.0f}, .r1 = 0.7f, .r2 = 0.2f, .h = 1.0f};
}

sw_shapes_octahedron_t sw_shapes_default_octahedron(void) {
	return (sw_shapes_octahedron_t){.m = {.r = 1.0f, .g = 1.0f, .b = 1.0f}, .s = 1.0f};
}
