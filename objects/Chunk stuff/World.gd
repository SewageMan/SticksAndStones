extends Node2D

@onready var chunknode = preload("res://objects/Chunk stuff/Chunk.tscn")
@onready var chunkObj = preload("res://objects/static_entity.tscn")
@onready var player = preload("res://objects/Player.tscn")
@onready var active_coord = []
@onready var active_chunks = []
@export var render_distance = 1
@export var chunk_size = 64*16
@export var day_cycle: float = 5000.0
@export var day_count: int = 0
@export var daynight_gradient: Gradient = preload("res://Sprites/Misc/sun_grad.tres")
@export var world_temp: float
@export var day_state: float = 0.5:
	set(value):
		day_state = value
		$cycle_hue.color = daynight_gradient.sample(value)
		#world temperature (regardless of biome)
		world_temp = 7.5*cos(day_state * 2*PI - PI) + 12.5*cos((day_count/190.0) * PI)

var current_chunk = Vector2()
var previous_chunk = Vector2()
var chunk_loaded = false

# Called when the node enters the scene tree for the first time.
func _ready():
	heightmap.seed = hash(Global.Seed)
	heightmap.frequency = 0.011
	moistmap.seed =  hash(Global.Seed)
	heightmap.noise_type = FastNoiseLite.TYPE_SIMPLEX_SMOOTH
	moistmap.noise_type = FastNoiseLite.TYPE_PERLIN
	current_chunk = floor(Global.player.global_position/chunk_size)
	load_chunk()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var playerPos = Global.player.global_position
	current_chunk = floor(playerPos/chunk_size)
	if previous_chunk != current_chunk:
		if !chunk_loaded:
			load_chunk()
		else:
			chunk_loaded = false
		previous_chunk = current_chunk

func _get_player_chunk(pos):
	var chunk_pos = Vector2()
	chunk_pos.y = int(pos.y/chunk_size)
	chunk_pos.x = int(pos.x/chunk_size)
	if pos.x < 0:
		chunk_pos.x -= 1
	if pos.y < 0:
		chunk_pos.y -= 1
	return chunk_pos
#TODO: The chunks spawn incorrectly based on chunk size, load_chunk function needs tweaking
#The render bound in the rect width of the render distance. ie render distance of 2
#will be multiplied by 2 to make it 4 the +1 which makes it 5, 5 chunks on the x and y axis with 
#a render distance of 2
func load_chunk():
	var render_bounds = (float(render_distance)*2.0)+1.0
	var loading_coord = []
	var blockCoords: Array
	#if x = 0, then x+1 = 1
	#if render_bounds = 5 (render distance = 2) then 5/2 = 2.5, (round(2.5)) = 3
	#then 1 - 3 = -2 which is the x coord in the chunk space, this same principle is used
	#for the y axis as well.
	for x in range(render_bounds):
		for y in range(render_bounds):
			var _x  = (x+1) - (round(render_bounds/2.0)) + current_chunk.x
			var _y  = (y+1) - (round(render_bounds/2.0)) + current_chunk.y
			
			var chunk_coords = Vector2(_x, _y)
			#the chunk key is the key the chunk will use to retreive data from the world save
			#it depends on the no of revolutions and the chunk coords
			var chunk_key = chunk_coords
			loading_coord.append(chunk_coords)
			#loading chunks stores the coords that are in the new render chunk
			#this if statement makes sure that only the coords that are not already active are loaded
			if active_coord.find(chunk_coords) == -1:
				#print("chunk created!")
				var chunk = chunknode.instantiate()
				chunk.position = chunk_coords * chunk_size
				active_chunks.append(chunk)
				active_coord.append(chunk_coords)
				chunk.World = self
				chunk.init(chunk_key)
				chunk.name = "Chunk " + str(x) + "_" + str(y)
				add_child(chunk)
	#deleting the chunks just makes an array of chunks that are in active chunks and not in the
	#chunks that are being loaded (loading coords), deleting chunks then deletes them from 
	#both the active chunk and coords array
	var deleting_chunks = []
	for x in active_coord:
		if loading_coord.find(x) == -1:
			deleting_chunks.append(x)
	for x in deleting_chunks:
		var index = active_coord.find(x)
		active_chunks[index].save()
		active_chunks.remove_at(index)
		active_coord.remove_at(index)
	chunk_loaded = true

var heightmap = FastNoiseLite.new()
var moistmap = FastNoiseLite.new()
var maps = [heightmap, moistmap]
func get_2d_noise(x,y, which):
	var value = maps[which].get_noise_2d(x,y)*10
	return value

func _on_timer_timeout() -> void:
	day_state += 1.0 / day_cycle * $Timer.wait_time
	if day_state >= 1.0:
		day_count += 1
		WorldSave.save_day(day_count, day_state)
		day_state = 0.0
