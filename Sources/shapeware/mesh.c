#include "mesh.h"

#include <krink/memory.h>
#include <sht/sht.h>
#include <util/list.h>

#include <assert.h>
#include <stdint.h>

typedef struct sw_vertex {
	kr_vec3_t pos;
	kr_vec3_t normal;
	kr_vec3_t color;
	sw_list_int_t *tris;
} sw_vertex_t;

typedef struct sw_triangle {
	int va;
	int vb;
	int vc;
	kr_vec3_t face_normal_mag;
} sw_triangle_t;

typedef struct sw_mesh {
	sw_vertex_t *vertices;
	sht_t *vert_id_map;
	sw_triangle_t *triangles;
	int next_vert;
	int vert_cap;
	int next_tris;
	int tris_cap;
	sw_mesh_normal_func_t fn;
	void *fparam;
} sw_mesh_t;

sw_mesh_t *sw_mesh_init(int reserve_vert, int reserve_tris, sw_mesh_normal_func_t fn, void *fparam) {
	sw_mesh_t *m = (sw_mesh_t *)kr_malloc(sizeof(sw_mesh_t));
	assert(m != NULL);
	reserve_vert = (reserve_vert > 0) ? reserve_vert : 16;
	reserve_tris = (reserve_tris > 0) ? reserve_tris : 8;
	m->vertices = (sw_vertex_t *)kr_malloc(reserve_vert * sizeof(sw_vertex_t));
	m->next_vert = 0;
	m->vert_cap = reserve_vert;
	m->vert_id_map = sht_init_alloc(sizeof(int), reserve_vert, 42, kr_malloc, kr_free);
	m->triangles = (sw_triangle_t *)kr_malloc(reserve_tris * sizeof(sw_triangle_t));
	m->next_tris = 0;
	m->tris_cap = reserve_tris;
	assert(m->vertices != NULL && m->triangles != NULL && m->vert_id_map != NULL);
	m->fn = fn;
	m->fparam = fparam;
	return m;
}

void sw_mesh_destroy(sw_mesh_t *m) {
	assert(m != NULL);
	for (int i = 0; i < m->next_vert; ++i) sw_list_int_destroy(m->vertices[i].tris);
	kr_free(m->vertices);
	m->vertices = NULL;
	sht_destroy(m->vert_id_map);
	kr_free(m->triangles);
	m->triangles = NULL;
	kr_free(m);
}

static kr_vec3_t sw_triangle_face_normal(kr_vec3_t a, kr_vec3_t b, kr_vec3_t c) {
	kr_vec3_t u = kr_vec3_subv(b, a);
	kr_vec3_t v = kr_vec3_subv(c, a);
	return (kr_vec3_t){
	    .x = u.y * v.z - u.z * v.y, .y = u.z * v.x - u.x * v.z, .z = u.x * v.y - u.y * v.x};
}

static void sw_resize_verts(sw_mesh_t *m) {
	if (m->next_vert < m->vert_cap) return;
	m->vertices = kr_realloc(m->vertices, m->vert_cap * 2 * sizeof(sw_vertex_t));
	assert(m->vertices != 0);
	m->vert_cap *= 2;
}

static void sw_resize_tris(sw_mesh_t *m) {
	if (m->next_tris < m->tris_cap) return;
	m->triangles = kr_realloc(m->triangles, m->tris_cap * 2 * sizeof(sw_triangle_t));
	assert(m->triangles != 0);
	m->tris_cap *= 2;
}

static int sw_add_vertex(sw_mesh_t *m, kr_vec3_t pos, kr_vec3_t color, int triangle_id) {
	int *id = sht_get(m->vert_id_map, &pos, sizeof(pos));
	int ret = -1;
	if (id == NULL) {
		sw_resize_verts(m);
		m->vertices[m->next_vert].pos = pos;
		m->vertices[m->next_vert].normal = m->fn(m->fparam, pos);
		m->vertices[m->next_vert].color = color;
		m->vertices[m->next_vert].tris = sw_list_int_init(4);
		sht_set(m->vert_id_map, &pos, sizeof(kr_vec3_t), &m->next_vert);
		ret = m->next_vert++;
	}
	else
		ret = *id;
	sw_list_int_push(m->vertices[ret].tris, triangle_id);
	return ret;
}

void sw_mesh_add_triangle(sw_mesh_t *m, kr_vec3_t a, kr_vec3_t b, kr_vec3_t c, kr_vec3_t ca,
                          kr_vec3_t cb, kr_vec3_t cc) {
	sw_resize_tris(m);
	m->triangles[m->next_tris] =
	    (sw_triangle_t){.va = sw_add_vertex(m, a, ca, m->next_tris),
	                    .vb = sw_add_vertex(m, b, cb, m->next_tris),
	                    .vc = sw_add_vertex(m, c, cc, m->next_tris),
	                    .face_normal_mag = sw_triangle_face_normal(a, b, c)};
	++m->next_tris;
}

int sw_mesh_vert_count(sw_mesh_t *m) {
	return m->next_vert;
}

int sw_mesh_tris_count(sw_mesh_t *m) {
	return m->next_tris;
}

static kr_vec3_t sw_smooth_vert_normal(sw_mesh_t *m, int vertex_id) {
	kr_vec3_t n = (kr_vec3_t){.x = 0.0f, .y = 0.0f, .z = 0.0f};
	sw_list_int_t *tris = m->vertices[vertex_id].tris;
	int count = sw_list_int_len(tris);
	assert(count > 0);
	for (int i = 0; i < count; ++i) {
		int tris_id = sw_list_int_get(tris, i);
		kr_vec3_t face_n = m->triangles[tris_id].face_normal_mag;
		// Experimental:
		// Inverse of the squared length from the face normal vector to give smaller faces more
		// weight
		float inv_mag = 1.0f / (face_n.x * face_n.x + face_n.y * face_n.y + face_n.z * face_n.z);
		n = kr_vec3_addv(n, kr_vec3_mult(face_n, inv_mag));
	}

	return kr_vec3_normalized(n);
}

void sw_mesh_write_vert_buffer(sw_mesh_t *m, float *buffer) {
	for (int i = 0; i < m->next_vert; ++i) {
		int offset = i * 9;
		buffer[offset + 0] = m->vertices[i].pos.x;
		buffer[offset + 1] = m->vertices[i].pos.y;
		buffer[offset + 2] = m->vertices[i].pos.z;

		/*
		// Could be used to compute normal by analyzing adjacent faces of a vertex
		kr_vec3_t n = sw_smooth_vert_normal(m, i);
		buffer[offset + 3] = n.x;
		buffer[offset + 4] = n.y;
		buffer[offset + 5] = n.z;
		*/
		buffer[offset + 3] = m->vertices[i].normal.x;
		buffer[offset + 4] = m->vertices[i].normal.y;
		buffer[offset + 5] = m->vertices[i].normal.z;

		buffer[offset + 6] = m->vertices[i].color.x;
		buffer[offset + 7] = m->vertices[i].color.y;
		buffer[offset + 8] = m->vertices[i].color.z;
	}
}

void sw_mesh_write_index_buffer(sw_mesh_t *m, int *buffer) {
	for (int i = 0; i < m->next_tris; ++i) {
		buffer[i * 3 + 0] = m->triangles[i].va;
		buffer[i * 3 + 1] = m->triangles[i].vb;
		buffer[i * 3 + 2] = m->triangles[i].vc;
	}
}
