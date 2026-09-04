#pragma once

#include <cmath>
#include <string>
#include <format>
#include <vector.hpp>

namespace engine {
	template <typename T, int V> // V is unique identifier for each unit type	
	struct UnitValue {
		float value;

		constexpr UnitValue() : value(0) {}
		explicit constexpr UnitValue(float value) : value(value) {}

		static constexpr UnitValue<T, V> zero() {
			return UnitValue(0);
		}

		constexpr UnitValue<T, V> operator+(UnitValue<T, V> other) const { return UnitValue<T, V>(value + other.value); }
		constexpr UnitValue<T, V> operator-(UnitValue<T, V> other) const { return UnitValue<T, V>(value - other.value); }
		constexpr UnitValue<T, V>& operator+=(UnitValue<T, V> other) { value += other.value; return *this; }
		constexpr UnitValue<T, V>& operator-=(UnitValue<T, V> other) { value -= other.value; return *this; }

		constexpr UnitValue<T, V> operator*(float scalar) const { return UnitValue<T, V>(value * scalar); }
		constexpr UnitValue<T, V> operator/(float scalar) const { return UnitValue<T, V>(value / scalar); }
		constexpr UnitValue<T, V>& operator*=(float scalar) { value *= scalar; return *this; }
		constexpr UnitValue<T, V>& operator/=(float scalar) { value /= scalar; return *this; }

		constexpr float operator/(UnitValue<T, V> other) const { return value / other.value; }

		constexpr auto operator<=>(const UnitValue<T, V>&) const = default;
		constexpr bool operator==(const UnitValue<T, V>&) const = default;
	};

	template <typename T, int V>
	UnitValue<T, V> operator*(float scalar, UnitValue<T, V> unit) { return UnitValue<T, V>(scalar * unit.value); }

	template <int V> // V is unique identifier for each unit type
	using FloatValue = UnitValue<float, V>;

	template <int V> // V is unique identifier for each unit type
	using IntValue = UnitValue<int, V>;

#define DERIVE_FLOAT_VALUE(DerivedType, ID) \
    using FloatValue<ID>::FloatValue; \
    constexpr DerivedType(const FloatValue<ID>& other) : FloatValue<ID>(other.value) {} \

	struct TimeSquared;

	struct Time : FloatValue<0> {

		DERIVE_FLOAT_VALUE(Time, 0);

		constexpr float seconds() {
			return value;
		}

		constexpr static Time seconds(float value) {
			return Time(value);
		}

		std::string to_string() const {
			float abs_val = std::abs(value);
			if (abs_val >= 3600.0f) return std::format("{:.2f} h", value / 3600.0f);
			else if (abs_val >= 60.0f) return std::format("{:.2f} min", value / 60.0f);
			else if (abs_val >= 0.001f) return std::format("{:.3f} s", value);
			else return std::format("{:.3f} ms", value * 1000.0f);
		}

		constexpr TimeSquared square();
	};

	struct Area;

	struct Volume;

	struct Distance : FloatValue<1> {

		DERIVE_FLOAT_VALUE(Distance, 1);

		constexpr float meters() {
			return value;
		}

		constexpr static Distance meters(float value) {
			return Distance(value);
		}

		std::string to_string() const {
			float abs_val = std::abs(value);
			if (abs_val >= 1e6f) return std::format("{:.2f} Mm", value / 1e6f);
			else if (abs_val >= 1e3f) return std::format("{:.2f} km", value / 1e3f);
			else if (abs_val >= 1.0f) return std::format("{:.3f} m", value);
			else if (abs_val >= 1e-3f) return std::format("{:.3f} mm", value * 1e3f);
			else if (abs_val >= 1e-6f) return std::format("{:.3f} µm", value * 1e6f);
			else return std::format("{:.3f} nm", value * 1e9f);
		}

		constexpr Area square();

		constexpr Volume cube();
	};

	struct Mass : FloatValue<2> {

		DERIVE_FLOAT_VALUE(Mass, 2);

		constexpr float kilograms() {
			return value;
		}

		constexpr static Mass kilograms(float value) {
			return Mass(value);
		}

		std::string to_string() const {
			float abs_val = std::abs(value);
			if (abs_val >= 1e3f) return std::format("{:.2f} t", value / 1e3f);
			else if (abs_val >= 1.0f) return std::format("{:.3f} kg", value);
			else if (abs_val >= 1e-3f) return std::format("{:.3f} g", value * 1e3f);
			else return std::format("{:.3f} mg", value * 1e6f);
		}
	};

	struct SpeedSquared;

	struct Speed : FloatValue<3> {

		DERIVE_FLOAT_VALUE(Speed, 3);

		constexpr float meters_per_second() {
			return value;
		}

		constexpr static Speed meters_per_second(float value) {
			return Speed(value);
		}

		std::string to_string() const {
			float abs_val = std::abs(value);
			if (abs_val >= 1000.0f) return std::format("{:.2f} km/s", value / 1000.0f);
			else if (abs_val >= 1.0f) return std::format("{:.3f} m/s", value);
			else if (abs_val >= 0.001f) return std::format("{:.3f} mm/s", value * 1000.0f);
			else return std::format("{:.3f} µm/s", value * 1e6f);
		}

		std::string to_string_kph() const {
			float kph = value * 3.6f;
			float abs_val = std::abs(kph);
			return std::format("{:.2f} km/h", kph);
		}

		constexpr SpeedSquared square();
	};

	constexpr Speed operator/(Distance a, Time b) {
		return Speed(a.value / b.value);
	}

	constexpr Distance operator*(Speed a, Time b) {
		return Distance(a.value * b.value);
	}

	constexpr Distance operator*(Time a, Speed b) {
		return Distance(a.value * b.value);
	}

	struct Area : FloatValue<4> {

		DERIVE_FLOAT_VALUE(Area, 4);

		constexpr float meters_squared() {
			return value;
		}

		constexpr static Area meters_squared(float value) {
			return Area(value);
		} 

		std::string to_string() const {
			float abs_val = std::abs(value);
			if (abs_val >= 1e6f) return std::format("{:.2f} km²", value / 1e6f);
			else if (abs_val >= 1.0f) return std::format("{:.3f} m²", value);
			else if (abs_val >= 1e-4f) return std::format("{:.3f} cm²", value * 1e4f);
			else return std::format("{:.3f} mm²", value * 1e6f);
		}
	};

	constexpr Area operator*(Distance a, Distance b) {
		return Area(a.value * b.value);
	}

	constexpr Area Distance::square() {
		return Area(value * value);
	}

	struct Volume : FloatValue<5> {

		DERIVE_FLOAT_VALUE(Volume, 5);

		constexpr float cubic_meters() {
			return value;
		}

		constexpr static Volume cubic_meters(float value) {
			return Volume(value);
		}

		std::string to_string() const {
			float abs_val = std::abs(value);
			if (abs_val >= 1e6f) return std::format("{:.2f} km³", value / 1e6f);
			else if (abs_val >= 1.0f) return std::format("{:.3f} m³", value);
			else if (abs_val >= 1e-6f) return std::format("{:.3f} cm³", value * 1e6f);
			else return std::format("{:.3f} mm³", value * 1e9f);
		}
	};

	constexpr Volume operator*(Area a, Distance b) {
		return Volume(a.value * b.value);
	}

	constexpr Volume operator*(Distance a, Area b) {
		return Volume(a.value * b.value);
	}

	constexpr Volume Distance::cube() {
		return Volume(value * value * value);
	}

	struct Density : FloatValue<6> {

		DERIVE_FLOAT_VALUE(Density, 6);

		constexpr float kilograms_per_cubic_meter() {
			return value;
		}

		constexpr static Density kilograms_per_cubic_meter(float value) {
			return Density(value);
		}

		std::string to_string() const {
			float abs_val = std::abs(value);
			if (abs_val >= 1e3f) return std::format("{:.3f} kg/m³", value);
			else if (abs_val >= 1.0f) return std::format("{:.3f} g/cm³", value / 1000.0f);
			else return std::format("{:.3f} mg/cm³", value);
		}
	};

	constexpr Density operator/(Mass a, Volume b) {
		return Density(a.value / b.value);
	}

	struct TimeSquared : FloatValue<7> {

		DERIVE_FLOAT_VALUE(TimeSquared, 7);

		float seconds() {
			return std::sqrt(value);
		}

		constexpr static TimeSquared seconds(float value) {
			return TimeSquared(value * value);
		}

		constexpr float seconds_squared() {
			return value;
		}

		constexpr static TimeSquared seconds_squared(float value) {
			return TimeSquared(value);
		}

		std::string to_string() const {
			return std::format("{:.3f} s²", value);
		}
	};

	constexpr TimeSquared operator*(Time a, Time b) {
		return TimeSquared(a.value * b.value);
	}

	constexpr TimeSquared Time::square() {
		return TimeSquared(value * value);
	}

	struct Acceleration : FloatValue<8> {

		DERIVE_FLOAT_VALUE(Acceleration, 8);

		constexpr float meters_per_second_squared() {
			return value;
		}

		constexpr static Acceleration meters_per_second_squared(float value) {
			return Acceleration(value);
		}

		std::string to_string() const {
			float abs_val = std::abs(value);
			if (abs_val >= 1e6f) return std::format("{:.2f} Mm/s²", value / 1e6f);
			else if (abs_val >= 1e3f) return std::format("{:.2f} km/s²", value / 1e3f);
			else if (abs_val >= 1.0f) return std::format("{:.3f} m/s²", value);
			else if (abs_val >= 1e-3f) return std::format("{:.3f} mm/s²", value * 1e3f);
			else return std::format("{:.3f} µm/s²", value * 1e6f);
		}
	};

	constexpr Acceleration operator/(Distance a, TimeSquared b) {
		return Acceleration(a.value / b.value);
	}

	struct Force : FloatValue<9> {

		DERIVE_FLOAT_VALUE(Force, 9);

		constexpr float newtons() {
			return value;
		}

		constexpr static Force newtons(float value) {
			return Force(value);
		}

		std::string to_string() const {
			float abs_val = std::abs(value);
			if (abs_val >= 1e6f) return std::format("{:.2f} MN", value / 1e6f);
			else if (abs_val >= 1e3f) return std::format("{:.2f} kN", value / 1e3f);
			else if (abs_val >= 1.0f) return std::format("{:.3f} N", value);
			else if (abs_val >= 1e-3f) return std::format("{:.3f} mN", value * 1e3f);
			else return std::format("{:.3f} µN", value * 1e6f);
		}
	};

	constexpr Force operator*(Mass a, Acceleration b) {
		return Force(a.value * b.value);
	}

	constexpr Force operator*(Acceleration a, Mass b) {
		return Force(a.value * b.value);
	}

	struct Impulse : FloatValue<10> {

		DERIVE_FLOAT_VALUE(Impulse, 10);

		constexpr float kilogram_meters_per_second() {
			return value;
		}

		constexpr static Impulse kilogram_meters_per_second(float value) {
			return Impulse(value);
		}

		std::string to_string() const {
			float abs_val = std::abs(value);
			if (abs_val >= 1e6f) return std::format("{:.2f} MN·s", value / 1e6f);
			else if (abs_val >= 1e3f) return std::format("{:.2f} kN·s", value / 1e3f);
			else if (abs_val >= 1.0f) return std::format("{:.3f} N·s", value);
			else if (abs_val >= 1e-3f) return std::format("{:.3f} mN·s", value * 1e3f);
			else return std::format("{:.3f} µN·s", value * 1e6f);
		}
	};

	constexpr Impulse operator*(Mass a, Speed b) {
		return Impulse(a.value * b.value);
	}

	constexpr Impulse operator*(Speed a, Mass b) {
		return Impulse(a.value * b.value);
	}

	constexpr Speed operator/(Impulse a, Mass b) {
		return Speed(a.value / b.value);
	}

	struct SpeedSquared : FloatValue<11> {

		DERIVE_FLOAT_VALUE(SpeedSquared, 11);

		constexpr float meters_squared_per_second_squared() {
			return value;
		}

		constexpr static SpeedSquared meters_squared_per_second_squared(float value) {
			return SpeedSquared(value);
		}

		std::string to_string() const {
			float abs_val = std::abs(value);
			if (abs_val >= 1e12f) return std::format("{:.2f} km²/s²", value / 1e12f);
			else if (abs_val >= 1e6f) return std::format("{:.2f} Mm²/s²", value / 1e6f);
			else if (abs_val >= 1.0f) return std::format("{:.3f} m²/s²", value);
			else if (abs_val >= 1e-6f) return std::format("{:.3f} mm²/s²", value * 1e6f);
			else return std::format("{:.3f} µm²/s²", value * 1e12f);
		}
	};

	constexpr SpeedSquared operator*(Speed a, Speed b) {
		return SpeedSquared(a.value * b.value);
	}

	constexpr SpeedSquared Speed::square() {
		return SpeedSquared(value * value);
	}

	struct Energy : FloatValue<12> {

		DERIVE_FLOAT_VALUE(Energy, 12);

		constexpr float joules() {
			return value;
		}

		constexpr static Energy joules(float value) {
			return Energy(value);
		}

		std::string to_string() const {
			float abs_val = std::abs(value);
			if (abs_val >= 1e9f) return std::format("{:.2f} GJ", value / 1e9f);
			else if (abs_val >= 1e6f) return std::format("{:.2f} MJ", value / 1e6f);
			else if (abs_val >= 1e3f) return std::format("{:.2f} kJ", value / 1e3f);
			else if (abs_val >= 1.0f) return std::format("{:.3f} J", value);
			else if (abs_val >= 1e-3f) return std::format("{:.3f} mJ", value * 1e3f);
			else return std::format("{:.3f} µJ", value * 1e6f);
		}
	};

	constexpr Energy operator*(Mass a, SpeedSquared b) {
		return Energy(a.value * b.value);
	}

	constexpr Energy operator*(SpeedSquared a, Mass b) {
		return Energy(a.value * b.value);
	}

	struct Power : FloatValue<13> {

		DERIVE_FLOAT_VALUE(Power, 13);

		constexpr float watts() {
			return value;
		}

		constexpr static Power watts(float value) {
			return Power(value);
		}

		std::string to_string() const {
			float abs_val = std::abs(value);
			if (abs_val >= 1e9f) return std::format("{:.2f} GW", value / 1e9f);
			else if (abs_val >= 1e6f) return std::format("{:.2f} MW", value / 1e6f);
			else if (abs_val >= 1e3f) return std::format("{:.2f} kW", value / 1e3f);
			else if (abs_val >= 1.0f) return std::format("{:.3f} W", value);
			else if (abs_val >= 1e-3f) return std::format("{:.3f} mW", value * 1e3f);
			else return std::format("{:.3f} µW", value * 1e6f);
		}
	};

	constexpr Power operator/(Energy a, Time b) {
		return Power(a.value / b.value);
	}

	struct Pressure : FloatValue<14> {

		DERIVE_FLOAT_VALUE(Pressure, 14);

		static constexpr float ATM = 101325.0f;

		constexpr float pascals() {
			return value;
		}

		constexpr static Pressure pascals(float value) {
			return Pressure(value);
		}

		std::string to_string() const {
			float abs_val = std::abs(value);
			if (abs_val >= 1e9f) return std::format("{:.2f} GPa", value / 1e9f);
			else if (abs_val >= 1e6f) return std::format("{:.2f} MPa", value / 1e6f);
			else if (abs_val >= 1e3f) return std::format("{:.2f} kPa", value / 1e3f);
			else if (abs_val >= 1.0f) return std::format("{:.3f} Pa", value);
			else if (abs_val >= 1e-3f) return std::format("{:.3f} mPa", value * 1e3f);
			else return std::format("{:.3f} µPa", value * 1e6f);
		}

		std::string to_string_atm() const {
			float atm = value / ATM;
			float abs_val = std::abs(atm);
			if (abs_val >= 1000.0f) {
				return std::format("{:.3f} kATM", atm / 1000.0f);
			}
			else if (abs_val >= 1.0f) {
				return std::format("{:.3f} ATM", atm);
			}
			else if (abs_val >= 0.001f) {
				return std::format("{:.3f} mATM", atm * 1000.0f);
			}
			else {
				return std::format("{:.3f} µATM", atm * 1e6f);
			}
		}
	};

	constexpr Pressure operator/(Force a, Area b) {
		return Pressure(a.value / b.value);
	}

	struct Temperature : FloatValue<15> {

		DERIVE_FLOAT_VALUE(Temperature, 15);

		static constexpr float K_at_0_C = 273.15;

		constexpr float kelvins() {
			return value;
		}

		constexpr static Temperature kelvins(float value) {
			return Temperature(value);
		}

		constexpr float celsius() {
			return value + K_at_0_C;
		}

		constexpr static Temperature celsius(float value) {
			return Temperature(value - K_at_0_C);
		}

		std::string to_string() const {
			float abs_val = std::abs(value);
			if (abs_val >= 1.0f) return std::format("{:.2f} K", value);
			else if (abs_val >= 1e-3f) return std::format("{:.3f} mK", value * 1e3f);
			else return std::format("{:.3f} µK", value * 1e6f);
		}

		std::string to_string_celsius() const {
			float celsius = value - K_at_0_C;
			return std::format("{:.1f} °C", celsius);
		}
	};

	typedef Vector2<Speed> Vector2Speed;
	typedef Vector2<Distance> Vector2Distance;

	template<typename T, int V, typename U>
	inline Vector2<UnitValue<T, V>> operator*(Vector2<UnitValue<T, V>> lhs, Vector2<U> rhs) {
		return Vector2<UnitValue<T, V>>(
			lhs.x.value * rhs.x,
			lhs.y.value * rhs.y
		);
	}

	template<typename T, int V, typename U>
	inline Vector2<UnitValue<T, V>> operator*(Vector2<U> lhs, Vector2<UnitValue<T, V>> rhs) {
		return rhs * lhs;
	}

	// current max FloatValue id = 15
	// current max IntValue id = none
}