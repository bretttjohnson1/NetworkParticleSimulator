#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <sys/time.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>

float offx=1.5f;
float offy=0.0f;
float offz=-6.0f;
char keys[256];
float phi=0;
float thet=0;
int window;
long numparticles = 0;
int sockfd = 0;
int xsize = 1000;int ysize = 1000;int zsize = 1000;
double *buffer;

void fail(){
   printf("Error %s \n",strerror(errno));
   exit(1);
}

void moveandrotate(){
  if(keys[32]){
    offy-=.05+keys[(int)'m'];
  }
  if(keys[(int)'c'])
    offy+=.05+keys[(int)'m'];
  if(keys[(int)'w']){
    offz+=.05+keys[(int)'m'];
  }
  if(keys[(int)'s']){
    offz-=.05+keys[(int)'m'];
  }
  if(keys[(int)'d']){
    offx-=.05+keys[(int)'m'];
  }
  if(keys[(int)'a']){
    offx+=.05+keys[(int)'m'];
  }
  if(keys[(int)'j'])
    thet+=1;
  if(keys[(int)'l'])
    thet+=-1;
  if(keys[(int)'i'])
    phi+=1;
  if(keys[(int)'k'])
    phi-=1;
}
void client_init(int argc, char **argv){
   if(argc>1){
      if(strcmp(argv[1],"-h")==0){
         printf("client usage\n./client <server_adress> <port>\n\nTo stream an mp3 file run the server as ./server file.mp3\nthen run the client as ./client <server_adress> <port> | mpg123 -\n");
         exit(0);
      }
   }

   if(argc !=3){
      printf("missing args\nRun ./client -h for help\n");
      exit(1);
   }
   char *address = argv[1];
   char *port = argv[2];

   int status;
   struct addrinfo hints;
   struct addrinfo *serverinfo;
   memset(&hints,0,sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE;
   status = getaddrinfo(address, port, &hints,&serverinfo);

   sockfd = socket(serverinfo->ai_family,serverinfo->ai_socktype,serverinfo->ai_protocol);
   int success = connect(sockfd,serverinfo->ai_addr,serverinfo->ai_addrlen);
   if(success!=0){
      fail();
   }

   success = recv(sockfd,&numparticles,4,0);
   if(success==-1){
      fail();
   }
   /*success = recv(sockfd,&xsize,sizeof(int),0);
   if(success==-1){
      fail();
   }
   success = recv(sockfd,&ysize,sizeof(int),0);
   if(success==-1){
      fail();
   }
   success = recv(sockfd,&zsize,sizeof(int),0);
   if(success==-1){
      fail();
   }*/
   printf("%d\n",ysize );
   buffer = (double *)malloc(numparticles*3*sizeof(double));

}

void init(int width, int height){
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(1.0);
  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (GLfloat)width/(GLfloat)height,0.1f,100.0f);
  glMatrixMode(GL_MODELVIEW);
}

void draw(){
   struct timeval begin,end;
   gettimeofday(&begin, NULL);
   for(long a = 0;a<numparticles*3;a+=3){
      recv(sockfd,&buffer[a],sizeof(double),0);
      recv(sockfd,&buffer[a+1],sizeof(double),0);
      recv(sockfd,&buffer[a+2],sizeof(double),0);
   }
   gettimeofday(&end,NULL);
   //printf("data recieved in %lu ms\n",end.tv_usec-begin.tv_usec);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();
   moveandrotate();

   //struct timeval stop, start;
   //gettimeofday(&start, NULL);
   glTranslatef(offx,offy,offz);
   glScalef(.01/(pow(xsize*ysize*zsize,1/3)+1),.01/(pow(xsize*ysize*zsize,1/3)+1),.01/(pow(xsize*ysize*zsize,1/3)+1));
   glRotatef(thet,0,1,0);
   glColor3f(0,255,0);

   gettimeofday(&begin, NULL);
   for(long a = 0;a<numparticles*3;a+=3){
      glBegin(GL_POINTS);
      glVertex3f(buffer[a]-xsize/2,buffer[a+1]-ysize/2,buffer[a+2]-zsize/2);
      glEnd();
    }
    gettimeofday(&end,NULL);
   //printf("points drawn in %lu ms\n",end.tv_usec-begin.tv_usec);


   // gettimeofday(&stop,NULL);


  ///gettimeofday(&stop,NULL);
  glutSwapBuffers();

}
void resize(int width,int height){
  if(height==0)
    height=1;
  glViewport(0,0,width,height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (GLfloat)width/(GLfloat)height,0.1f,100.0f);
  glMatrixMode(GL_MODELVIEW);

}
void keyPressed(unsigned char key, int x, int y){
  keys[key]=1;
  usleep(100);
  if(key == 27){
    glutDestroyWindow(window);
    exit(0);
  }

}
void keyUp(unsigned char key, int x, int y){
  keys[key]=0;
}

int main(int argc, char **argv){
   int a = 1;
   printf("Hello\n");
   fflush(stdout);
   glutInit(&a,argv);
   glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
   glutInitWindowSize(640,480);
   window = glutCreateWindow("penor XDDDD");
   glutDisplayFunc(&draw);
   glutIdleFunc(&draw);
   glutFullScreen();
   glutReshapeFunc(&resize);
   glutKeyboardFunc(&keyPressed);
   glutKeyboardUpFunc(&keyUp);
   init(640,480);
   client_init(argc,argv);
   glutMainLoop();
}
