#pragma once

#include <helper.hpp>


namespace engine {

	enum class ShapeType {
		AxisAlignedRect
	};

	struct Shape {

		bool is_colliding(const Shape& other) const {
			if (get_bbox().is_colliding(other.get_bbox())) {
				return is_colliding_internal(other);
			}
			else {
				return false;
			}
		};

		virtual void move(Vector2f move_by) = 0;

		virtual bool is_colliding_internal(const Shape& other) const = 0;

		virtual bool is_mesh() const = 0;

		virtual ShapeType get_type() const = 0;

		virtual BoundingBox get_bbox() const = 0;

		virtual Shape* copy() const = 0;
	};

	struct MeshShape : Shape {

		virtual bool is_mesh() const override {
			return true;
		}

		virtual bool is_colliding_internal(const Shape& other) const override {
			if (other.is_mesh()) {
				return is_colliding_internal_mesh(*static_cast<const MeshShape*>(&other));
			}
			else {
				panic("not implemented");
			}
		}

		virtual bool is_colliding_internal_mesh(const MeshShape& other) const = 0;

		virtual Vector2f get_mtv(const MeshShape& other) const = 0;

		virtual std::span<Vector2f> get_axis() const = 0;

		virtual MeshShape* copy() const = 0;
	};

	struct AxisAlignedRect final : MeshShape  {

		inline static std::array<Vector2f, 2> axis = { Vector2f(0,1), Vector2f(1,0) };
		inline static std::span<Vector2f> axis_span{ axis };

		BoundingBox bbox;

		AxisAlignedRect(BoundingBox bbox) : bbox(bbox) {}

		virtual bool is_colliding_internal_mesh(const MeshShape& other) const {
			if (other.get_type() == ShapeType::AxisAlignedRect) {
				return true;
			}
			else {
				panic("not implemented");
			}
		};

		virtual Vector2f get_mtv(const MeshShape& other) const {
			if (other.get_type() == ShapeType::AxisAlignedRect) {
				BoundingBox other_bbox = static_cast<const AxisAlignedRect&>(other).bbox;

				float overlap_x = std::min(bbox.high.x, other_bbox.high.x) - std::max(bbox.low.x, other_bbox.low.x);
				float overlap_y = std::min(bbox.high.y, other_bbox.high.y) - std::max(bbox.low.y, other_bbox.low.y);

				if (overlap_x <= 0 or overlap_y <= 0) {
					return Vector2f(0, 0);
				}

				if (overlap_x < overlap_y) {
					float direction = (bbox.center().x < other_bbox.center().x) ? -1 : 1;
					return Vector2f(direction * overlap_x, 0);
				}
				else {
					float direction = (bbox.center().y < other_bbox.center().y) ? -1 : 1;
					return Vector2f(0, direction * overlap_y);
				}
			}
			else {
				panic("not implemented");
			}
		}

		virtual void move(Vector2f move_by) override {
			bbox.move(move_by);
		}

		virtual ShapeType get_type() const override {
			return ShapeType::AxisAlignedRect;
		}

		virtual BoundingBox get_bbox() const override {
			return bbox;
		}

		virtual std::span<Vector2f> get_axis() const override {
			return axis_span;
		}

		virtual AxisAlignedRect* copy() const override {
			return new AxisAlignedRect(bbox);
		}
	};
}