extends Node

const tile_size: int = 50
const chunk_size_tiles: int = 10
const chunk_size_units: int = chunk_size_tiles * tile_size
const tiles_in_chunk: int = chunk_size_tiles * chunk_size_tiles

var cpp_core: CppCore = null
