
#include <main_engine.hpp>

namespace engine {
	namespace bullshit {
		TextureId get_texture_id(std::string texture_path) {
			return 0;
		};
		DrawElementId get_multimesh_id(uint32_t layer_id, uint32_t sublayer_id, TextureId texture_id) {
			return 0;
		};
		DrawElementId add_to_multimesh(uint32_t layer_id, uint32_t sublayer_id, DrawElementId multimesh_id, Vector2f pos, Vector2f size) {
			return 0;
		};
		void edit_in_multimesh(uint32_t layer_id, uint32_t sublayer_id, DrawElementId multimesh_id, DrawElementId element_id, Vector2f pos, Vector2f size) {

		};
		void delete_from_multimesh(uint32_t layer_id, uint32_t sublayer_id, DrawElementId multimesh_id, DrawElementId element_id) {

		};
	}
}

int main() {
	engine::out_stream = &std::cout;
	engine::print("asfassdf",engine::Vector2<int>(515,56432));
	engine::MainEngine::instance.perform_process(1);
	return 0;
}