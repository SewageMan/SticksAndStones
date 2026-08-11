extends StaticBody2D

#@export var Id = -1
@export var mineTime = [10, null, 25]
@export var ID: int
var mine
# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass

func identify(Id):
	mine = mineTime[Id]
	ID = Id
	#id 0 - tree, 1 - grass, 2 - stone
	if Id == 0:
		self.get_child(Id).get_child(0).frame = floor(0.5+randf())
	if Id == 1:
		self.z_index = 1
		self.get_child(Id).get_child(0).frame = floor(0.5+2*randf())
	if Id == 2:
		self.z_index = 1
	for _i in self.get_children():
		#TODO: will create problems with oduble digits, have to redoo this, but for right now im not gonna, you can do it future me!
		if str(Id) in _i.name:
			_i.visible = true
			if Id % 2 == 0:
				#print(_i.name)
				_i.disabled = false
