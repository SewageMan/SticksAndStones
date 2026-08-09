extends Node

var loaded_coords = []
var data_in_chunk = []
var loaded_blocks = []
var blockdata_in_chunk = []
var circuit_data = {}
var day_data = []

#Redoo this to use dictionaries, or dont, this doesn't really make anything worse
#It just might anger the programmers

func add_chunk(coords):
	loaded_coords.append(coords)
	data_in_chunk.append([])
	blockdata_in_chunk.append([])
	loaded_blocks.append([])

func save_blocks(coords, blockCoords,data):
	blockdata_in_chunk[loaded_coords.find(coords)].append(data)
	loaded_blocks[loaded_coords.find(coords)].append(blockCoords)

func save_chunk(coords, data):
	data_in_chunk[loaded_coords.find(coords)] = data

func save_circuit(index, data):
	circuit_data[index] = data

func load_chunk_data(coords):
	var data = data_in_chunk[loaded_coords.find(coords)]
	return data

func load_block_data(coords):
	var data = blockdata_in_chunk[loaded_coords.find(coords)]
	return data

func load_block_id(coords, blockCoords):
	var data = blockdata_in_chunk[loaded_coords.find(coords)]
	var index = loaded_blocks[loaded_coords.find(coords)].find(blockCoords)
	return data[index][1]

func remove_block_data(coords, blockCoords):
	var data = blockdata_in_chunk[loaded_coords.find(coords)]
	var index = loaded_blocks[loaded_coords.find(coords)].find(blockCoords)
	blockdata_in_chunk[loaded_coords.find(coords)].remove_at(index)
	loaded_blocks[loaded_coords.find(coords)].remove_at(index)

func save_day(day, time):
	day_data = [day, time]
