extends Node2D

#@onready var connect_element = preload("res://objects/Elements/connection_element.tscn")

signal clickOutput
signal remconnection

func CreateOutputs(size, placement, element):
	var connect_element = preload("res://objects/Elements/connection_element.tscn")
	for i in range(size):
		var connect = connect_element.instantiate()
		connect.name = "Output_" + str(i)
		connect.belongingElement = element
		connect.position = placement[i]
		add_child(connect)
	init()

func clickPass():
	print("clickered")
	clickOutput.emit()

func rempass():
	print("remPassed")
	remconnection.emit()

func init():
	print("Initialized outputs!")
	for _i in self.get_children():
		print(_i.name)
		_i.clicked.connect(clickPass)
		_i.removed.connect(rempass)
