/**
 * @file project.cpp
 * @brief OpenGL project
 *
 * @author H. Q. Bovik (hqbovik)
 * @bug Unimplemented
 */

#include "p1/project.hpp"


// use this header to include the OpenGL headers
// DO NOT include gl.h or glu.h directly; it will not compile correctly.
#include "application/opengl.hpp"

// A namespace declaration. All proejct files use this namespace.
// Add this declration (and its closing) to all source/headers you create.
// Note that all #includes should be BEFORE the namespace declaration.
namespace _462 {

// definitions of functions for the OpenglProject class

// constructor, invoked when object is created
OpenglProject::OpenglProject()
{
    // TODO any basic construction or initialization of members
    // Warning: Although members' constructors are automatically called,
    // ints, floats, pointers, and classes with empty contructors all
    // will have uninitialized data!
}

// destructor, invoked when object is destroyed
OpenglProject::~OpenglProject()
{
    // TODO any final cleanup of members
    // Warning: Do not throw exceptions or call virtual functions from deconstructors!
    // They will cause undefined behavior (probably a crash, but perhaps worse).
}

/**
 * Initialize the project, doing any necessary opengl initialization.
 * @param camera An already-initialized camera.
 * @param scene The scene to render.
 * @return true on success, false on error.
 */
bool OpenglProject::initialize( Camera* camera, Scene* scene )
{
    // copy scene
    this->scene = *scene;

    // TODO opengl initialization code and precomputation of mesh/heightmap

    // Compute the normals
    // first zero out
    for ( size_t i = 0; i < (this->scene).mesh.num_vertices; ++i ) {
      (this->scene).mesh.normals[i] = Vector3::Zero;
    }

    // then sum in all triangle normals
    for ( size_t i = 0; i < (this->scene).mesh.num_triangles; ++i ) {
      Vector3 pos[3];
      for ( size_t j = 0; j < 3; ++j ) {
        pos[j] = (this->scene).mesh.vertices[(this->scene).mesh.triangles[i].vertices[j]];
      }
      Vector3 normal = normalize( cross( pos[1] - pos[0], pos[2] - pos[0] ) );
      for ( size_t j = 0; j < 3; ++j ) {
        (this->scene).mesh.normals[(this->scene).mesh.triangles[i].vertices[j]] += normal;
      }
    }

    // then normalize
    for ( size_t i = 0; i < (this->scene).mesh.num_vertices; ++i ) {
      (this->scene).mesh.normals[i] = normalize( (this->scene).mesh.normals[i] );
    }

    // Let there be light
    GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f};
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat specref[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CCW);
    glEnable(GL_CULL_FACE);

    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glEnable(GL_LIGHT0);

    glEnable(GL_COLOR_MATERIAL);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
    glMateriali(GL_FRONT, GL_SHININESS, 128);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glEnable(GL_NORMALIZE);
    glEnable(GL_RESCALE_NORMAL);

    // Compute the heightmap
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glColor3f(0.75f, 0.75f, 0.75f);

    // specify where to get vertex data
    glVertexPointer(3, GL_DOUBLE, 0, (this->scene).mesh.vertices);
    glNormalPointer(GL_DOUBLE, 0, (this->scene).mesh.normals);

    return true;
}

/**
 * Clean up the project. Free any memory, etc.
 */
void OpenglProject::destroy()
{
    // TODO any cleanup code, e.g., freeing memory
}

/**
 * Perform an update step. This happens on a regular interval.
 * @param dt The time difference from the previous frame to the current.
 */
void OpenglProject::update( real_t dt )
{
    // update our heightmap
    scene.heightmap->update( dt );

    // TODO any update code, e.g. commputing heightmap mesh positions and normals
}

/**
 * Clear the screen, then render the mesh using the given camera.
 * @param camera The logical camera to use.
 * @see math/camera.hpp
 */
void OpenglProject::render( const Camera* camera )
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO render code
    glDrawElements(GL_TRIANGLES, 3*(this->scene).mesh.num_triangles, GL_UNSIGNED_INT,
    (unsigned int *) &((this->scene).mesh.triangles[0]));

    glMatrixMode(GL_PROJECTION);
    // set current matrix
    glLoadIdentity();
    gluPerspective(camera->get_fov_degrees(),
                   camera->get_aspect_ratio(),
                   camera->get_near_clip(),
                   camera->get_far_clip());

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float eye[3], center[3], up[3];
    camera->get_position().to_array(eye);
    (camera->get_direction()-camera->get_position()).to_array(center);
    camera->get_up().to_array(up);

    gluLookAt(eye[0], eye[1], eye[2],
              center[0], center[1], center[2],
              up[0], up[1], up[2]);
}

} /* _462 */
