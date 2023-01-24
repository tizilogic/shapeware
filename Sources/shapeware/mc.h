/**
 * @file mc.h
 * @author Tiziano Bettio
 * @brief Provides the Marching Cubes algorithm implementation.
 * @date 2023-01-14
 */
#pragma once

#include <krink/math/vector.h>

#include "sdf.h"

typedef float (*sw_density_func_t)(void *, kr_vec3_t);
typedef void (*sw_add_triangle_func_t)(void *, kr_vec3_t, kr_vec3_t, kr_vec3_t);

typedef struct sw_mc_chunk {
	kr_vec3_t origin;
	float halfsidelen;
	int steps;
	float iso_level;
} sw_mc_chunk_t;

typedef struct sw_mc_custom {
	const sw_mc_chunk_t chunk;
	sw_density_func_t density;
	void *density_param;
	sw_add_triangle_func_t add_tris;
	void *add_tris_param;
} sw_mc_custom_t;

/**
 * @brief Generalized function that extracts a surface using a custom density function and outputs
 * triangles using a provided callback function.
 *
 * @param init
 */
void sw_mc_process_custom_chunk(const sw_mc_custom_t *init);

/**
 * @brief Extract surface in a given grid chunk using a SDF outputs triangles using the provided
 * callback function.
 *
 * @param sdf
 * @param chunk
 * @param f
 */
void sw_mc_process_sdf_chunk(const sw_sdf_t *sdf, const sw_mc_chunk_t *chunk,
                             sw_add_triangle_func_t f, void *f_param);
