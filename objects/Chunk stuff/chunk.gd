extends Node2D

@onready var tilemap = $TileMapLayer
@export var chunk_coords = Vector2()
@export var chunk_data = []
@export var block_data = []
@export var World:Node2D 
var type
var waterlevel = -1.2
var rand_obj_numb = []
var rand_obj_pos = []
var rng = RandomNumberGenerator.new()
#Use thise later maybe
var heightmap = FastNoiseLite.new()
var moisturemap = FastNoiseLite.new()
#TODO: make chunk look for interactable entities in the chunk and save their position and state
#TODO: Objects in each biome type:
#Forest:
#Singular objects: weed (id: 1), trees (id: 0), rocks - 2, sticks
#Tile objects: water resource
#Disable/enable collisions
#get_node("CollisionShape2D").disabled = true    # disable
#get_node("CollisionShape2D").disabled = false   # enable
func init(_chunk_coords):
	rng.seed = hash(Global.Seed + str(_chunk_coords))

	#print("Noise: ", noise.get_noise_2d(1,1))
	#print(rng.seed, " ", _chunk_coords)
	chunk_coords = _chunk_coords
	if WorldSave.loaded_coords.find(_chunk_coords) == -1:
		#world generation (doesn't have player placed objects)
		WorldSave.add_chunk(chunk_coords)
		#Tree data:
		rand_obj_numb.append(floor(10 + rng.randf() * 20.0))
		rand_obj_pos.append(generateObjects(rand_obj_numb[0], chunk_coords, ""))
		
		#Grass data:
		rand_obj_numb.append(floor(40 + rng.randf() * 80.0))
		rand_obj_pos.append(generateObjects(rand_obj_numb[1], chunk_coords, "forier"))
		
		#Boulder data:
		rand_obj_numb.append(floor(rng.randf() * 1.5))
		rand_obj_pos.append(generateObjects(rand_obj_numb[2], chunk_coords, ""))
		#chunk data order, 1 - color of the rect, 2 - array for rand objects, 3 - array for the position of those rnd obj
		chunk_data = [Color(rng.randf(), rng.randf(), rng.randf(), 1), rand_obj_numb, rand_obj_pos]
	else:
		chunk_data = WorldSave.load_chunk_data(chunk_coords)
		block_data = WorldSave.load_block_data(chunk_coords)
	#APPLYING THE LOADED DATA, REMEMBER TO UPDATE WHEN A NEW ENTITY IS ADDED
	for i in range(3):
		applyObjectData(i)
	$Position.text = str(chunk_coords)
	$Position.z_index = 1
	$ColorRect.set_color(chunk_data[0])

func generateObjects(rand_numb, chunk_coords, useNoise):
	var randomVector = Vector2()
	var randomPos = []
	#print(World)
	if useNoise == "":
		for i in range(rand_numb):
			randomVector.x = chunk_coords.x + 64*16 * (0.5-rng.randf())
			randomVector.y = chunk_coords.y + 64*16 * (0.5-rng.randf())
			randomPos.append(randomVector)
	if useNoise == "forier":
		for i in range(rand_numb):
			randomVector.x = chunk_coords.x + 64*8*fourierNoise(1.15*rng.randf())
			randomVector.y = chunk_coords.y + 64*8*fourierNoise(1.15*rng.randf())
			randomPos.append(randomVector)
	return randomPos

func fourierNoise(x):
	var definition = 5
	var sinValue = 0
	for i in range(definition):
		sinValue += sin(rng.randf()*PI*x + rng.randf()*PI)
		sinValue /= log(definition)
	return sinValue

func applyObjectData(Id):
	var chunkObj = preload("res://objects/static_entity.tscn")
	for i in range(chunk_data[1][Id]):
		#print(chunk_data[2][0][i])
		var height = World.get_2d_noise(round((self.position.x+chunk_data[2][Id][i].x)/64.0) + 8, round((self.position.y+chunk_data[2][Id][i].y)/64.0) + 8, 0)
		if height > waterlevel+0.1:
			var obj = chunkObj.instantiate()
			obj.position = chunk_data[2][Id][i]
			obj.z_index = 2
			obj.name = "static_entity" + str(Id) + " : " + str(i)
			obj.identify(Id)
			#obj.get_child(Id).get_child(0).frame = floor(0.5+randf())
			add_child(obj)

func save():
	WorldSave.save_chunk(chunk_coords, chunk_data)
	call_deferred("queue_free")

func _ready():
	#Grass filling
	#typeList: Array
	var waterList: Array
	for i in range(16):
		for j in range(16):
			var tilepos: Vector2 = Vector2(round(self.position.x/64.0)+i, round(self.position.y/64.0)+j)
			var height = World.get_2d_noise(tilepos.x, tilepos.y, 0)
			#old multitile solution that didn't look quite right
			#tilemap.set_cell(-1, Vector2i(i-8,j-8), 0, Vector2i(1,1 + abs(3*ceil(2.25*fourierNoise((i*16 + j)*0.075+0.01*rng.randf())))), 0)
			if height < waterlevel:
				#print(blockList)
				BetterTerrain.set_cell(tilemap, Vector2i(i-8,j-8), 1)
				#tilemap.set_cell(Vector2i(i-8,j-8), 0, Vector2i(15,47))
				waterList.append(Vector2i(i-8,j-8))
			else:
				#other tile
				BetterTerrain.set_cell(tilemap, Vector2i(i-8,j-8), 0)
				#tilemap.set_cell(Vector2i(i-8,j-8), 0, Vector2i(15,15))
	BetterTerrain.update_terrain_cells(tilemap, waterList, true)
	#tilemap.set_cells_terrain_connect(waterList, 0, 2)
	#self.get_child(3).get_child(0).
