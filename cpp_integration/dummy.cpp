
#include <main_engine.hpp>

int main() {
	engine::out_stream = &std::cout;
	engine::print("asfassdf",engine::Vector2<int>(515,56432));
	engine::MainEngine::instance.perform_process(1);
	return 0;
}