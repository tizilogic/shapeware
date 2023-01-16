#pragma once

#include "graph.h"
#include <krink/math/vector.h>

typedef struct sw_sdf sw_sdf_t;
typedef struct sw_sdf_stack_frame sw_sdf_stack_frame_t;
/**
 * @brief Generate a computable SDF from a graph.
 *
 * @param g The graph containing the model
 * @param start_node The node in the graph to treat as starting point (use `-1` for the entire
 * graph).
 * @return sw_sdf_t*
 */
sw_sdf_t *sw_sdf_generate(sw_graph_t *g, int start_node);

void sw_sdf_destroy(sw_sdf_t *sdf);

/**
 * @brief Initialize a stack of the right size for SDF computation. Use this to avoid allocation
 * when computing multiple points for a given SDF.
 *
 * @param sdf
 * @return sw_sdf_stack_frame_t*
 */
sw_sdf_stack_frame_t *sw_sdf_stack_init(sw_sdf_t *sdf);

void sw_sdf_stack_destroy(sw_sdf_stack_frame_t *stack);

/**
 * @brief Compute only distance for a given position.
 *
 * @param sdf
 * @param pos The position to evaluate the SDF
 * @param stack If not `NULL`, a previously initialized stack will be used, otherwise the stack will
 * be allocated and subsequently freed for each call of this.
 * @return float
 */
float sw_sdf_compute(sw_sdf_t *sdf, kr_vec3_t pos, sw_sdf_stack_frame_t *stack);

/**
 * @brief Compute color and distance for a given position.
 *
 * @param sdf
 * @param pos
 * @param stack If not `NULL`, a previously initialized stack will be used, otherwise the stack will
 * be allocated and subsequently freed for each call of this.
 * @return kr_vec4_t xyz = rgb, w = distance
 */
kr_vec4_t sw_sdf_compute_color(sw_sdf_t *sdf, kr_vec3_t pos, sw_sdf_stack_frame_t *stack);
