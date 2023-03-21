#include <assert.h>
#include <kinc/graphics4/graphics.h>
#include <kinc/graphics4/indexbuffer.h>
#include <kinc/graphics4/pipeline.h>
#include <kinc/graphics4/shader.h>
#include <kinc/graphics4/vertexbuffer.h>
#include <kinc/graphics4/vertexstructure.h>
#include <kinc/io/filereader.h>
#include <kinc/log.h>
#include <kinc/math/matrix.h>
#include <kinc/math/vector.h>
#include <kinc/system.h>
#include <krink/math/matrix.h>
#include <krink/math/vector.h>
#include <krink/memory.h>
#include <krink/system.h>
#include <shapeware/csg.h>
#include <shapeware/graph.h>
#include <shapeware/mc.h>
#include <shapeware/mesh.h>
#include <shapeware/raymarch.h>
#include <shapeware/sdf.h>
#include <shapeware/shapes.h>
#include <shapeware/transform.h>
#include <stdint.h>
#include <stdlib.h>

// Dev testing the lib here

static void print_triangle(void *p, kr_vec3_t a, kr_vec3_t b, kr_vec3_t c) {
	kinc_log(KINC_LOG_LEVEL_INFO,
	         "Triangle (%5.3f, %5.3f, %5.3f), (%5.3f, %5.3f, %5.3f), (%5.3f, %5.3f, %5.3f)", a.x,
	         a.y, a.z, b.x, b.y, b.z, c.x, c.y, c.z);
}

static void print_triangle_color(void *p, kr_vec3_t a, kr_vec3_t b, kr_vec3_t c, kr_vec3_t ca,
                                 kr_vec3_t cb, kr_vec3_t cc) {
	kinc_log(KINC_LOG_LEVEL_INFO,
	         "Triangle (%5.3f, %5.3f, %5.3f), (%5.3f, %5.3f, %5.3f), (%5.3f, %5.3f, %5.3f) - Color "
	         "(%5.3f, %5.3f, %5.3f), (%5.3f, %5.3f, %5.3f), (%5.3f, %5.3f, %5.3f)",
	         a.x, a.y, a.z, b.x, b.y, b.z, c.x, c.y, c.z, ca.x, ca.y, ca.z, cb.x, cb.y, cb.z, cc.x,
	         cc.y, cc.z);
}

static kinc_g4_vertex_buffer_t vert_buff;
static kinc_g4_index_buffer_t index_buff;
static kinc_g4_shader_t vert_shader;
static kinc_g4_shader_t frag_shader;
static kinc_g4_pipeline_t pipeline;
static kinc_g4_constant_location_t proj_mat_loc;
static kinc_matrix4x4_t proj_mat;
static kinc_matrix4x4_t mvp_mat;
static kinc_matrix4x4_t model_mat;
static kinc_matrix4x4_t view_mat;
static kinc_g4_vertex_structure_t structure;
static kinc_g4_constant_location_t mvp_loc;
static kinc_g4_constant_location_t view_loc;
static kinc_g4_constant_location_t model_loc;
static kinc_g4_constant_location_t light_loc;

static void init_shader(kinc_g4_shader_t *s, const char *filename, kinc_g4_shader_type_t t) {
	kinc_file_reader_t r;
	kinc_file_reader_open(&r, filename, KINC_FILE_TYPE_ASSET);
	size_t size = kinc_file_reader_size(&r);
	void *data = kr_malloc(size);
	assert(data != NULL);
	kinc_file_reader_read(&r, data, size);
	kinc_file_reader_close(&r);

	kinc_g4_shader_init(s, data, size, t);
}

static void init_pipeline(void) {
	init_shader(&vert_shader, "simple.vert", KINC_G4_SHADER_TYPE_VERTEX);
	init_shader(&frag_shader, "simple.frag", KINC_G4_SHADER_TYPE_FRAGMENT);
	kinc_g4_vertex_structure_init(&structure);
	kinc_g4_vertex_structure_add(&structure, "pos", KINC_G4_VERTEX_DATA_FLOAT3);
	kinc_g4_vertex_structure_add(&structure, "nor", KINC_G4_VERTEX_DATA_FLOAT3);
	kinc_g4_vertex_structure_add(&structure, "col", KINC_G4_VERTEX_DATA_FLOAT3);

	kinc_g4_pipeline_init(&pipeline);
	pipeline.input_layout[0] = &structure;
	pipeline.input_layout[1] = NULL;
	pipeline.vertex_shader = &vert_shader;
	pipeline.fragment_shader = &frag_shader;
	pipeline.depth_write = true;
	pipeline.depth_mode = KINC_G4_COMPARE_LESS;
	pipeline.cull_mode = KINC_G4_CULL_CLOCKWISE;
	pipeline.blend_source = KINC_G4_BLEND_SOURCE_ALPHA;
	pipeline.blend_destination = KINC_G4_BLEND_INV_SOURCE_ALPHA;
	pipeline.alpha_blend_source = KINC_G4_BLEND_SOURCE_ALPHA;
	pipeline.alpha_blend_destination = KINC_G4_BLEND_INV_SOURCE_ALPHA;
	kinc_g4_pipeline_compile(&pipeline);

	mvp_loc = kinc_g4_pipeline_get_constant_location(&pipeline, "MVP");
	view_loc = kinc_g4_pipeline_get_constant_location(&pipeline, "V");
	model_loc = kinc_g4_pipeline_get_constant_location(&pipeline, "M");
	light_loc = kinc_g4_pipeline_get_constant_location(&pipeline, "lightPos");

	kr_matrix4x4_t proj = kr_matrix4x4_perspective_projection(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);
	proj_mat = kr_matrix4x4_to_kinc(&proj);

	kr_matrix4x4_t v = kr_matrix4x4_look_at(
	    (kr_vec3_t){0, 0, -4.0f}, // Camera is at (4, 3, 3), in World Space
	    (kr_vec3_t){0, 0, 0},     // and looks at the origin
	    (kr_vec3_t){0, 1.0f, 0}   // Head is up (set to (0, -1, 0) to look upside-down)
	);
	view_mat = kr_matrix4x4_to_kinc(&v);
	kr_matrix4x4_t m = kr_matrix4x4_identity();
	model_mat = kr_matrix4x4_to_kinc(&m);

	mvp_mat = model_mat;
	mvp_mat = kinc_matrix4x4_multiply(&mvp_mat, &proj_mat);
	mvp_mat = kinc_matrix4x4_multiply(&mvp_mat, &view_mat);
	mvp_mat = kinc_matrix4x4_multiply(&mvp_mat, &model_mat);
}

typedef struct sdf_normal_arg {
	sw_sdf_t *sdf;
	sw_sdf_stack_frame_t *stack;
} sdf_normal_arg_t;

static kr_vec3_t vertex_normal(void *param, kr_vec3_t pos) {
	return sw_raymarch_surface_normal(((sdf_normal_arg_t *)param)->sdf,
	                                  ((sdf_normal_arg_t *)param)->stack, pos);
}

static void sdf_to_buffer(const sw_sdf_t *sdf) {
	sdf_normal_arg_t arg = (sdf_normal_arg_t){.sdf = sdf, .stack = sw_sdf_stack_init(sdf)};
	sw_mesh_t *m = sw_mesh_init(1000, 1000, vertex_normal, &arg);
	sw_mc_process_sdf_chunk_color(sdf,
	                              &(sw_mc_chunk_t){.halfsidelen = 1.5f,
	                                               .iso_level = 0.0f,
	                                               .steps = 30,
	                                               .origin = {.x = 0.0f, .y = 0.0f, .z = 0.0f}},
	                              sw_mesh_add_triangle, m);
	sw_sdf_stack_destroy(arg.stack);
	kinc_g4_vertex_buffer_init(&vert_buff, sw_mesh_vert_count(m), &structure, KINC_G4_USAGE_STATIC,
	                           0);
	float *verts = kinc_g4_vertex_buffer_lock_all(&vert_buff);
	sw_mesh_write_vert_buffer(m, verts);
	kinc_g4_vertex_buffer_unlock_all(&vert_buff);

	kinc_g4_index_buffer_init(&index_buff, sw_mesh_tris_count(m) * 3,
	                          KINC_G4_INDEX_BUFFER_FORMAT_32BIT, KINC_G4_USAGE_STATIC);
	int *indices = kinc_g4_index_buffer_lock_all(&index_buff);
	sw_mesh_write_index_buffer(m, indices);
	kinc_g4_index_buffer_unlock_all(&index_buff);
	sw_mesh_destroy(m);
}

static void update(void *unused) {
	kinc_g4_begin(0);
	kinc_g4_clear(KINC_G4_CLEAR_COLOR | KINC_G4_CLEAR_DEPTH, 0xff111111, 1.0f, 0);

	kr_matrix4x4_t m = kr_matrix4x4_identity();
	mvp_mat = kr_matrix4x4_to_kinc(&m);

	kr_matrix4x4_t mod = kr_matrix4x4_rotation(kinc_time() * 0.5, 0, 0);
	mod = kr_matrix4x4_multmat(&m, &mod);
	model_mat = kr_matrix4x4_to_kinc(&mod);
	mvp_mat = kinc_matrix4x4_multiply(&mvp_mat, &proj_mat);
	mvp_mat = kinc_matrix4x4_multiply(&mvp_mat, &view_mat);
	mvp_mat = kinc_matrix4x4_multiply(&mvp_mat, &model_mat);

	kinc_g4_set_pipeline(&pipeline);
	kinc_g4_set_matrix4(mvp_loc, &mvp_mat);
	kinc_g4_set_matrix4(model_loc, &model_mat);
	kinc_g4_set_matrix4(view_loc, &view_mat);

	kinc_g4_set_float3(light_loc, 1.8f, 0.5f, -1.5f);

	kinc_g4_set_vertex_buffer(&vert_buff);
	kinc_g4_set_index_buffer(&index_buff);

	kinc_g4_draw_indexed_vertices();
	kinc_g4_end(0);
	kinc_g4_swap_buffers();
}

int kickstart(int argc, char **argv) {
	static uint8_t mem[2 * 1024 * 1024];
	kr_init(&mem, 2 * 1024 * 1024, NULL, 0);
	sw_graph_t g;
	sw_graph_init(&g, 8, 1024);
	sw_shapes_sphere_t s1 = sw_shapes_default_sphere();
	sw_shapes_sphere_t s2 = sw_shapes_default_sphere();
	sw_shapes_ellipsoid_t s3 = sw_shapes_default_ellipsoid();
	s1.m.r = 1.0f;
	s1.m.g = 0.0f;
	s1.m.b = 0.0f;
	s2.m.r = 0.0f;
	s2.m.g = 0.0f;
	s2.m.b = 1.0f;
	s3.r.x = 0.5f;
	s3.r.y = 0.5f;
	s3.r.z = 2.5f;
	s3.m.r = 0.0f;
	s3.m.g = 1.0f;
	s3.m.b = 0.0f;
	int s1_id =
	    sw_graph_insert_node(&g, -1, SW_SHAPE_SPHERE, "Sphere 1", &s1, sizeof(sw_shapes_sphere_t));
	int s2_id =
	    sw_graph_insert_node(&g, -1, SW_SHAPE_SPHERE, "Sphere 2", &s2, sizeof(sw_shapes_sphere_t));
	int s3_id = sw_graph_insert_node(&g, -1, SW_SHAPE_ELLIPSOID, "Ellipsoid 1", &s3, sizeof(sw_shapes_ellipsoid_t));

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

	sw_csg_smooth_subtraction_t subtr = (sw_csg_smooth_subtraction_t){.subtractor_id = s3_id, .k = 0.05f};
	int subtr_id = sw_graph_insert_node(&g, -1, SW_CSG_SMOOTH_SUBTRACTION, "Smooth Subtraction", &subtr,
	                                    sizeof(sw_csg_smooth_subtraction_t));
	sw_graph_set_parent(&g, s3_id, subtr_id);
	sw_graph_set_parent(&g, smunion_id, subtr_id);

	sw_shapes_box_frame_t s4 = sw_shapes_default_box_frame();
	s4.b.x = 1.2f;
	s4.b.y = 1.2f;
	s4.b.z = 1.2f;
	sw_graph_insert_node(&g, -1, SW_SHAPE_BOX_FRAME, "Box Frame", &s4, sizeof(s4));

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

	sw_mc_process_sdf_chunk(sdf,
	                        &(sw_mc_chunk_t){.halfsidelen = 1.0f, .iso_level = 0.0f, .steps = 10},
	                        print_triangle, NULL);

	sw_mc_process_sdf_chunk_color(
	    sdf, &(sw_mc_chunk_t){.halfsidelen = 1.0f, .iso_level = 0.0f, .steps = 10},
	    print_triangle_color, NULL);

	kinc_init("Shapeware Demo", 1280, 720, NULL, NULL);
	kinc_set_update_callback(update, NULL);
	init_pipeline();
	sdf_to_buffer(sdf);
	sw_sdf_destroy(sdf);
	kinc_start();
	kr_destroy();
	return 0;
}