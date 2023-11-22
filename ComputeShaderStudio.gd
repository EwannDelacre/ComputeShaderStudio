extends Node


var SX:int = 32
var SY:int = 32

# Called when the node enters the scene tree for the first time.
func _ready():
	# Create a local rendering device.
	var rd := RenderingServer.create_local_rendering_device()

	# Load GLSL shader
	var shader_file := load("res://script_1.glsl")
	var shader_spirv: RDShaderSPIRV = shader_file.get_spirv()
	var shader := rd.shader_create_from_spirv(shader_spirv)

	# Prepare our data. We use floats in the shader, so we need 32 bit.
	var input :PackedFloat32Array = PackedFloat32Array()
	for i in range(SX):
		for j in range(SY):
			input.append(randf())
	var input_bytes := input.to_byte_array()

	# Create a storage buffer that can hold our float values.
	# Each float has 4 bytes (32 bit) so 10 x 4 = 40 bytes
	var buffer := rd.storage_buffer_create(input_bytes.size(), input_bytes)

	# Create a uniform to assign the buffer to the rendering device
	var uniform := RDUniform.new()
	uniform.uniform_type = RenderingDevice.UNIFORM_TYPE_STORAGE_BUFFER
	uniform.binding = 0 # this needs to match the "binding" in our shader file
	uniform.add_id(buffer)
	var uniform_set := rd.uniform_set_create([uniform], shader, 0) # the last parameter (the 0) needs to match the "set" in our shader file

	# Create a compute pipeline
	var pipeline := rd.compute_pipeline_create(shader)
	var compute_list := rd.compute_list_begin()
	rd.compute_list_bind_compute_pipeline(compute_list, pipeline)
	rd.compute_list_bind_uniform_set(compute_list, uniform_set, 0)
	rd.compute_list_dispatch(compute_list, SX/2, SY/2, 1)
	rd.compute_list_end()

	# Submit to GPU and wait for sync
	rd.submit()
	rd.sync()

	# Read back the data from the buffer
	var output_bytes := rd.buffer_get_data(buffer)
	var output := output_bytes.to_float32_array()
	display_values(output)

func display_values(values : PackedFloat32Array):
	var img:Image = Image.create(SX,SY,false, Image.FORMAT_RGBA8)
	for i in range(SX):
		for j in range(SY):
			var p:int = i+j*SX
			var v:float = values[p]
			var rgb = var_to_bytes(v)
			img.set_pixel(i,j,Color(rgb[4],rgb[5],rgb[6],1.0) )
	$ComputeShaderMatrices/Matrix1.set_texture(ImageTexture.create_from_image(img))

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

