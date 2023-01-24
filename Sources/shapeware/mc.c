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
	if (kr_vec3_lt(p2, p1)) {
		kr_vec3_t temp;
		temp = p1;
		p1 = p2;
		p2 = temp;
	}

	kr_vec3_t p;
	if (fabsf(valp1 - valp2) > 0.00001f)
		p = kr_vec3_mult(kr_vec3_div(kr_vec3_addv(p1, kr_vec3_subv(p2, p1)), valp2 - valp1),
		                 isolevel - valp1);
	else
		p = p1;
	return p;
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

static void comp_gridcell(gridcell_t *c, sw_density_func_t f, void *p, kr_vec3_t o, float s) {
	c->p[0] = (kr_vec3_t){o.x, o.y, o.z + s};
	c->p[1] = (kr_vec3_t){o.x + s, o.y, o.z + s};
	c->p[2] = (kr_vec3_t){o.x + s, o.y, o.z};
	c->p[3] = (kr_vec3_t){o.x, o.y, o.z};
	c->p[4] = (kr_vec3_t){o.x, o.y + s, o.z + s};
	c->p[5] = (kr_vec3_t){o.x + s, o.y + s, o.z + s};
	c->p[6] = (kr_vec3_t){o.x + s, o.y + s, o.z};
	c->p[7] = (kr_vec3_t){o.x, o.y + s, o.z};
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
