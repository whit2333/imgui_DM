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


int main(int argc, char* argv[]) {
  cout << "Version " << IMGUIDM_VERSION_MAJOR << "." << IMGUIDM_VERSION_MINOR << endl;


  bool sRgb = false;

  auto gMyRootApp = new TApplication("My ROOT Application", &argc, argv);
  gMyRootApp->SetReturnFromRun(true);

  // Request a 24-bits depth buffer when creating the window
  sf::ContextSettings contextSettings;
  contextSettings.depthBits = 24;
  contextSettings.sRgbCapable = sRgb;

  // Create the main window
  sf::RenderWindow window(sf::VideoMode(800, 600), "SFML graphics with OpenGL", sf::Style::Default, contextSettings);
  window.setVerticalSyncEnabled(true);
  //sf::RenderWindow window(sf::VideoMode(800, 600), "IMGUIDM");

  //glEnable(GL_TEXTURE_2D);
  
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
  //
  // Add this window to TVirtualX list of windows
  // in return get its TVirtualX identifier
  int wid = gVirtualX->AddWindow((ULong_t)handle, 500, 500);
  //int wid = gVirtualX->AddWindow((ULong_t)handle, 500, 500);

  // Create a new TCanvas, specifying this window's id as parent
  auto fCanvas = new TCanvas("glCanvas", 500, 500, wid);
  TH3F *h31 = new TH3F("h31", "h31", 10, -1, 1, 10, -1, 1, 10, -1, 1);
  h31->FillRandom("gaus");
  TH1F *h1 = new TH1F("h1", "h11", 10, -1, 1);
  h1->FillRandom("gaus");
  h1->SetFillColor(2);
  fCanvas->cd();
  //h1->Draw("hist");
  h31->Draw("box");

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

    // clear the window with black color
    window.clear(sf::Color::Black);

    // inside the main loop, between window.clear() and window.display()
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    sf::Text text;

    // select the font
    text.setFont(font); // font is a sf::Font

    // set the string to display
    text.setString("Hello world");

    // set the character size
    text.setCharacterSize(100); // in pixels, not points!

    // set the color
    text.setFillColor(sf::Color::Red);

    // set the text style
    text.setStyle(sf::Text::Bold | sf::Text::Underlined);


    //gApplication->StartIdleing();
    //gSystem->InnerLoop();
    //gApplication->StopIdleing();

    if (fCanvas) fCanvas->Update();
    window.draw(text);

    std::cout << " asdf\n";
    window.display();

  }
}
