#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "GL/glu.h"

void cube()
{
    glBegin(GL_QUADS);      // Draw The Cube Using quads

    glColor3f(0.0f,1.0f,0.0f);  // Color Green
    glNormal3f(0.f, 1.f, 0.f);
    glVertex3f( 1.0f, 1.0f,-1.0f);  // Top Right Of The Quad (Top)
    glNormal3f(0.f, 1.f, 0.f);
    glVertex3f(-1.0f, 1.0f,-1.0f);  // Top Left Of The Quad (Top)
    glNormal3f(0.f, 1.f, 0.f);
    glVertex3f(-1.0f, 1.0f, 1.0f);  // Bottom Left Of The Quad (Top)
    glNormal3f(0.f, 1.f, 0.f);
    glVertex3f( 1.0f, 1.0f, 1.0f);  // Bottom Right Of The Quad (Top)

    glColor3f(1.0f,0.5f,0.0f);  // Color Orange
    glNormal3f(0.f, -1.f, 0.f);
    glVertex3f( 1.0f,-1.0f, 1.0f);  // Top Right Of The Quad (Bottom)
    glNormal3f(0.f, -1.f, 0.f);
    glVertex3f(-1.0f,-1.0f, 1.0f);  // Top Left Of The Quad (Bottom)
    glNormal3f(0.f, -1.f, 0.f);
    glVertex3f(-1.0f,-1.0f,-1.0f);  // Bottom Left Of The Quad (Bottom)
    glNormal3f(0.f, -1.f, 0.f);
    glVertex3f( 1.0f,-1.0f,-1.0f);  // Bottom Right Of The Quad (Bottom)

    glColor3f(1.0f, 0.0f, 0.0f);    // Color Red
    glNormal3f(0.f, 0.f, 1.f);
    glVertex3f( 1.0f, 1.0f, 1.0f);  // Top Right Of The Quad (Front)
    glNormal3f(0.f, 0.f, 1.f);
    glVertex3f(-1.0f, 1.0f, 1.0f);  // Top Left Of The Quad (Front)
    glNormal3f(0.f, 0.f, 1.f);
    glVertex3f(-1.0f,-1.0f, 1.0f);  // Bottom Left Of The Quad (Front)
    glNormal3f(0.f, 0.f, 1.f);
    glVertex3f( 1.0f,-1.0f, 1.0f);  // Bottom Right Of The Quad (Front)

    glColor3f(1.0f, 1.0f, 0.0f);    // Color Yellow
    glNormal3f(0.f, 0.f, -1.f);
    glVertex3f( 1.0f,-1.0f,-1.0f);  // Top Right Of The Quad (Back)
    glNormal3f(0.f, 0.f, -1.f);
    glVertex3f(-1.0f,-1.0f,-1.0f);  // Top Left Of The Quad (Back)
    glNormal3f(0.f, 0.f, -1.f);
    glVertex3f(-1.0f, 1.0f,-1.0f);  // Bottom Left Of The Quad (Back)
    glNormal3f(0.f, 0.f, -1.f);
    glVertex3f( 1.0f, 1.0f,-1.0f);  // Bottom Right Of The Quad (Back)

    glColor3f(0.0f, 0.0f, 1.0f);    // Color Blue
    glNormal3f(-1.f, 0.f, 0.f);
    glVertex3f(-1.0f, 1.0f, 1.0f);  // Top Right Of The Quad (Left)
    glNormal3f(-1.f, 0.f, 0.f);
    glVertex3f(-1.0f, 1.0f,-1.0f);  // Top Left Of The Quad (Left)
    glNormal3f(-1.f, 0.f, 0.f);
    glVertex3f(-1.0f,-1.0f,-1.0f);  // Bottom Left Of The Quad (Left)
    glNormal3f(-1.f, 0.f, 0.f);
    glVertex3f(-1.0f,-1.0f, 1.0f);  // Bottom Right Of The Quad (Left)

    glColor3f(1.0f, 0.0f, 1.0f);    // Color Violet
    glNormal3f(1.f, 0.f, 0.f);
    glVertex3f( 1.0f, 1.0f,-1.0f);  // Top Right Of The Quad (Right)
    glNormal3f(1.f, 0.f, 0.f);
    glVertex3f( 1.0f, 1.0f, 1.0f);  // Top Left Of The Quad (Right)
    glNormal3f(1.f, 0.f, 0.f);
    glVertex3f( 1.0f,-1.0f, 1.0f);  // Bottom Left Of The Quad (Right)
    glNormal3f(1.f, 0.f, 0.f);
    glVertex3f( 1.0f,-1.0f,-1.0f);  // Bottom Right Of The Quad (Right)

    glEnd();          // End Drawing The Cube
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!", sf::Style::Default);
    window.setVerticalSyncEnabled(true);
    window.setActive(true);
    window.resetGLStates();
    //window.PreserveOpenGLStates();
 
    sf::Font font;
    if (!font.loadFromFile("sansation.ttf")) {
      // error...
    }
    sf::Text text;
    sf::Text text2;
    // select the font
    //text.setFont(font); // font is a sf::Font
    //text2.setFont(font); // font is a sf::Font
    // set the string to display
    text.setString("Hello world");
    text2.setString("DERP");
    // set the character size
    text.setCharacterSize(100); // in pixels, not points!
    text2.setCharacterSize(100); // in pixels, not points!
    // set the color
    text.setFillColor(sf::Color::Red);
    text2.setFillColor(sf::Color::Blue);
    // set the text style
    text.setStyle(sf::Text::Bold | sf::Text::Underlined);
    text2.setStyle(sf::Text::Bold | sf::Text::Underlined);

    bool running = true;
    while(running)
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            switch(event.type)
            {
            case sf::Event::KeyPressed:
                if(event.key.code != sf::Keyboard::Escape) { break; }
            case sf::Event::Closed:
                running = false;
                break;
            default:
                break;
            }
        }

        glClearColor(0.3f, 0.3f, 0.3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        sf::Vector2u wsize = window.getSize();

        glViewport(0, 0, wsize.x, wsize.y);
        gluPerspective(60, (float)wsize.x / (float)wsize.y, 0.1f, 512.f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glPushMatrix();

        glTranslatef(0.f, 0.f, -5.f);

        cube();

        glPopMatrix();

        glViewport(0, 0, wsize.x/3, wsize.y/2);
        gluPerspective(60, (float)wsize.x / (float)wsize.y, 0.1f, 512.f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glPushMatrix();

        cube();

        glPopMatrix();

        window.pushGLStates();

        window.draw(text);
        window.draw(text2);

        window.popGLStates();


        GLenum err = glGetError();

        window.display();
    }

    return 0;
}
