extends Control

func _fixed_process(delta):
	get_node("Label").set_rotation_deg(-1*self.rotation_degrees)
