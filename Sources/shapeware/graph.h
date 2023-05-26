#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SW_MAX_NAME_LEN 32

#define SW_FREE_NODE -1
typedef int32_t sw_type_t;
typedef enum sw_delete_op {
	SW_DELOP_DEL_NODE_ONLY,
	SW_DELOP_DEL_NODE_AND_CHILDREN,
	SW_DELOP_DEL_NODE_AND_REPARENT,
} sw_delete_op_t;

typedef struct sw_node {
	int parent;
	int start;
	int size;
	char name[SW_MAX_NAME_LEN];
	sw_type_t type;
} sw_node_t;

typedef struct sw_graph {
	int node_cap, data_cap;
	int size;
	int data_top;
	sw_node_t *nodes;
	uint8_t *data;
} sw_graph_t;

void sw_graph_init(sw_graph_t *g, int reserve_nodes, int reserve_data);
void sw_graph_destroy(sw_graph_t *g);
int sw_graph_insert_node(sw_graph_t *g, int parent, sw_type_t t, const char *name, void *data,
                         int size);
void sw_graph_rename(sw_graph_t *g, int id, const char *name);
void sw_graph_delete_node(sw_graph_t *g, int id, sw_delete_op_t op);
sw_node_t *sw_graph_get_node(sw_graph_t *g, int id);
int sw_graph_get_node_id(sw_graph_t *g, sw_node_t *n);
void *sw_graph_get_data(sw_graph_t *g, sw_node_t *n);
void sw_graph_set_parent(sw_graph_t *g, int id, int parent);
sw_type_t sw_graph_type(sw_graph_t *g, int id);
bool sw_graph_has_children(sw_graph_t *g, int id, sw_type_t *ignore, int ignore_size);
void sw_graph_deep_copy(sw_graph_t *target, const sw_graph_t *src);

// Iterating

typedef struct sw_iter {
	int parent;
	int last;
	sw_graph_t *g;
} sw_iter_t;

sw_node_t *sw_iter_begin(sw_graph_t *g, sw_iter_t *it, int parent);
sw_node_t *sw_iter_next(sw_iter_t *it);

#define sw_foreach(node, g, it, parent)                                                            \
	for ((node) = sw_iter_begin((g), (it), (parent)); (node) != NULL; (node) = sw_iter_next((it)))

// Serializing

void sw_graph_store(sw_graph_t *g, const char *filename);
void sw_graph_load(sw_graph_t *g, const char *filename);
