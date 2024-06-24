#include "SDLGraphicsProgram.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "weights.h"
#include <algorithm>

// Helper Functions

// Linearly spaced vector of n doubles from a to b
std::vector<double> LinearlySpacedArray(double a, double b, std::size_t N)
{
	double h = (b - a) / static_cast<double>(N - 1);
	std::vector<double> xs(N);
	std::vector<double>::iterator x;
	double val;
	for (x = xs.begin(), val = a; x != xs.end(); ++x, val += h) {
		*x = val;
	}
	return xs;
}

// Shade Color for lines, given a weight; greenish for positive, reddish for negative
inline glm::vec3 lineshadeColor(float s) {
	if (s > 0) {
		float _o = (3 * s >= 1.0) ? 1.0 : 3 * s;
		return glm::vec3(0.275, _o, 0.275);
	} else {
		float _o = (-5 * s >= 1.0) ? 1.0 : -5 * s;
		return glm::vec3(_o, 0.275, 0.275);
	}
}

// returns a vector3 given a float
inline glm::vec3 shadeColor(float s) {
	return glm::vec3(s, s, s);
}

// Given a set of numbers, normalize them into range m1 - m2, and makes it positive
// Used for interpolating correct Sphere sizes
inline std::vector<float> sphereSize(std::vector<float> biases, float m1 = -1.2f, float m2 = 1.2f) {
	float minsize = m1;
	float maxsize = m2;
	float minb = 100.0;
	float maxb = -100.0;

	// Find min and max of original
	int lenArray = biases.size();
	for (int i = 0; i < lenArray; ++i)
	{
		if (biases[i] < minb) {
			minb = biases[i];
		}
		if (biases[i] > maxb) {
			maxb = biases[i];
		}
	}

	// Interpolate size
	std::vector<float> out = biases;
	for (int i = 0; i < lenArray; ++i)
	{
		float j = (maxsize - minsize) / (maxb - minb) * (biases[i] - maxb) + maxsize;
		// Making sure size is positive always
		out[i] = (j > 0) ? j : -j;
	}
	return out;
}

// Set Red and Green Color
glm::vec3 red = glm::vec3(236.0 / 255.0, 85.0 / 255.0, 85.0 / 255.0);
glm::vec3 green = glm::vec3(85.0 / 255.0, 236.0 / 255.0, 151.0 / 255.0);


// Initialization function
// Returns a true or false value based on successful completion of setup.
// Takes in dimensions of window.
SDLGraphicsProgram::SDLGraphicsProgram(int w, int h): screenWidth(w), screenHeight(h) {
	// Initialization flag
	bool success = true;
	// String to hold any errors that occur.
	std::stringstream errorStream;
	// The window we'll be rendering to
	gWindow = NULL;
	// Render flag

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		errorStream << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		success = false;
	}
	else {
		//Use OpenGL 3.3 core
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
		// We want to request a double buffer for smooth updating.
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		//Create window
		gWindow = SDL_CreateWindow( "Lab",
		                            SDL_WINDOWPOS_UNDEFINED,
		                            SDL_WINDOWPOS_UNDEFINED,
		                            screenWidth,
		                            screenHeight,
		                            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

		// Check if Window did not create.
		if ( gWindow == NULL ) {
			errorStream << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}

		//Create an OpenGL Graphics Context
		gContext = SDL_GL_CreateContext( gWindow );
		if ( gContext == NULL) {
			errorStream << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}

		// Initialize GLAD Library
		if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
			errorStream << "Failed to iniitalize GLAD\n";
			success = false;
		}

		//Initialize OpenGL
		if (!initGL()) {
			errorStream << "Unable to initialize OpenGL!\n";
			success = false;
		}
	}

	// If initialization did not work, then print out a list of errors in the constructor.
	if (!success) {
		errorStream << "SDLGraphicsProgram::SDLGraphicsProgram - Failed to initialize!\n";
		std::string errors = errorStream.str();
		SDL_Log("%s\n", errors.c_str());
	} else {
		SDL_Log("SDLGraphicsProgram::SDLGraphicsProgram - No SDL, GLAD, or OpenGL, errors detected during initialization\n\n");
	}

	// SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN); // Uncomment to enable extra debug support!
	getOpenGLVersionInfo();

	// Initiliaze Drawing Board - all to 0/Empty/White
	objState.fill(0);

	// Starting Positions and change to render Correct Positions
	float startx = -23.0f;
	float starty = 6.0f;
	float change = 1.10f;

	// Drawing Board Objects - Each reprenting a Pixel
	for (int j = 0; j < rows; ++j)
	{
		for (int i = 0; i < cols; ++i)
		{
			allObjects.push_back(new Object(shadeColor(1.0f), glm::vec3(startx + i * change, starty - j * change, -25.0f)));
		}
	}


	// Initilize Spheres with the Correct Sizes
	std::vector<float> b0(layer0biases, layer0biases + sizeof layer0biases / sizeof layer0biases[0]);
	b0out = sphereSize(b0);
	std::vector<float> b1(layer1biases, layer1biases + sizeof layer1biases / sizeof layer1biases[0]);
	b1out = sphereSize(b1);
	std::vector<float> b2(layer2biases, layer2biases + sizeof layer2biases / sizeof layer2biases[0]);
	b2out = sphereSize(b2);
	std::vector<float> b3(layer3biases, layer3biases + sizeof layer3biases / sizeof layer3biases[0]);
	b3out = sphereSize(b3);

	// Create the Sphere Objects
	startx = -4.0f;
	starty = 12.0f;
	change = 2.7f;
	for (int i = 0; i < 10; ++i)
	{
		layer1Spheres.push_back(new Sphere((layer0biases[i] > 0) ? green : red, glm::vec3(startx, starty - i * change, -30.0f)));
		layer2Spheres.push_back(new Sphere((layer1biases[i] > 0) ? green : red, glm::vec3(startx + 8.0f , starty - i * change, -30.0f)));
		layer3Spheres.push_back(new Sphere((layer2biases[i] > 0) ? green : red, glm::vec3(startx + 16.0f , starty - i * change, -30.0f)));
		outputSpheres.push_back(new Sphere((layer3biases[i] > 0) ? green : red, glm::vec3(startx + 24.0f , starty - i * change, -30.0f)));
	}

	// Scale the Spheres Accordingly
	for (int i = 0; i < 10; ++i)
	{
		layer1Spheres[i]->scale = b0out[i];
		layer2Spheres[i]->scale = b1out[i];
		layer3Spheres[i]->scale = b2out[i];
		// outputSpheres[i]->scale = b3out[i];
	}


	// Input to layer 1 LINES
	for (int i = 0; i < allObjects.size(); ++i)
	{
		for (int j = 0; j < layer1Spheres.size(); ++j)
		{
			inputTOlayer1.push_back(new Line(lineshadeColor(layer0weights[i][j]), allObjects[i]->coordinates, layer1Spheres[j]->coordinates));
		}

	}

	// Lines for All other layers
	for (int i = 0; i < layer1Spheres.size(); ++i)
	{
		for (int j = 0; j < layer2Spheres.size(); ++j)
		{
			layer1TOlayer2.push_back(new Line(lineshadeColor(layer1weights[i][j]), layer1Spheres[i]->coordinates, layer2Spheres[j]->coordinates));
			layer2TOlayer3.push_back(new Line(lineshadeColor(layer2weights[i][j]), layer2Spheres[i]->coordinates, layer3Spheres[j]->coordinates));
			layer3TOlayerout.push_back(new Line(lineshadeColor(layer3weights[i][j]), layer3Spheres[i]->coordinates, outputSpheres[j]->coordinates));
		}
	}

	// Number Representation Objects
	// Filenames for textures
	std::array<std::string, 10> filenames = {"0.ppm", "1.ppm", "2.ppm", "3.ppm", "4.ppm", "5.ppm", "6.ppm", "7.ppm", "8.ppm", "9.ppm"};
	startx = 20.0f;
	starty = 10.5f;
	change = 2.3f;
	for (int i = 0; i < 10; ++i)
	{
		numbersObjects.push_back(new Number(glm::vec3(startx, starty - i * change, -25.0), filenames[i]));
	}

}


// Proper shutdown and destroy initialized objects
SDLGraphicsProgram::~SDLGraphicsProgram() {
	//Destroy window
	// delete allObjects;
	SDL_DestroyWindow( gWindow );
	// Point gWindow to NULL to ensure it points to nothing.
	gWindow = NULL;
	//Quit SDL subsystems
	SDL_Quit();
}



// Initialize OpenGL
// Setup any of our shaders here.
bool SDLGraphicsProgram::initGL() {
	//Success flag
	bool success = true;

	// Setup our OpenGL State machine
	// TODO: Read this
	// The below command is new!
	// What we are doing, is telling opengl to create a depth(or Z-buffer)
	// for us that is stored every frame.
	glEnable(GL_DEPTH_TEST);
	return success;
}

// Updates the input to layer1, set line colors, sphere sizes, sphere colors
void SDLGraphicsProgram::layer1Update() {
	layer1result.fill(0);

	// Multuplies and adds together weights and biases
	for (int i = 0; i < 10; ++i)
	{
		std::array<float, 100> tomult;
		for (int j = 0; j < 100; ++j)
		{
			tomult[j] = layer0weights[j][i];
		}

		for (int j = 0; j < 100; ++j)
		{
			layer1result[i] += objState[j] * tomult[j];
		}
	}

	// Relu/Activation Function
	for (int i = 0; i < 10; ++i)
	{
		layer1result[i] += layer0biases[i];
		layer1result[i] = (layer1result[i] > 0) ? layer1result[i] : 0;
	}

	// Sphere Size and Color
	std::vector<float> r1(layer1result.begin(), layer1result.end());
	std::vector<float> r1out = sphereSize(r1);
	for (int i = 0; i < 10; ++i)
	{
		layer1Spheres[i]->rgb = (layer1result[i] > 0) ? green : red;
		layer1Spheres[i]->scale = r1out[i];
	}

	// Line Size and Color
	for (int i = 0; i < 100; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			glm::vec3 _c = lineshadeColor(layer0weights[i][j]);
			inputTOlayer1[i * 10 + j]->rgb = _c * (layer1result[j] <= 0 ? 0.0 : (r1out[j] > 0 ? r1out[j] : -r1out[j]));
		}
	}
}

// Updates the layer1 to layer2, set line colors, sphere sizes, sphere colors
void SDLGraphicsProgram::layer2Update() {
	layer2result.fill(0);

	// Multuplies and adds together weights and biases
	for (int i = 0; i < 10; ++i)
	{
		std::array<float, 10> tomult;
		for (int j = 0; j < 10; ++j)
		{
			tomult[j] = layer1weights[j][i];
		}

		for (int j = 0; j < 10; ++j)
		{
			layer2result[i] += layer1result[j] * tomult[j];

		}
	}

	// Relu/Activation Function
	for (int i = 0; i < 10; ++i)
	{
		layer2result[i] += layer1biases[i];
		layer2result[i] = (layer2result[i] > 0) ? layer2result[i] : 0;
	}

	// Sphere Size and Color
	std::vector<float> r2(layer2result.begin(), layer2result.end());
	std::vector<float> r2out = sphereSize(r2);
	for (int i = 0; i < 10; ++i)
	{
		layer2Spheres[i]->rgb = (layer2result[i] > 0) ? green : red;
		layer2Spheres[i]->scale = r2out[i];
	}

	// Line Size and Color
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			glm::vec3 _c = lineshadeColor(layer1weights[i][j]);
			layer1TOlayer2[i * 10 + j]->rgb = _c * (layer2result[j] <= 0 ? 0.0 : (r2out[j] > 0 ? r2out[j] : -r2out[j]));
			if (layer1result[i] <= 0) {
				layer1TOlayer2[i * 10 + j]->rgb = _c * 0;
			}
		}
	}
}

// Updates the layer2 to layer3, set line colors, sphere sizes, sphere colors
void SDLGraphicsProgram::layer3Update() {
	layer3result.fill(0);

	// Multuplies and adds together weights and biases
	for (int i = 0; i < 10; ++i)
	{
		std::array<float, 10> tomult;
		for (int j = 0; j < 10; ++j)
		{
			tomult[j] = layer2weights[j][i];
		}

		for (int j = 0; j < 10; ++j)
		{
			layer3result[i] += layer2result[j] * tomult[j];

		}
	}

	// Relu/Activation Function
	for (int i = 0; i < 10; ++i)
	{
		layer3result[i] += layer2biases[i];
		layer3result[i] = (layer3result[i] > 0) ? layer3result[i] : 0;
		std::cout << layer3result[i] << std::endl;
	}

	// Sphere Size and Color
	std::vector<float> r3(layer3result.begin(), layer3result.end());
	std::vector<float> r3out = sphereSize(r3);
	for (int i = 0; i < 10; ++i)
	{
		layer3Spheres[i]->rgb = (layer3result[i] > 0) ? green : red;
		layer3Spheres[i]->scale = r3out[i];

	}

	// Line Size and Color
	for (int i = 0; i < 10; ++i)
	{

		for (int j = 0; j < 10; ++j)
		{
			glm::vec3 _c = lineshadeColor(layer2weights[i][j]);
			layer2TOlayer3[i * 10 + j]->rgb = _c * (layer3result[j] <= 0 ? 0.0 : (r3out[j] > 0 ? r3out[j] : -r3out[j]));
			if (layer2result[i] <= 0) {
				layer2TOlayer3[i * 10 + j]->rgb = _c * 0;
			}
		}
	}
}

// Updates the layer3 to output layer,
//set line colors, sphere sizes, sphere colors, and NUMBERs size
void SDLGraphicsProgram::layer4Update() {
	layer4result.fill(0);
	// Multuplies and adds together weights and biases
	for (int i = 0; i < 10; ++i)
	{
		std::array<float, 10> tomult;
		for (int j = 0; j < 10; ++j)
		{
			tomult[j] = layer3weights[j][i];
		}

		for (int j = 0; j < 10; ++j)
		{
			layer4result[i] += layer3result[j] * tomult[j];
		}
	}
	// SoftMAX /Activation Function
	float sum_exps = 0.0f;
	for (int i = 0; i < 10; ++i)
	{
		layer4result[i] += layer3biases[i];
		sum_exps += exp(layer4result[i]);
	}

	// Outputs probabilities
	std::cout << "Output Probabilities" << std::endl;
	for (int i = 0; i < 10; ++i)
	{
		layer4result[i] = exp(layer4result[i]) / sum_exps;
		std::cout << i << ": " << layer4result[i] << std::endl;
	}

	// Sphere Size and Color and Number size
	std::vector<float> r4(layer4result.begin(), layer4result.end());
	std::vector<float> r4out = sphereSize(r4, 0.0, 1.0);
	for (int i = 0; i < 10; ++i)
	{
		outputSpheres[i]->rgb = (layer4result[i] > 0) ? green : red;
		outputSpheres[i]->scale = r4out[i];
		numbersObjects[i]->scale = r4out[i];
	}

	// Line Size and Color
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			glm::vec3 _c = lineshadeColor(layer3weights[i][j]);
			// inputTOlayer1[i*10+j]->rgb= _c ;//* (layer1result[j]>0?layer1result[j]:-layer1result[j]);
			layer3TOlayerout[i * 10 + j]->rgb = _c * (r4out[j] > 0 ? r4out[j] : -r4out[j]);
			if (layer3result[i] <= 0) {
				layer3TOlayerout[i * 10 + j]->rgb = _c * 0;
			}
		}
	}
}

// Update OpenGL
void SDLGraphicsProgram::update()
{
	layer1Update();
	layer2Update();
	layer3Update();
	layer4Update();
	// Update Pixel Objects by stored state
	for (int i = 0; i < allObjects.size(); ++i)
	{
		allObjects[i]->rgb = (objState[i] == 1) ? shadeColor(0.0f) : shadeColor(1.0f);
		allObjects[i]->update(screenWidth, screenHeight);
	}

	// Updates Spheres
	for (int i = 0; i < layer1Spheres.size(); ++i)
	{
		layer1Spheres[i]->update(screenWidth, screenHeight);
		layer2Spheres[i]->update(screenWidth, screenHeight);
		layer3Spheres[i]->update(screenWidth, screenHeight);
		outputSpheres[i]->update(screenWidth, screenHeight);
		numbersObjects[i]->update(screenWidth, screenHeight);
	}

	// Update intput to layer1 Lines
	for (int i = 0; i < inputTOlayer1.size(); ++i)
	{
		inputTOlayer1[i]->update(screenWidth, screenHeight);
	}

	// Update all other lines
	for (int i = 0; i < layer1TOlayer2.size(); ++i)
	{
		layer1TOlayer2[i]->update(screenWidth, screenHeight);
		layer2TOlayer3[i]->update(screenWidth, screenHeight);
		layer3TOlayerout[i]->update(screenWidth, screenHeight);
	}

}



// Render
// The render function gets called once per loop
void SDLGraphicsProgram::render() {
	// Initialize clear color
	// This is the background of the screen.
	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor( 0.2f, 0.2f, 0.2f, 1.f );

	// Clear buffer
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	// Render the pixels objects
	for (int i = 0; i < allObjects.size(); ++i)
	{
		allObjects[i]->render();
	}

	// Render Spheres Objects and Numbers
	for (int i = 0; i < layer1Spheres.size(); ++i)
	{
		layer1Spheres[i]->render();
		layer2Spheres[i]->render();
		layer3Spheres[i]->render();
		outputSpheres[i]->render();
		numbersObjects[i]->render();
	}

	// Render Lines
	for (int i = 0; i < inputTOlayer1.size(); ++i)
	{
		inputTOlayer1[i]->render();
	}
	for (int i = 0; i < layer1TOlayer2.size(); ++i)
	{
		layer1TOlayer2[i]->render();
		layer2TOlayer3[i]->render();
		layer3TOlayerout[i]->render();
	}

	SDL_Delay(50);
}


//Loops forever!
void SDLGraphicsProgram::loop() {
	// Main loop flag
	// If this is quit = 'true' then the program terminates.
	bool quit = false;
	// Event handler that handles various events in SDL
	// that are related to input and output
	SDL_Event e;
	// Enable text input
	SDL_StartTextInput();
	// Mouse position
	int mouseX;
	int mouseY;
	// While application is running
	while (!quit) {
		//Handle events on queue
		while (SDL_PollEvent( &e ) != 0) {
			// User posts an event to quit
			// An example is hitting the "x" in the corner of the window.
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			// Record Mouse positions
			if (e.type == SDL_MOUSEMOTION) {
				mouseX = e.motion.x;
				mouseY = e.motion.y;
			}
			// Click to change pixel from white to black or back
			if (e.type == SDL_MOUSEBUTTONDOWN) {
				std::vector<double> linx = LinearlySpacedArray(34.0, 315.0, 11);
				std::vector<double> liny = LinearlySpacedArray(194.0, 475.0, 11);
				for (int i = 0; i < 10; ++i)
				{
					if ( (mouseX > linx[i]) && (mouseX < linx[i + 1])) {
						selectcol = i;
					}
					if ( (mouseY > liny[i]) && (mouseY < liny[i + 1])) {
						selectrol = i;
					}
				}
				objState[selectrol * rows + selectcol] = 1 - objState[selectrol * rows + selectcol];
			}

			//https://wiki.libsdl.org/SDL_Keycode
			// Pressing the Keys
			// C for clearing the drawing board
			// W.E.R.T. for layers 1 to 4 updates respectively
			// and Enter for WERT together
			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_q:
					quit = true;
					break;
				case SDLK_w:
					layer1Update();
					break;
				case SDLK_e:
					layer2Update();
					break;
				case SDLK_r:
					layer3Update();
					break;
				case SDLK_t:
					layer4Update();
					break;
				case SDLK_c:
					objState.fill(0);
					break;
				// runs the whole inference
				case SDLK_RETURN:
					layer1Update();
					layer2Update();
					layer3Update();
					layer4Update();
					break;
				}
			}
		}
		update();
		render();
		SDL_GL_SwapWindow(getSDLWindow());
	}

	//Disable text input
	SDL_StopTextInput();
}

// Get Pointer to Window
SDL_Window* SDLGraphicsProgram::getSDLWindow() {
	return gWindow;
}

// Helper Function to get OpenGL Version Information
void SDLGraphicsProgram::getOpenGLVersionInfo() {
	SDL_Log("(Note: If you have two GPU's, make sure the correct one is selected)");
	SDL_Log("Vendor: %s", (const char*)glGetString(GL_VENDOR));
	SDL_Log("Renderer: %s", (const char*)glGetString(GL_RENDERER));
	SDL_Log("Version: %s", (const char*)glGetString(GL_VERSION));
	SDL_Log("Shading language: %s", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

}
