extends Node2D

class_name Chunk

var chunk_pos: Vector2i  # position of chunk corner in chunk grid
var cunk_pos_tile: Vector2i  # position of chunk corner tile in tile grid
var chunk_pos_unit: Vector2i  # position of chunk corner in world coordinates

var flat_matrix_tiles: Array[Tile] = []  # matrix made with flat array to store tiles attached to chunk

var neighbours: Array[Chunk]

var is_data_initialised: bool = false
var is_graphics_initialised: bool = false
var is_process_running: bool = false
var is_graphics_running: bool = false


func _init(chunk_pos_in: Vector2i) -> void:
	chunk_pos = chunk_pos_in
	cunk_pos_tile = chunk_pos_in * Global.chunk_size_tiles
	chunk_pos_unit = chunk_pos_in * Global.chunk_size_units

	# we fill the matrix of tiles with fresh tiles
	flat_matrix_tiles.resize(Global.tiles_in_chunk)
	for x in range(Global.chunk_size_tiles):
		for y in range(Global.chunk_size_tiles):
			flat_matrix_tiles[x * Global.chunk_size_tiles + y] = Tile.new(Vector2i(x, y))


func _ready() -> void:
	pass


func _process(delta: float) -> void:
	pass
