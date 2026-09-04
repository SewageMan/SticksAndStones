

//#include <main_engine.hpp>
#include <godot_adapter.hpp>

//namespace engine {
//	namespace bullshit {
//		TextureId get_texture_id(std::string texture_path) {
//			return 0;
//		}
//		DrawElementId get_multimesh_id(uint32_t layer_id, uint32_t sublayer_id, TextureId texture_id) {
//			return 0;
//		}
//		DrawElementId add_to_multimesh(uint32_t layer_id, uint32_t sublayer_id, DrawElementId multimesh_id, Vector2Chunks pos_chunks, Vector2Unitsf pos_units, Vector2Unitsf size) {
//			return 0;
//		}
//		void edit_in_multimesh(uint32_t layer_id, uint32_t sublayer_id, DrawElementId multimesh_id, DrawElementId element_id, Vector2Chunks pos_chunks, Vector2Unitsf pos_units, Vector2Unitsf size) {
//
//		}
//		void delete_from_multimesh(uint32_t layer_id, uint32_t sublayer_id, DrawElementId multimesh_id, DrawElementId element_id) {
//
//		}
//	}
//}

int main() {
	engine::out_stream = &std::cout;
	engine::print("asfassdf",engine::Vector2<int>(515,56432));
	engine::MainEngine::instance.perform_process(engine::Time::seconds(1));
	return 0;
}