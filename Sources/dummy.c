#include <kinc/log.h>
#include <kinc/system.h>
#include <krink/math/vector.h>
#include <krink/memory.h>
#include <krink/system.h>
#include <shapeware/csg.h>
#include <shapeware/graph.h>
#include <shapeware/sdf.h>
#include <shapeware/shapes.h>
#include <shapeware/transform.h>
#include <stdint.h>
#include <stdlib.h>

// Dev testing the lib here

int kickstart(int argc, char **argv) {
	static uint8_t mem[1024 * 1024];
	kr_init(&mem, 1024 * 1024, NULL, 0);
	sw_graph_t g;
	sw_graph_init(&g, 8, 1024);
	sw_shapes_sphere_t s1 = sw_shapes_default_sphere();
	sw_shapes_sphere_t s2 = sw_shapes_default_sphere();
	s1.m.r = 1.0f;
	s1.m.g = 0.0f;
	s1.m.b = 0.0f;
	s2.m.r = 0.0f;
	s2.m.g = 0.0f;
	s2.m.b = 1.0f;
	int s1_id =
	    sw_graph_insert_node(&g, -1, SW_SHAPE_SPHERE, "Sphere 1", &s1, sizeof(sw_shapes_sphere_t));
	int s2_id =
	    sw_graph_insert_node(&g, -1, SW_SHAPE_SPHERE, "Sphere 2", &s2, sizeof(sw_shapes_sphere_t));

	sw_transform_translation_t t1 = sw_transform_default_translation();
	sw_transform_translation_t t2 = sw_transform_default_translation();
	t1.x = -0.25f;
	t2.x = 0.25f;

	sw_graph_insert_node(&g, s1_id, SW_TRANSFORM_TRANSLATION, "Translation 1", &t1,
	                     sizeof(sw_transform_translation_t));
	sw_graph_insert_node(&g, s2_id, SW_TRANSFORM_TRANSLATION, "Translation 2", &t2,
	                     sizeof(sw_transform_translation_t));

	sw_csg_smooth_t smunion = (sw_csg_smooth_t){.k = 0.05f};
	int smunion_id = sw_graph_insert_node(&g, -1, SW_CSG_SMOOTH_UNION, "Smooth Union", &smunion,
	                                      sizeof(sw_csg_smooth_t));
	sw_graph_set_parent(&g, s1_id, smunion_id);
	sw_graph_set_parent(&g, s2_id, smunion_id);

	kinc_log(KINC_LOG_LEVEL_INFO, "%d bytes in %d allocations", kr_allocation_size(), kr_allocation_count());

	sw_sdf_t *sdf = sw_sdf_generate(&g, -1);
	kr_vec4_t d;
	sw_sdf_stack_frame_t *stack = sw_sdf_stack_init(sdf);
	d = sw_sdf_compute_color(sdf, (kr_vec3_t){-1.25f, 0.0f, 0.0f}, stack);
	kinc_log(KINC_LOG_LEVEL_INFO, "R %0.3f G %0.3f B %0.3f Distance %0.3f", d.x, d.y, d.z, d.w);
	d = sw_sdf_compute_color(sdf, (kr_vec3_t){1.25f, 0.0f, 0.0f}, stack);
	kinc_log(KINC_LOG_LEVEL_INFO, "R %0.3f G %0.3f B %0.3f Distance %0.3f", d.x, d.y, d.z, d.w);
	d = sw_sdf_compute_color(sdf, (kr_vec3_t){2.25f, 0.0f, 0.0f}, NULL);
	kinc_log(KINC_LOG_LEVEL_INFO, "R %0.3f G %0.3f B %0.3f Distance %0.3f", d.x, d.y, d.z, d.w);

	sw_sdf_stack_destroy(stack);
	sw_sdf_destroy(sdf);

	kr_destroy();
	return 0;
}