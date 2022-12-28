#pragma once

#include "graph.h"
#include <krink/math/vector.h>

typedef struct sw_sdf sw_sdf_t;

sw_sdf_t *sw_sdf_generate(sw_graph_t *g, int start_node);
void sw_sdf_destroy(sw_sdf_t *sdf);
float sw_sdf_compute(sw_sdf_t *sdf, kr_vec3_t pos);
kr_vec4_t sw_sdf_compute_color(sw_sdf_t *sdf, kr_vec3_t pos);
