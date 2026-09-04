extends Node

const block_size: int = 32
const chunk_size_units: int = 32

const world_scale: int = 32

var player: Player = null

var cpp_core: CppGodotAdapter = null
