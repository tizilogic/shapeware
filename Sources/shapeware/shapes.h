#pragma once

#include "graph.h"
#include "shared.h"
#include <krink/math/vector.h>

typedef enum sw_shape_type {
	SW_SHAPE_SPHERE = SW_SHAPES_START,
	SW_SHAPE_ELLIPSOID,
	SW_SHAPE_BOX,
	SW_SHAPE_BOX_FRAME,
	SW_SHAPE_TORUS,
	SW_SHAPE_CAPPED_TORUS,
	SW_SHAPE_LINK,
	SW_SHAPE_PLANE,
	SW_SHAPE_HEX_PRISM,
	SW_SHAPE_TRI_PRISM,
	SW_SHAPE_CAPSULE,
	SW_SHAPE_CAPPED_CYLINDER,
	SW_SHAPE_CAPPED_CONE,
	SW_SHAPE_SOLID_ANGLE,
	SW_SHAPE_CUT_SPHERE,
	SW_SHAPE_CUT_HOLLOW_SPHERE,
	SW_SHAPE_DEATH_STAR,
	SW_SHAPE_ROUND_CONE,
	SW_SHAPE_OCTAHEDRON,
} sw_shape_type_t;

typedef struct sw_material {
	float r, g, b;
} sw_material_t;

typedef struct sw_shapes_sphere {
	float r;
	sw_material_t m;
} sw_shapes_sphere_t;

typedef struct sw_shapes_ellipsoid {
	kr_vec3_t r;
	sw_material_t m;
} sw_shapes_ellipsoid_t;

typedef struct sw_shapes_box {
	kr_vec3_t b;
	sw_material_t m;
} sw_shapes_box_t;

typedef struct sw_shapes_box_frame {
	kr_vec3_t b;
	float t;
	sw_material_t m;
} sw_shapes_box_frame_t;

typedef struct sw_shapes_torus {
	kr_vec2_t t;
	sw_material_t m;
} sw_shapes_torus_t;

typedef struct sw_shapes_capped_torus {
	kr_vec2_t t;
	kr_vec2_t r;
	sw_material_t m;
} sw_shapes_capped_torus_t;

typedef struct sw_shapes_link {
	float le;
	float r1;
	float r2;
	sw_material_t m;
} sw_shapes_link_t;

typedef struct sw_shapes_plane {
	kr_vec3_t n;
	float h;
	sw_material_t m;
} sw_shapes_plane_t;

typedef struct sw_shapes_hex_prism {
	kr_vec2_t h;
	sw_material_t m;
} sw_shapes_hex_prism_t;

typedef sw_shapes_hex_prism_t sw_shapes_tri_prism_t;

typedef struct sw_shapes_capsule {
	kr_vec3_t a;
	kr_vec3_t b;
	float r;
	sw_material_t m;
} sw_shapes_capsule_t;

typedef struct sw_shapes_capped_cylinder {
	float r;
	float h;
	sw_material_t m;
} sw_shapes_capped_cylinder_t;

typedef struct sw_shapes_capped_cone {
	float h;
	float r1;
	float r2;
	sw_material_t m;
} sw_shapes_capped_cone_t;

typedef struct sw_shapes_solid_angle {
	kr_vec2_t sc;
	float r;
	sw_material_t m;
} sw_shapes_solid_angle_t;

typedef struct sw_shapes_cut_sphere {
	float r;
	float h;
	sw_material_t m;
} sw_shapes_cut_sphere_t;

typedef struct sw_shapes_cut_hollow_sphere {
	float r;
	float h;
	float t;
	sw_material_t m;
} sw_shapes_cut_hollow_sphere_t;

typedef struct sw_shapes_death_star {
	float ra;
	float rb;
	float d;
	sw_material_t m;
} sw_shapes_death_star_t;

typedef struct sw_shapes_round_cone {
	float r1;
	float r2;
	float h;
	sw_material_t m;
} sw_shapes_round_cone_t;

typedef struct sw_shapes_octahedron {
	float s;
	sw_material_t m;
} sw_shapes_octahedron_t;

float sw_shapes_evaluate(sw_type_t t, void *data, kr_vec3_t pos);
kr_vec4_t sw_shapes_evaluate_color(sw_type_t t, void *data, kr_vec3_t pos);
sw_shapes_sphere_t sw_shapes_default_sphere(void);
sw_shapes_ellipsoid_t sw_shapes_default_ellipsoid(void);
sw_shapes_box_t sw_shapes_default_box(void);
sw_shapes_box_frame_t sw_shapes_default_box_frame(void);
sw_shapes_torus_t sw_shapes_default_torus(void);
sw_shapes_capped_torus_t sw_shapes_default_capped_torus(void);
sw_shapes_link_t sw_shapes_default_link(void);
sw_shapes_plane_t sw_shapes_default_plane(void);
sw_shapes_hex_prism_t sw_shapes_default_hex_prism(void);
sw_shapes_tri_prism_t sw_shapes_default_tri_prism(void);
sw_shapes_capsule_t sw_shapes_default_capsule(void);
sw_shapes_capped_cylinder_t sw_shapes_default_capped_cylinder(void);
sw_shapes_capped_cone_t sw_shapes_default_capped_cone(void);
sw_shapes_solid_angle_t sw_shapes_default_solid_angle(void);
sw_shapes_cut_sphere_t sw_shapes_default_cut_sphere(void);
sw_shapes_cut_hollow_sphere_t sw_shapes_default_cut_hollow_sphere(void);
sw_shapes_death_star_t sw_shapes_default_death_star(void);
sw_shapes_round_cone_t sw_shapes_default_round_cone(void);
sw_shapes_octahedron_t sw_shapes_default_octahedron(void);
