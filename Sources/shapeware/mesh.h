#pragma once

#include <krink/math/vector.h>

typedef struct sw_mesh sw_mesh_t;
typedef kr_vec3_t (*sw_mesh_normal_func_t)(void *, kr_vec3_t);

sw_mesh_t *sw_mesh_init(int reserve_vert, int reserve_tris, sw_mesh_normal_func_t fn, void *fparam);
void sw_mesh_destroy(sw_mesh_t *m);
void sw_mesh_add_triangle(sw_mesh_t *m, kr_vec3_t a, kr_vec3_t b, kr_vec3_t c, kr_vec3_t ca,
                          kr_vec3_t cb, kr_vec3_t cc);
int sw_mesh_vert_count(sw_mesh_t *m);
int sw_mesh_tris_count(sw_mesh_t *m);

/**
 * @brief Writes the vertex buffer as follows: position, normal, color
 *
 * @param m
 * @param buffer
 */
void sw_mesh_write_vert_buffer(sw_mesh_t *m, float *buffer);
void sw_mesh_write_index_buffer(sw_mesh_t *m, int *buffer);
