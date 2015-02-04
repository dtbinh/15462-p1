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
    HEIGHTMAP_SIZE = 128;
    HEIGHTMAP_SPAN = (2.0 / (HEIGHTMAP_SIZE - 1));

    heightmapMesh.num_vertices = HEIGHTMAP_SIZE * HEIGHTMAP_SIZE;
    heightmapMesh.vertices = new Vector3 [heightmapMesh.num_vertices];
    heightmapMesh.normals = new Vector3 [heightmapMesh.num_vertices];
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

    // Compute the normals
    initHeightmap();

    computeNormals(&(this->scene.mesh));
    computeNormals(&heightmapMesh);

    initLight();

    initMeshBuffers(&(this->scene.mesh), Mesh);
    initMeshBuffers(&heightmapMesh, Heightmap);

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
    delete [] heightmapMesh.normals;
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
    // bind then map the VBO
    computeHeight();
    computeNormals (&heightmapMesh);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[Heightmap].buffers[Vertices]);

    double* p = (double*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    // if the pointer is valid(mapped), update VBO
    if(p) {
        // modify buffer data
        for (unsigned int i=0; i < heightmapMesh.num_vertices; i++)
          for (int j=0; j < 3; j++) {
            *(p + (j + i * 3)) = heightmapMesh.vertices[i][j];
          }
        glUnmapBuffer(GL_ARRAY_BUFFER); // unmap it after use
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO[Heightmap].buffers[Normals]);

    p = (double*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    // if the pointer is valid(mapped), update VBO
    if(p) {
        // modify buffer data
        for (unsigned int i=0; i < heightmapMesh.num_vertices; i++)
          for (int j=0; j < 3; j++) {
            *(p + (j + i * 3)) = heightmapMesh.normals[i][j];
          }
        glUnmapBuffer(GL_ARRAY_BUFFER); // unmap it after use
    }
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
    // Draw pool mesh
    glPushMatrix();
    glColor3f(0.65f, 0.15f, 0.15f);

    // set material
    GLfloat specref1[] = { 0.05f, 0.05f, 0.05f, 0.5f };
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specref1);
    glMateriali(GL_FRONT, GL_SHININESS, 23);

    transform (&(this->scene.mesh_position));

    // bind buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO[Mesh].buffers[Vertices]);
    glVertexPointer(3, GL_DOUBLE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[Mesh].buffers[Normals]);
    glNormalPointer(GL_DOUBLE, 0, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[Mesh].buffers[Elements]);
    glDrawElements(GL_TRIANGLES, 3*(this->scene).mesh.num_triangles,
      GL_UNSIGNED_INT, 0);
    glPopMatrix();

    // Draw water
    glPushMatrix();
    glColor4f( 0.15f, 0.35f, 0.85f, 0.2f);

    // set material
    GLfloat specref2[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specref2);
    glMateriali(GL_FRONT, GL_SHININESS, 128);

    transform (&(this->scene.heightmap_position));

    // bind buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO[Heightmap].buffers[Vertices]);
    glVertexPointer(3, GL_DOUBLE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[Heightmap].buffers[Normals]);
    glNormalPointer(GL_DOUBLE, 0, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[Heightmap].buffers[Elements]);
    glDrawElements(GL_TRIANGLES, 3*heightmapMesh.num_triangles,
      GL_UNSIGNED_INT, 0);
    glPopMatrix();

    setCamera (camera);
}

void OpenglProject::computeHeight() {
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
}


void OpenglProject::initHeightmap() {
  computeHeight();
  int idx = 0;
  for (int i=0; i < HEIGHTMAP_SIZE - 1; i++) {
    for (int j=0; j < HEIGHTMAP_SIZE - 1; j++) {
      heightmapMesh.triangles[idx].vertices[0] = j + HEIGHTMAP_SIZE * i;
      heightmapMesh.triangles[idx].vertices[1] = j + 1 + HEIGHTMAP_SIZE * i;
      heightmapMesh.triangles[idx].vertices[2] = j + HEIGHTMAP_SIZE * (i+1);
      ++idx;
      heightmapMesh.triangles[idx].vertices[0] = j + 1 + HEIGHTMAP_SIZE * i;
      heightmapMesh.triangles[idx].vertices[1] = j + 1 + HEIGHTMAP_SIZE * (i+1);
      heightmapMesh.triangles[idx].vertices[2] = j + HEIGHTMAP_SIZE * (i+1);
      ++idx;
    }
  }
}

void OpenglProject::initLight () {
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
}

void OpenglProject::initMeshBuffers ( MeshData* mesh, unsigned int MeshIndex ) {
  glGenBuffers(NumMeshes, VBO[MeshIndex].buffers);
  glBindBuffer(GL_ARRAY_BUFFER, VBO[MeshIndex].buffers[Vertices]);
  glBufferData(GL_ARRAY_BUFFER,
    3 * sizeof(double) * mesh->num_vertices,
    mesh->vertices, GL_STATIC_DRAW);
  glEnableClientState(GL_VERTEX_ARRAY);

  glBindBuffer(GL_ARRAY_BUFFER, VBO[MeshIndex].buffers[Normals]);
  glBufferData(GL_ARRAY_BUFFER,
    3 * sizeof(double) * mesh->num_vertices,
    mesh->normals, GL_STATIC_DRAW);
  glEnableClientState(GL_NORMAL_ARRAY);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
    VBO[MeshIndex].buffers[Elements]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
    3 * sizeof(unsigned int) * mesh->num_triangles,
    (unsigned int *) &(mesh->triangles[0]), GL_STATIC_DRAW);
}

void OpenglProject::computeNormals ( MeshData* mesh ) {
  for ( size_t i = 0; i < mesh->num_vertices; ++i ) {
    mesh->normals[i] = Vector3::Zero;
  }

  // then sum in all triangle normals
  for ( size_t i = 0; i < mesh->num_triangles; ++i ) {
    Vector3 pos[3];
    for ( size_t j = 0; j < 3; ++j ) {
      pos[j] = mesh->vertices[mesh->triangles[i].vertices[j]];
    }
    Vector3 normal = normalize( cross( pos[1] - pos[0], pos[2] - pos[0] ) );
    for ( size_t j = 0; j < 3; ++j ) {
      mesh->normals[mesh->triangles[i].vertices[j]] += normal;
    }
  }

  // then normalize
  for ( size_t i = 0; i < mesh->num_vertices; ++i ) {
    mesh->normals[i] = normalize( mesh->normals[i] );
  }
}

void OpenglProject::setCamera ( const Camera* camera ) {
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

void OpenglProject::transform ( PositionData* p ) {
  glTranslatef( p->position[0],
                p->position[1],
                p->position[2]);
  Vector3 v;
  real_t a;
  p->orientation.to_axis_angle(&v, &a);
  glRotatef( a, v[0], v[1], v[2] );
  glScalef( p->scale[0], p->scale[1], p->scale[2] );
}
} /* _462 */
