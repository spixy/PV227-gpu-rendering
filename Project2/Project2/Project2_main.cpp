#include "Project2_main.h"

#include "glm/gtx/color_space.hpp"

//---------------------
//----    SCENE    ----
//---------------------

// Geometry of the glass: one quad from four vertices
const int glass_quad_vertices_count = 4;
const float glass_size = 10.0f;		// Half of the glass size
const float glass_quad_vertices[glass_quad_vertices_count * 8] =
{
//       Position                             Normal                Tex Coord
	-glass_size, 0.0f, -glass_size,		 0.0f, 1.0f, 0.0f,		0.0f, 1.0f,
	-glass_size, 0.0f, glass_size,		 0.0f, 1.0f, 0.0f,		0.0f, 0.0f,
	 glass_size, 0.0f, -glass_size,		 0.0f, 1.0f, 0.0f,		1.0f, 1.0f,
	 glass_size, 0.0f, glass_size,		 0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
};

/// Reloads all shaders
void reload_shaders()
{
	// Program for objects without textures
	notexture_program.Init();
	notexture_program.AddVertexShader("Shaders/notexture_vertex.glsl");
	notexture_program.AddFragmentShader("Shaders/notexture_fragment.glsl");
	notexture_program.Link();

	// Program for objects with textures
	texture_program.Init();
	texture_program.AddVertexShader("Shaders/texture_vertex.glsl");
	texture_program.AddFragmentShader("Shaders/texture_fragment.glsl");
	texture_program.Link();

	cout << "Shaders are reloaded" << endl;
}

/// Initializes all objects of our scene
void init_scene()
{
	//----------------------------------------------
	//--  Display some information about the OpenGL

	PrintOpenGLInfo();

	//----------------------------------------------
	//--  Load shaders

	reload_shaders();
	
	//----------------------------------------------
	//--  Prepare the lights
	
	PhongLights_ubo.Init();
	PhongLights_ubo.SetGlobalAmbient(glm::vec3(0.0f));
	PhongLights_ubo.PhongLights.push_back(
		PhongLight::CreateDirectionalLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.7f), glm::vec3(0.3f), glm::vec3(0.0f)));
	PhongLights_ubo.UpdateOpenGLData();

	//----------------------------------------------
	//--  Prepare materials

	RedMaterial_ubo.Init();
	GreenMaterial_ubo.Init();
	BlueMaterial_ubo.Init();
	CyanMaterial_ubo.Init();
	MagentaMaterial_ubo.Init();
	YellowMaterial_ubo.Init();
	WhiteMaterial_ubo.Init();
	FloorMaterial_ubo.Init();
	GlassMaterial_ubo.Init();
	RedMaterial_ubo.SetMaterial		(PhongMaterial::CreateBasicMaterial(glm::vec3(1.0f, 0.0f, 0.0f), true, 200.0f, 1.0f));
	GreenMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(0.0f, 1.0f, 0.0f), true, 200.0f, 1.0f));
	BlueMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(0.0f, 0.0f, 1.0f), true, 200.0f, 1.0f));
	CyanMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(0.0f, 1.0f, 1.0f), true, 200.0f, 1.0f));
	MagentaMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(1.0f, 0.0f, 1.0f), true, 200.0f, 1.0f));
	YellowMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(1.0f, 1.0f, 0.0f), true, 200.0f, 1.0f));
	WhiteMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(1.0f, 1.0f, 1.0f), true, 200.0f, 1.0f));
	FloorMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(0.7f, 0.7f, 0.7f), true, 200.0f, 1.0f));
	GlassMaterial_ubo.SetMaterial	(PhongMaterial::CreateBasicMaterial(glm::vec3(1.0f, 1.0f, 1.0f), false, 0.0f, 0.5f));
	RedMaterial_ubo.UpdateOpenGLData();
	GreenMaterial_ubo.UpdateOpenGLData();
	BlueMaterial_ubo.UpdateOpenGLData();
	CyanMaterial_ubo.UpdateOpenGLData();
	MagentaMaterial_ubo.UpdateOpenGLData();
	YellowMaterial_ubo.UpdateOpenGLData();
	WhiteMaterial_ubo.UpdateOpenGLData();
	FloorMaterial_ubo.UpdateOpenGLData();
	GlassMaterial_ubo.UpdateOpenGLData();

	// Add the materials into our list of materials (we do not add the material for the floor, only the basic colors)
	Colors_ubo.push_back(&RedMaterial_ubo);
	Colors_ubo.push_back(&GreenMaterial_ubo);
	Colors_ubo.push_back(&BlueMaterial_ubo);
	Colors_ubo.push_back(&CyanMaterial_ubo);
	Colors_ubo.push_back(&MagentaMaterial_ubo);
	Colors_ubo.push_back(&YellowMaterial_ubo);
	Colors_ubo.push_back(&WhiteMaterial_ubo);

	//----------------------------------------------
	//--  Prepare all cameras

	the_camera.SetCenter(glm::vec3(0.0f, 1.0f, 0.0f));
	the_camera.SetCamera(1.25f, -0.5f, 40.0f);
	CameraData_ubo.Init();

	//----------------------------------------------
	//--  Prepare geometries

	geom_cube = CreateCube();
	geom_sphere = CreateSphere();
	geom_torus = CreateTorus();
	geom_cylinder = CreateCylinder();
	geom_capsule = CreateCapsule();
	geom_teapot = CreateTeapot();

	// Add the geometries into our list of geometries. Also add matrices that describe
	// how to transform the objects so that they "sit" on xz plane.
	Geometries.push_back(make_pair(&geom_cube,
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f))));
	Geometries.push_back(make_pair(&geom_sphere,
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f))));
	Geometries.push_back(make_pair(&geom_torus,
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f))));
	Geometries.push_back(make_pair(&geom_cylinder,
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f))));
	Geometries.push_back(make_pair(&geom_capsule,
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f))));
	Geometries.push_back(make_pair(&geom_teapot,
		glm::mat4(1.0f)));

	//----------------------------------------------
	//--  Prepare textures

	wood_tex = CreateAndLoadTexture2D(L"../../textures/wood.jpg");
	glBindTexture(GL_TEXTURE_2D, wood_tex);
	SetTexture2DParameters(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	lenna_tex = CreateAndLoadTexture2D(L"../../textures/Lenna.jpg");
	glBindTexture(GL_TEXTURE_2D, lenna_tex);
	SetTexture2DParameters(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	// Add the textures into our list of textures
	Textures.push_back(wood_tex);
	Textures.push_back(lenna_tex);

	//----------------------------------------------
	//--  Prepare objects in the scene

	srand(12345);
	float x_start = -18.0f;
	float z_start = -18.0f;
	float x_spacing = 3.5f;
	float z_spacing = 3.5f;
	int x_grid_size = 11;
	int z_grid_size = 11;

	Models_ubo.resize(x_grid_size * z_grid_size);

	for (int x = 0; x < x_grid_size; x++)
	for (int z = 0; z < z_grid_size; z++)
	{
		// Choose the geometry randomly
		int geometry = rand() % int(Geometries.size());

		// Rotate the object randomly around the y axis
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), float(rand()) / float(RAND_MAX) * float(M_PI) * 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));

		// Place the object at the proper place in the grid
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(x_start + float(x) * x_spacing, 0.0f, z_start + float(z) * z_spacing));

		// Choose the material randomly, choose randomly between the colors and the textures
		int material = rand() % int(Colors_ubo.size() + Textures.size());

		// Compute the model matrix
		Models_ubo[z*x_grid_size + x].Init();
		Models_ubo[z*x_grid_size + x].SetMatrix(translation * rotation * Geometries[geometry].second);
		Models_ubo[z*x_grid_size + x].UpdateOpenGLData();

		// Create the scene object and add it into the list
		SceneObject scene_object;
		scene_object.geometry = Geometries[geometry].first;
		scene_object.model_ubo = &Models_ubo[z*x_grid_size + x];
		if (material < int(Colors_ubo.size()))
		{
			// Object with a color without textures
			scene_object.shading_program = &notexture_program;
			scene_object.material_ubo = Colors_ubo[material];
			scene_object.texture = 0;
		}
		else
		{
			// Object with a texture
			scene_object.shading_program = &texture_program;
			scene_object.material_ubo = &WhiteMaterial_ubo;
			scene_object.texture = Textures[material - int(Colors_ubo.size())];
		}
		ObjectsInScene.push_back(scene_object);
	}

	// Prepare the floor model matrix. Its size corresponds to the size of the scene
	FloorModel_ubo.Init();
	FloorModel_ubo.SetMatrix(
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.1f, 0.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(x_spacing * float(x_grid_size) / 2.0f + 5.0f, 0.1f, z_spacing * float(z_grid_size) / 2.0f + 5.0f)));
	FloorModel_ubo.UpdateOpenGLData();

	// Add the floor into the list of scene objects
	SceneObject floor_scene_object;
	floor_scene_object.geometry = &geom_cube;
	floor_scene_object.model_ubo = &FloorModel_ubo;
	floor_scene_object.shading_program = &notexture_program;
	floor_scene_object.material_ubo = &FloorMaterial_ubo;
	floor_scene_object.texture = 0;
	ObjectsInScene.push_back(floor_scene_object);

	//------------------------
	//--  Create the glass  --

	// Prepare the glass model matrix.
	glm::mat4 glass_model_matrix =
		glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 10.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));
	GlassModel_ubo.Init();
	GlassModel_ubo.SetMatrix(glass_model_matrix);
	GlassModel_ubo.UpdateOpenGLData();

	// Create glass geometry
	geom_glass.VertexBuffers.resize(1, 0);
	glGenBuffers(1, &geom_glass.VertexBuffers[0]);
	glBindBuffer(GL_ARRAY_BUFFER, geom_glass.VertexBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, glass_quad_vertices_count * sizeof(float) * 8, glass_quad_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create a vertex array object for glass geometry
	glGenVertexArrays(1, &geom_glass.VAO);

	// Set the parameters of the geometry
	glBindVertexArray(geom_glass.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, geom_glass.VertexBuffers[0]);
	glEnableVertexAttribArray(DEFAULT_POSITION_LOC);
	glVertexAttribPointer(DEFAULT_POSITION_LOC, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
	glEnableVertexAttribArray(DEFAULT_NORMAL_LOC);
	glVertexAttribPointer(DEFAULT_NORMAL_LOC, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void *)(sizeof(float) * 3));
	glEnableVertexAttribArray(DEFAULT_TEX_COORD_LOC);
	glVertexAttribPointer(DEFAULT_TEX_COORD_LOC, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void *)(sizeof(float) * 6));
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Set the Mode and the number of vertices to draw
	geom_glass.Mode = GL_TRIANGLE_STRIP;
	geom_glass.DrawArraysCount = 4;
	geom_glass.DrawElementsCount = 0;

	//----------------------------------------------
	//--  Miscellaneous

	// Create the query object
	glGenQueries(1, &RenderTimeQuery);
}

/// Updates the scene: performs animations, updates the data of the buffers, etc.
void update_scene(int app_time_diff_ms)
{
	// Data of the main camera
	CameraData_ubo.SetProjection(
		glm::perspective(glm::radians(45.0f), float(win_width) / float(win_height), 0.5f, 1000.0f));
	CameraData_ubo.SetCamera(the_camera);
	CameraData_ubo.UpdateOpenGLData();

	glm::vec3 light_position = glm::vec3(
		cosf(light_pos / 6.0f) * sinf(light_pos),
		sinf(light_pos / 6.0f),
		cosf(light_pos / 6.0f) * cosf(light_pos));

	// Data of the lights
	PhongLights_ubo.PhongLights.clear();
	PhongLights_ubo.PhongLights.push_back(PhongLight::CreateDirectionalLight(light_position, glm::vec3(0.7f), glm::vec3(0.3f), glm::vec3(0.0f)));
	PhongLights_ubo.UpdateOpenGLData();
}

/// Renders the whole frame
void render_scene()
{
	// Start measuring the elapsed time
	glBeginQuery(GL_TIME_ELAPSED, RenderTimeQuery);

	//----------------------------------------------
	//--  Render the scene onto the main window

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, win_width, win_height);

	// Clear the main window
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	// Set the data of the camera and the lights
	CameraData_ubo.BindBuffer(DEFAULT_CAMERA_BINDING);
	PhongLights_ubo.BindBuffer(DEFAULT_LIGHTS_BINDING);

	// Render all objects in the scene
	for (auto iter = ObjectsInScene.begin(); iter != ObjectsInScene.end(); ++iter)
	{
		if (iter->shading_program && iter->shading_program->IsValid())
			iter->shading_program->Use();
		else continue;		// The program is not ready, skip this object and render another one

		// Set the data of the material
		if (iter->material_ubo)
			iter->material_ubo->BindBuffer(DEFAULT_MATERIAL_BINDING);
		// Set the data of the object
		if (iter->model_ubo)
			iter->model_ubo->BindBuffer(DEFAULT_OBJECT_BINDING);

		// Set the texture
		glActiveTexture(GL_TEXTURE0);	glBindTexture(GL_TEXTURE_2D, iter->texture);

		// Render the object
		if (iter->geometry)
		{
			iter->geometry->BindVAO();
			iter->geometry->Draw();
		}
	}

	// Transparent objects
	if (notexture_program.IsValid())
	{
		// Set up depth test and blending
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Set the data of the material
		GlassMaterial_ubo.BindBuffer(DEFAULT_MATERIAL_BINDING);
		// Set the data of the object
		GlassModel_ubo.BindBuffer(DEFAULT_OBJECT_BINDING);

		// Use the proper program and set its uniform variables
		notexture_program.Use();

		// Render the glass quad
		geom_glass.BindVAO();
		geom_glass.Draw();
	
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}

	//----------------------------------------------

	// Reset the VAO and the program
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	// Stop measuring the elapsed time
	glEndQuery(GL_TIME_ELAPSED);

	// Evaluate the query
	glFinish();					// Wait for OpenGL, don't forget OpenGL is asynchronous
	GLuint64 render_time;
	glGetQueryObjectui64v(RenderTimeQuery, GL_QUERY_RESULT, &render_time);
	render_time_ms = float(render_time) * 1e-6f;
}

void resize_fullscreen_textures()
{
}

//-------------------
//----    GUI    ----
//-------------------

/// Callback when user clicks on Reload button
void TW_CALL reload(void *)
{
	reload_shaders();
}

void init_gui()
{
	// Initial values
	light_pos = 4.0f;

	// Initialize AntTweakBar
	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(win_width, win_height);
	TwGLUTModifiersFunc(glutGetModifiers);

	// Initialize GUI
	the_gui = TwNewBar("Parameters");
	TwAddButton(the_gui, "Reload", reload, nullptr, nullptr);
	TwAddVarRW(the_gui, "Light direction", TW_TYPE_FLOAT, &light_pos, "min=1 max=8 step=0.03");

	TwAddVarRO(the_gui, "Render time (ms)", TW_TYPE_FLOAT, &render_time_ms, nullptr);
}

//---------------------------
//----    APPLICATION    ----
//---------------------------

/// GLUT callback - when the user presses a key. See glutKeyboardFunc for more info
void on_keyboard_func(unsigned char key, int x, int y)
{
	// Inform AntTweakBar
	if (TwEventKeyboardGLUT(key, x, y))
		return;		// Already handled by AntTweakBar

	switch (key)
	{
	case 27:		// Escape key
		exit(0);
		break;
	}
}

/// GLUT callback - when the user presses a special key (like F1 key). See glutSpecialFunc for more info
void on_special_func(int key, int x, int y)
{
	// Inform AntTweakBar
	if (TwEventSpecialGLUT(key, x, y))
		return;		// Already handled by AntTweakBar
}

/// GLUT callback - when the user presses or releases a mouse button. See glutMouseFunc for more info
void on_mouse_func(int button, int state, int x, int y)
{
	// Inform AntTweakBar
	if (TwEventMouseButtonGLUT(button, state, x, y))
		return;		// Already handled by AntTweakBar

	the_camera.OnMouseFunc(button, state, x, y);
}

/// GLUT callback - when the user moves the mouse when holding a botton. See glutMotionFunc for more info
void on_motion_func(int x, int y)
{
	// Inform AntTweakBar
	if (TwEventMouseMotionGLUT(x, y))
		return;		// Already handled by AntTweakBar

	the_camera.OnMotionFunc(x, y);
}

/// GLUT callback - when the user moves the mouse when not holding a botton. See glutPassiveMotionFunc for more info
void on_passive_motion_func(int x, int y)
{
	// Inform AntTweakBar
	if (TwEventMouseMotionGLUT(x, y))
		return;		// Already handled by AntTweakBar
}

/// GLUT callback - when the window is resized. See glutReshapeFunc for more info
void on_reshape(int width, int height)
{
	win_width = width;
	win_height = height;
	
	glViewport(0, 0, win_width, win_height);
	resize_fullscreen_textures();
	
	// Inform AntTweakBar
	TwWindowSize(win_width, win_height);
}

/// GLUT callback - when our animation timer requests a new frame. See glutTimerFunc for more info
void on_timer(int)
{
	// Register the timer to be called again in 20 ms, and rerender the scene
	glutTimerFunc(20, on_timer, 0);
	glutPostRedisplay();
}

/// GLUT callback - when the window needs to be redrawn. See glutDisplayFunc for more info
void on_display()
{
	//--  Update all the data

	// Update the application time
	int current_glut_time = glutGet(GLUT_ELAPSED_TIME);
	int app_time_diff_ms = current_glut_time - last_glut_time;
	app_time_ms += app_time_diff_ms;
	last_glut_time = current_glut_time;

	// Update the scene
	update_scene(app_time_diff_ms);

	//--  Render the scene

	// Render the scene
	render_scene();
	
	// Render the GUI
	TwDraw();

	// Swaps the front and back buffer (double-buffering)
	glutSwapBuffers();
}

/// Callback function to be called when we make an error in OpenGL
void GLAPIENTRY simple_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char* message, const void* userParam)
{
	switch (type)			// Report only some errors
	{
	case GL_DEBUG_TYPE_ERROR:
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		cout << message << endl;		// Put the breakpoint here
		return;
	default:
		return;
	}
}

/// C main function :-)
int main(int argc, char ** argv)
{
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);		// Return from the main loop

	// Set OpenGL Context parameters
	glutInitContextVersion(4, 3);				// Use OpenGL 4.3
	glutInitContextProfile(GLUT_CORE_PROFILE);	// Use OpenGL core profile
	glutInitContextFlags(GLUT_DEBUG);			// Use OpenGL debug context

	// Initialize window
	glutInitWindowSize(win_width, win_height);
	glutCreateWindow("PV227 - Project 2 - 2017");

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	// Initialize DevIL library
	InitDevIL();

	// Set the OpenGL debug callback
	SetDebugCallback(simple_debug_callback);

	// Register GLUT callbacks
	glutDisplayFunc(on_display);
	glutReshapeFunc(on_reshape);
	glutTimerFunc(20, on_timer, 0);

	glutKeyboardFunc(on_keyboard_func);
	glutSpecialFunc(on_special_func);
	glutMouseFunc(on_mouse_func);
	glutMotionFunc(on_motion_func);
	glutPassiveMotionFunc(on_passive_motion_func);

	// Initialize OpenGL stuff
	init_gui();
	init_scene();

	// Everything is loaded and prepared, reset the application time
	last_glut_time = glutGet(GLUT_ELAPSED_TIME);
	app_time_ms = 0;

	// Run the main loop
	glutMainLoop();

	// Unload AntTweakBar
	TwTerminate();

	return 0;
}