//nclude <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <iostream>
#include <vector>



//////////////////////////////////////////

namespace OpenGLSkeleton {


    // In order to achieve rotation and zooming
    // we move the camera around, this is moved
    // by converting spherical coordinates to cartesian coordinates.
    float rho=2.0f, phi=0.01, theta=0.01;

    // laziest Triangle implementation 
    // not even set/gets.. modify as you see fit.
    // just containing the points and the normals associated to them.
    class Triangle {
	 public:
		float p1[3];
 	        float n1[3];

		float p2[3];
 	        float n2[3];

		float p3[3];
 	        float n3[3];
    };
    

    std::vector<Triangle> surface;
    int lightScale = 10;

    // How far away the light is located from the origin...
    void ChangeLightScale(int value){ lightScale = value; }


    void Initialize(int argc, char** argv){
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
        glutInitWindowSize(500, 500);
        glutInitWindowPosition(100, 100);
        glutCreateWindow("triangle window");
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);

        gluPerspective(90,1.0, 0.1, 1000);
	glShadeModel(GL_SMOOTH);
	//Tell OpenGL to use lightning calculations.
	glEnable(GL_LIGHTING);
	//Enable the light source
	glEnable(GL_LIGHT0);

        // Create a grey light
        GLfloat white_light[] = {0.8,0.8,0.8,1};
	glLightfv(GL_LIGHT0, GL_AMBIENT, white_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);

        // locate it a certain distance from the origin...
	GLfloat light_position[] = { lightScale*cos(0.25*3.14159)*sin(0.25*3.14159), lightScale*sin(0.25*3.14159)*sin(0.25*3.14159), lightScale*cos(3.14159), 0.0};
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    }

    void (*keyboardInteraction)(unsigned char key, int x, int y) = NULL;


    void SetKeyboardInteraction(void (*func)(unsigned char, int,int)  ){
        keyboardInteraction = func;
    }

   
    void display(void)
    {
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	    float z=rho*cos(phi)*sin(theta);
	    float x=rho*sin(phi)*sin(theta);
	    float y=rho*cos(theta);

	    // initialize modelview with identity matrix
	    glMatrixMode(GL_MODELVIEW);
	    glLoadIdentity();
	    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);


            gluLookAt(x, y, z, 0.0f, 0.0f, 0.0f, 0.0f, (float)sin(theta), 0.0f);
            glColor3f(1.0,1.0, 1.0);

	    glBegin(GL_TRIANGLES);
	    for(unsigned int i = 0; i < surface.size();i++)
	    {            
		      Triangle t = surface.at(i);
		      glNormal3fv(t.n1);
		      glVertex3fv(t.p1);
		      glNormal3fv(t.n2);
		      glVertex3fv(t.p2);
		      glNormal3fv(t.n3);
		      glVertex3fv(t.p3);
	
	    }
	    glEnd();
	    glutSwapBuffers();
   }
    

   void keyPress(unsigned char key, int x, int y){

        if ( keyboardInteraction == NULL) return;
        keyboardInteraction(key,x,y);
        display();
    }

    
    void otherkeys(int key, int x, int y){
         switch(key){
               case GLUT_KEY_LEFT:theta+=0.1f; break;
               case GLUT_KEY_UP: phi+=0.1f; break;
               case GLUT_KEY_RIGHT:theta-=0.1f; break;
               case GLUT_KEY_DOWN: phi-=0.1f; break;
	       case GLUT_KEY_PAGE_UP:  rho-=0.3f; break;
	       case GLUT_KEY_PAGE_DOWN:  rho+=0.3f; break;
         }
         
         // Keep the spherical coordinates in the 0 < x < 2*pi range
         if ( rho < 0.1 ) rho = 0.1; 
         if ( theta > 2*3.14159 ) theta -= 2*3.14159;
         if ( theta < 0 ) theta += 2*3.14159;
         if ( phi > 2*3.14159 ) phi -= 2*3.14159;
         if ( phi < 0 ) phi += 2*3.14159;

         display();
    }

  void reshape(int w, int h)
  {
       glViewport(0, 0, (GLsizei) w, (GLsizei) h);
       glMatrixMode(GL_PROJECTION);
       glLoadIdentity();
       gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 30.0);
       glMatrixMode(GL_MODELVIEW);
       glLoadIdentity();
   }
   
   void SetSurface(std::vector<Triangle>* marchingCubesResult){
      surface.clear();
      for(unsigned int i = 0; i < marchingCubesResult->size(); i++)
           surface.push_back( marchingCubesResult->at(i));
      display();
   }

   void Redraw(){
      display();
   }
   
   void Run(){
      glClearColor (0.7, 0.7, 0.7, 0.0);
      glShadeModel(GL_SMOOTH);
      glEnable(GL_DEPTH_TEST);
      glutDisplayFunc(display);
      glutReshapeFunc(reshape);
      glutKeyboardFunc(keyPress);
      glutSpecialFunc(otherkeys);
      glutMainLoop();
   }

}
