#include<stdio.h>
#include<stdlib.h>
#include<X11/X.h>
#include<X11/Xlib.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

#include "TX11GL.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TVirtualGL.h"
//#include "TX11GL.h"
#include "TCanvas.h"
#include "TH3F.h"
#include "TH1F.h"
#include "GL/glu.h"

Display                 *dpy;
Window                  root;
GLint                   att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
XVisualInfo             *vi;
Colormap                cmap;
XSetWindowAttributes    swa;
Window                  win;
GLXContext              glc;
XWindowAttributes       gwa;
XEvent                  xev;

void DrawAQuad() {
 glClearColor(1.0, 1.0, 1.0, 1.0);
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 glOrtho(-1., 1., -1., 1., 1., 20.);

 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
 gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);

 glBegin(GL_QUADS);
  glColor3f(1., 0., 0.); glVertex3f(-.75, -.75, 0.);
  glColor3f(0., 1., 0.); glVertex3f( .75, -.75, 0.);
  glColor3f(0., 0., 1.); glVertex3f( .75,  .75, 0.);
  glColor3f(1., 1., 0.); glVertex3f(-.75,  .75, 0.);
 glEnd();

 glBegin(GL_TRIANGLES);
 glVertex3f(-0.7, 0.7, 0);
 glVertex3f(0.7, 0.7, 0);
 glVertex3f(0, -1, 0);
 glEnd();
} 

int main(int argc, char *argv[]) {


  auto gMyRootApp = new TApplication("My ROOT Application", &argc, argv);
  gMyRootApp->SetReturnFromRun(true);
  gStyle->SetCanvasPreferGL(kTRUE) ;

 dpy = XOpenDisplay(NULL);
 
 if(dpy == NULL) {
        printf("\n\tcannot connect to X server\n\n");
        exit(0);
 }
        
 root = DefaultRootWindow(dpy);

 vi = glXChooseVisual(dpy, 0, att);

 if(vi == NULL) {
        printf("\n\tno appropriate visual found\n\n");
        exit(0);
 } 
 else {
        printf("\n\tvisual %p selected\n", (void *)vi->visualid); /* %p creates hexadecimal output like in glxinfo */
 }


 cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);

 swa.colormap = cmap;
 swa.event_mask = ExposureMask | KeyPressMask;
 
 win = XCreateWindow(dpy, root, 0, 0, 600, 600, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);

  new TX11GLManager();
  //int wid = gVirtualX->AddWindow((ULong_t)handle, 300, 300);
  int wid = gGLManager->InitGLWindow(win);
  gStyle->SetCanvasColor(3);
  auto fCanvas = new TCanvas("glCanvas", 200, 200, wid);
  //fCanvas->Divide(2,2);
  fCanvas->SetFillStyle(3001);
  fCanvas->SetFillColor(4);
  fCanvas->SetFrameFillColor(4);
  TH1F *h1 = new TH1F("h1", "h11", 10, -1, 1);
  h1->FillRandom("gaus");
  h1->SetFillColor(2);
  fCanvas->SetFillColorAlpha(0,0.4);
  fCanvas->SetFrameFillColor(0);
  fCanvas->cd();
  h1->Draw("hist");

 XMapWindow(dpy, win);
 XStoreName(dpy, win, "VERY SIMPLE APPLICATION");
 
 glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
 glXMakeCurrent(dpy, win, glc);
 
 glEnable(GL_DEPTH_TEST); 
 
 while(1) {
        XNextEvent(dpy, &xev);
        
        if(xev.type == Expose) {
                XGetWindowAttributes(dpy, win, &gwa);
                glViewport(0, 0, gwa.width, gwa.height);
                DrawAQuad(); 
                glXSwapBuffers(dpy, win);
        }
                
        else if(xev.type == KeyPress) {
                glXMakeCurrent(dpy, None, NULL);
                glXDestroyContext(dpy, glc);
                XDestroyWindow(dpy, win);
                XCloseDisplay(dpy);
                exit(0);
        }

    if(fCanvas) fCanvas->Update();
    } /* this closes while(1) { */
} /* this is the } which closes int main(int argc, char *argv[]) { */
