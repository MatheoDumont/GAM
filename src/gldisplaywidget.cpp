#include "gldisplaywidget.h"

#ifdef __APPLE__
#include <glu.h>
#else
#include <GL/glu.h>
#endif

#include "QDebug"

GLDisplayWidget::GLDisplayWidget(QWidget *parent) : QGLWidget(parent), _X(0), _Y(0), _Z(0), _type_display(2)
{
    // Update the scene
    connect(&_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    _timer.start(16); // Starts or restarts the timer with a timeout interval of 16 milliseconds.

    file.open("data/franke4.off");
    std::string entete;
    file >> entete;

    int vertices, faces, bidon;
    file >> vertices >> faces >> bidon;
}

void GLDisplayWidget::initializeGL()
{
    // background color
    glClearColor(0.2, 0.2, 0.2, 1);

    // Shader
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    //** TP : To add....
    // Construction of the GeometricWorld before it is displayed
    // It can also be constructed following a signal (button)
}

void GLDisplayWidget::paintGL()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Center the camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0); //gluLookAt(eye, center, up)  //deprecated
                                          // Returns a 4x4 matrix that transforms world coordinates to eye coordinates.
    // Translation
    glTranslated(_X, _Y, _Z);

    // Rotation
    glRotatef(_xangle, 0.0f, 1.0f, 0.0f);
    glRotatef(_yangle, 1.0f, 0.0f, 0.0f);

    // if (_dir == 1)
    // {
    //     glRotatef(_angle, 0.0f, 0.0f, 1.0f);
    // }
    // else
    //     glRotatef(_angle, 1.0f, 0.0f, 0.0f);

    // Color for your _geomWorld
    glColor3f(0, 1, 0);

    // example with a tetraedre
    if (_type_display == 0)
        _geomWorld.draw();
    else if (_type_display == 1)
        _geomWorld.drawWireFrame();
    else if (_type_display == 2)
        _geomWorld.drawVoronoi();
}

void GLDisplayWidget::resizeGL(int width, int height)
{

    glViewport(0, 0, width, height); //Viewport in the world window
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

    updateGL();
}

// - - - - - - - - - - - - Mouse Management  - - - - - - - - - - - - - - - -
// When you click, the position of your mouse is saved
void GLDisplayWidget::mousePressEvent(QMouseEvent *event)
{
    if (event != NULL)
        _lastPosMouse = event->pos();

    if (event->button() == Qt::RightButton)
    {
        double v1, v2, v3;
        file >> v1 >> v2 >> v3;
        Point p(v1, v2, 0.);
        std::cout << p << std::endl;
        _geomWorld._mesh.add_delaunay_point(p);

        // float x = _lastPosMouse.x() - geometry().width() / 2.0f;
        // float y = (geometry().height() - _lastPosMouse.y()) - geometry().height() / 2.0f;
        // _geomWorld._mesh.add_delaunay_point(Point(0.25, 0.25, 0.f));

        // std::cout << x << ", " << y << std::endl;
        // _geomWorld._mesh.add_delaunay_point(Point(y, x, 0.f));
    }
}

// Mouse movement management
void GLDisplayWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - _lastPosMouse.x();
    int dy = event->y() - _lastPosMouse.y();

    if (event != NULL)
    {
        _xangle += dx;
        _yangle += dy;
        // if (dx > dy)
        // {
        //     _angle += dy;
        //     _dir = 1;
        // }
        // else
        // {
        //     _angle += dx;
        //     _dir = 2;
        // }

        _lastPosMouse = event->pos();

        updateGL();
    }
}

// Mouse Management for the zoom
void GLDisplayWidget::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta();
    double stepZoom = 0.25;
    if (!numDegrees.isNull())
    {
        _Z = (numDegrees.x() > 0 || numDegrees.y() > 0) ? _Z + stepZoom : _Z - stepZoom;
    }
}

void GLDisplayWidget::keyPressEvent(QKeyEvent *event)
{
    int key = static_cast<QKeyEvent *>(event)->key();
    qDebug("The Key is : %d", key);

    switch (static_cast<QKeyEvent *>(event)->key())
    {
    case Qt::Key_Enter:
        this->_type_display = (this->_type_display + 1) % 1;
        break;
    }
}
