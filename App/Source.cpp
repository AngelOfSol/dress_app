#include "gl_includes.h"
#include "OpenGLHandler.h"
#include <map>
#include "Scene.h"
#include "RenderScene.h"
#include "SetupScreen.h"
#include "fileio.h"
#include "file_loc.h"
#include "Pipe.h"
#include <boost/program_options.hpp>
#include "CommandLineArguments.h"
#include "Config.h"
#include "WindowsDialog.h"
#include "ACRConnection.h"
OpenGLHandler glHandler;

void update(void)
{
	glHandler.update();
}

void draw(void)
{
	glHandler.draw();
}
void reshape(int w, int h)
{
	glHandler.reshape(w, h);
}

void mouseDown(int x, int y)
{
	glHandler.mouseDown(x, y);
}


void mouseUp(int x, int y)
{
	glHandler.mouseUp(x, y);
}

void mouseChange(int button, int state, int x, int y)
{
	glHandler.mouseChange(button, state, x, y);
}

void asciiKeyDown(unsigned char key, int x, int y)
{
	glHandler.asciiKeyDown(key, x, y);
}
void asciiKeyUp(unsigned char key, int x, int y)
{
	glHandler.asciiKeyUp(key, x, y);
}
void keyDown(int key, int x, int y)
{
	glHandler.keyDown(key, x, y);
}
void keyUp(int key, int x, int y)
{
	glHandler.keyUp(key, x, y);
}

int main(int argc, char ** argv)
{
	auto args = rmg_com::make_args(argc, argv);

	std::string absolute = "";

	Pipe pipe(get_path("x.pipe"));


	// attempt to grab hold of a mutex for the application
	auto mutex = CreateMutex(nullptr, true, "Global\\rgm_modelviewer");
	// if it doesn't return a mutex something went wrong and we need to exit
	if (mutex == nullptr)
	{
		return 0;
	}
	else
	{
		// if it already exists send information to the previous instance
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			/***
			This is the code that remains cross-platform regardless of whether or not
			the Windows API is being used.
			(i.e. if you change the code, keep this section)
			***/
			if (args.hasOption("close"))
				pipe.write("close");
			if (args.hasOption("x"))
			{
				pipe.write("move " + args["x"] + " " + args["y"] + " " + args["w"] + " " + args["h"]);
			}
			return 0;
		}
		// otherwise start the program as normal
	}
	if (args.hasOption("close"))
		return 0;
	if (!args.hasOption("r"))
		return 0;


	COMInit com;
	
	Config config(get_path("cfg.cfg"));

	glHandler.shaderVersion = config.as<int>("gl-shader");

	bool borderless = config.as<int>("window-borderless") == 1;

	if (args.hasOption("noborder"))
		borderless = true;
	if (args.hasOption("border"))
		borderless = false;

	glm::vec2 windowSize(config.as<int>("window-width"), config.as<int>("window-height"));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE | GLUT_STENCIL | GLUT_DOUBLE | (borderless ? GLUT_BORDERLESS : 0));
	glutInitWindowSize((int)windowSize.x, (int)windowSize.y);
	glutInitContextVersion(config.as<int>("gl-major"), config.as<int>("gl-minor"));
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glEnable(GLUT_MULTISAMPLE);
	glutCreateWindow("Model Viewer");
	auto hwndTest = GetActiveWindow();
	

	FileDialog::handle = hwndTest;

	std::cout << glGetString(GL_VENDOR) << std::endl <<
		glGetString(GL_RENDERER) << std::endl <<
		glGetString(GL_VERSION) << std::endl <<
		glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	// required to have for some of the extensions used here work
	glewExperimental = true;

	if(glewInit() != GLEW_OK)
	{
		return 1;
	}

	glHandler.init(windowSize);
	glHandler.loadFont(get_path("ariblk.ttf"), "courier", config.as<int>("font-size"));

	// load up all the shaders
	glHandler.loadShader("vertex", get_path("Shaders/vertexShader.glsl"), GL_VERTEX_SHADER);

	glHandler.loadShader("discGeometry", get_path("Shaders/discShader.glsl"), GL_GEOMETRY_SHADER);

	glHandler.loadShader("cylinderGeometry", get_path("Shaders/geometryShader.glsl"), GL_GEOMETRY_SHADER);

	glHandler.loadShader("fragment", get_path("Shaders/fragmentShader.glsl"), GL_FRAGMENT_SHADER);

	glHandler.loadShader("axisVertex", get_path("Shaders/axisVertexShader.glsl"), GL_VERTEX_SHADER);
	glHandler.loadShader("axisFragment", get_path("Shaders/axisFragmentShader.glsl"), GL_FRAGMENT_SHADER);

	glHandler.loadShader("concavePolygonVertex", get_path("Shaders/concavePolygonVertexShader.glsl"), GL_VERTEX_SHADER);
	glHandler.loadShader("concavePolygonFragment", get_path("Shaders/concavePolygonFragmentShader.glsl"), GL_FRAGMENT_SHADER);

	glHandler.loadShader("indicatorVS", get_path("Shaders/pointerVS.glsl"), GL_VERTEX_SHADER);
	glHandler.loadShader("indicatorGS", get_path("Shaders/pointerGS.glsl"), GL_GEOMETRY_SHADER);
	glHandler.loadShader("indicatorFS", get_path("Shaders/pointerFS.glsl"), GL_FRAGMENT_SHADER);

	// build up the programs
	
	glHandler.createProgram("cylinderSweep");
	glHandler.createProgram("discRevolve");
	glHandler.createProgram("axisDraw");
	glHandler.createProgram("concaveRender");
	glHandler.createProgram("indicator");
	
	glHandler.attachShader("cylinderSweep", "vertex");
	glHandler.attachShader("cylinderSweep", "cylinderGeometry");
	glHandler.attachShader("cylinderSweep", "fragment");

	glHandler.attachShader("discRevolve", "vertex");
	glHandler.attachShader("discRevolve", "discGeometry");
	glHandler.attachShader("discRevolve", "fragment");
	
	glHandler.attachShader("axisDraw", "axisVertex");
	glHandler.attachShader("axisDraw", "axisFragment");

	glHandler.attachShader("concaveRender", "concavePolygonVertex");
	glHandler.attachShader("concaveRender", "concavePolygonFragment");
	
	glHandler.attachShader("indicator", "indicatorVS");
	glHandler.attachShader("indicator", "indicatorGS");
	glHandler.attachShader("indicator", "indicatorFS");

	//indicator

	glHandler.compileAllPrograms();

	cylinder_data message;

	message.hwnd = hwndTest;

	message.config = &config;
	message.size = glm::vec2(8, 1);
	message.toolHome = vec2d(stor<base_int>(".05"), stor<base_int>(".01"));
	message.toolType = tools::rectangle;
	message.unitType = units::inches;

	message.fileName = "ERROR";
	if (args.arguments().size() > 0)
		message.fileName = args.arguments()[0];

	if (message.fileName == "ERROR")
		message.fileName = GetFileName("Select a Parts File");
	if (message.fileName == "ERROR")
	{
		MessageBox(NULL, "No part file provided.", "No File", MB_ICONINFORMATION | MB_OK | MB_SETFOREGROUND);
		return 0;
		
	}
	message.pipe = &pipe;
	message.acr = args.hasOption("acr");
	message.live = args.hasOption("live");

	auto renderScene = std::make_shared<RenderScene>(glHandler, message);

	try
	{
		renderScene->init();
	}
	catch (missing_data e)
	{
		std::string message = "Your recipe had an error at: " + e.location_name;
		if (e.data_name != "NONE")
			message += "\rFor the data specifed as: " + e.data_name;
		MessageBox(NULL, message.c_str() , NULL, MB_ICONERROR | MB_OK | MB_SETFOREGROUND);
		return 0;
	}

	//glHandler.addScene(setupScene);
	glHandler.addScene(renderScene);
 
	glHandler.currentScene() = RenderScene::id;

	glutIdleFunc(update);
	
	glutDisplayFunc(draw);
	glutReshapeFunc(reshape);

	glutMouseFunc(mouseChange);
	glutMotionFunc(mouseDown);
	glutPassiveMotionFunc(mouseUp);

	glutKeyboardFunc(asciiKeyDown);
	glutKeyboardUpFunc(asciiKeyUp);
	glutSpecialFunc(keyDown);
	glutSpecialUpFunc(keyUp);
	/*
	// set up keyboard callbacks

	// set up mouse call backs

	// set up drawing and updating callbacks
	
	//set up window resize call backs
	*/
	
	if (args.hasOption("x"))
	{
		int x = std::stoi(args["x"]);
		int y = std::stoi(args["y"]);
		int w = std::stoi(args["w"]);
		int h = std::stoi(args["h"]);
		SetWindowPos(hwndTest, HWND_TOPMOST, x, y, w, h, SWP_SHOWWINDOW);
	}
	else if (borderless)
		SetWindowPos(hwndTest, HWND_TOPMOST, config.as<int>("window-x"), config.as<int>("window-y"), windowSize.x, windowSize.y, SWP_SHOWWINDOW);


	glutGet(GLUT_ELAPSED_TIME);
	glutMainLoop();
	return 0;
} 