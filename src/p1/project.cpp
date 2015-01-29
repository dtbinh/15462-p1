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

    // Initialize heightmap
    HEIGHTMAP_SIZE = 192;
    HEIGHTMAP_SPAN = (2.0 / (HEIGHTMAP_SIZE - 1));

    heightmapMesh.num_vertices = HEIGHTMAP_SIZE * HEIGHTMAP_SIZE;
    heightmapMesh.vertices = new Vector3 [heightmapMesh.num_vertices];
    heightmapMesh.num_triangles = (HEIGHTMAP_SIZE - 1) * (HEIGHTMAP_SIZE - 1) * 2;
    heightmapMesh.triangles = new Triangle [heightmapMesh.num_triangles];
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

    // Initialize VAO
    glGenVertexArraysAPPLE(NumVAOs, VAO);

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


    initHeightmap();

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
    //glEnableClientState(GL_NORMAL_ARRAY);

    glColor3f(0.75f, 0.75f, 0.75f);

    // specify where to get vertex data
    //glVertexPointer(3, GL_DOUBLE, 0, (this->scene).mesh.vertices);
    //glNormalPointer(GL_DOUBLE, 0, (this->scene).mesh.normals);

    glVertexPointer(3, GL_DOUBLE, 0, heightmapMesh.vertices);

    return true;
}

/**
 * Clean up the project. Free any memory, etc.
 */
void OpenglProject::destroy()
{
    // TODO any cleanup code, e.g., freeing memory
    delete [] heightmapMesh.vertices;
    delete [] heightmapMesh.triangles;
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
    //glDrawElements(GL_TRIANGLES, 3*(this->scene).mesh.num_triangles, GL_UNSIGNED_INT,
    //(unsigned int *) &((this->scene).mesh.triangles[0]));

    glDrawElements(GL_TRIANGLES, 3*heightmapMesh.num_triangles, GL_UNSIGNED_INT,
    (unsigned int *) &heightmapMesh.triangles[0]);

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

void OpenglProject::initHeightmap() {
  double height;
  double x = -1.0, y = -1.0;
  int idx = 0;

  for (int i=0; i < HEIGHTMAP_SIZE; i++) {
    y = -1.0;
    if (i == HEIGHTMAP_SIZE - 1)
      x = 1.0;
    for (int j=0; j < HEIGHTMAP_SIZE; j++) {
      if (j == HEIGHTMAP_SIZE - 1)
        y = 1.0;
      height = scene.heightmap->compute_height(Vector2(x, y));
      heightmapMesh.vertices[idx] = Vector3(x, height, y);
      y += HEIGHTMAP_SPAN;
      ++idx;
    }
    x += HEIGHTMAP_SPAN;
  }

  idx = 0;
  for (int i=0; i < HEIGHTMAP_SIZE - 1; i++) {
    for (int j=1; j < HEIGHTMAP_SIZE; j++) {
      heightmapMesh.triangles[idx].vertices[0] = j + HEIGHTMAP_SIZE * i;
      heightmapMesh.triangles[idx].vertices[1] = j - 1 + HEIGHTMAP_SIZE * i;
      heightmapMesh.triangles[idx].vertices[2] = j + HEIGHTMAP_SIZE * (i+1);
      ++idx;
      heightmapMesh.triangles[idx].vertices[0] = j - 1 + HEIGHTMAP_SIZE * i;
      heightmapMesh.triangles[idx].vertices[1] = j - 1 + HEIGHTMAP_SIZE * (i+1);
      heightmapMesh.triangles[idx].vertices[2] = j + HEIGHTMAP_SIZE * (i+1);
      ++idx;
    }
  }
}

void OpenglProject::initMeshBuffers() {
  // Should change this
  glBindVertexArrayAPPLE (VAO[Mesh]);
  glGenBuffers(NumVBOs, VBO[Mesh].buffers);
  glBindBuffer(GL_ARRAY_BUFFER, VBO[Mesh].buffers[Vertices]);
  glBufferData(GL_ARRAY_BUFFER,
    3 * sizeof(GL_DOUBLE) * (this->scene).mesh.num_vertices,
    (this->scene).mesh.vertices, GL_STATIC_DRAW);
  glVertexPointer(3, GL_DOUBLE, 0, 0);
  glEnableClientState(GL_VERTEX_ARRAY);

  glBindBuffer(GL_ARRAY_BUFFER, VBO[Mesh].buffers[Normals]);
  glBufferData(GL_ARRAY_BUFFER,
    3 * sizeof(GL_DOUBLE) * (this->scene).mesh.num_vertices,
    (this->scene).mesh.normals, GL_STATIC_DRAW);
  glVertexPointer(3, GL_DOUBLE, 0, 0);
  glEnableClientState(GL_NORMAL_ARRAY);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
    VBO[Mesh].buffers[Elements]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
    3 * sizeof(GL_UNSIGNED_INT) * (this->scene).mesh.num_triangles,
    (unsigned int *) &((this->scene).mesh.triangles[0]), GL_STATIC_DRAW);
}

} /* _462 */
