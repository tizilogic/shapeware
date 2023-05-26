#include "graph.h"

#include <assert.h>
#include <kinc/io/filereader.h>
#include <kinc/io/filewriter.h>
#include <kinc/log.h>
#include <kinc/math/core.h>
#include <kinc/system.h>
#include <krink/memory.h>
#include <string.h>

void sw_graph_init(sw_graph_t *g, int reserve_nodes, int reserve_data) {
	assert(reserve_nodes > 0 && reserve_data > 0);
	g->size = 0;
	g->data_top = 0;
	g->node_cap = reserve_nodes;
	g->data_cap = reserve_data;
	g->nodes = (sw_node_t *)kr_malloc(reserve_nodes * sizeof(sw_node_t));
	assert(g->nodes);
	for (int i = 0; i < reserve_nodes; ++i) g->nodes[i].type = SW_FREE_NODE;
	g->data = (uint8_t *)kr_malloc(reserve_data);
	assert(g->data);
}

void sw_graph_destroy(sw_graph_t *g) {
	if (g->nodes) kr_free(g->nodes);
	if (g->data) kr_free(g->data);
	g->nodes = NULL;
	g->data = NULL;
	g->size = 0;
	g->node_cap = 0;
	g->data_top = 0;
	g->data_cap = 0;
}

static int sw_get_next_free(sw_graph_t *g) {
	if (g->size >= g->node_cap) {
		g->nodes = (sw_node_t *)kr_realloc(g->nodes, g->node_cap * 2 * sizeof(sw_node_t));
		assert(g->nodes);
		for (int i = g->node_cap; i < g->node_cap * 2; ++i) g->nodes[i].type = SW_FREE_NODE;
		g->node_cap *= 2;
	}
	for (int i = 0; i < g->node_cap; ++i)
		if (g->nodes[i].type == SW_FREE_NODE) {
			++g->size;
			return i;
		}
	kinc_log(KINC_LOG_LEVEL_ERROR, "Unable to get empty node");
	assert(true);
	return -1;
}

static int sw_internal_insert_data(sw_graph_t *g, int size, void *data) {
	assert(size % 4 == 0 && size > 0);

	if (g->data_cap <= g->data_top + size) {
		g->data = (uint8_t *)kr_realloc(g->data, g->data_cap * 2);
		assert(g->data);
		g->data_cap *= 2;
	}
	memcpy(&g->data[g->data_top], data, size);
	g->data_top += size;
	return g->data_top - size;
}

static void sw_internal_rename_node(sw_graph_t *g, int id, const char *name) {
	assert(id >= 0 && id < g->node_cap && g->nodes[id].type != SW_FREE_NODE);
	int len = kinc_mini(SW_MAX_NAME_LEN - 1, strlen(name));
	memcpy(g->nodes[id].name, name, len);
	g->nodes[id].name[len] = 0;
}

int sw_graph_insert_node(sw_graph_t *g, int parent, sw_type_t t, const char *name, void *data,
                         int size) {
	assert(t >= 0);
	int ret = sw_get_next_free(g);
	if (parent < -1) parent = -1;
	g->nodes[ret].parent = parent;
	g->nodes[ret].type = t;
	g->nodes[ret].size = size;
	if (size > 0)
		g->nodes[ret].start = sw_internal_insert_data(g, size, data);
	else
		g->nodes[ret].start = -1;
	sw_internal_rename_node(g, ret, name);
	return ret;
}

void sw_graph_rename(sw_graph_t *g, int id, const char *name) {
	sw_internal_rename_node(g, id, name);
}

void sw_graph_delete_node(sw_graph_t *g, int id, sw_delete_op_t op) {
	assert(op == SW_DELOP_DEL_NODE_ONLY || op == SW_DELOP_DEL_NODE_AND_CHILDREN ||
	       op == SW_DELOP_DEL_NODE_AND_REPARENT);
	int start = g->nodes[id].start;
	int size = g->nodes[id].size;
	g->nodes[id].type = SW_FREE_NODE;
	int move_size = g->data_top - (start + size);
	if (move_size > 0) {
		void *res = memmove(&g->data[start], &g->data[start + size], move_size);
		g->data_cap -= size;
		for (int i = 0; i < g->node_cap; ++i) {
			if (g->nodes[i].type == SW_FREE_NODE) continue;
			if (g->nodes[i].start > start) g->nodes[i].start -= size;
		}
	}
	switch (op) {
	case SW_DELOP_DEL_NODE_AND_CHILDREN: {
		sw_iter_t it;
		sw_node_t *n;
		sw_foreach(n, g, &it, id) {
			int child = n - g->nodes;
			sw_graph_delete_node(g, child, op);
		}
	} break;
	case SW_DELOP_DEL_NODE_AND_REPARENT: {
		sw_iter_t it;
		sw_node_t *n;
		sw_foreach(n, g, &it, id) {
			n->parent = g->nodes[id].parent;
		}
	} break;
	case SW_DELOP_DEL_NODE_ONLY:
		break;
	default:
		kinc_log(KINC_LOG_LEVEL_ERROR, "Invalid delete operation!");
		break;
	}
	g->nodes[id].parent = -1;
}

sw_node_t *sw_graph_get_node(sw_graph_t *g, int id) {
	assert(id > -1 && id < g->node_cap);
	return &g->nodes[id];
}

int sw_graph_get_node_id(sw_graph_t *g, sw_node_t *n) {
	return n - g->nodes;
}

void *sw_graph_get_data(sw_graph_t *g, sw_node_t *n) {
	assert(n && n->start >= 0 && g->data_top > n->start);
	return &g->data[n->start];
}

void sw_graph_set_parent(sw_graph_t *g, int id, int parent) {
	assert(id > -1 && id < g->node_cap);
	g->nodes[id].parent = parent;
}

sw_type_t sw_graph_type(sw_graph_t *g, int id) {
	return g->nodes[id].type;
}

bool sw_graph_has_children(sw_graph_t *g, int id, sw_type_t *ignore, int ignore_size) {
	assert(id > -1 && id < g->node_cap);
	for (int i = 0; i < g->size; ++i) {
		if (g->nodes[i].type == SW_FREE_NODE) continue;
		bool found = false;
		if (g->nodes[i].parent == id) {
			found = true;
			for (int j = 0; j < ignore_size; ++j)
				if (g->nodes[i].type == ignore[j]) {
					found = false;
					break;
				}
		}
		if (found) return true;
	}
	return false;
}

void sw_graph_deep_copy(sw_graph_t *target, const sw_graph_t *src) {
	sw_graph_init(target, src->node_cap, src->data_cap);
	target->data_top = src->data_top;
	target->size = src->size;
	memcpy(target->nodes, src->nodes, src->node_cap * sizeof(sw_node_t));
	memcpy(target->data, src->data, src->data_cap);
}

// Iterating

sw_node_t *sw_iter_begin(sw_graph_t *g, sw_iter_t *it, int parent) {
	it->parent = parent;
	it->last = -1;
	it->g = g;
	return sw_iter_next(it);
}

sw_node_t *sw_iter_next(sw_iter_t *it) {
	if (it->last + 1 >= it->g->node_cap) return NULL;
	for (int i = it->last + 1; i < it->g->size; ++i) {
		if (it->g->nodes[i].type == SW_FREE_NODE) continue;
		if (it->g->nodes[i].parent == it->parent) {
			it->last = i;
			return &it->g->nodes[i];
		}
	}
	it->last = it->g->node_cap;
	return NULL;
}

// Serializing

void sw_graph_store(sw_graph_t *g, const char *filename) {
	kinc_file_writer_t writer;
	bool success = kinc_file_writer_open(&writer, filename);
	if (!success) {
		kinc_log(KINC_LOG_LEVEL_ERROR, "Unable to open file '%s' for writing", filename);
		return;
	}
	kinc_file_writer_write(&writer, g, sizeof(sw_graph_t));
	kinc_file_writer_write(&writer, g->nodes, g->node_cap * sizeof(sw_node_t));
	kinc_file_writer_write(&writer, g->data, g->data_top);
	kinc_file_writer_close(&writer);
}

void sw_graph_load(sw_graph_t *g, const char *filename) {
	kinc_file_reader_t reader;
	bool success = kinc_file_reader_open(&reader, filename, KINC_FILE_TYPE_SAVE);
	if (!success) {
		kinc_log(KINC_LOG_LEVEL_ERROR, "Unable to open file '%s' for reading", filename);
		return;
	}
	kinc_file_reader_read(&reader, g, sizeof(sw_graph_t));
	g->nodes = (sw_node_t *)kr_malloc(g->node_cap * sizeof(sw_node_t));
	assert(g->nodes);
	kinc_file_reader_read(&reader, g->nodes, g->node_cap * sizeof(sw_node_t));
	g->data = (uint8_t *)kr_malloc(g->data_cap);
	assert(g->data);
	kinc_file_reader_read(&reader, g->data, g->data_top);
	kinc_file_reader_close(&reader);
}
