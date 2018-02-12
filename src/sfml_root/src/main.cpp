#include "config.h"
#include <iostream>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
using namespace std;

#ifndef GL_SRGB8_ALPHA8
#define GL_SRGB8_ALPHA8 0x8C43
#endif

#include "TApplication.h"
#include "TSystem.h"
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
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.f, 0.f, -200.f);
    glRotatef(elapsedTime * 10.f, 1.f, 0.f, 0.f);
    glRotatef(elapsedTime * 6.f, 0.f, 1.f, 0.f);
    glRotatef(elapsedTime * 18.f, 0.f, 0.f, 1.f);

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

int main(int argc, char* argv[]) {

  cout << "Version " << IMGUIDM_VERSION_MAJOR << "." << IMGUIDM_VERSION_MINOR << endl;

  bool sRgb = false;
  auto gMyRootApp = new TApplication("My ROOT Application", &argc, argv);
  gMyRootApp->SetReturnFromRun(true);

  // Request a 24-bits depth buffer when creating the window
  sf::ContextSettings contextSettings;
  contextSettings.depthBits = 24;
  contextSettings.sRgbCapable = sRgb;
  contextSettings.majorVersion = 2;
  contextSettings.minorVersion = 0;

  // Create the main window
  sf::RenderWindow window(sf::VideoMode(800, 600), "SFML graphics with OpenGL", sf::Style::Default, contextSettings);
  window.setVerticalSyncEnabled(true);
  //sf::RenderWindow window(sf::VideoMode(800, 600), "IMGUIDM");
  glEnable(GL_TEXTURE_2D);
  glMatrixMode(GL_PROJECTION);
  
  auto handle = window.getSystemHandle();
  std::cout << " handle " << handle << std::endl;
  //auto glmgr = TGLManager::Instance();
  //auto cntx = glmgr->CreateGLContext(handle);

  sf::Font font;
  if (!font.loadFromFile("sansation.ttf")) {
    // error...
  }
  window.setActive(true);

  // run the program as long as the window is open
  // Add this window to TVirtualX list of windows
  // in return get its TVirtualX identifier
  int wid = gVirtualX->AddWindow((ULong_t)handle, 300, 300);

  // Create a new TCanvas, specifying this window's id as parent
  auto fCanvas = new TCanvas("glCanvas", 300, 300, wid);
  //TH3F *h31 = new TH3F("h31", "h31", 10, -1, 1, 10, -1, 1, 10, -1, 1);
  //h31->FillRandom("gaus");
  TH1F *h1 = new TH1F("h1", "h11", 10, -1, 1);
  h1->FillRandom("gaus");
  h1->SetFillColor(2);
  fCanvas->SetFillColorAlpha(0,0.4);
  fCanvas->SetFrameFillColor(0);
  fCanvas->cd();
  h1->Draw("hist");
  //h31->Draw("box");

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

  //sf::View view1;
  //view1.reset(sf::FloatRect(200, 200, 300, 200));
  //view1.setViewport(sf::FloatRect(0.25f, 0.25, 0.5f, 0.5f));

  //sf::View view2;
  //view2.setCenter(sf::Vector2f(350, 300));
  //view2.setSize(sf::Vector2f(200, 200));

  sf::View viewLeft(sf::FloatRect(0, 0, window.getSize().x/2, window.getSize().y));
  viewLeft.setViewport(sf::FloatRect(0, 0, 0.5, 1));

  sf::View viewRight(sf::FloatRect(0, 0, window.getSize().x/2, window.getSize().y));
  viewRight.setViewport(sf::FloatRect(0.5, 0, 0.5, 1));

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
 
    sf::Vector2u wsize = window.getSize();

   // glMatrixMode(GL_PROJECTION);
   // glLoadIdentity();

   // glViewport(0, 0, wsize.x/3, wsize.y/2);
   // gluPerspective(60, (float)wsize.x / (float)wsize.y, 0.1f, 512.f);
   // glDisable(GL_DEPTH_TEST);
   // glDisable(GL_CULL_FACE);
   // glDisable(GL_LIGHTING);

   ////Clear the buffer
   ////glViewport(0, 0, GLsizei(gPad->GetWw()), GLsizei(gPad->GetWh()));

   ////glDepthMask(GL_TRUE);
   //////glClearColor(1.,1.,1.,1.);
   //////glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   ////glDepthMask(GL_FALSE);

   //glMatrixMode(GL_PROJECTION);
   //glLoadIdentity();
   ////glOrtho(gPad->GetX1(), gPad->GetX2(), gPad->GetY1(), gPad->GetY2(), -10., 10.);
   //glOrtho(wsize.x/4, wsize.x/4, wsize.x/4, wsize.x/4, -10., 10.);

   //glMatrixMode(GL_MODELVIEW);
   //glLoadIdentity();
   //glTranslated(0., 0., -1.);
   //glViewport(wsize.x/3, wsize.y/3, wsize.x/3, wsize.y/3);
   //gluPerspective(60, (float)wsize.x / (float)wsize.y, 0.1f, 512.f);
   //if(fCanvas) fCanvas->Update();
   
   //gApplication->StartIdleing();
   //gSystem->InnerLoop();
   //gApplication->StopIdleing();

    // clear the window with black color
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // inside the main loop, between window.clear() and window.display()
    //window.setView(view1);

    window.setView(viewLeft);
    window.draw(text);

    //window.setView(window.getDefaultView());
    window.setView(viewRight);
    window.draw(text2);

    //window.pushGLStates();
    //window.popGLStates();


   //draw_box(window, 1.0);
   //std::cout << " asdf\n";

   window.display();

  }
}
