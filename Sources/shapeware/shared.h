#pragma once

#define SW_SHAPES_START 1000
#define SW_SHAPES_MAX 1999
#define SW_CSW_START 2000
#define SW_CSW_MAX 2999
#define SW_TRANSFOM_START 99000 // Range from where not to show names
#define SW_TRANSFOM_MAX 99999
#define SW_DUMMIES_START 100000
#define SW_DUMMIES_MAX 100999
#define SW_SKEL_START 101000
#define SW_SKEL_MAX 101999
#define SW_OPS_START 3000
#define SW_OPS_MAX 3999
#define SW_MISC_START 4000
#define SW_MISC_MAX 4999

typedef enum sw_node_type_group {
	SW_NODE_TYPE_SHAPE,
	SW_NODE_TYPE_CSG,
	SW_NODE_TYPE_TRANSFORM,
	SW_NODE_TYPE_DUMMY,
	SW_NODE_TYPE_OP,
	SW_NODE_TYPE_MISC,
	SW_NODE_TYPE_UNKNOWN,
} sw_node_type_group_t;

static sw_node_type_group_t sw_node_type_group_get(int t) {
	if (t >= SW_SHAPES_START && t <= SW_SHAPES_MAX) return SW_NODE_TYPE_SHAPE;
	if (t >= SW_CSW_START && t <= SW_CSW_MAX) return SW_NODE_TYPE_CSG;
	if (t >= SW_TRANSFOM_START && t <= SW_TRANSFOM_MAX) return SW_NODE_TYPE_TRANSFORM;
	if (t >= SW_DUMMIES_START && t <= SW_DUMMIES_MAX) return SW_NODE_TYPE_DUMMY;
	if (t >= SW_OPS_START && t <= SW_OPS_MAX) return SW_NODE_TYPE_OP;
	if (t >= SW_MISC_START && t <= SW_MISC_MAX) return SW_NODE_TYPE_MISC;
	return SW_NODE_TYPE_UNKNOWN;
}

#define SW_SDF_STACK_SIZE_START 8
