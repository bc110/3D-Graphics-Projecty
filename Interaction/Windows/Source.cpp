// 3D Graphics and Animation - Main Template
// This uses Visual Studio Code - https://code.visualstudio.com/docs/cpp/introvideos-cpp
// Two versions available -  Win64 and Apple MacOS - please see notes
// Last changed August 2022

//#pragma comment(linker, "/NODEFAULTLIB:MSVCRT")

//#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h> // Extend OpenGL Specfication to version 4.5 for WIN64 and versions 4.1 for Apple (note: two different files).

#include <imgui/imgui.h>			  // Load GUI library - Dear ImGui - https://github.com/ocornut/imgui
#include <imgui/imgui_impl_glfw.h>	  // Platform ImGui using GLFW
#include <imgui/imgui_impl_opengl3.h> // Platform new OpenGL - aka better than 3.3 core version.

#include <GLFW/glfw3.h> // Add GLFW - library to launch a window and callback mouse and keyboard - https://www.glfw.org

#define GLM_ENABLE_EXPERIMENTAL	 // Enable all functions include non LTS
#include <glm/glm.hpp>			 // Add helper maths library - GLM 0.9.9.9 - https://github.com/g-truc/glm - for example variables vec3, mat and operators.
#include <glm/gtx/transform.hpp> // Help us with transforms
using namespace glm;
 
//#include <tinygltf/tiny_gltf.h> // Model loading library - tiny gltf - https://github.com/syoyo/tinygltf
//#include "src/stb_image.hpp" // Add library to load images for textures

//#include "src/Mesh.hpp" // Simplest mesh holder and OBJ loader - can update more - from https://github.com/BennyQBD/ModernOpenGLTutorial

#include "src/Pipeline.hpp"		// Setup pipeline and load shaders.
#include "src/Content.hpp"		// Setup content loader and drawing functions - https://github.com/KhronosGroup/glTF - https://github.com/syoyo/tinygltf 
#include "src/Debugger.hpp"		// Setup debugger functions.



// Main fuctions
void startup();
void update();
void render();
void ui();
void endProgram();
void drawObject(Content content,vec3 pos,vec3 rot,vec3 sca);

// HELPER FUNCTIONS OPENGL
void hintsGLFW();
//string readShader(string name);
//void checkErrorShader(GLuint shader);
inline void errorCallbackGLFW(int error, const char *description){cout << "Error GLFW: " << description << "\n";};
void debugGL();

void APIENTRY openGLDebugCallback(GLenum source,
								  GLenum type,
								  GLuint id,
								  GLenum severity,
								  GLsizei length,
								  const GLchar *message,
								  const GLvoid *userParam);
GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)

// Setup all the message loop callbacks - do this before Dear ImGui
// Callback functions for the window and interface devices
void onResizeCallback(GLFWwindow *window, int w, int h);
void onKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void onMouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void onMouseMoveCallback(GLFWwindow *window, double x, double y);
void onMouseWheelCallback(GLFWwindow *window, double xoffset, double yoffset);

// VARIABLES
GLFWwindow *window; 								// Keep track of the window
auto windowWidth = 800;								// Window width					
auto windowHeight =800;								// Window height
auto running(true);							  		// Are we still running our main loop
mat4 projMatrix;							 		// Our Projection Matrix
vec3 cameraPosition = vec3(0.0f, 0.0f, 5.0f);		// Where is our camera
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);			// Camera front vector
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);				// Camera up vector
auto aspect = (float)windowWidth / (float)windowHeight;	// Window aspect ration
auto fovy = 45.0f;									// Field of view (y axis)
bool keyStatus[1024];								// Track key strokes
auto currentTime = 0.0f;							// Framerate
auto deltaTime = 0.0f;								// time passed
auto lastTime = 0.0f;								// Used to calculate Frame rate
bool moved = false;

bool shrink = false;
float scaler = 1.0f;

vec3 scale1 = vec3(1.0f, 1.0f, 1.0f);	
vec3 scale2 = vec3(0.6f, 0.6f, 0.6f);
vec3 scale3 = vec3(1.0f, 1.0f, 1.0f);	
vec3 scale4 = vec3(0.6f, 0.6f, 0.6f);
vec3 scale5 = vec3(1.0f, 1.0f, 1.0f);	
vec3 scale6 = vec3(0.6f, 0.6f, 0.6f);
vec3 scale7 = vec3(0.6f, 0.6f, 0.6f);

int modelChosen = 1;
int swapToo = 1;

auto lastY = 0.0f;
auto lastX = 0.0f;
auto pitch = 0.0f;
auto yaw = -100.0f;
auto moveSpeed = 0.03f;

Pipeline pipeline;									// Add one pipeline plus some shaders.
Content content1;									// Add one content loader (+drawing).
Content content2;									// Add one content loader (+drawing).
Content content3;									// Add one content loader (+drawing).
Content content4;									// Add one content loader (+drawing).
Content content5;									// Add one content loader (+drawing).
Content content6;									// Add one content loader (+drawing).
Content content7;									// Add one content loader (+drawing).
Debugger debugger;									// Add one debugger to use for callbacks ( Win64 - openGLDebugCallback() ) or manual calls ( Apple - glCheckError() ) 

vec3 modelPosition;									// Model position
vec3 modelRotation;									// Model rotation


int main()
{
	cout << endl << "===" << endl << "3D Graphics and Animation - Running..." << endl;

	if (!glfwInit()) // Check if GLFW is working, if not then stop all
	{
		cout << "Could not initialise GLFW...";
		return -1;
	} 

	glfwSetErrorCallback(errorCallbackGLFW); // Setup a function callback to catch and display all GLFW errors.
	hintsGLFW();							 // Setup glfw with various hints.

	string title = "Coursework Application";
	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor()); // Grab reference to monitor
	windowWidth = mode->width; windowHeight = mode->height; //fullscreen
	window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), glfwGetPrimaryMonitor(), NULL); // fullscreen

	// Create our Window
	//const auto windowTitle = "My 3D Graphics and Animation OpenGL Application"s;
	//window = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), NULL, NULL);
	if (!window) // Test if Window or OpenGL context creation failed
	{
		cout << "Could not initialise GLFW...";
		glfwTerminate();
		return -1;
	} 

	glfwSetWindowPos(window, 10, 10); // Place it in top corner for easy debugging.
	glfwMakeContextCurrent(window);	  // making the OpenGL context current

	// GLAD: Load OpenGL function pointers - aka update specs to newest versions - plus test for errors.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD...";
		glfwMakeContextCurrent(NULL);
		glfwTerminate();
		return -1;
	}

	glfwSetWindowSizeCallback(window, onResizeCallback);	   // Set callback for resize
	glfwSetKeyCallback(window, onKeyCallback);				   // Set Callback for keys
	glfwSetMouseButtonCallback(window, onMouseButtonCallback); // Set callback for mouse click
	glfwSetCursorPosCallback(window, onMouseMoveCallback);	   // Set callback for mouse move
	glfwSetScrollCallback(window, onMouseWheelCallback);	   // Set callback for mouse wheel.
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);	// Set mouse cursor Fullscreen
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// Set mouse cursor FPS fullscreen.

	// Setup Dear ImGui and add context	-	https://blog.conan.io/2019/06/26/An-introduction-to-the-Dear-ImGui-library.html
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO(); //(void)io;
								  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::StyleColorsLight(); // ImGui::StyleColorsDark(); 		// Setup Dear ImGui style

	// Setup Platform/Renderer ImGui backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const auto glsl_version = "#version 410";
	ImGui_ImplOpenGL3_Init(glsl_version);

	#if defined(__WIN64__)
		debugGL(); // Setup callback to catch openGL errors.	V4.2 or newer
	#elif(__APPLE__)
		glCheckError(); // Old way of checking for errors. Newest not implemented by Apple. Manually call function anywhere in code to check for errors.
	#endif

	glfwSwapInterval(1);			 // Ony render when synced (V SYNC) - https://www.tomsguide.com/features/what-is-vsync-and-should-you-turn-it-on-or-off
	glfwWindowHint(GLFW_SAMPLES, 2); // Multisampling - covered in lectures - https://www.khronos.org/opengl/wiki/Multisampling

	startup(); // Setup all necessary information for startup (aka. load texture, shaders, models, etc).

	cout << endl << "Starting main loop and rendering..." << endl;	

	do{											 // run until the window is closed
		auto currentTime = (float)glfwGetTime(); // retrieve timelapse
		deltaTime = currentTime - lastTime;		 // Calculate delta time
		lastTime = currentTime;					 // Save for next frame calculations.

		glfwPollEvents(); 						// poll callbacks

		update(); 								// update (physics, animation, structures, etc)
		render(); 								// call render function.
		ui();									// call function to render ui.

		#if defined(__APPLE__)
			glCheckError();				// Manually checking for errors for MacOS, Windows has a callback.
		#endif

		glfwSwapBuffers(window); 		// swap buffers (avoid flickering and tearing)

		running &= (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE); // exit if escape key pressed
		running &= (glfwWindowShouldClose(window) != GL_TRUE);
	} while (running);

	endProgram(); // Close and clean everything up...

	// cout << "\nPress any key to continue...\n";
	// cin.ignore(); cin.get(); // delay closing console to read debugging errors.

	return 0;
}


void hintsGLFW(){
	
	auto majorVersion = 3; auto minorVersion = 3; // define OpenGL version - at least 3.3 for bare basic NEW OpenGL

	#if defined(__WIN64__)	
		majorVersion = 4; minorVersion = 5;					// Recommended for Windows 4.5, but latest is 4.6 (not all drivers support 4.6 version).
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // Create context in debug mode - for debug message callback
	#elif(__APPLE__)
		majorVersion = 4; minorVersion = 1; 				// Max support for OpenGL in MacOS
	#endif

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 	// https://www.glfw.org/docs/3.3/window_guide.html
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
}

void endProgram()
{
	// Clean ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwMakeContextCurrent(NULL); 	// destroys window handler
	glfwTerminate();				// destroys all windows and releases resources.
}

void startup()
{
	// Output some debugging information
	cout << "VENDOR: " << (char *)glGetString(GL_VENDOR) << endl;
	cout << "VERSION: " << (char *)glGetString(GL_VERSION) << endl;
	cout << "RENDERER: " << (char *)glGetString(GL_RENDERER) << endl;	

	cout << endl << "Loading content..." << endl;	
	
	content1.LoadGLTF("assets/dog.gltf");
	content2.LoadGLTF("assets/Chisel.gltf");
	content3.LoadGLTF("assets/Axe.gltf");
	content4.LoadGLTF("assets/Stump3.gltf");
	content5.LoadGLTF("assets/Saw4.gltf");
	content6.LoadGLTF("assets/ChiselHammer3.gltf");
	content7.LoadGLTF("assets/Sign5.gltf");
	

	pipeline.CreatePipeline();
	pipeline.LoadShaders("shaders/vs_model.glsl", "shaders/fs_model.glsl");

	// Start from the centre
	modelPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	modelRotation = glm::vec3(0.0f, 0.0f, 0.0f);

	// A few optimizations.
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Get the correct size in pixels - E.g. if retina display or monitor scaling
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

	// Calculate proj_matrix for the first time.
	aspect = (float)windowWidth / (float)windowHeight;
	projMatrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
}

void update()
{
	modelRotation.y += 0.01f;
	//if (scaler > 0) scaler -= 0.001f;
	if (modelChosen != swapToo){
		if (scaler > 0) scaler -= 0.005f;
		else{
			modelChosen = swapToo;
		}
	}
	else{
		if (scaler < 1)scaler += 0.005f;
	}

	if (keyStatus[GLFW_KEY_W]){
		cameraPosition.x += cos(glm::radians(yaw)) * cos(glm::radians(pitch))*moveSpeed;
		cameraPosition.y += sin(glm::radians(pitch))*moveSpeed;
		cameraPosition.z += sin(glm::radians(yaw)) * cos(glm::radians(pitch))*moveSpeed;
	} 
	if (keyStatus[GLFW_KEY_S]) {
		cameraPosition.x -= cos(glm::radians(yaw)) * cos(glm::radians(pitch))*moveSpeed;
		cameraPosition.y -= sin(glm::radians(pitch))*moveSpeed;
		cameraPosition.z -= sin(glm::radians(yaw)) * cos(glm::radians(pitch))*moveSpeed;
	} 

	if (keyStatus[GLFW_KEY_A]) {
		cameraPosition -=glm::normalize(glm::cross(cameraFront, cameraUp)) *moveSpeed;
	} 

	if (keyStatus[GLFW_KEY_D]) cameraPosition +=glm::normalize(glm::cross(cameraFront, cameraUp)) *moveSpeed;;

	if (keyStatus[GLFW_KEY_R]) pipeline.ReloadShaders();

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void render()
{
	glViewport(0, 0, windowWidth, windowHeight);

	// Clear colour buffer
	glm::vec4 inchyraBlue = glm::vec4(0.345f, 0.404f, 0.408f, 1.0f);
	glm::vec4 backgroundColor = inchyraBlue;
	glClearBufferfv(GL_COLOR, 0, &backgroundColor[0]);

	// Clear deep buffer
	static const GLfloat one = 1.0f;
	glClearBufferfv(GL_DEPTH, 0, &one);

	// Enable blend
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Use our shader programs
	glUseProgram(pipeline.pipe.program);

	// Setup camera
	glm::mat4 viewMatrix = glm::lookAt(cameraPosition,				 // eye
									   cameraPosition + cameraFront, // centre
									   cameraUp);					 // up

	// Do some translations, rotations and scaling
	//glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(modelPosition.x, modelPosition.y, modelPosition.z));


	vec3 lightDisp = vec3(3.0f,3.0f,4.0f);

	vec4 ia = vec4(0.2f,1.0f,0.2f,1.0f);
	vec4 id = vec4(1.0f,0.9f,1.0f,1.0f);
	vec4 is = vec4(0.6f,0.6f,0.4f,1.0f);
	float ka = 0.1f;

	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "viewPosition"), cameraPosition.x,cameraPosition.y, cameraPosition.z, 1.0);
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "lightPosition"), lightDisp.x,lightDisp.y, lightDisp.z, 1.0); 
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "ia"), ia.r, ia.g, ia.b, 1.0); 
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "ka"), ka); 
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "id"), id.r, id.g, id.b, 1.0); 
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "kd"), 1.0f); 
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "is"), is.r, is.g, is.b, 1.0); 
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "ks"), 1.0f); 
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "shininess"), 64.0f); 


	// -- Draw models --
	if (modelChosen == 1){
		glm::mat4 modelMatrix2 = glm::translate(glm::mat4(1.0f), glm::vec3(modelPosition.x, modelPosition.y, modelPosition.z));
		modelMatrix2 = glm::rotate(modelMatrix2, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix2 = glm::rotate(modelMatrix2, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(scale1.x*scaler, scale1.y*scaler, scale1.z*scaler));

		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &modelMatrix2[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "proj_matrix"), 1, GL_FALSE, &projMatrix[0][0]);

		content1.DrawModel(content1.vaoAndEbos, content1.model);
	}
	if (modelChosen == 2){
		glm::mat4 modelMatrix2 = glm::translate(glm::mat4(1.0f), glm::vec3(modelPosition.x, modelPosition.y, modelPosition.z));
		modelMatrix2 = glm::rotate(modelMatrix2, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix2 = glm::rotate(modelMatrix2, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(scale2.x*scaler, scale2.y*scaler, scale2.z*scaler));

		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &modelMatrix2[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "proj_matrix"), 1, GL_FALSE, &projMatrix[0][0]);

		content2.DrawModel(content2.vaoAndEbos, content2.model);
	}
	if (modelChosen == 3){
		glm::mat4 modelMatrix2 = glm::translate(glm::mat4(1.0f), glm::vec3(modelPosition.x, modelPosition.y, modelPosition.z));
		modelMatrix2 = glm::rotate(modelMatrix2, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix2 = glm::rotate(modelMatrix2, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(scale3.x*scaler, scale3.y*scaler, scale3.z*scaler));

		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &modelMatrix2[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "proj_matrix"), 1, GL_FALSE, &projMatrix[0][0]);

		content3.DrawModel(content3.vaoAndEbos, content3.model);
	}
	if (modelChosen == 4){
		glm::mat4 modelMatrix2 = glm::translate(glm::mat4(1.0f), glm::vec3(modelPosition.x, modelPosition.y, modelPosition.z));
		modelMatrix2 = glm::rotate(modelMatrix2, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix2 = glm::rotate(modelMatrix2, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(scale4.x*scaler, scale4.y*scaler, scale4.z*scaler));

		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &modelMatrix2[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "proj_matrix"), 1, GL_FALSE, &projMatrix[0][0]);

		content4.DrawModel(content4.vaoAndEbos, content4.model);
	}
	if (modelChosen == 5){
		glm::mat4 modelMatrix2 = glm::translate(glm::mat4(1.0f), glm::vec3(modelPosition.x, modelPosition.y, modelPosition.z));
		modelMatrix2 = glm::rotate(modelMatrix2, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix2 = glm::rotate(modelMatrix2, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(scale5.x*scaler, scale5.y*scaler, scale5.z*scaler));

		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &modelMatrix2[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "proj_matrix"), 1, GL_FALSE, &projMatrix[0][0]);

		content5.DrawModel(content5.vaoAndEbos, content5.model);
	}
	if (modelChosen == 6){
		glm::mat4 modelMatrix2 = glm::translate(glm::mat4(1.0f), glm::vec3(modelPosition.x, modelPosition.y, modelPosition.z));
		modelMatrix2 = glm::rotate(modelMatrix2, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix2 = glm::rotate(modelMatrix2, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(scale6.x*scaler, scale6.y*scaler, scale6.z*scaler));

		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &modelMatrix2[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "proj_matrix"), 1, GL_FALSE, &projMatrix[0][0]);

		content6.DrawModel(content6.vaoAndEbos, content6.model);
	}
	if (modelChosen == 7){
		glm::mat4 modelMatrix2 = glm::translate(glm::mat4(1.0f), glm::vec3(modelPosition.x, modelPosition.y, modelPosition.z));
		modelMatrix2 = glm::rotate(modelMatrix2, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix2 = glm::rotate(modelMatrix2, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(scale7.x*scaler, scale7.y*scaler, scale7.z*scaler));

		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &modelMatrix2[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "proj_matrix"), 1, GL_FALSE, &projMatrix[0][0]);

		content7.DrawModel(content7.vaoAndEbos, content7.model);
	}
}

void ui()
{
	ImGuiIO &io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration; 
	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
	window_flags |= ImGuiWindowFlags_NoSavedSettings; 
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing; 
	window_flags |= ImGuiWindowFlags_NoNav;

	const auto PAD = 10.0f;
	const ImGuiViewport *viewport = ImGui::GetMainViewport();
	ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
	ImVec2 work_size = viewport->WorkSize;
	ImVec2 window_pos, window_pos_pivot;
	window_pos.x = work_pos.x + work_size.x - PAD;
	window_pos.y = work_pos.y + work_size.y - PAD;
	window_pos_pivot.x = 1.0f;
	window_pos_pivot.y = 1.0f;

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	window_flags |= ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	bool *p_open = NULL;
	if (ImGui::Begin("Info", nullptr, window_flags)) {
		ImGui::Text("About: 3D Graphics and Animation 2022"); // ImGui::Separator();
		ImGui::Text("Performance: %.3fms/Frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Pipeline: %s", pipeline.pipe.error?"ERROR":"OK");
	}
	ImGui::End();

	// Rendering imgui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void onResizeCallback(GLFWwindow *window, int w, int h)
{
	windowWidth = w;
	windowHeight = h;

	// Get the correct size in pixels
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

	if (windowWidth > 0 && windowHeight > 0)
	{ // Avoid issues when minimising window - it gives size of 0 which fails division.
		aspect = (float)w / (float)h;
		projMatrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
	}
}

void onKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
		keyStatus[key] = true;
	else if (action == GLFW_RELEASE)
		keyStatus[key] = false;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		swapToo = 1;
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		swapToo = 2;
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		swapToo = 3;
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		swapToo = 4;
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
		swapToo = 5;
	if (key == GLFW_KEY_6 && action == GLFW_PRESS)
		swapToo = 6;
	if (key == GLFW_KEY_7 && action == GLFW_PRESS)
		swapToo = 7;
}

void onMouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
}

void onMouseMoveCallback(GLFWwindow *window, double x, double y)
{
	int mouseX = static_cast<int>(x);
	int mouseY = static_cast<int>(y);

	if (!moved){
		moved = true;
		lastX = mouseX; lastY = mouseY;
	}

	GLfloat xoffset = mouseX - lastX;
	GLfloat yoffset = lastY - mouseY; // Reversed
	lastX = mouseX; lastY = mouseY;
	GLfloat sensitivity = 0.05;
	xoffset *= sensitivity; yoffset *= sensitivity;
	yaw += xoffset; pitch += yoffset;
	// check for pitch out of bounds otherwise screen gets flipped
	if (pitch > 89.0f) pitch = 89.0f; if (pitch < -89.0f) pitch = -89.0f;
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

	//printf("%g\n", yaw);
	//printf("%g\n", pitch);
}

void onMouseWheelCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	int yoffsetInt = static_cast<int>(yoffset);
}

void APIENTRY openGLDebugCallback(GLenum source,
								  GLenum type,
								  GLuint id,
								  GLenum severity,
								  GLsizei length,
								  const GLchar *message,
								  const GLvoid *userParam)  // Debugger callback for Win64 - OpenGL versions 4.2 or better.
{
	debugger.OpenGLDebugCallback(source, type, id, severity, length, message, userParam);
}

void debugGL() // Debugger function for Win64 - OpenGL versions 4.2 or better.
{
	// Enable Opengl Debug
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback((GLDEBUGPROC)openGLDebugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
}

void drawObject(Content content,vec3 pos,vec3 rot,vec3 sca)
{
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, pos.z));
	modelMatrix = glm::rotate(modelMatrix, rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(sca.x, sca.y, sca.z));
	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
	content.DrawModel(content.vaoAndEbos, content.model);
}

GLenum glCheckError_(const char *file, int line) // Debugger manual function for Apple - OpenGL versions 3.3 to 4.1.
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) debugger.GlGetError(errorCode, file, line);

	return errorCode;
}
