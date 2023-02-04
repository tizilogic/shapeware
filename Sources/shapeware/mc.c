/*
This is adapted from this article from Paul Bourke:
http://paulbourke.net/geometry/polygonise/
*/

#include "mc.h"
#include "mtables.h"

#include <math.h>
#include <stdbool.h>

typedef struct gridcell {
	kr_vec3_t p[8];
	float val[8];
} gridcell_t;

typedef struct gridcell_color {
	kr_vec3_t p[8];
	kr_vec4_t val[8];
} gridcell_color_t;

static bool kr_vec3_lt(const kr_vec3_t left, const kr_vec3_t right) {
	if (left.x < right.x)
		return true;
	else if (left.x > right.x)
		return false;

	if (left.y < right.y)
		return true;
	else if (left.y > right.y)
		return false;

	if (left.z < right.z)
		return true;
	else if (left.z > right.z)
		return false;

	return false;
}

static kr_vec3_t vertex_interpolate(float isolevel, kr_vec3_t p1, kr_vec3_t p2, float valp1,
                                    float valp2) {
	float t = (fabsf(valp2 - valp1) > 1e-5f) ? (isolevel - valp1) / (valp2 - valp1) : 0.5f;
	t = fmaxf(fminf(t, 1.0f), 0.0f);
	return kr_vec3_addv(p1, kr_vec3_mult(kr_vec3_subv(p2, p1), t));
}

/*
   Given a grid cell and an isolevel, calculate the triangular
   facets required to represent the isosurface through the cell.
   Return the number of triangular facets, the array "triangles"
   will be loaded up with the vertices at most 5 triangular facets.
    0 will be returned if the grid cell is either totally above
   of totally below the isolevel.
*/
static void polygonise(gridcell_t grid, float isolevel, sw_add_triangle_func_t triangle_cb,
                       void *t_param) {
	kr_vec3_t vertlist[12];
	/*
	   Determine the index into the edge table which
	   tells us which vertices are inside of the surface
	*/
	uint8_t cubeindex = 0;
	if (grid.val[0] < isolevel) cubeindex |= 1;
	if (grid.val[1] < isolevel) cubeindex |= 2;
	if (grid.val[2] < isolevel) cubeindex |= 4;
	if (grid.val[3] < isolevel) cubeindex |= 8;
	if (grid.val[4] < isolevel) cubeindex |= 16;
	if (grid.val[5] < isolevel) cubeindex |= 32;
	if (grid.val[6] < isolevel) cubeindex |= 64;
	if (grid.val[7] < isolevel) cubeindex |= 128;

	/* Cube is entirely in/out of the surface */
	if (edge_table[cubeindex] == 0) return;

	/* Find the vertices where the surface intersects the cube */
	if (edge_table[cubeindex] & 1)
		vertlist[0] = vertex_interpolate(isolevel, grid.p[0], grid.p[1], grid.val[0], grid.val[1]);
	if (edge_table[cubeindex] & 2)
		vertlist[1] = vertex_interpolate(isolevel, grid.p[1], grid.p[2], grid.val[1], grid.val[2]);
	if (edge_table[cubeindex] & 4)
		vertlist[2] = vertex_interpolate(isolevel, grid.p[2], grid.p[3], grid.val[2], grid.val[3]);
	if (edge_table[cubeindex] & 8)
		vertlist[3] = vertex_interpolate(isolevel, grid.p[3], grid.p[0], grid.val[3], grid.val[0]);
	if (edge_table[cubeindex] & 16)
		vertlist[4] = vertex_interpolate(isolevel, grid.p[4], grid.p[5], grid.val[4], grid.val[5]);
	if (edge_table[cubeindex] & 32)
		vertlist[5] = vertex_interpolate(isolevel, grid.p[5], grid.p[6], grid.val[5], grid.val[6]);
	if (edge_table[cubeindex] & 64)
		vertlist[6] = vertex_interpolate(isolevel, grid.p[6], grid.p[7], grid.val[6], grid.val[7]);
	if (edge_table[cubeindex] & 128)
		vertlist[7] = vertex_interpolate(isolevel, grid.p[7], grid.p[4], grid.val[7], grid.val[4]);
	if (edge_table[cubeindex] & 256)
		vertlist[8] = vertex_interpolate(isolevel, grid.p[0], grid.p[4], grid.val[0], grid.val[4]);
	if (edge_table[cubeindex] & 512)
		vertlist[9] = vertex_interpolate(isolevel, grid.p[1], grid.p[5], grid.val[1], grid.val[5]);
	if (edge_table[cubeindex] & 1024)
		vertlist[10] = vertex_interpolate(isolevel, grid.p[2], grid.p[6], grid.val[2], grid.val[6]);
	if (edge_table[cubeindex] & 2048)
		vertlist[11] = vertex_interpolate(isolevel, grid.p[3], grid.p[7], grid.val[3], grid.val[7]);

	/* Create the triangle */
	for (int i = 0; tri_table[cubeindex][i] != -1; i += 3) {
		triangle_cb(t_param, vertlist[tri_table[cubeindex][i]],
		            vertlist[tri_table[cubeindex][i + 1]], vertlist[tri_table[cubeindex][i + 2]]);
	}
}

static void polygonise_color(gridcell_color_t grid, float isolevel,
                             sw_add_triangle_color_func_t triangle_cb, void *t_param) {
	kr_vec3_t vertlist[12];
	kr_vec3_t colorlist[12];
	/*
	   Determine the index into the edge table which
	   tells us which vertices are inside of the surface
	*/
	uint8_t cubeindex = 0;
	if (grid.val[0].w < isolevel) cubeindex |= 1;
	if (grid.val[1].w < isolevel) cubeindex |= 2;
	if (grid.val[2].w < isolevel) cubeindex |= 4;
	if (grid.val[3].w < isolevel) cubeindex |= 8;
	if (grid.val[4].w < isolevel) cubeindex |= 16;
	if (grid.val[5].w < isolevel) cubeindex |= 32;
	if (grid.val[6].w < isolevel) cubeindex |= 64;
	if (grid.val[7].w < isolevel) cubeindex |= 128;

	/* Cube is entirely in/out of the surface */
	if (edge_table[cubeindex] == 0) return;

	/* Find the vertices where the surface intersects the cube */
	if (edge_table[cubeindex] & 1) {
		vertlist[0] =
		    vertex_interpolate(isolevel, grid.p[0], grid.p[1], grid.val[0].w, grid.val[1].w);
		colorlist[0] = (grid.val[0].w < grid.val[1].w)
		                   ? (kr_vec3_t){grid.val[0].x, grid.val[0].y, grid.val[0].z}
		                   : (kr_vec3_t){grid.val[1].x, grid.val[1].y, grid.val[1].z};
	}
	if (edge_table[cubeindex] & 2) {
		vertlist[1] =
		    vertex_interpolate(isolevel, grid.p[1], grid.p[2], grid.val[1].w, grid.val[2].w);
		colorlist[1] = (grid.val[1].w < grid.val[2].w)
		                   ? (kr_vec3_t){grid.val[1].x, grid.val[1].y, grid.val[1].z}
		                   : (kr_vec3_t){grid.val[2].x, grid.val[2].y, grid.val[2].z};
	}
	if (edge_table[cubeindex] & 4) {
		vertlist[2] =
		    vertex_interpolate(isolevel, grid.p[2], grid.p[3], grid.val[2].w, grid.val[3].w);
		colorlist[2] = (grid.val[2].w < grid.val[3].w)
		                   ? (kr_vec3_t){grid.val[2].x, grid.val[2].y, grid.val[2].z}
		                   : (kr_vec3_t){grid.val[3].x, grid.val[3].y, grid.val[3].z};
	}
	if (edge_table[cubeindex] & 8) {
		vertlist[3] =
		    vertex_interpolate(isolevel, grid.p[3], grid.p[0], grid.val[3].w, grid.val[0].w);
		colorlist[3] = (grid.val[3].w < grid.val[0].w)
		                   ? (kr_vec3_t){grid.val[3].x, grid.val[3].y, grid.val[3].z}
		                   : (kr_vec3_t){grid.val[0].x, grid.val[0].y, grid.val[0].z};
	}
	if (edge_table[cubeindex] & 16) {
		vertlist[4] =
		    vertex_interpolate(isolevel, grid.p[4], grid.p[5], grid.val[4].w, grid.val[5].w);
		colorlist[4] = (grid.val[4].w < grid.val[5].w)
		                   ? (kr_vec3_t){grid.val[4].x, grid.val[4].y, grid.val[4].z}
		                   : (kr_vec3_t){grid.val[5].x, grid.val[5].y, grid.val[5].z};
	}
	if (edge_table[cubeindex] & 32) {
		vertlist[5] =
		    vertex_interpolate(isolevel, grid.p[5], grid.p[6], grid.val[5].w, grid.val[6].w);
		colorlist[5] = (grid.val[5].w < grid.val[6].w)
		                   ? (kr_vec3_t){grid.val[5].x, grid.val[5].y, grid.val[5].z}
		                   : (kr_vec3_t){grid.val[6].x, grid.val[6].y, grid.val[6].z};
	}
	if (edge_table[cubeindex] & 64) {
		vertlist[6] =
		    vertex_interpolate(isolevel, grid.p[6], grid.p[7], grid.val[6].w, grid.val[7].w);
		colorlist[6] = (grid.val[6].w < grid.val[7].w)
		                   ? (kr_vec3_t){grid.val[6].x, grid.val[6].y, grid.val[6].z}
		                   : (kr_vec3_t){grid.val[7].x, grid.val[7].y, grid.val[7].z};
	}
	if (edge_table[cubeindex] & 128) {
		vertlist[7] =
		    vertex_interpolate(isolevel, grid.p[7], grid.p[4], grid.val[7].w, grid.val[4].w);
		colorlist[7] = (grid.val[7].w < grid.val[4].w)
		                   ? (kr_vec3_t){grid.val[7].x, grid.val[7].y, grid.val[7].z}
		                   : (kr_vec3_t){grid.val[4].x, grid.val[4].y, grid.val[4].z};
	}
	if (edge_table[cubeindex] & 256) {
		vertlist[8] =
		    vertex_interpolate(isolevel, grid.p[0], grid.p[4], grid.val[0].w, grid.val[4].w);
		colorlist[8] = (grid.val[0].w < grid.val[4].w)
		                   ? (kr_vec3_t){grid.val[0].x, grid.val[0].y, grid.val[0].z}
		                   : (kr_vec3_t){grid.val[4].x, grid.val[4].y, grid.val[4].z};
	}
	if (edge_table[cubeindex] & 512) {
		vertlist[9] =
		    vertex_interpolate(isolevel, grid.p[1], grid.p[5], grid.val[1].w, grid.val[5].w);
		colorlist[9] = (grid.val[1].w < grid.val[5].w)
		                   ? (kr_vec3_t){grid.val[1].x, grid.val[1].y, grid.val[1].z}
		                   : (kr_vec3_t){grid.val[5].x, grid.val[5].y, grid.val[5].z};
	}
	if (edge_table[cubeindex] & 1024) {
		vertlist[10] =
		    vertex_interpolate(isolevel, grid.p[2], grid.p[6], grid.val[2].w, grid.val[6].w);
		colorlist[10] = (grid.val[2].w < grid.val[6].w)
		                    ? (kr_vec3_t){grid.val[2].x, grid.val[2].y, grid.val[2].z}
		                    : (kr_vec3_t){grid.val[6].x, grid.val[6].y, grid.val[6].z};
	}
	if (edge_table[cubeindex] & 2048) {
		vertlist[11] =
		    vertex_interpolate(isolevel, grid.p[3], grid.p[7], grid.val[3].w, grid.val[7].w);
		colorlist[11] = (grid.val[3].w < grid.val[7].w)
		                    ? (kr_vec3_t){grid.val[3].x, grid.val[3].y, grid.val[3].z}
		                    : (kr_vec3_t){grid.val[7].x, grid.val[7].y, grid.val[7].z};
	}

	/* Create the triangle */
	for (int i = 0; tri_table[cubeindex][i] != -1; i += 3) {
		triangle_cb(t_param, vertlist[tri_table[cubeindex][i]],
		            vertlist[tri_table[cubeindex][i + 1]], vertlist[tri_table[cubeindex][i + 2]],
		            colorlist[tri_table[cubeindex][i]], colorlist[tri_table[cubeindex][i + 1]],
		            colorlist[tri_table[cubeindex][i + 2]]);
	}
}

static void set_cube_points(kr_vec3_t o, float s, kr_vec3_t *p) {
	p[0] = (kr_vec3_t){o.x, o.y, o.z};
	p[1] = (kr_vec3_t){o.x + s, o.y, o.z};
	p[2] = (kr_vec3_t){o.x + s, o.y, o.z + s};
	p[3] = (kr_vec3_t){o.x, o.y, o.z + s};
	p[4] = (kr_vec3_t){o.x, o.y + s, o.z};
	p[5] = (kr_vec3_t){o.x + s, o.y + s, o.z};
	p[6] = (kr_vec3_t){o.x + s, o.y + s, o.z + s};
	p[7] = (kr_vec3_t){o.x, o.y + s, o.z + s};
}

static void comp_gridcell(gridcell_t *c, sw_density_func_t f, void *p, kr_vec3_t o, float s) {
	set_cube_points(o, s, c->p);
	for (int i = 0; i < 8; ++i) c->val[i] = f(p, c->p[i]);
}

static void comp_gridcell_color(gridcell_color_t *c, sw_density_color_func_t f, void *p,
                                kr_vec3_t o, float s) {
	set_cube_points(o, s, c->p);
	for (int i = 0; i < 8; ++i) c->val[i] = f(p, c->p[i]);
}

void sw_mc_process_custom_chunk(const sw_mc_custom_t *init) {
	float step = (init->chunk.halfsidelen * 2.0f) / init->chunk.steps;
	kr_vec3_t bnl = kr_vec3_addf(init->chunk.origin, -init->chunk.halfsidelen);
	float z = bnl.z;
	for (int zi = 0; zi < init->chunk.steps; ++zi) {
		float y = bnl.y;
		for (int yi = 0; yi < init->chunk.steps; ++yi) {
			float x = bnl.x;
			for (int xi = 0; xi < init->chunk.steps; ++xi) {
				gridcell_t c;
				comp_gridcell(&c, init->density, init->density_param, (kr_vec3_t){x, y, z}, step);
				polygonise(c, init->chunk.iso_level, init->add_tris, init->add_tris_param);
				x += step;
			}
			y += step;
		}
		z += step;
	}
}

typedef struct sdf_arg {
	const sw_sdf_t *sdf;
	sw_sdf_stack_frame_t *stack;
} sdf_arg_t;

static float sdf_compute_wrapper(void *a, kr_vec3_t p) {
	sdf_arg_t *arg = (sdf_arg_t *)a;
	return sw_sdf_compute(arg->sdf, p, arg->stack);
}

static kr_vec4_t sdf_compute_wrapper_color(void *a, kr_vec3_t p) {
	sdf_arg_t *arg = (sdf_arg_t *)a;
	return sw_sdf_compute_color(arg->sdf, p, arg->stack);
}

void sw_mc_process_sdf_chunk(const sw_sdf_t *sdf, const sw_mc_chunk_t *chunk,
                             sw_add_triangle_func_t f, void *f_param) {
	sw_sdf_stack_frame_t *stack = sw_sdf_stack_init(sdf);
	sdf_arg_t a = (sdf_arg_t){.sdf = sdf, .stack = stack};
	sw_mc_process_custom_chunk(&(sw_mc_custom_t){.add_tris = f,
	                                             .add_tris_param = f_param,
	                                             .chunk = *chunk,
	                                             .density = sdf_compute_wrapper,
	                                             .density_param = &a});
	sw_sdf_stack_destroy(stack);
}

void sw_mc_process_custom_chunk_color(const sw_mc_custom_color_t *init) {
	float step = (init->chunk.halfsidelen * 2.0f) / init->chunk.steps;
	kr_vec3_t bnl = kr_vec3_addf(init->chunk.origin, -init->chunk.halfsidelen);
	float z = bnl.z;
	for (int zi = 0; zi < init->chunk.steps; ++zi) {
		float y = bnl.y;
		for (int yi = 0; yi < init->chunk.steps; ++yi) {
			float x = bnl.x;
			for (int xi = 0; xi < init->chunk.steps; ++xi) {
				gridcell_color_t c;
				comp_gridcell_color(&c, init->density, init->density_param, (kr_vec3_t){x, y, z},
				                    step);
				polygonise_color(c, init->chunk.iso_level, init->add_tris, init->add_tris_param);
				x += step;
			}
			y += step;
		}
		z += step;
	}
}

void sw_mc_process_sdf_chunk_color(const sw_sdf_t *sdf, const sw_mc_chunk_t *chunk,
                                   sw_add_triangle_color_func_t f, void *f_param) {
	sw_sdf_stack_frame_t *stack = sw_sdf_stack_init(sdf);
	sdf_arg_t a = (sdf_arg_t){.sdf = sdf, .stack = stack};
	sw_mc_process_custom_chunk_color(&(sw_mc_custom_color_t){.add_tris = f,
	                                                         .add_tris_param = f_param,
	                                                         .chunk = *chunk,
	                                                         .density = sdf_compute_wrapper_color,
	                                                         .density_param = &a});
	sw_sdf_stack_destroy(stack);
}
