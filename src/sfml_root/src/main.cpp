#include "config.h"
#include <iostream>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
using namespace std;

#ifndef GL_SRGB8_ALPHA8
#define GL_SRGB8_ALPHA8 0x8C43
#endif

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

void draw_box(sf::Window& window, float elapsedTime)
{
  // Activate the window
  //window.setActive();

  // Clear color and depth buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Apply some transformations
  //glMatrixMode(GL_MODELVIEW);
  //glLoadIdentity();
  //glTranslatef(0.f, 0.f, -200.f);
  //glRotatef(elapsedTime * 10.f, 1.f, 0.f, 0.f);
  //glRotatef(elapsedTime * 6.f, 0.f, 1.f, 0.f);
  //glRotatef(elapsedTime * 18.f, 0.f, 0.f, 1.f);

  // Define a 3D cube (6 faces made of 2 triangles composed by 3 vertices)
  static const GLfloat cube[] =
  {
    // positions    // colors
    -50, -50, -50,  1, 1, 0,
    -50,  50, -50,  1, 1, 0,
    -50, -50,  50,  1, 1, 0,
    -50, -50,  50,  1, 1, 0,
    -50,  50, -50,  1, 1, 0,
    -50,  50,  50,  1, 1, 0,

    50, -50, -50,  1, 1, 0,
    50,  50, -50,  1, 1, 0,
    50, -50,  50,  1, 1, 0,
    50, -50,  50,  1, 1, 0,
    50,  50, -50,  1, 1, 0,
    50,  50,  50,  1, 1, 0,

    -50, -50, -50,  1, 0, 1,
    50, -50, -50,  1, 0, 1,
    -50, -50,  50,  1, 0, 1,
    -50, -50,  50,  1, 0, 1,
    50, -50, -50,  1, 0, 1,
    50, -50,  50,  1, 0, 1,

    -50,  50, -50,  1, 0, 1,
    50,  50, -50,  1, 0, 1,
    -50,  50,  50,  1, 0, 1,
    -50,  50,  50,  1, 0, 1,
    50,  50, -50,  1, 0, 1,
    50,  50,  50,  1, 0, 1,

    -50, -50, -50,  0, 1, 1,
    50, -50, -50,  0, 1, 1,
    -50,  50, -50,  0, 1, 1,
    -50,  50, -50,  0, 1, 1,
    50, -50, -50,  0, 1, 1,
    50,  50, -50,  0, 1, 1,

    -50, -50,  50,  0, 1, 1,
    50, -50,  50,  0, 1, 1,
    -50,  50,  50,  0, 1, 1,
    -50,  50,  50,  0, 1, 1,
    50, -50,  50,  0, 1, 1,
    50,  50,  50,  0, 1, 1
  };

  // Draw the cube
  glVertexPointer(3, GL_FLOAT, 6 * sizeof(GLfloat), cube);
  glColorPointer(3, GL_FLOAT, 6 * sizeof(GLfloat), cube + 3);
  glDrawArrays(GL_TRIANGLES, 0, 36);
}

int main(int argc, char* argv[])
{
  cout << "Version " << IMGUIDM_VERSION_MAJOR << "." << IMGUIDM_VERSION_MINOR << endl;
  bool sRgb = false;


  auto gMyRootApp = new TApplication("My ROOT Application", &argc, argv);
  gMyRootApp->SetReturnFromRun(true);
  gStyle->SetCanvasPreferGL(kTRUE) ;

  // Request a 24-bits depth buffer when creating the window
  sf::ContextSettings contextSettings;
  contextSettings.depthBits = 24;
  contextSettings.stencilBits = 8;
  contextSettings.antialiasingLevel = 4;
  contextSettings.majorVersion = 2;
  contextSettings.minorVersion = 3;

  // Create the main window
  sf::RenderWindow window(sf::VideoMode(800, 600), "SFML graphics with OpenGL", sf::Style::Default, contextSettings);
  window.setVerticalSyncEnabled(true);
  window.setActive(true);

  auto handle = window.getSystemHandle();
  std::cout << " handle " << handle << std::endl;

  //auto glmgr = TGLManager::Instance();
  //auto cntx = glmgr->CreateGLContext(handle);


  //if (glXQueryExtension(, NULL, NULL)) {
  //  const char *glx_ext = glXGetClientString(display, GLX_EXTENSIONS);
  //  std::cout << glx_ext << "\n";
  //  if (... check_string_for_extension(glx_ext, "GLX_SOME_EXTENSION")) {
  //    printf("We have the extension!\n");
  //  }
  //}

  new TX11GLManager();
  //// run the program as long as the window is open
  //// Add this window to TVirtualX list of windows
  //// in return get its TVirtualX identifier
  //int wid = gVirtualX->AddWindow((ULong_t)handle, 300, 300);
  int wid = gGLManager->InitGLWindow(handle);
  //fCanvas = new TCanvas("Root Canvas", width(), height(), wid2);
  gStyle->SetCanvasColor(3);
  auto fCanvas = new TCanvas("glCanvas", 200, 200, wid);
  //fCanvas->EmbedInto(wid,100,100);
  //fCanvas->Divide(2,2);
  std::cout << "wid " << wid <<"\n";
  fCanvas->SetFillStyle(3001);
  fCanvas->SetFillColor(4);
  fCanvas->SetFrameFillColor(4);

  //// Create a new TCanvas, specifying this window's id as parent
  //std::cout << "test1 \n";
  //
  //TH3F *h31 = new TH3F("h31", "h31", 10, -1, 1, 10, -1, 1, 10, -1, 1);
  //h31->FillRandom("gaus");
  TH1F *h1 = new TH1F("h1", "h11", 10, -1, 1);
  h1->FillRandom("gaus");
  h1->SetFillColor(2);
  fCanvas->SetFillColorAlpha(0,0.4);
  fCanvas->SetFrameFillColor(0);
  fCanvas->cd();
  //h31->Draw("box");
  //fCanvas->cd(3);
  h1->Draw("hist");


  // -----------------------------
  sf::Font font;
  if (!font.loadFromFile("sansation.ttf")) {
    // error...
  }
  sf::Text text;
  sf::Text text2;
  // select the font
  text.setFont(font); // font is a sf::Font
  text2.setFont(font); // font is a sf::Font
  // set the string to display
  text.setString("Hello world");
  text2.setString("DERP");
  // set the character size
  text.setCharacterSize(50); // in pixels, not points!
  text2.setCharacterSize(50); // in pixels, not points!

  // set the color
  text.setFillColor(sf::Color::Red);
  text2.setFillColor(sf::Color::Blue);
  // set the text style
  text.setStyle(sf::Text::Bold | sf::Text::Underlined);
  text2.setStyle(sf::Text::Bold | sf::Text::Underlined);


  //glDisable(GL_DEPTH_TEST);
  //glDisable(GL_CULL_FACE);
  //glDisable(GL_LIGHTING);
  
  sf::View viewLeft(sf::FloatRect(0, 0, window.getSize().x/2, window.getSize().y));
  viewLeft.setViewport(sf::FloatRect(0, 0, 0.5, 1));

  sf::View viewRight(sf::FloatRect(0, 0, window.getSize().x/2, window.getSize().y));
  viewRight.setViewport(sf::FloatRect(0.5, 0, 0.5, 1));

  int w_it = 0;

  while (window.isOpen())
  {
    // check all the window's events that were triggered since the last iteration of the loop
    sf::Event event;
    while (window.pollEvent(event))
    {
      // "close requested" event: we close the window
      if (event.type == sf::Event::Closed)
        window.close();
    }

    window.clear(sf::Color::Black);
    if(fCanvas) fCanvas->Update();

    sf::Vector2u wsize = window.getSize();


    //window.resetGLStates();

    window.setView(viewLeft);
    window.draw(text);
    //window.setView(window.getDefaultView());
    
    window.setView(viewRight);
    window.draw(text2);

    //window.popGLStates();



    std::cout << w_it << std::endl;
    w_it++;

    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();

    //draw_box(window, 1.0);
    //std::cout << " asdf\n";
    //glMatrixMode(GL_PROJECTION);
    ////glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    //glViewport(10, 500, wsize.x/2, wsize.y/2);
    //gluPerspective(60, (float)wsize.x / (float)wsize.y, 0.1f, 512.f);
    //glOrtho(wsize.x/4, wsize.x/4, wsize.x/4, wsize.x/4, -10., 10.);

    //Clear the buffer
    //glViewport(0, 0, GLsizei(gPad->GetWw()), GLsizei(gPad->GetWh()));

    //glDepthMask(GL_TRUE);
    ////glClearColor(1.,1.,1.,1.);
    ////glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glDepthMask(GL_FALSE);
    //glMatrixMode(GL_PROJECTION);//[1
    
    //glMatrixMode(GL_MODELVIEW);//[2
    //glPushMatrix();


    //
    //glMatrixMode(GL_PROJECTION);//[1
    //glPopMatrix();
    //glMatrixMode(GL_MODELVIEW);//[2
    //glPopMatrix();

    //window.setView(viewLeft);
    //window.draw(text);

    //glFinish();

    ////window.setView(window.getDefaultView());
    //window.setView(viewRight);
    //window.draw(text2);
    //glFinish();
    
    //window.popGLStates();
 
    //jglLoadIdentity();
    //jglTranslated(0., 0., -1.);
    //jglViewport(wsize.x/3, wsize.y/3, wsize.x/3, wsize.y/3);
    //jgluPerspective(60, (float)wsize.x / (float)wsize.y, 0.1f, 512.f);

    // clear the window with black color
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    window.display();
    //gApplication->StartIdleing();
    //gSystem->InnerLoop();
    //gApplication->StopIdleing();

  }
}
