#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#include "osusphere.cpp"
//light sources
//light global
bool Light0On, Light1On, Light2On;
bool stopAnim;
bool Frozen;
int dist= 250;
unsigned char* Texture;	// the texels
unsigned int    WorldTex;	// the texture object

//=============Animate===============================
//for animate functions
float position = 0;
float Time;
//global timer variable for blades
#define MS_IN_THE_ANIMATION_CYCLE	10000
//================================================


//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			John Perez Almejo

// title of these windows:

const char *WINDOWTITLE = "OpenGL / GLUT Sample -- John Perez Almejo";
const char *GLUITITLE   = "User Interface Window";

// what the glui package defines as true and false:

const int GLUITRUE  = true;
const int GLUIFALSE = false;

// the escape key:

const int ESCAPE = 0x1b;

// initial window size:

const int INIT_WINDOW_SIZE = 600;

// size of the 3d box to be drawn:

const float BOXSIZE = 2.f;

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;

// minimum allowable scale factor:

const float MINSCALE = 0.05f;

// scroll wheel button values:

const int SCROLL_WHEEL_UP   = 3;
const int SCROLL_WHEEL_DOWN = 4;

// equivalent mouse movement when we click the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = 5.f;

// active mouse buttons (or them together):

const int LEFT   = 4;
const int MIDDLE = 2;
const int RIGHT  = 1;

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};
//which view
enum View
{
	inside,
	outside
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH   = 3.;

// the color numbers:
// this order must match the radio button order, which must match the order of the color names,
// 	which must match the order of the color RGB values

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};

char * ColorNames[ ] =
{
	(char *)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta",
	(char*)"White",
	(char*)"Black"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};
float White[3] = { 1., 1., 1. };

// fog parameters:

const GLfloat FOGCOLOR[4] = { .0f, .0f, .0f, 1.f };
const GLenum  FOGMODE     = GL_LINEAR;
const GLfloat FOGDENSITY  = 0.30f;
const GLfloat FOGSTART    = 1.5f;
const GLfloat FOGEND      = 4.f;


// what options should we compile-in?
// in general, you don't need to worry about these
// i compile these in to show class examples of things going wrong

//#define DEMO_Z_FIGHTING
//#define DEMO_DEPTH_BUFFER


// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
GLuint	ship;				// object display list
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		ShadowsOn;				// != 0 means to turn shadows on
int		WhichColor;				// index into Colors[ ]
int		WhichProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees
int		whichView;


// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDepthBufferMenu( int );
void	DoDepthFightingMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );

void			Axes( float );

unsigned char *	BmpToTexture( char *, int *, int * );
int				ReadInt( FILE * );
short			ReadShort( FILE * );

void			HsvRgb( float[3], float [3] );

void			Cross(float[3], float[3], float[3]);
float			Dot(float [3], float [3]);
float			Unit(float [3], float [3]);

void	SetMaterial(float, float, float, float);
void	SetPointLight(int, float, float, float, float, float, float);
void	SetSpotLight(int, float, float, float, float, float, float, float, float, float);

float* Array3(float, float, float);
float* Array4(float, float, float, float);
float* BlendArray3(float, float[3], float[3]);
float* MulArray3(float, float[3]);
// main program:

int
main( int argc, char *argv[ ] )
{
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)

	glutInit( &argc, argv );

	// setup all the graphics stuff:

	InitGraphics( );

	// create the display structures that will not change:

	InitLists( );

	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset( );

	// setup all the user interface stuff:

	InitMenus( );

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );

	// glutMainLoop( ) never actually returns
	// the following line is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutPostRedisplay( ) do it

void
Animate( )
{
	// put animation stuff in here -- change some global variables
	// for Display( ) to find:

	// force a call to Display( ) next time it is convenient:

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
	position++;
	int ms = glutGet(GLUT_ELAPSED_TIME);	// milliseconds
	ms %= MS_IN_THE_ANIMATION_CYCLE;
	Time = (float)ms / (float)MS_IN_THE_ANIMATION_CYCLE;        // [ 0., 1. )
}


// draw the complete scene:

void
Display( )
{
	// set which window we want to do the graphics into:

	glutSetWindow( MainWindow );


	// erase the background:

	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_DEPTH_TEST );
#ifdef DEMO_DEPTH_BUFFER
	if( DepthBufferOn == 0 )
		glDisable( GL_DEPTH_TEST );
#endif


	// specify shading to be flat:

	glShadeModel( GL_FLAT );


	// set the viewport to a square centered in the window:

	GLsizei vx = glutGet( GLUT_WINDOW_WIDTH );
	GLsizei vy = glutGet( GLUT_WINDOW_HEIGHT );
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = ( vx - v ) / 2;
	GLint yb = ( vy - v ) / 2;
	glViewport( xl, yb,  v, v );


	// set the viewing volume:
	// remember that the Z clipping  values are actually
	// given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	if( WhichProjection == ORTHO )
		glOrtho( -2.f, 2.f,     -2.f, 2.f,     0.1f, 1000.f );
	else
		gluPerspective( 70.f, 1.f,	0.1f, 1000.f );

	//inside outside view
	if (whichView == inside)
	{
		gluLookAt(0.f, 1.5, -1.75   ,   0.f, 0.f, 6.f   ,    0.f, 1.f, 0.f);
	}
	else 
	{
		// set the eye position, look-at position, and up-vector:
		glPushMatrix();
		gluLookAt(0.f, 0.f, 3.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);
		glPopMatrix;
	}

	// place the objects into the scene:

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );


	



	// rotate the scene:

	// rotate the scene:
	if (whichView == outside) {
		glRotatef((GLfloat)Yrot, 0.f, 1.f, 0.f);
		glRotatef((GLfloat)Xrot, 1.f, 0.f, 0.f);
	};

	// uniformly scale the scene:

	if (whichView == outside) {
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);
	};


	// set the fog parameters:

	if( DepthCueOn != 0 )
	{
		glFogi( GL_FOG_MODE, FOGMODE );
		glFogfv( GL_FOG_COLOR, FOGCOLOR );
		glFogf( GL_FOG_DENSITY, FOGDENSITY );
		glFogf( GL_FOG_START, FOGSTART );
		glFogf( GL_FOG_END, FOGEND );
		glEnable( GL_FOG );
	}
	else
	{
		glDisable( GL_FOG );
	}


	// possibly draw the axes:

	if( AxesOn != 0 )
	{
		glColor3fv( &Colors[WhichColor][0] );
		glCallList( AxesList );
	}


	// since we are using glScalef( ), be sure the normals get unitized:

	glEnable( GL_NORMALIZE );

	// 	//============Point light========================================================================

	glPopMatrix();
	//=========Point Light End==============================================================================
	//======SHIP===========================================================
	glPushMatrix();
	glCallList( ship );
	glPopMatrix();
	//===== Stars =========================================================
	//T
	if (!stopAnim) //while aniamtion is not over
	{
		glutIdleFunc(Animate);
		int x = 10;
		for (int y = 36; y < 1500; y++)
		{

			for (int i = 0; i < 25; i++)
			{

				glPushMatrix(); // spin!
				glRotatef((y * i), 0.0, 0.0, 1.0);
				glPushMatrix();
				glTranslatef(0.0, 0.0, x);
				glTranslatef(0.0, 5, Time * (-position * 3.0));
				glColor3f(1.0, 1.0, 0.0);
				glPointSize(1.0);
				glBegin(GL_POINTS);
				glVertex3f(0.0, 0.0, 0.0);
				glEnd();
				glPopMatrix();
				glPopMatrix();//spin!
			}
			x++;
		}
	}
	else 
	{
		if (dist > 50)
		{
			int x = 10;
			for (int y = 36; y < 750; y++)
			{

				for (int i = 0; i < 25; i++)
				{

					glPushMatrix(); // spin!
					glRotatef((y * i), 0.0, 0.0, 1.0);
					glPushMatrix();
					glTranslatef(0.0, 0.0, x);
					glTranslatef(0.0, 5, Time * (-position * 3.0));
					glColor3f(1.0, 1.0, 1.0);
					glPointSize(1.0);
					glBegin(GL_POINTS);
					glVertex3f(0.0, 0.0, 0.0);
					glEnd();
					glPopMatrix();
					glPopMatrix();//spin!
				}
				x++;

			}
		}
		if (dist > 20)
		{

			glPushMatrix();
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, WorldTex);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glTranslatef(0.0, 1.0, dist);
			//glTranslatef(0.0, 0.0, -(100.0 * Time * 1.25));
			OsuSphere(1.0, 100.0, 100.0);
			glDisable(GL_TEXTURE_2D);
			glPopMatrix();
			dist--;
		}
		else if(dist == 20)
		{
			glPushMatrix();
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, WorldTex);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glTranslatef(0.0, 1.0, 28);
			//glTranslatef(0.0, 0.0, -(100.0 * Time * 1.25));
			OsuSphere(1.0, 100.0, 100.0);
			glDisable(GL_TEXTURE_2D);
			glPopMatrix();
			dist = 250;
			glutIdleFunc(NULL);
	
		}
	}
#ifdef DEMO_Z_FIGHTING
	if( DepthFightingOn != 0 )
	{
		glPushMatrix( );
			glRotatef( 90.f,   0.f, 1.f, 0.f );
			glCallList( ship );
		glPopMatrix( );
	}
#endif


	// draw some gratuitous text that just rotates on top of the scene:
	// i commented out the actual text-drawing calls -- put them back in if you have a use for them
	// a good use for thefirst one might be to have your name on the screen
	// a good use for the second one might be to have vertex numbers on the screen alongside each vertex

	glDisable( GL_DEPTH_TEST );
	glColor3f( 0.f, 1.f, 1.f );
	//DoRasterString( 0.f, 1.f, 0.f, (char *)"Text That Moves" );


	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	glDisable( GL_DEPTH_TEST );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluOrtho2D( 0.f, 100.f,     0.f, 100.f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	glColor3f( 1.f, 1.f, 1.f );
	//DoRasterString( 5.f, 5.f, 0.f, (char *)"Text That Doesn't" );

	// swap the double-buffered framebuffers:

	glutSwapBuffers( );

	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush( );
}


void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	WhichColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthBufferMenu( int id )
{
	DepthBufferOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthFightingMenu( int id )
{
	DepthFightingOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthMenu( int id )
{
	DepthCueOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	WhichProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(int) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthbuffermenu = glutCreateMenu( DoDepthBufferMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthfightingmenu = glutCreateMenu( DoDepthFightingMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Axis Colors",   colormenu);

#ifdef DEMO_DEPTH_BUFFER
	glutAddSubMenu(   "Depth Buffer",  depthbuffermenu);
#endif

#ifdef DEMO_Z_FIGHTING
	glutAddSubMenu(   "Depth Fighting",depthfightingmenu);
#endif

	glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}



// initialize the glut and OpenGL libraries:
//	also setup callback functions

void
InitGraphics( )
{
	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc(MouseMotion);
	//glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );

	// setup glut to call Animate( ) every time it has
	// 	nothing it needs to respond to (which is most of the time)
	// we don't need to do this for this program, and really should set the argument to NULL
	// but, this sets us up nicely for doing animation

		//Texture stuff added
	int width, height;
	Texture = BmpToTexture((char*)"worldtex.bmp", &width, &height);
	if (Texture == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", "worldtex.bmp");
	else
		fprintf(stderr, "Width = %d ; Height = %d\n", width, height);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &WorldTex);
	glBindTexture(GL_TEXTURE_2D, WorldTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);


	glutIdleFunc( Animate );

	// init the glew package (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{
	float dx = BOXSIZE / 2.f;
	float dy = BOXSIZE / 2.f;
	float dz = BOXSIZE / 2.f;
	glutSetWindow( MainWindow );

	// create the object:

	ship = glGenLists( 1 );
	glNewList( ship, GL_COMPILE );

		//========nose cone================================
		glBegin( GL_TRIANGLE_FAN );
			glColor3f( 0.9, 0.0, .0 ); //B
					glVertex3f(0.0, 0.0,  2.5 );
					glVertex3f(0.0, 0.5, 0.0);
					glVertex3f(-0.5, 0.0, 0.0);
					glVertex3f(0.0, -0.5, 0.0);
					glVertex3f(0.5, 0.0, 0.0);
					glVertex3f(0.0, 0.5, 0.0);
		glEnd( );

	//==============body===========================
		glBegin(GL_QUAD_STRIP);
			glColor3f(0.9, .1, .1);
				glVertex3f(-0.5, 0.0, 0.0);
				glVertex3f(-0.5, 0.0, -1.0);
				glVertex3f(0.0, 0.5, 0.0);
				glVertex3f(0.0, 0.5, -1.0);
				glVertex3f(0.5, 0.0, 0.0);
				glVertex3f(0.5, 0.0, -1.0);
				glVertex3f(0.0, -0.5, 0.0);
				glVertex3f(0.0, -0.5, -1.0);
				glVertex3f(-0.5, 0.0, 0.0);
				glVertex3f(-0.5, 0.0, -1.0);
		glEnd();

		glBegin(GL_QUADS);
			//back of body
			glColor3f(1.0, 0.1, .1);
				glVertex3f(-0.5, 0.0, -1.0);
				glVertex3f(0.0, 0.5, -1.0);
				glVertex3f(0.5, 0.0, -1.0);
				glVertex3f(0.0, -0.5, -1.0);
		glEnd();

	//================left wing==================================
		glBegin(GL_QUAD_STRIP);
			glColor3f(1.0, .05, .05);
				glVertex3f(0.0, 0.10, -1.0);
				glVertex3f(4.0, 0.10, -0.75);
				glVertex3f(0.0, 0.0, 0.0);
				glVertex3f(4.0, 0.0, 0.25);
				glVertex3f(0.0, -0.10, -1.0);
				glVertex3f(4.0, -0.10, -0.75);
				glVertex3f(0.0, 0.10, -1.0);
				glVertex3f(4.0, 0.10, -0.75);
		glEnd();


	//left wing tip
		glBegin(GL_QUADS);
			glColor3f(1.0, .05, .05);
				glVertex3f(4.0,0.10,-0.75);
				glVertex3f(4.0, 0.0, 0.25);
				glVertex3f(4.25, 0.0, 1.0);
				glVertex3f(4.25, 0.10, -0.75);
		//bottom
				glVertex3f(4.0, -0.10, -0.75);
				glVertex3f(4.0, 0.0, 0.25);
				glVertex3f(4.25, 0.0, 0.1);
				glVertex3f(4.25, -0.10, -0.75);
		//back 
				glVertex3f(4.0, 0.10, -0.75);
				glVertex3f(4.25, 0.10, -0.75);
				glVertex3f(4.0, -0.10, -0.75);
				glVertex3f(4.25, -0.10, -0.75);
		glEnd();
		//side tip
		glBegin(GL_TRIANGLES);
			glColor3f(1.0, .05, .05);
				glVertex3f(4.25, 0.10, -0.75);
				glVertex3f(4.25, 0.0, 0.1);
				glVertex3f(4.25, -0.10, -0.75);
		glEnd();

		//vertical up back
		glBegin(GL_TRIANGLES);
			glColor3f(1.0, .10, .10);
				glVertex3f(4.0, 0.10, -0.75);
				glVertex3f(4.125, 0.75, -0.75);
				glVertex3f(4.25, 0.10, -0.75);
		//vertical up front
				glVertex3f(4.0, 0.0, 0.25);
				glVertex3f(4.125, 0.75, 0.25);
				glVertex3f(4.25, 0.0, 0.25);
		glEnd();
		//vertical up sides
		glBegin(GL_QUAD_STRIP);
			glColor3f(1.0, .17, .17);
				glVertex3f(4.0, 0.10, -0.75);
				glVertex3f(4.0, 0.0, 0.25);
				glVertex3f(4.125, 0.75, -0.75);
				glVertex3f(4.125, 0.75, 0.25);
				glVertex3f(4.25, 0.10, -0.75);
				glVertex3f(4.25, 0.0, 0.25);
		glEnd();
		//vertical down back
		glBegin(GL_TRIANGLES);
			glColor3f(1., .10, .10);
				glVertex3f(4.0, -0.10, -0.75);
				glVertex3f(4.125, -0.75, -0.75);
				glVertex3f(4.25, -0.10, -0.75);
		//vertical down front
				glVertex3f(4.0, 0.0, 0.25);
				glVertex3f(4.125, -0.75, 0.25);
				glVertex3f(4.25, 0.0, 0.25);
		glEnd();
		//vertical down sides
		glBegin(GL_QUAD_STRIP);
			glColor3f(1.0, .17, .17);
				glVertex3f(4.0, -0.10, -0.75);
				glVertex3f(4.0, 0.0, 0.25);
				glVertex3f(4.125, -0.75, -0.75);
				glVertex3f(4.125, -0.75, 0.25);
				glVertex3f(4.25, -0.10, -0.75);
				glVertex3f(4.25, 0.0, 0.25);
		glEnd();

	//==============right wing==============================================
		glBegin(GL_QUAD_STRIP);
			glColor3f(1.0, .05, .05);
				glVertex3f(0.0, 0.10, -1.0);
				glVertex3f(-4.0, 0.10, -0.75);
				glVertex3f(0.0, 0.0, 0.0);
				glVertex3f(-4.0, 0.0, 0.25);
				glVertex3f(0.0, -0.10, -1.0);
				glVertex3f(-4.0, -0.10, -0.75);
				glVertex3f(0.0, 0.10, -1.0);
				glVertex3f(-4.0, 0.10, -0.75);
		glEnd();


		//right wing tip
		glBegin(GL_QUADS);
			glColor3f(1.0, .05, .05);
				glVertex3f(-4.0, 0.10, -0.75);
				glVertex3f(-4.0, 0.0, 0.25);
				glVertex3f(-4.25, 0.0, 1.0);
				glVertex3f(-4.25, 0.10, -0.75);
		//bottom
				glVertex3f(-4.0, -0.10, -0.75);
				glVertex3f(-4.0, 0.0, 0.25);
				glVertex3f(-4.25, 0.0, 1.0);
				glVertex3f(-4.25, -0.10, -0.75);
		// back
				glVertex3f(-4.0, 0.10, -0.75);
				glVertex3f(-4.25, 0.10, -0.75);
				glVertex3f(-4.0, -0.10, -0.75);
				glVertex3f(-4.25, -0.10, -0.75);
		glEnd();
		//side
		glBegin(GL_TRIANGLES);
			glColor3f(1.0, .05, .05);
				glVertex3f(-4.25, 0.10, -0.75);
				glVertex3f(-4.25, 0.0, 1.0);
				glVertex3f(-4.25, -0.10, -0.75);
		glEnd();

		//vertical up back
		glBegin(GL_TRIANGLES);
			glColor3f(1.0, .10, .10);
				glVertex3f(-4.0, 0.10, -0.75);
				glVertex3f(-4.125, 0.75, -0.75);
				glVertex3f(-4.25, 0.10, -0.75);
		//vertical up front
				glVertex3f(-4.0, 0.0, 0.25);
				glVertex3f(-4.125, 0.75, 0.25);
				glVertex3f(-4.25, 0.0, 0.25);
		glEnd();
		//vertical up sides
		glBegin(GL_QUAD_STRIP);
			glColor3f(1.0, .17, .17);
				glVertex3f(-4.0, 0.10, -0.75);
				glVertex3f(-4.0, 0.0, 0.25);
				glVertex3f(-4.125, 0.75, -0.75);
				glVertex3f(-4.125, 0.75, 0.25);
				glVertex3f(-4.25, 0.10, -0.75);
				glVertex3f(-4.25, 0.0, 0.25);
		glEnd();
		//vertical down back
		glBegin(GL_TRIANGLES);
			glColor3f(1.0, .10, .10);
				glVertex3f(-4.0, -0.10, -0.75);
				glVertex3f(-4.125, -0.75, -0.75);
				glVertex3f(-4.25, -0.10, -0.75);
		//vertical down front
				glVertex3f(-4.0, 0.0, 0.25);
				glVertex3f(-4.125, -0.75, 0.25);
				glVertex3f(-4.25, 0.0, 0.25);
		glEnd();
		//vertical down sides
		glBegin(GL_QUAD_STRIP);
			glColor3f(1.0, .17, .17);
				glVertex3f(-4.0, -0.10, -0.75);
				glVertex3f(-4.0, 0.0, 0.25);
				glVertex3f(-4.125, -0.75, -0.75);
				glVertex3f(-4.125, -0.75, 0.25);
				glVertex3f(-4.25, -0.10, -0.75);
				glVertex3f(-4.25, 0.0, 0.25);
		glEnd();

		//=================== COCKPIT ===========================
		glBegin(GL_TRIANGLES);
		//R side
		glColor3f(1.0, .10, .10);
			glVertex3f(0.0, 0.5, -0.5);
			glVertex3f(0.0, 1.0, -1.0);
			glVertex3f(-1.0, 1.0, -2.0);

			glVertex3f(0.0, 0.5, -0.5);
			glVertex3f(-1.0, 1.0, -2.0);
			glVertex3f(-1.0, 0.5, -2.0);
	//R side 2
			glVertex3f(-1.0, 1.0, -2.0);
			glVertex3f(-0.0, 0.5, -3.0);
			glVertex3f(-0.0, 1.0, -3.0);

			glVertex3f(-1.0, 1.0, -2.0);
			glVertex3f(-0.0, 0.5, -3.0);
			glVertex3f(-1.0, 0.5, -2.0);



		glEnd();
		//L side
		glBegin(GL_TRIANGLES);
			glColor3f(1.0, .10, .10);
				glVertex3f(0.0, 0.5, -0.5);
				glVertex3f(0.0, 1.0, -1.0);
				glVertex3f(1.0, 1.0, -2.0);

				glVertex3f(0.0, 0.5, -0.5);
				glVertex3f(1.0, 1.0, -2.0);
				glVertex3f(1.0, 0.5, -2.0);

				glVertex3f(1.0, 1.0, -2.0);
				glVertex3f(0.0, 0.5, -3.0);
				glVertex3f(0.0, 1.0, -3.0);

				glVertex3f(1.0, 1.0, -2.0);
				glVertex3f(0.0, 0.5, -3.0);
				glVertex3f(1.0, 0.5, -2.0);
		glEnd();

		//cockpit top
		glBegin(GL_QUADS);
			glColor3f(1.0,0.25,0.25);
				glVertex3f(0.0, 1.0, -1.0);
				glVertex3f(1.0, 1.0, -2.0);
				glVertex3f(0.0, 1.0, -3.0);
				glVertex3f(-1.0, 1.0, -2.0);
			//cockpit bottom
			glColor3f(0.3, 0.3, 0.3);
				glVertex3f(0.0, 0.5, -0.5);
				glVertex3f(1.0, 0.5, -2.0);
				glVertex3f(0.0, 0.5, -3.0);
				glVertex3f(-1.0, 0.5, -2.0);
		glEnd();

		glPushMatrix();
		glTranslatef(0.0, -0.6, -0.5);
		glColor3f(0.8, 0.0, 0.0);
		OsuSphere(0.4, 100.0, 100.0);
		glPopMatrix();


		glPushMatrix();
		glTranslatef(0.0, 0.5, -2.0);
		glColor3f(0.27, 0.27, 0.27);
		OsuSphere(0.6, 100.0, 100.0);
		glPopMatrix();
		//OsuCone(0.25, 0.0, 10.0, 100, 10);
	//===============Engine!==================
		//sides
		glBegin(GL_QUAD_STRIP);
			glColor3f(0.3, .3, .3);
				glVertex3f(0.0, 0.3, -2.75);
				glVertex3f(0.0, 0.75, -3.75);
				glVertex3f(0.3, 0.0, -2.75);
				glVertex3f(0.75, 0.0, -3.75);
				glVertex3f(0.0, -0.3, -2.75);
				glVertex3f(0.0, -0.75, -3.75);
				glVertex3f(-0.3, 0.0, -2.75);
				glVertex3f(-0.75, 0.0, -3.75);
				glVertex3f(0.0, 0.3, -2.75);
				glVertex3f(0.0, 0.75, -3.75);
		glEnd();

		//burner
		glBegin(GL_QUADS);
			glColor3f(0.0, 0.5, 0.8);
				glVertex3f(0.0, 0.75, -3.75);
				glVertex3f(0.75, 0.0, -3.75);
				glVertex3f(0.0, -0.75, -3.75);
				glVertex3f(-0.75, 0.0, -3.75);
		glEnd();
		//balljoint
		glPushMatrix();
		glTranslatef(0.0, 0.35, -2.75);
		glColor3f(0.27, 0.27, 0.27);
		OsuSphere(0.2, 100.0, 100.0);
		glPopMatrix();
		//thruster shield
		glBegin(GL_QUAD_STRIP);
			glColor3f(0.9, 0.0, .0);
				glVertex3f(-1.0, 0.25, -3.25);
				glVertex3f(-1.0, 0.25, -3.75);
				glVertex3f(-1.0, -0.25, -3.25);
				glVertex3f(-1.0, -0.25, -3.75);
				glVertex3f(-0.25, -1.0, -3.25);
				glVertex3f(-0.25, -1.0, -3.75);
				glVertex3f(0.25, -1.0, -3.25);
				glVertex3f(0.25, -1.0, -3.75);
				glVertex3f(1.0, -0.25, -3.25);
				glVertex3f(1.0, -0.25, -3.75);
				glVertex3f(1.0, 0.25, -3.25);
				glVertex3f(1.0, 0.25, -3.75);
				glVertex3f(0.25, 1.0, -3.25);
				glVertex3f(0.25, 1.0, -3.75);
				glVertex3f(-0.25, 1.0, -3.25);
				glVertex3f(-0.25, 1.0, -3.75);
				glVertex3f(-1.0, 0.25, -3.25);
				glVertex3f(-1.0, 0.25, -3.75);
		glEnd();
		//thruster supports
		glBegin(GL_QUADS);
		glColor3f(0.5, 0.5, 0.5);
		glVertex3f(-1.0, 0.0, -3.25);
		glVertex3f(-1.0, 0.0, -3.75);
		glVertex3f(1.0, 0.0, -3.25);
		glVertex3f(1.0, 0.0, -3.75);

		glEnd();
		//shield fins
		glBegin(GL_TRIANGLES);
			glColor3f(0.8, 0.0, .0);
			glVertex3f(-0.1, 1.0, -3.75);
			glVertex3f(0.0, 1.25, -3.75);
			glVertex3f(0.1, 1.0, -3.75);
			glVertex3f(-0.1, 1.0, -3.75);
			glVertex3f(0.0, 1.25, -3.75);
			glVertex3f(0.0, 1.0, -3.5);
			glVertex3f(0.1, 1.0, -3.75);
			glVertex3f(0.0, 1.25, -3.75);
			glVertex3f(0.0, 1.0, -3.5);
			glVertex3f(-0.1, -1.0, -3.75);
			glVertex3f(0.0, -1.25, -3.75);
			glVertex3f(0.1, -1.0, -3.75);
			glVertex3f(-0.1, -1.0, -3.75);
			glVertex3f(0.0, -1.25, -3.75);
			glVertex3f(0.0, -1.0, -3.5);
			glVertex3f(0.1, -1.0, -3.75);
			glVertex3f(0.0, -1.25, -3.75);
			glVertex3f(0.0, -1.0, -3.5);
		glEnd();

	//==============COCKPIT shield==========================

		glPushMatrix();
		glTranslatef(0.0, 0.75, -2.0);
		glColor3f(0.8, 0.5, 0.0);
		OsuSphere(0.6, 100.0, 100.0);
		glPopMatrix();

		//shield braces

		glLineWidth(8);
		glBegin(GL_LINE_STRIP);
			glColor3f(0.25, 0.25, 0.25);
				glVertex3f(0.35, 1.0, -1.5);
				glVertex3f(0.35, 1.3, -1.75);
				glVertex3f(0.35, 1.3, -2.25);
				glVertex3f(0.35, 1.0, -2.5);
		glEnd();


		glBegin(GL_LINE_STRIP);
			glColor3f(0.25, 0.25, 0.25);
				glVertex3f(-0.35, 1.0, -1.5);
				glVertex3f(-0.35, 1.3, -1.75);
				glVertex3f(-0.35, 1.3, -2.25);
				glVertex3f(-0.35, 1.0, -2.5);
		glEnd();

		glBegin(GL_LINE_STRIP);
			glColor3f(0.5, 0.5, 0.5);
				glVertex3f(0.4, 0.9, -1.25);
				glVertex3f(0.4, 0.9, -1.0);
				glVertex3f(0.4, 0.6, -1.0);
				glVertex3f(0.4, 0.6, -1.25);

		glEnd();

		glBegin(GL_LINE_STRIP);
			glColor3f(0.5, 0.5, 0.5);
		glVertex3f(-0.4, 0.9, -1.25);
		glVertex3f(-0.4, 0.9, -1.0);
		glVertex3f(-0.4, 0.6, -1.0);
		glVertex3f(-0.4, 0.6, -1.25);
		glEnd();
	//======GUN BARREL======================================================
		glBegin(GL_QUAD_STRIP);
		glColor3f(0.3, 0.3, 0.3);
		glVertex3f(-0.1, -0.75, -0.5);
		glVertex3f(-0.06, -0.65, 4.0);
		glVertex3f(0.1, -0.75, -0.5);
		glVertex3f(0.06, -0.65, 4.0);
		glVertex3f(0.1, -0.5, -0.5);
		glVertex3f(0.06, -0.5, 4.0);
		glVertex3f(-0.1, -0.5, -0.5);
		glVertex3f(-0.06, -0.5, 4.0);
		glVertex3f(-0.1, -0.75, -0.5);
		glVertex3f(-0.06, -0.65, 4.0);
		glEnd();

//==============headlights!======================================================
		glBegin(GL_TRIANGLES);
		glColor3f(0.4, 0.4, 0.4);
		glVertex3f(0.3, 0.4, 0.0);
		glVertex3f(0.3, 0.1, -0.3);
		glVertex3f(0.4, 0.1, 0.0);
		glVertex3f(0.3, 0.4, 0.0);
		glVertex3f(0.3, 0.1, -0.3);
		glVertex3f(0.1, 0.1, 0.0);

		glVertex3f(-0.3, 0.4, 0.0);
		glVertex3f(-0.3, 0.1, -0.3);
		glVertex3f(-0.4, 0.1, 0.0);
		glVertex3f(-0.3, 0.4, 0.0);
		glVertex3f(-0.3, 0.1, -0.3);
		glVertex3f(-0.1, 0.1, 0.0);
		glEnd();

		glPushMatrix();
		glTranslatef(0.29, 0.25, -0.01);
		glColor3f(1.0, 1.0, 0.0);
		OsuSphere(0.05, 100.0, 100.0);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-0.29, 0.25, -0.01);
		glColor3f(1.0, 1.0, 0.0);
		OsuSphere(0.05, 100.0, 100.0);
		glPopMatrix();

	glEndList( );



	// create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList( );
}


// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{

		case 's':
		case 'S':
			if (stopAnim)
			{
				stopAnim = false;
			}
			else
			{
				stopAnim = true;
			}
		
			break;

		case 'i':
		case 'I':
			whichView = inside;
			break;

		case 'u':
		case 'U':
			whichView = outside;
			break;

		case 'o':
		case 'O':
			WhichProjection = ORTHO;
			break;

		case 'p':
		case 'P':
			WhichProjection = PERSP;
			break;

		case 'q':
		case 'Q':
		case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		case SCROLL_WHEEL_UP:
			Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		case SCROLL_WHEEL_DOWN:
			Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}

	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}

	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	stopAnim = false;
	Light0On = Light1On = Light2On = false;
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	ShadowsOn = 0;
	WhichColor = WHITE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;
	whichView = outside;

	
}


// called when user resizes the window:

void
Resize( int width, int height )
{
	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = { 0.f, 1.f, 0.f, 1.f };

static float xy[ ] = { -.5f, .5f, .5f, -.5f };

static int xorder[ ] = { 1, 2, -3, 4 };

static float yx[ ] = { 0.f, 0.f, -.5f, .5f };

static float yy[ ] = { 0.f, .6f, 1.f, 1.f };

static int yorder[ ] = { 1, 2, 3, -2, 4 };

static float zx[ ] = { 1.f, 0.f, 1.f, 0.f, .25f, .75f };

static float zy[ ] = { .5f, .5f, -.5f, -.5f, 0.f, 0.f };

static int zorder[ ] = { 1, 2, 3, 4, -5, 6 };

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}

// read a BMP file into a Texture:

#define VERBOSE				false
#define BMP_MAGIC_NUMBER	0x4d42
#ifndef BI_RGB
#define BI_RGB				0
#define BI_RLE8				1
#define BI_RLE4				2
#endif


// bmp file header:
struct bmfh
{
	short bfType;		// BMP_MAGIC_NUMBER = "BM"
	int bfSize;		// size of this file in bytes
	short bfReserved1;
	short bfReserved2;
	int bfOffBytes;		// # bytes to get to the start of the per-pixel data
} FileHeader;

// bmp info header:
struct bmih
{
	int biSize;		// info header size, should be 40
	int biWidth;		// image width
	int biHeight;		// image height
	short biPlanes;		// #color planes, should be 1
	short biBitCount;	// #bits/pixel, should be 1, 4, 8, 16, 24, 32
	int biCompression;	// BI_RGB, BI_RLE4, BI_RLE8
	int biSizeImage;
	int biXPixelsPerMeter;
	int biYPixelsPerMeter;
	int biClrUsed;		// # colors in the palette
	int biClrImportant;
} InfoHeader;



// read a BMP file into a Texture:

unsigned char *
BmpToTexture( char *filename, int *width, int *height )
{
	FILE *fp;
#ifdef _WIN32
        errno_t err = fopen_s( &fp, filename, "rb" );
        if( err != 0 )
        {
		fprintf( stderr, "Cannot open Bmp file '%s'\n", filename );
		return NULL;
        }
#else
		fp = fopen( filename, "rb" );
		if( fp == NULL )
		{
			fprintf( stderr, "Cannot open Bmp file '%s'\n", filename );
			return NULL;
		}
#endif

	FileHeader.bfType = ReadShort( fp );


	// if bfType is not BMP_MAGIC_NUMBER, the file is not a bmp:

	if( VERBOSE ) fprintf( stderr, "FileHeader.bfType = 0x%0x = \"%c%c\"\n",
			FileHeader.bfType, FileHeader.bfType&0xff, (FileHeader.bfType>>8)&0xff );
	if( FileHeader.bfType != BMP_MAGIC_NUMBER )
	{
		fprintf( stderr, "Wrong type of file: 0x%0x\n", FileHeader.bfType );
		fclose( fp );
		return NULL;
	}


	FileHeader.bfSize = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "FileHeader.bfSize = %d\n", FileHeader.bfSize );

	FileHeader.bfReserved1 = ReadShort( fp );
	FileHeader.bfReserved2 = ReadShort( fp );

	FileHeader.bfOffBytes = ReadInt( fp );


	InfoHeader.biSize = ReadInt( fp );
	InfoHeader.biWidth = ReadInt( fp );
	InfoHeader.biHeight = ReadInt( fp );

	const int nums = InfoHeader.biWidth;
	const int numt = InfoHeader.biHeight;

	InfoHeader.biPlanes = ReadShort( fp );

	InfoHeader.biBitCount = ReadShort( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biBitCount = %d\n", InfoHeader.biBitCount );

	InfoHeader.biCompression = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biCompression = %d\n", InfoHeader.biCompression );

	InfoHeader.biSizeImage = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biSizeImage = %d\n", InfoHeader.biSizeImage );

	InfoHeader.biXPixelsPerMeter = ReadInt( fp );
	InfoHeader.biYPixelsPerMeter = ReadInt( fp );

	InfoHeader.biClrUsed = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biClrUsed = %d\n", InfoHeader.biClrUsed );

	InfoHeader.biClrImportant = ReadInt( fp );

	// fprintf( stderr, "Image size found: %d x %d\n", ImageWidth, ImageHeight );

	// pixels will be stored bottom-to-top, left-to-right:
	unsigned char *texture = new unsigned char[ 3 * nums * numt ];
	if( texture == NULL )
	{
		fprintf( stderr, "Cannot allocate the texture array!\n" );
		return NULL;
	}

	// extra padding bytes:

	int requiredRowSizeInBytes = 4 * ( ( InfoHeader.biBitCount*InfoHeader.biWidth + 31 ) / 32 );
	if( VERBOSE )	fprintf( stderr, "requiredRowSizeInBytes = %d\n", requiredRowSizeInBytes );

	int myRowSizeInBytes = ( InfoHeader.biBitCount*InfoHeader.biWidth + 7 ) / 8;
	if( VERBOSE )	fprintf( stderr, "myRowSizeInBytes = %d\n", myRowSizeInBytes );

	int numExtra = requiredRowSizeInBytes - myRowSizeInBytes;
	if( VERBOSE )	fprintf( stderr, "New NumExtra padding = %d\n", numExtra );


	// this function does not support compression:

	if( InfoHeader.biCompression != 0 )
	{
		fprintf( stderr, "Wrong type of image compression: %d\n", InfoHeader.biCompression );
		fclose( fp );
		return NULL;
	}
	
	// we can handle 24 bits of direct color:
	if( InfoHeader.biBitCount == 24 )
	{
		rewind( fp );
		fseek( fp, FileHeader.bfOffBytes, SEEK_SET );
		int t;
		unsigned char *tp;
		for( t = 0, tp = texture; t < numt; t++ )
		{
			for( int s = 0; s < nums; s++, tp += 3 )
			{
				*(tp+2) = fgetc( fp );		// b
				*(tp+1) = fgetc( fp );		// g
				*(tp+0) = fgetc( fp );		// r
			}

			for( int e = 0; e < numExtra; e++ )
			{
				fgetc( fp );
			}
		}
	}

	// we can also handle 8 bits of indirect color:
	if( InfoHeader.biBitCount == 8 && InfoHeader.biClrUsed == 256 )
	{
		struct rgba32
		{
			unsigned char r, g, b, a;
		};
		struct rgba32 *colorTable = new struct rgba32[ InfoHeader.biClrUsed ];

		rewind( fp );
		fseek( fp, sizeof(struct bmfh) + InfoHeader.biSize - 2, SEEK_SET );
		for( int c = 0; c < InfoHeader.biClrUsed; c++ )
		{
			colorTable[c].r = fgetc( fp );
			colorTable[c].g = fgetc( fp );
			colorTable[c].b = fgetc( fp );
			colorTable[c].a = fgetc( fp );
			if( VERBOSE )	fprintf( stderr, "%4d:\t0x%02x\t0x%02x\t0x%02x\t0x%02x\n",
				c, colorTable[c].r, colorTable[c].g, colorTable[c].b, colorTable[c].a );
		}

		rewind( fp );
		fseek( fp, FileHeader.bfOffBytes, SEEK_SET );
		int t;
		unsigned char *tp;
		for( t = 0, tp = texture; t < numt; t++ )
		{
			for( int s = 0; s < nums; s++, tp += 3 )
			{
				int index = fgetc( fp );
				*(tp+0) = colorTable[index].r;	// r
				*(tp+1) = colorTable[index].g;	// g
				*(tp+2) = colorTable[index].b;	// b
			}

			for( int e = 0; e < numExtra; e++ )
			{
				fgetc( fp );
			}
		}

		delete[ ] colorTable;
	}

	fclose( fp );

	*width = nums;
	*height = numt;
	return texture;
}

int
ReadInt( FILE *fp )
{
	const unsigned char b0 = fgetc( fp );
	const unsigned char b1 = fgetc( fp );
	const unsigned char b2 = fgetc( fp );
	const unsigned char b3 = fgetc( fp );
	return ( b3 << 24 )  |  ( b2 << 16 )  |  ( b1 << 8 )  |  b0;
}

short
ReadShort( FILE *fp )
{
	const unsigned char b0 = fgetc( fp );
	const unsigned char b1 = fgetc( fp );
	return ( b1 << 8 )  |  b0;
}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = (float)floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r=0., g=0., b=0.;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;


}
void
SetMaterial(float r, float g, float b, float shininess)
{
	glMaterialfv(GL_BACK, GL_EMISSION, Array3(0., 0., 0.));
	glMaterialfv(GL_BACK, GL_AMBIENT, MulArray3(.4f, White));
	glMaterialfv(GL_BACK, GL_DIFFUSE, MulArray3(1., White));
	glMaterialfv(GL_BACK, GL_SPECULAR, Array3(0., 0., 0.));
	glMaterialf(GL_BACK, GL_SHININESS, 5.f);

	glMaterialfv(GL_FRONT, GL_EMISSION, Array3(0., 0., 0.));
	glMaterialfv(GL_FRONT, GL_AMBIENT, Array3(r, g, b));
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Array3(r, g, b));
	glMaterialfv(GL_FRONT, GL_SPECULAR, MulArray3(.8f, White));
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

void
SetPointLight(int ilight, float x, float y, float z, float r, float g, float b)
{
	glLightfv(ilight, GL_POSITION, Array3(x, y, z));
	glLightfv(ilight, GL_AMBIENT, Array3(0., 0., 0.));
	glLightfv(ilight, GL_DIFFUSE, Array3(r, g, b));
	glLightfv(ilight, GL_SPECULAR, Array3(r, g, b));
	glLightf(ilight, GL_CONSTANT_ATTENUATION, 1.);
	glLightf(ilight, GL_LINEAR_ATTENUATION, 0.);
	glLightf(ilight, GL_QUADRATIC_ATTENUATION, 0.);
	glEnable(ilight);
}


void
SetSpotLight(int ilight, float x, float y, float z, float xdir, float ydir, float zdir, float r, float g, float b)
{
	glLightfv(ilight, GL_POSITION, Array3(x, y, z));
	glLightfv(ilight, GL_SPOT_DIRECTION, Array3(xdir, ydir, zdir));
	glLightf(ilight, GL_SPOT_EXPONENT, 1.);
	glLightf(ilight, GL_SPOT_CUTOFF, 45.);
	glLightfv(ilight, GL_AMBIENT, Array3(0., 0., 0.));
	glLightfv(ilight, GL_DIFFUSE, Array3(r, g, b));
	glLightfv(ilight, GL_SPECULAR, Array3(r, g, b));
	glLightf(ilight, GL_CONSTANT_ATTENUATION, 1.);
	glLightf(ilight, GL_LINEAR_ATTENUATION, 0.);
	glLightf(ilight, GL_QUADRATIC_ATTENUATION, 0.);
	glEnable(ilight);
}

float*
Array3(float a, float b, float c)
{
	static float array[4];

	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}

float*
Array4(float a, float b, float c, float d)
{
	static float array[4];

	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = d;
	return array;
}

float*
BlendArray3(float factor, float array0[3], float array1[3])
{
	static float array[4];

	array[0] = factor * array0[0] + (1.f - factor) * array1[0];
	array[1] = factor * array0[1] + (1.f - factor) * array1[1];
	array[2] = factor * array0[2] + (1.f - factor) * array1[2];
	array[3] = 1.;
	return array;
}

float*
MulArray3(float factor, float array0[3])
{
	static float array[4];

	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}
//========================================================================