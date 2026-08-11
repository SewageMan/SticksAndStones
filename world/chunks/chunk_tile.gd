extends Object

class_name Tile

var tile_pos: Vector2i
var tile_pos_units: Vector2i

var floor_stack: Array[FloorTileDescriptor] = []
var current_floor: FloorTileDescriptor = null


func _init(tile_pos_in: Vector2i) -> void:
	tile_pos = tile_pos_in
	tile_pos_units = Global.tile_size * tile_pos_in

	current_floor = FloorTileManager.test_floor
