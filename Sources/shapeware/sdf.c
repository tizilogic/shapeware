#include "sdf.h"

#include "csg.h"
#include "misc.h"
#include "ops.h"
#include "shapes.h"
#include "shared.h"
#include "transform.h"
#include <assert.h>
#include <kinc/log.h>
#include <krink/math/matrix.h>
#include <krink/memory.h>
#include <math.h>
#include <util/list.h>

typedef struct sw_sdf_stack_frame {
	int node_id;
	kr_vec4_t dist_a;
	kr_vec4_t dist_b;
	sw_type_t node_type;
	void *data;
	kr_vec3_t pos;
} sw_sdf_stack_frame_t;

typedef struct sw_sdf {
	sw_graph_t *g;
	sw_list_int_t *nodes;
	sw_list_int_t *stack_direction;
	int empty_count;
	int max_stack_depth;
} sw_sdf_t;

static int sw_sdf_find_of_type(sw_graph_t *g, int parent, sw_type_t t) {
	sw_iter_t it;
	sw_node_t *n;
	sw_foreach(n, g, &it, parent) {
		if (n->type == t) return sw_graph_get_node_id(g, n);
	}
	return -1;
}

static bool sw_sdf_is_dummy(sw_type_t t) {
	return (t >= SW_DUMMIES_START && t <= SW_DUMMIES_MAX);
}

static void sw_sdf_populate_empty_to_root(sw_sdf_t *sdf, sw_graph_t *g, int start_node) {
	if (start_node < 0) {
		sdf->empty_count = 0;
		return;
	}
	sw_node_t *n = sw_graph_get_node(g, start_node);
	sw_list_int_t *path = sw_list_int_init(8);
	while (n->parent >= 0) {
		sw_list_int_push(path, n->parent);
	}
	sdf->empty_count = sw_list_int_len(path);
	while (sw_list_int_len(path) > 0) {
		int node_id = sw_list_int_pop(path);
		sw_list_int_push(sdf->nodes, sw_sdf_find_of_type(g, node_id, SW_TRANSFORM_TRANSLATION));
		sw_list_int_push(sdf->nodes, sw_sdf_find_of_type(g, node_id, SW_TRANSFORM_ROTATION));
	}
	sw_list_int_destroy(path);
}

static void sw_sdf_traverse(sw_sdf_t *sdf, sw_graph_t *g, int parent, int depth) {
	sdf->max_stack_depth = (sdf->max_stack_depth < depth) ? depth : sdf->max_stack_depth;
	sw_node_t *n = NULL;
	sw_iter_t it;
	sw_foreach(n, g, &it, parent) {
		if (sw_sdf_is_dummy(n->type)) continue;
		if (n->type == SW_TRANSFORM_TRANSLATION || n->type == SW_TRANSFORM_ROTATION) continue;
		int node_id = sw_graph_get_node_id(g, n);
		sw_list_int_push(sdf->nodes, node_id);
		sw_list_int_push(sdf->nodes, sw_sdf_find_of_type(g, node_id, SW_TRANSFORM_TRANSLATION));
		sw_list_int_push(sdf->nodes, sw_sdf_find_of_type(g, node_id, SW_TRANSFORM_ROTATION));
		sw_list_int_push(sdf->stack_direction, 1);
		sw_sdf_traverse(sdf, g, node_id, depth + 1);
		sw_list_int_push(sdf->stack_direction, -1);
	}
}

sw_sdf_t *sw_sdf_generate(sw_graph_t *g, int start_node) {
	sw_sdf_t *sdf = (sw_sdf_t *)kr_malloc(sizeof(sw_sdf_t));
	assert(sdf);
	sdf->g = g;
	sdf->nodes = sw_list_int_init(g->size * 3);
	sdf->stack_direction = sw_list_int_init(g->size * 2);
	sdf->max_stack_depth = -1;
	sw_sdf_populate_empty_to_root(sdf, g, start_node);
	sw_sdf_traverse(sdf, g, start_node, 0);
	return sdf;
}

void sw_sdf_destroy(sw_sdf_t *sdf) {
	assert(sdf);
	if (sdf->nodes) sw_list_int_destroy(sdf->nodes);
	if (sdf->stack_direction) sw_list_int_destroy(sdf->stack_direction);
	kr_free(sdf);
}

sw_sdf_stack_frame_t *sw_sdf_stack_init(sw_sdf_t *sdf) {
	// TODO: Verify that the additional frame is needed!
	sw_sdf_stack_frame_t *stack = (sw_sdf_stack_frame_t *)kr_malloc((sdf->max_stack_depth + 1) *
	                                                                sizeof(sw_sdf_stack_frame_t));
	assert(stack != NULL);
	return stack;
}

void sw_sdf_stack_destroy(sw_sdf_stack_frame_t *stack) {
	assert(stack != NULL);
	kr_free(stack);
}

static kr_vec3_t sw_sdf_transform(sw_sdf_t *sdf, kr_vec3_t pos, int translation, int rotation) {
	if (rotation > -1) {
		kr_vec3_t *r = sw_graph_get_data(sdf->g, sw_graph_get_node(sdf->g, rotation));
		kr_matrix4x4_t m = kr_matrix4x4_identity();
		kr_matrix4x4_t rm = kr_matrix4x4_rotation(-r->z, -r->x, -r->y);
		m = kr_matrix4x4_multmat(&m, &rm);
		if (translation > -1) {
			kr_vec3_t *t = sw_graph_get_data(sdf->g, sw_graph_get_node(sdf->g, translation));
			rm = kr_matrix4x4_translation(t->x, t->y, t->z);
			m = kr_matrix4x4_multmat(&rm, &m);
		}
		m = kr_matrix4x4_inverse(&m);
		kr_vec4_t tmp = (kr_vec4_t){.x = pos.x, .y = pos.y, .z = pos.z, 1.0f};
		tmp = kr_matrix4x4_multvec(&m, tmp);
		pos.x = tmp.x;
		pos.y = tmp.y;
		pos.z = tmp.z;
	}
	else if (translation > -1) {
		kr_vec3_t *t = sw_graph_get_data(sdf->g, sw_graph_get_node(sdf->g, translation));
		pos = kr_vec3_subv(pos, *t);
	}
	return pos;
}

static void sw_sdf_compute_stack_frame_push(sw_sdf_stack_frame_t *frame) {
	if (sw_node_type_group_get(frame->node_type) == SW_NODE_TYPE_OP) {
		frame->pos = sw_ops_evaluate_pos(frame->node_type, frame->pos, frame->data);
	}
}

static kr_vec4_t sw_sdf_compute_stack_frame_pop(sw_sdf_stack_frame_t *frame) {
	kr_vec4_t tmp_dist = frame->dist_a;
	tmp_dist.w = INFINITY;
	sw_node_type_group_t g = sw_node_type_group_get(frame->node_type);
	switch (g) {
	case SW_NODE_TYPE_SHAPE: {
		tmp_dist = sw_shapes_evaluate_color(frame->node_type, frame->data, frame->pos);
	} break;
	case SW_NODE_TYPE_CSG: {
		tmp_dist =
		    sw_csg_evaluate_color(frame->node_type, frame->dist_a, frame->dist_b, frame->data);
	} break;
	case SW_NODE_TYPE_OP: {
		tmp_dist.w =
		    sw_ops_evaluate_dist(frame->node_type, frame->dist_a.w, frame->pos, frame->data);
	} break;
	case SW_NODE_TYPE_MISC: {
		tmp_dist = frame->dist_a;
	} break;
	default:
		kinc_log(KINC_LOG_LEVEL_WARNING, "Unhandled node type %d", frame->node_type);
		break;
	}
	return tmp_dist;
}

kr_vec4_t sw_sdf_compute_color(sw_sdf_t *sdf, kr_vec3_t pos, sw_sdf_stack_frame_t *stack) {
	sw_sdf_stack_frame_t *frames = NULL;
	if (stack == NULL) {
		int stack_size = sdf->max_stack_depth + 1;
		frames = (sw_sdf_stack_frame_t *)kr_malloc(stack_size * sizeof(sw_sdf_stack_frame_t));
		assert(frames);
	}
	else
		frames = stack;

	int stack_top = 0;
	for (int i = 0; i < sdf->empty_count; ++i) {
		int translation = sw_list_int_get(sdf->nodes, i * 2);
		int rotation = sw_list_int_get(sdf->nodes, i * 2 + 1);
		pos = sw_sdf_transform(sdf, pos, translation, rotation);
	}
	kr_vec3_t base_pos = pos;
	int node_top = sdf->empty_count * 2;
	kr_vec4_t res = (kr_vec4_t){.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = INFINITY};

	int instruction_count = sw_list_int_len(sdf->stack_direction);
	for (int i = 0; i < instruction_count; ++i) {
		if (sw_list_int_get(sdf->stack_direction, i) == -1) { // POP
			kr_vec4_t tmp_dist = sw_sdf_compute_stack_frame_pop(&frames[stack_top - 1]);
			if (stack_top > 1)
				pos = frames[stack_top - 2].pos;
			else
				pos = base_pos;
			--stack_top;
			if (stack_top == 0) {
				res = (res.w < tmp_dist.w) ? res : tmp_dist;
			}
			else {
				sw_type_t t = frames[stack_top - 1].node_type;
				if (t == SW_CSG_UNION || t == SW_CSG_INTERSECTION || t == SW_CSG_SMOOTH_UNION ||
				    t == SW_CSG_SMOOTH_INTERSECTION) {
					if (isinf(frames[stack_top - 1].dist_a.w))
						frames[stack_top - 1].dist_a = tmp_dist;
					else
						frames[stack_top - 1].dist_b = tmp_dist;
				}
				else if (t == SW_CSG_SUBTRACTION) {
					sw_csg_subtraction_t *csg = (sw_csg_subtraction_t *)frames[stack_top - 1].data;
					if (frames[stack_top].node_id == csg->subtractor_id)
						frames[stack_top - 1].dist_a = tmp_dist;
					else
						frames[stack_top - 1].dist_b = tmp_dist;
				}
				else if (t == SW_CSG_SMOOTH_SUBTRACTION) {
					sw_csg_smooth_subtraction_t *csg =
					    (sw_csg_smooth_subtraction_t *)frames[stack_top - 1].data;
					if (frames[stack_top].node_id == csg->subtractor_id)
						frames[stack_top - 1].dist_a = tmp_dist;
					else
						frames[stack_top - 1].dist_b = tmp_dist;
				}
				else {
					frames[stack_top - 1].dist_a = tmp_dist;
				}
			}
			continue;
		}

		// PUSH
		int node_id = sw_list_int_get(sdf->nodes, node_top++);
		int translation = sw_list_int_get(sdf->nodes, node_top++);
		int rotation = sw_list_int_get(sdf->nodes, node_top++);
		sw_node_t *n = sw_graph_get_node(sdf->g, node_id);
		pos = sw_sdf_transform(sdf, pos, translation, rotation);
		frames[stack_top].node_id = node_id;
		frames[stack_top].pos = pos;
		frames[stack_top].node_type = n->type;
		frames[stack_top].data = n->size > 0 ? sw_graph_get_data(sdf->g, n) : NULL;
		frames[stack_top].dist_a = (kr_vec4_t){0.0f, 0.0f, 0.0f, INFINITY};
		frames[stack_top++].dist_b = (kr_vec4_t){0.0f, 0.0f, 0.0f, INFINITY};
		sw_sdf_compute_stack_frame_push(&frames[stack_top - 1]);
		pos = frames[stack_top - 1].pos;
	}

	if (stack == NULL) kr_free(frames);
	return res;
}

float sw_sdf_compute(sw_sdf_t *sdf, kr_vec3_t pos, sw_sdf_stack_frame_t *stack) {
	return sw_sdf_compute_color(sdf, pos, stack).w;
}
