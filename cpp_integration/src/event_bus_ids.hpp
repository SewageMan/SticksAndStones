#pragma once

namespace engine {
	namespace event_bus_ids {

		// NOTE
		// if data fits in 8 bytes we send it by value,
		// so the buffer that you get on the other end directly contains it
		// if the data more than 8 bytes,
		// then we send a pointer to where the data lies

		// !!! IMPORTANT !!!
		// if a we send pointer to data -
		// dont fotget to dereference it,
		// otherwise you will read garbage or reach end of buffer
		// use: "data_buffer = payload.dereference_data(size)"
		// where size is supposed size of data
		// that is serialized on the other size
		// (can be less, but never more than it)
		// since payload is only 8 bytes it can only fit
		// pointer to data but not its size.

		enum event_bus_ids {
			RESERVED_ZERO_INDEX = 0,
			// reserved just incase

			SET_PLAYER_STATE = 1,
			// sends pointer to data, data includes:
			// 8 bytes - vector2f player position
			// 8 bytes - vector2f player speed

			SET_PLAYER_SPEED = 2,
			// sends data directly:
			// 8 bytes - vector2f new player speed

			UPDATE_CAMERA_CHUNK = 3,
			// sends data directly:
			// 8 bytes - Vector2Chunks camera chunk offset

			MULTIPLY_CAMERA_ZOOM = 4
			// sends data directly:
			// 4 bytes - float number to multiply zoom by
		};
	}
}