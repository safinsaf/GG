#include "rasterizer_renderer.h"

#include "utils/resource_utils.h"


void cg::renderer::rasterization_renderer::init()
{
	model = std::make_shared<cg::world::model>();
	model->load_obj(settings->model_path);
	render_target =
		std::make_shared<cg::resource<cg::unsigned_color>>(1920, 1080);
	depth_buffer = std::make_shared<cg::resource<float>>(1920, 1080);

	camera = std::make_shared<cg::world::camera>();
	camera->set_height(static_cast<float>(settings->height));
	camera->set_width(static_cast<float>(settings->width));
	camera->set_position(float3{ settings->camera_position[0],
								 settings->camera_position[1],
								 settings->camera_position[2]});
	camera->set_theta(settings->camera_theta);
	camera->set_phi(settings->camera_phi);
	camera->set_angle_of_view(settings->camera_angle_of_view);
	camera->set_z_far(settings->camera_z_far);
	camera->set_z_near(settings->camera_z_near);

	rasterizer =std::make_shared<cg::renderer::rasterizer<cg::vertex, cg::unsigned_color>>();
	rasterizer->set_render_target(render_target, depth_buffer);
	rasterizer->set_vertex_buffer(model->get_vertex_buffer());
	rasterizer->set_viewport(settings->width, settings->height);
}

void cg::renderer::rasterization_renderer::destroy() {}

void cg::renderer::rasterization_renderer::update() {}

void cg::renderer::rasterization_renderer::render()
{
	float4x4 matrix = mul(camera->get_projection_matrix(), camera->get_view_matrix(), model->get_world_matrix());
	rasterizer->vertex_shader = [&](float4 vertex, cg::vertex vertex_data) {
		auto processed_vertex = mul(matrix, vertex);
		return std::make_pair(processed_vertex, vertex_data);
	};
	rasterizer->pixel_shader = [](cg::vertex vertex_data, float z) {
		return cg::color{ vertex_data.ambient_r, vertex_data.ambient_g,
						  vertex_data.ambient_b };
	};
	rasterizer->clear_render_target({255,255,0});
	rasterizer->draw(model->get_vertex_buffer()->get_number_of_elements(), 0);
	cg::utils::save_resource(*render_target, settings->result_path);
}