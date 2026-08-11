extends Node2D

@export var temperature: float
@export var tempVel: float
@export var tempMass: float
@export var selfEnergy: float
@export var emitted_energy: float 
@export var HTCoeff: float
@export var HConCoeff: float
@export var outsideOBJ: Array
@export var selfItem: InvItem
@onready var world = get_node("/root/World")

var isActive: bool = false
var connected: bool = false

func init(id, item):
	selfItem = item
	var heatKeys = ["thermalMass", "heatTransCoeff", "convCoeff", "generatedEnergy"]
	var values: Array
	for _i in heatKeys:
		if _i in item.defValues:
			values.append(item.defValues[_i])
	tempMass = values[0]
	HTCoeff = values[1]
	HConCoeff = values[2]
	selfEnergy = values[3]
	print("Attributes: ", values)

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	set_as_top_level(true)
	temperature = world.world_temp

func setActivation(value):
	isActive = value

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float):
	var outside_influence = 0
	emitted_energy += (selfEnergy*int(isActive) - emitted_energy)/tempMass
	for _i in outsideOBJ:
		if is_instance_valid(_i):
			var InverseSquare = (32/(position - _i.position).length())**2
			outside_influence += int(_i.isActive)*_i.selfEnergy * InverseSquare
			_i.temperature -= _i.HConCoeff*InverseSquare*(_i.temperature - temperature)/_i.tempMass
	tempVel = (emitted_energy - HTCoeff*(temperature - world.world_temp) + HConCoeff*outside_influence)/tempMass
	temperature += tempVel
	%tempShow.set_color(Color(temperature/1000.0, 0.5, 0.1, 1.0))
	%Label.text = str(floor(temperature))
	$PointLight2D.energy = temperature/3000.0
	$PointLight2D.texture_scale = clamp(temperature/500.0, 0, 10)

func _on_area_2d_body_entered(body: Node2D) -> void:
	print("thingie thing: ", body.position)
	if "heat_OBJ" in body.name && body.name != self.name:
		print(32/(position - body.position).length())
		#outsideOBJ.append(body.selfEnergy * (32/(self.position - body.position).length())**2)
		outsideOBJ.append(body)

func _on_area_2d_body_exited(body: Node2D) -> void:
	print("thongie thong: ", body.position)
	if "heat_OBJ" in body.name && body.name != self.name:
		print(32/(position - body.position).length())
		#outsideOBJ.append(body.selfEnergy * (32/(self.position - body.position).length())**2)
		var index = outsideOBJ.find(body)
		outsideOBJ.remove_at(index)
