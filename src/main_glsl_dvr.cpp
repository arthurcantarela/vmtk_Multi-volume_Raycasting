#define FREEGLUT_STATIC

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#ifdef __WIN32__
#include <GL/glew.h>
#elif __linux__
#include <GL/glew.h>
#elif __APPLE__
#include <glew.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
#include <stdio.h>     
#include <stdint.h>
#include <stdlib.h>    
#include <string.h>
#include <limits.h>
#include <math.h>
#include <GL/freeglut.h>   

#include "vmtkRender3D.h"

using namespace std;

static vmtkRender3D volumeRender; ///< instance of the class vmtkRender3D for rendering the volume
static GLfloat spin_x=0.0, spin_y=0.0, spin_z=0.0; ///< parameters for rotating the volume
static GLuint VIEWPORT_WIDTH=900, VIEWPORT_HEIGHT=900; ///< size of view the viewport
static int threshold = 23; ///< initial threshold
static float blender=0.5f; ///< initial blender
static int slice_x = 0; ///< initial slice x
static int window; ///< holds the name of the window created
static bool m_mprPreState = false; ///< checks if the plane equation for multiplanar reformatting was provided
static bool m_mprState = false; ///< checks if the plane equation for multiplanar reformatting is correct

/**
* @brief initialize context OpenGL
*/
void init()
{
    volumeRender.initialize(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
}
/**
* @brief keyboard callback function 
*/
void keyboard(unsigned char key, int x, int y)
{

switch (key) {
    case 'w':
    case 'W':
        spin_x += 5.0;
        volumeRender.setRotation(spin_x,spin_y,spin_z);
        break;
    case 's':
    case 'S':
        spin_x -= 5.0;
        volumeRender.setRotation(spin_x,spin_y,spin_z);
        break;
    case 'q':
    case 'Q':
        spin_y += 5.0;
        volumeRender.setRotation(spin_x,spin_y,spin_z);
        break;
    case 'e':
    case 'E':
        spin_y -= 5.0;
        volumeRender.setRotation(spin_x,spin_y,spin_z);
        break;
    case 'a':
    case 'A':
        spin_z += 5.0;
        volumeRender.setRotation(spin_x,spin_y,spin_z);
        break;
    case 'd':
    case 'D':
        spin_z -= 5.0;
        volumeRender.setRotation(spin_x,spin_y,spin_z);
        break;
    case 'r':
    case 'R': ///<< reset
        spin_x = spin_y = spin_z = 0.0;
        volumeRender.setRotation(spin_x,spin_y,spin_z);
        break;
    case '+': ///<< increase threshold
        threshold += 1;
        if(threshold<=0){ threshold = 0; }
        if(threshold>=volumeRender.getAcquisition(0)->umax-1){ threshold = volumeRender.getAcquisition(0)->umax-1; }
        volumeRender.setThreshold(threshold);
        break;
    case '-': ///<< decrease threshold
        threshold -= 1;
        if(threshold<=0){ threshold = 0;}
        volumeRender.setThreshold(threshold);
        break;
    case 'b':
    case 'B': ///<< blender
        blender += 0.1;
        if (blender > 1.0){
			blender = 1.0;
        }
        volumeRender.setBlender(blender);
        break;
    case 'n':
    case 'N':
		blender -= 0.1;
        if (blender < 0.0){
			blender = 0.0;
        }
        volumeRender.setBlender(blender);
        break;
	case 'o':
	case 'O':
		slice_x++;
        if (slice_x > volumeRender.getMaxSliceLeft()){
			slice_x = volumeRender.getMaxSliceLeft();
        }
        volumeRender.setClipLeftX(slice_x);
		break;
	case 'p':
	case 'P':
		slice_x--;
        if (slice_x < 0){
			slice_x = 0;
        }
        volumeRender.setClipLeftX(slice_x);
		break;
    case '1':
        if(m_mprState){
            volumeRender.setEnableMPR(true);
        }
        break;
    case '2':
        if(m_mprState){
            volumeRender.setEnableMPR(false);
        }
        break;
    default:
        return;
    }

  glutPostRedisplay();
}

void reshape(int w, int h)
{
	volumeRender.resize(w, h);
}

/**
* @brief render the volume
*/
void drawProxyGeometry()
{
	volumeRender.render();
	glutSwapBuffers();

    glutPostRedisplay();
}

void menu(int num){
  if(num == 0){
    glutDestroyWindow(window);
    exit(0);
  }
  glutPostRedisplay();
 }

int main(int argc, char *argv[])
{
    Import load;
    Import::ImgFormat *data;
    string *path;
    if( argc == 1 )
	{
        cerr << "Minimum usage: " << endl;
		cerr << argv[0] << " inputVolume1  inputVolume2 registeredMatrix" << endl;
		return EXIT_FAILURE;
	}
    int nParameters = argc-1;
    if( nParameters >12 ){
        cerr << "Too many parameters." << std::endl;
        cerr << "Maximum paramters usage: 12." << endl;
        return EXIT_FAILURE;
    }

    std::cout << "Numbers of parameters: " << nParameters << std::endl;
    int nVolumes=0;
    int nMatrices=0;

    if(nParameters%2==0){
        nVolumes = nParameters/2;
        nMatrices = nVolumes-1;
        std::cout << "Numbers of volumes for register: " << nVolumes <<std::endl;
        std::cout << "Numbers of matrices for register: " << nMatrices <<std::endl;
        std::cout << "With equation plane for multiplanar reformation." << std::endl;
        m_mprPreState = true;
    }
    else{
        nVolumes = (nParameters+1)/2;
        nMatrices = nVolumes-1;
        std::cout << "Numbers of volumes for register: " << nVolumes <<std::endl;
        std::cout << "Numbers of matrices for register: " << nMatrices <<std::endl;
        std::cout << "Without equation plane for multiplanar reformation." << std::endl;
        m_mprPreState = false;
    }
    data = new Import::ImgFormat[nVolumes];
    path = new string[nVolumes];

    std::vector<Import::ImgFormat*> acqVector;
    for(int i = 0; i< nVolumes; i++){
        path[i] = argv[i+1];
        data[i].umax = -pow(2,30);
        data[i].umin = pow(2,30);
        if (!load.DICOMImage (path[i], &data[i])) {///<<reading volumes
          std::cout << path[i] << "Cannot be imported!" << std::endl;
          return 0;
        }
        acqVector.push_back(&data[i]);
    }
    volumeRender.setAcquisition(acqVector);///<<setting volumes acquisitions

    std::vector<vmath::Matrix4f> vectorInvMatrixReg;
    for(int i = 0; i< nMatrices; i++){
        vmath::Matrix4f imrv;
        if(!volumeRender.readMatrix(argv[i+1+nVolumes] ,imrv)){ return 0; }///<< reading coregister matrices
        vectorInvMatrixReg.push_back(imrv);
    }
    volumeRender.setVectorInvMatrixReg(vectorInvMatrixReg);

    vmath::Vector4f eqp;
    if(m_mprPreState){
        if(!volumeRender.readPlane(argv[1+nVolumes+nMatrices], eqp) ){///<< reading plane equation for multiplanar reformatting
            m_mprState=false; return 0;
        }
        m_mprState=true;
        volumeRender.setEnableMPR(false);
        volumeRender.setMPR(eqp);      
    }
    else{
        m_mprState=false;
    }
    volumeRender.setStateMPRInput(m_mprState);

	glutInit(&argc, argv);///<< initialize OpenGL
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	  
    window = glutCreateWindow("VMTK-SPS");///<< set OpenGL context
	std::cout << "OpenGL version supported by this platform: " << glGetString(GL_VERSION) << std::endl;
	
	glewInit();///<< initialize Glew
	init();///<< initialize context parameters
	
	glutPositionWindow(10, 10);///<< intialize window position
	glutReshapeWindow(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
	
	glutReshapeFunc(reshape);
	glutDisplayFunc(drawProxyGeometry);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
	
	return 0;
}
