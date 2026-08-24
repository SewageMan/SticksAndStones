#pragma once

#include <vector.hpp>

namespace engine {

	template <typename T>
	struct Camera {
		Vector2<T> pos;
		Vector2<T> size;
		Vector2<T> half_size;
		double zoom;
		double zoom_inv;

		Camera(Vector2<T> pos, Vector2<T> size, double zoom) : pos(pos), size(size), half_size(size / 2), zoom(zoom), zoom_inv(1 / zoom) {}

		void set(Vector2<T> pos, Vector2<T> size, double zoom) {
			this->pos = pos;
			this->size = size;
			this->half_size = size / 2;
			this->zoom = zoom;
			this->zoom_inv = 1 / zoom;
		}

		template<typename IN, typename OUT>
		Vector2<OUT> world_to_screen(const Vector2<IN> world_pos) const {
			Vector2<OUT> result = static_cast<Vector2<OUT>>(world_pos);
			result -= static_cast<Vector2<OUT>>(pos);
			result *= zoom;
			result += static_cast<Vector2<OUT>>(half_size);
			result.flip_y();
			return result;
		}

		Vector2<T> world_to_screen(const Vector2<T> world_pos) const {
			return world_to_screen<T, T>(world_pos);
		}

		template<typename IN, typename OUT>
		Vector2<OUT> screen_to_world(const Vector2<IN> screen_pos) const {
			Vector2<OUT> result = static_cast<Vector2<OUT>>(screen_pos);
			result -= static_cast<Vector2<OUT>>(half_size);
			result *= zoom_inv;
			result += static_cast<Vector2<OUT>>(pos);
			result.flip_y();
			return result;
		}

		Vector2<T> screen_to_world(const Vector2<T> screen_pos) const {
			return screen_to_world<T, T>(screen_pos);
		}

		template<typename IN, typename OUT>
		Vector2<OUT> scale_world_to_screen(const Vector2<IN> world_size) const {
			return (static_cast<Vector2<OUT>>(world_size)) * zoom;
		}

		Vector2<T> scale_world_to_screen(const Vector2<T> world_size) const {
			return scale_world_to_screen<T, T>(world_size);
		}

		template<typename IN, typename OUT>
		Vector2<OUT> scale_screen_to_world(const Vector2<IN> screen_size) const {
			return (static_cast<Vector2<OUT>>(screen_size)) * zoom_inv;
		}

		Vector2<T> scale_screen_to_world(const Vector2<T> screen_size) const {
			return scale_screen_to_world<T, T>(screen_size);
		}
	};
}