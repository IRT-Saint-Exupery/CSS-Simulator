#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"
#include "stb_image_write.h"

#include "constants.h"
#include "shader.h"
#include "stars.h"

#include <math.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <ctime>
#include <unistd.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "primitive3dBucket.h"
#include "sphere.h"

using namespace std;

#ifdef WIN32
#define imageFile	"c:/TEMP/image.jpg"
#define coordFile	"c:/TEMP/coord.txt"
#define picReqFile	"c:/TEMP/picreq"
#else
#define imageFile	"/tmp/image.jpg"
#define coordFile	"/tmp/coord.txt"
#define picReqFile	"/tmp/picreq"
#endif

// global variables
int screenWidth;
int screenHeight;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void renderDisplay (GLFWwindow* window);
void updateProjection (double fov, int width, int height);
void updateCamera (glm::vec3 &cameraPos, glm::vec3 &cameraFront, glm::vec3 &cameraUp);
void updateTarget (glm::vec3 &target);
void updateSun (glm::vec3 &sunPos);
void setModel (double angle);
void checkAndReadCoordinates(const char *coordFilename);
void checkExistingPictureReq(const char *coordFilename);
void takeAPicture();

// constants
const int   SCREEN_WIDTH    = 800;
const int   SCREEN_HEIGHT   = 600;
const double CAMERA_3D_DISTANCE = 1.15f*RT;

// Perspective
double fov   =  25.0f degres;

double raan = 0 degres;

bool targetVisible = true;

bool initCamera();

t_sphere Earth("Earth.jpg", RT, 60);
t_primitive3dBucket stars (GL_POINTS);
t_primitive3dBucket target(GL_POINTS);

Shader *shader3d;
Shader *shader3DBucket;

GLFWwindow* window;

//************************************************************************************************************
//
//			STARS          STARS            STARS
//
//************************************************************************************************************

void initStars (string filename)
{
	t_stars::initStars (filename);
	// points
	stars.empty();
	for (t_stars &star: t_stars::listStars) {
		stars.setColor(star.intensityR,star.intensityV,star.intensityB);
		stars.setWidth(3);
		stars.addPoint3D(star.position.x,star.position.y,star.position.z);
	}
}

//************************************************************************************************************
//
//			TARGET          TARGET            TARGET
//
//************************************************************************************************************

void updateTarget (glm::vec3 &targetPos)
{
	target.empty();
	target.setWidth(5);
	target.setColor(1,0,0);
	target.addPoint3D(targetPos.x,targetPos.y,targetPos.z);
}

//************************************************************************************************************
//
//			INTERFACE          INTERFACE            INTERFACE
//
//************************************************************************************************************

void checkAndReadFiles () {
	checkAndReadCoordinates(coordFile);
	checkExistingPictureReq (picReqFile);
}

glm::vec3 ECEF23D (glm::vec3 ecef)
{
	return glm::vec3 (ecef.y,-ecef.x, ecef.z);
}

void checkAndReadCoordinates(const char *coordFilename)
{
    static  time_t prevModTime = 0;
    struct stat coordFileStat;

    if (stat(coordFilename, &coordFileStat) == -1) { // No coord file. No change.
        return;
    }

    time_t modTime = coordFileStat.st_mtime;

    if (modTime != prevModTime) {
    	glm::vec3 cameraPos(0,0,0);
    	glm::vec3 satDirX(0,0,0);
    	glm::vec3 satDirY(0,0,0);
    	glm::vec3 satDirZ(0,0,0);
    	glm::vec3 targetPos(0,0,0);
    	glm::vec3 sunPos(0,0,0);

    	std::ifstream fi (coordFilename);
    	if (fi.good()) {
			fi >> cameraPos.x >> cameraPos.y >> cameraPos.z;
			fi >> satDirX.x >> satDirX.y >> satDirX.z;
			fi >> satDirY.x >> satDirY.y >> satDirY.z;
			fi >> satDirZ.x >> satDirZ.y >> satDirZ.z;
			fi >> targetPos.x  >> targetPos.y  >> targetPos.z;
			fi >> sunPos.x >> sunPos.y >> sunPos.z;
			fi >> raan;

			if (cameraPos.x != 0 || cameraPos.y != 0 || cameraPos.z != 0) {  // En cas de pb de lecture sur le stream... �a arrive sur Windows
				prevModTime = modTime;

				glm::vec3 cameraFront (-satDirY);
				glm::vec3 cameraUp (satDirX);		// Le haut de l'image est orient� comme le vecteur vitesse du satellite

				// On passe du rep�re ECEF au rep�re 3D OpenGl
				cameraPos   = ECEF23D (cameraPos);
				cameraFront = ECEF23D (cameraFront);
				cameraUp    = ECEF23D (cameraUp);
				sunPos 		= ECEF23D (sunPos);
				targetPos 	= ECEF23D (targetPos);

				cameraFront *= 1'000'000;	// Permet d'�viter les pertes de pr�cision lors de l'addition avec cameraPos

				updateCamera(cameraPos, cameraFront, cameraUp);
				updateTarget(targetPos);
				updateSun (sunPos);
			}
		}
    }
}

void checkExistingPictureReq (const char *filename)
{
    struct stat picreqFileStat;
    if (stat(filename, &picreqFileStat) == -1) {	// Le fichier de requ�te n'existe pas
        return;
    }

    // Si le fichier de requ�te existe on prend la photo et on supprime le fichier de requ�te
    takeAPicture();
    remove(filename);
}

void takeAPicture()
{
	unsigned char *pixels = (unsigned char *) malloc (3*screenWidth*screenHeight);
	targetVisible = false;
	renderDisplay(window);
	renderDisplay(window);  // sinon le glReadPixels voit encore la target :-(
	targetVisible = true;
	glReadPixels(0, 0, screenWidth, screenHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	stbi_flip_vertically_on_write(true);
	stbi_write_jpg(imageFile, screenWidth, screenHeight, 3, pixels, 100);  // 1 (low quality) up to 100 (high quality)
	free (pixels);
}

//************************************************************************************************************

int main(int argc, char **argv)
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "IRT CSS Imager", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

	shader3d 		= new Shader("shaders/shader3d.vs","shaders/shader3d.fs");
	shader3DBucket 	= new Shader("shaders/primitive3dshader.vs", "shaders/primitive3dshader.fs");

    Earth.init(shader3d);
    target.init(shader3DBucket);
    stars.init(shader3DBucket);
    initStars ("stars.txt");

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	screenWidth = SCREEN_WIDTH;
	screenHeight = SCREEN_HEIGHT;

    initCamera();

    // Set the shader variables that don't move every frame
    // ------------------------------------
    shader3d->use();
    shader3d->setInt("texture", 0);

    updateProjection (fov, screenWidth, screenHeight);
	glm::vec3 sunPos = glm::vec3(0, -SUN_DIST,0);
	updateSun (sunPos);

    // infinite loop
    // -------------
    while (!glfwWindowShouldClose(window))
    {
    	checkAndReadFiles();
    	renderDisplay(window); // - Render function
    	usleep (50'000);		// 50 ms
    }

    glfwTerminate();
    return 0;
}

// rendering
// ---------
void renderDisplay (GLFWwindow* window)
{
	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor (0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Earth.draw();
	if (targetVisible) {
		setModel(0);
		target.draw();
	}
	setModel(raan);
	stars.draw();

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
	glfwPollEvents();
}


///////////////////////////////////////////////////////////////////////////////
// initialize global variables
///////////////////////////////////////////////////////////////////////////////
bool initCamera()
{
	glm::vec3 cameraPos   (0, -CAMERA_3D_DISTANCE, 0);
	glm::vec3 cameraFront (0,  CAMERA_3D_DISTANCE, 0);
	glm::vec3 cameraUp (0,0,1);

	updateCamera (cameraPos, cameraFront, cameraUp);

	return true;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    screenWidth = width;
    screenHeight = height;

    if (screenWidth && screenHeight) {
    	updateProjection (fov, screenWidth, screenHeight);
    }
}

void setModel (double angle)
{
	static glm::mat4 model=glm::mat4(1.0);
	bool first = true;
	if (first) {
		shader3d->use();
		shader3d->setMat4("model", model);
		first = false;
	}

	shader3DBucket->use();
	glm::mat4 modelStars = glm::rotate(model, (float)angle , glm::vec3(0,0,1));
	shader3DBucket->setMat4("model", modelStars);
}

void updateProjection (double fov, int width, int height)
{
    glm::mat4 projection = glm::perspective(fov, (double)screenWidth / (double)screenHeight, (double)(RT*0.02f), (double)(RT*100000.0f));
    shader3d->use();
    shader3d->setMat4("projection", projection);

    shader3DBucket->use();
    shader3DBucket->setMat4("projection", projection);
}

void updateCamera (glm::vec3 &cameraPos, glm::vec3 &cameraFront, glm::vec3 &cameraUp)
{
	// camera/view transformation
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	shader3d->use();
	shader3d->setMat4("view", view);

	shader3DBucket->use();
	shader3DBucket->setMat4("view", view);
}

void updateSun (glm::vec3 &sunPos)
{
	glm::vec3 sunDir = glm::normalize(sunPos);
	shader3d->use();
	shader3d->setVec3("sunDir", sunDir);
}


