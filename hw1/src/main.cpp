#include <algorithm>
#include <memory>
#include <vector>

#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION
#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "camera.h"
#include "opengl_context.h"
#include "utils.h"

#include "iostream"

#define ANGEL_TO_RADIAN(x) (float)((x)*M_PI / 180.0f)
#define RADIAN_TO_ANGEL(x) (float)((x)*180.0f / M_PI)

#define CIRCLE_SEGMENT 64

/* Components size definition */
#define ARM_LEN 1.0f
#define ARM_RADIUS 0.05f
#define ARM_DIAMETER (ARM_RADIUS * 2)
#define JOINT_RADIUS 0.05f
#define JOINT_DIAMETER (JOINT_RADIUS * 2)
#define JOINT_WIDTH 0.1f
#define BASE_RADIUS 0.5f
#define BASE_DIAMETER (BASE_RADIUS * 2)
#define BASE_HEIGHT 0.1f
#define TARGET_RADIUS 0.05f
#define TARGET_DIAMETER (TARGET_RADIUS * 2)
#define TARGET_HEIGHT 0.1f
#define ROTATE_SPEED 1.0f
#define CATCH_POSITION_OFFSET 0.05f
#define TOLERANCE 0.1f

#define RED 0.905f, 0.298f, 0.235f
#define BLUE 0.203f, 0.596f, 0.858f
#define GREEN 0.18f, 0.8f, 0.443f

float joint0_degree = 0;
float joint1_degree = 0;
float joint2_degree = 0;
float joint0_degree_ans = 0;
float joint1_degree_ans = 0;
float joint2_degree_ans = 0;
glm::vec3 target_pos(1.0f, 0.0f, 1.0f);
bool isCatching = false;
bool isAutoMoving = false;
float drop_speed = 0;
float gravity = 0.00098f;

void resizeCallback(GLFWwindow* window, int width, int height) {
  OpenGLContext::framebufferResizeCallback(window, width, height);
  auto ptr = static_cast<Camera*>(glfwGetWindowUserPointer(window));
  if (ptr) {
    ptr->updateProjectionMatrix(OpenGLContext::getAspectRatio());
  }
}

void getTargetJoint(float& j0, float& j1, float& j2);

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
  // Press ESC to close the window.
  if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    return;
  }

  // There are three actions: press, release, hold(repeat)
  if ( action == GLFW_REPEAT || action == GLFW_PRESS) {
    if (key == GLFW_KEY_U) {
      joint0_degree += ROTATE_SPEED;
    } else if (key == GLFW_KEY_J) {
      joint0_degree -= ROTATE_SPEED;
    } else if (key == GLFW_KEY_I) {
      joint1_degree -= ROTATE_SPEED;
    } else if (key == GLFW_KEY_K) {
      joint1_degree += ROTATE_SPEED;
    } else if (key == GLFW_KEY_O) {
      joint2_degree -= ROTATE_SPEED;
    } else if (key == GLFW_KEY_L) {
      joint2_degree += ROTATE_SPEED;
    }
  };
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_SPACE) {
      isCatching = true;
    } else if (key == GLFW_KEY_M and isAutoMoving == false) {
      getTargetJoint(joint0_degree_ans, joint1_degree_ans, joint2_degree_ans);
      isAutoMoving = true;
    }
  }

  if (action == GLFW_RELEASE) {
    if (key == GLFW_KEY_SPACE) {
      isCatching = false;
    }
  }
  /* TODO#4-1: Detect key-events, perform rotation or catch target object
   *       1. Use switch/case to find the key you want.
   *       2. Define and modify some global variable to trigger update in rendering loop
   * Hint:
   *       glfw3's key list (https://www.glfw.org/docs/3.3/group__keys.html)
   *       glfw3's action codes (https://www.glfw.org/docs/3.3/group__input.html#gada11d965c4da13090ad336e030e4d11f)
   * Note:
   *       You should finish your robotic arm first.
   *       Otherwise you will spend a lot of time debugging this with a black screen.
   */
}

void initOpenGL() {
  // Initialize OpenGL context, details are wrapped in class.
#ifdef __APPLE__
  // MacOS need explicit request legacy support
  OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
#else
  OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
//  OpenGLContext::createContext(43, GLFW_OPENGL_COMPAT_PROFILE);
#endif
  GLFWwindow* window = OpenGLContext::getWindow();
  /* TODO#0: Change window title to "HW1 - `your student id`"
   *         Ex. HW1 - 311550000
   */
  glfwSetWindowTitle(window, "HW1 - 0816117");
  glfwSetKeyCallback(window, keyCallback);
  glfwSetFramebufferSizeCallback(window, resizeCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#ifndef NDEBUG
  OpenGLContext::printSystemInfo();
  // This is useful if you want to debug your OpenGL API calls.
  OpenGLContext::enableDebugCallback();
#endif
}

void drawUnitCylinder() {
  /* TODO#2-1: Render a unit cylinder
   * Hint:
   *       glBegin/glEnd (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glBegin.xml)
   *       glColor3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glBegin.xml)
   *       glVertex3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glVertex.xml)
   *       glNormal (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glNormal.xml)
   *       glScalef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glScale.xml)
   * Note:
   *       You can refer to ppt "Draw Cylinder" page and `CIRCLE_SEGMENT`
   *       You should set normal for lighting
   */

  // top
  glBegin(GL_TRIANGLE_STRIP);
  for (int i = 0; i <= CIRCLE_SEGMENT; ++i) {
    float rad = ANGEL_TO_RADIAN(360 * (float)i / CIRCLE_SEGMENT);
    float nRad = ANGEL_TO_RADIAN(360 * (float)(i + 1) / CIRCLE_SEGMENT);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(cos(rad), 1, -sin(rad));
    glVertex3f(cos(nRad), 1, -sin(nRad));
  }
  glEnd();

  // side
  for (int i = 0; i <= CIRCLE_SEGMENT; ++i) {
    glBegin(GL_TRIANGLE_STRIP);
    float rad = ANGEL_TO_RADIAN(360 * (float)i / CIRCLE_SEGMENT);
    float nRad = ANGEL_TO_RADIAN(360 * (float)(i + 1) / CIRCLE_SEGMENT);
    float mRad = (rad + nRad) / 2.0f;
    glNormal3f(cos(mRad), 0.0f, -sin(mRad));
    // glColor3f(0.0, 1.0, 0);
    glVertex3f(cos(rad), 0.0, -sin(rad));
    glVertex3f(cos(nRad), 0.0, -sin(nRad));
    glVertex3f(cos(rad), 1.0, -sin(rad));
    glVertex3f(cos(nRad), 1.0, -sin(nRad));
    glEnd();
  }

  // down
  glBegin(GL_TRIANGLE_STRIP);
  for (int i = 0; i <= CIRCLE_SEGMENT; ++i) {
    float rad = ANGEL_TO_RADIAN(360 * (float)i / CIRCLE_SEGMENT);
    float nRad = ANGEL_TO_RADIAN(360 * (float)(i + 1) / CIRCLE_SEGMENT);
    // glColor3f(0.0f, 1.0f, 0.0f);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0, 0.0, 0);
    glVertex3f(cos(rad), 0.0, -sin(rad));
    glVertex3f(cos(nRad), 0.0, -sin(nRad));
  }
  glEnd();
}

void drawBase() {
  glPushMatrix();
  glScalef(BASE_RADIUS, BASE_HEIGHT, BASE_RADIUS);
  glColor3f(GREEN);
  drawUnitCylinder();
  glPopMatrix();
}

void drawArm() {
  glPushMatrix();
  glScalef(ARM_RADIUS, ARM_LEN, ARM_RADIUS);
  glColor3f(BLUE);
  drawUnitCylinder();
  glPopMatrix();
}
void drawJoint() {
  glPushMatrix();
  glTranslatef(0, JOINT_RADIUS, -JOINT_WIDTH / 2);
  glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
  glScalef(JOINT_RADIUS, JOINT_WIDTH, JOINT_RADIUS);
  glColor3f(GREEN);
  drawUnitCylinder();
  glPopMatrix();
}

void getTargetJoint(float& j0, float& j1, float& j2) {
  /*
    y axis

    ^
    |
    
    B         
     _________D
    | \      /
    |  \    /
    |   \  /
    |____\/                 -> x axis
    A    C          

    A: joint0 (0,0,0) which represent arm base.
    B: joint1 (0,BASE_HEIGHT+ARM_LEN+JOINT_RADIUS,0) 
    C: target pos
    D: joint2
    
    known: A, B, C position, AB, BD, BC length
    solve: ABD BDC angle.
  */

  // rotate joint0 to same 2D plane
  glm::vec2 AC(target_pos.x, target_pos.z);
  float xAngle = RADIAN_TO_ANGEL(glm::angle(glm::vec2(1,0), glm::normalize(AC)));
  float zAngle = RADIAN_TO_ANGEL(glm::angle(glm::vec2(0, 1), glm::normalize(AC)));
  if (xAngle < 90) {
    j0 = -90 + zAngle;
  } else {
    j0 = -90 - zAngle;
  }
  //std::cout << "j0 " << j0 << "\n";
  glm::vec2 A(0, 0);
  glm::vec2 B(0, BASE_HEIGHT + ARM_LEN + JOINT_RADIUS);
  glm::vec2 C(glm::length(glm::vec2(target_pos.x, target_pos.z)), target_pos.y+TARGET_HEIGHT/2.0);
  float ABC = RADIAN_TO_ANGEL(glm::angle(glm::normalize(A - B), glm::normalize(C - B)));
  
  float BD_distance = ARM_LEN + 2 * JOINT_RADIUS;
  float DC_distance = ARM_LEN + JOINT_RADIUS + CATCH_POSITION_OFFSET;
  float BC_distance = glm::distance(B,C);
 
  auto getCosTheta = [](const float& a, const float& b, const float& c) {
      // cosine law
      return (a * a + b * b - c * c) / (2 * a * b);
  };

  float cosCBD = getCosTheta(BD_distance, BC_distance, DC_distance);
  float CBD = RADIAN_TO_ANGEL(glm::acos(cosCBD));
  float ABD = ABC + CBD;
  j1 = (180.0 - ABD);
  
  float cosBDC = getCosTheta(BD_distance, DC_distance, BC_distance);
  float BDC = RADIAN_TO_ANGEL(glm::acos(cosBDC));
  j2 = (180 - BDC);  
  //std::cout << "BA dist: " << glm::length(B - A) << " BC dist: " << glm::length(A - C) << "\n";
  //std::cout << "AC dist: " << glm::distance(A, C) << "\n";
  //std::cout << "ABC " << ABC << "\n";
  //std::cout << "cosCBD: " << cosCBD << "\n";
  //std::cout << "BD dist: " << BD_distance << " BC_dist: " << BC_distance << " DC dist: " << DC_distance << "\n";
  //std::cout << "CBD: " << CBD << "\n";
  //std::cout << "j1: " << j1 << "\n";
  //std::cout << "j2 " << j2 << "\n";
}

glm::vec3 getCatchPostion() {
  glm::vec4 pos(glm::vec3(0.0f), 1.0f);
  static const glm::mat4x4 identity = glm::mat4x4(1.0f);
  static const glm::mat4x4 baseTrans = glm::translate(identity, glm::vec3(0, BASE_HEIGHT, 0));
  static const glm::mat4x4 armTrans = glm::translate(identity, glm::vec3(0, ARM_LEN, 0));
  static const glm::mat4x4 jointTrans = glm::translate(identity, glm::vec3(0, JOINT_RADIUS, 0));
  static const glm::mat4x4 offsetTrans = glm::translate(identity, glm::vec3(0, CATCH_POSITION_OFFSET, 0));
  const glm::mat4x4 jointRotate0 = glm::rotate(identity, ANGEL_TO_RADIAN(joint0_degree), glm::vec3(0, 1, 0));
  const glm::mat4x4 jointRotate1 = glm::rotate(identity, ANGEL_TO_RADIAN(joint1_degree), glm::vec3(0, 0, -1));
  const glm::mat4x4 jointRotate2 = glm::rotate(identity, ANGEL_TO_RADIAN(joint2_degree), glm::vec3(0, 0, -1));
  pos = jointRotate0 * baseTrans * 
        armTrans *
        jointTrans * jointRotate1 * jointTrans *
        armTrans * 
        jointTrans * jointRotate2 * jointTrans * 
        armTrans * 
        offsetTrans * pos;
  return glm::vec3(pos.x/pos.w, pos.y/pos.w, pos.z/pos.w);
}

void light() {
  GLfloat light_specular[] = {0.6, 0.6, 0.6, 1};
  GLfloat light_diffuse[] = {0.6, 0.6, 0.6, 1};
  GLfloat light_ambient[] = {0.4, 0.4, 0.4, 1};
  GLfloat light_position[] = {50.0, 75.0, 80.0, 1.0};
  // z buffer enable
  glEnable(GL_DEPTH_TEST);
  // enable lighting
  glEnable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_NORMALIZE);
  // set light property
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
}

int main() {
  initOpenGL();
  GLFWwindow* window = OpenGLContext::getWindow();

  // Init Camera helper
  Camera camera(glm::vec3(0, 2, 5));
  camera.initialize(OpenGLContext::getAspectRatio());
  // Store camera as glfw global variable for callbasks use
  glfwSetWindowUserPointer(window, &camera);

  // Main rendering loop
  while (!glfwWindowShouldClose(window)) {
    // Polling events.
    glfwPollEvents();
    // Update camera position and view
    camera.move(window);
    // GL_XXX_BIT can simply "OR" together to use.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /// TO DO Enable DepthTest
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // Projection Matrix
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(camera.getProjectionMatrix());
    // ModelView Matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(camera.getViewMatrix());

#ifndef DISABLE_LIGHT
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearDepth(1.0f);
    light();
#endif

    /* TODO#4-2: Update joint degrees
     *       1. Finish keyCallback to detect key events
     *       2. Update jointx_degree if the correspond key is pressed
     * Note:
     *       You can use `ROTATE_SPEED` as the speed constant.
     *       If the rotate speed is too slow or too fast, please change `ROTATE_SPEED` value
     */

    /* TODO#5: Catch the target object with robotic arm
     *       1. Calculate coordinate of the robotic arm endpoint
     *       2. Test if arm endpoint and the target object are close enough
     *       3. Update coordinate fo the target object to the arm endpoint
     *          if the space key is pressed
     * Hint:
     *       GLM fransform API (https://glm.g-truc.net/0.9.4/api/a00206.html)
     * Note:
     *       You might use `ANGEL_TO_RADIAN`
     *       and refer to `CATCH_POSITION_OFFSET` and `TOLERANCE`
     */
    glPushMatrix();
    glm::vec3 pos = getCatchPostion();
    glTranslatef(pos.x, pos.y, pos.z);
    glScalef(0.005, 0.005, 0.005);
    glColor3f(0.9, 0.1, 0.1);
    drawUnitCylinder();
    glPopMatrix();

    
    if (isAutoMoving == true) {
      bool isReach = true;
      if (std::abs(joint0_degree - joint0_degree_ans) > ROTATE_SPEED) {
        joint0_degree += (joint0_degree > joint0_degree_ans) ? -ROTATE_SPEED : +ROTATE_SPEED;
        isReach = false;
      }
      if (std::abs(joint1_degree - joint1_degree_ans) > ROTATE_SPEED) {
        joint1_degree += (joint1_degree > joint1_degree_ans) ? -ROTATE_SPEED : +ROTATE_SPEED;
        isReach = false;
      }
      if (std::abs(joint2_degree - joint2_degree_ans) > ROTATE_SPEED) {
        joint2_degree += (joint2_degree > joint2_degree_ans) ? -ROTATE_SPEED : +ROTATE_SPEED;
        isReach = false;
      }
      if (isReach) {
        joint0_degree = joint0_degree_ans;
        joint1_degree = joint1_degree_ans;
        joint2_degree = joint2_degree_ans;
        isAutoMoving = false;
      }
    }

    if (isCatching && glm::distance(pos, glm::vec3(target_pos.x, target_pos.y + TARGET_HEIGHT/2.0, target_pos.z)) <= TOLERANCE) {
      target_pos = pos;
      target_pos.y -= TARGET_HEIGHT/2.0;
      drop_speed = 0;
    }    

    // Render a white board
    glPushMatrix();
    glScalef(3, 1, 3);
    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1.0f, 1.0f, 1.0f);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, -0.1f, -1.0f);
    glVertex3f(-1.0f, -0.1f, 1.0f);
    glVertex3f(1.0f, -0.1f, -1.0f);
    glVertex3f(1.0f, -0.1f, 1.0f);
    glEnd();
    glPopMatrix();

    /* TODO#2: Render a cylinder at target_pos
     *       1. Translate to target_pos
     *       2. Setup vertex color
     *       3. Setup cylinder scale
     *       4. Call drawUnitCylinder
     * Hint:
     *       glTranslatef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glTranslate.xml)
     *       glColor3f (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glColor.xml)
     *       glScalef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glScale.xml)
     * Note:
     *       The coordinates of the cylinder are `target_pos`
     *       The cylinder's size can refer to `TARGET_RADIUS`, `TARGET_DIAMETER` and `TARGET_DIAMETER`
     *       The cylinder's color can refer to `RED`
     */


    glPushMatrix();
    glTranslatef(target_pos.x, target_pos.y, target_pos.z);
    glScalef(TARGET_RADIUS, TARGET_HEIGHT, TARGET_RADIUS);
    glColor3f(RED);
    drawUnitCylinder();
    glPopMatrix();

    /* TODO#3: Render the robotic arm
     *       1. Render the base
     *       2. Translate to top of the base
     *       3. Render an arm
     *       4. Translate to top of the arm
     *       5. Render the joint
     *       6. Translate and rotate to top of the join
     *       7. Repeat step 3-6
     * Hint:
     *       glPushMatrix/glPopMatrix (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glPushMatrix.xml)
     *       glRotatef (https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glRotate.xml)
     * Note:
     *       The size of every component can refer to `Components size definition` section
     *       Rotate degree for joints are `joint0_degree`, `joint1_degree` and `joint2_degree`
     *       You may implement drawBase, drawArm and drawJoint first
     */

    // base
    glPushMatrix();
    glRotated(joint0_degree, 0, 1, 0);
    drawBase();

    glTranslated(0, BASE_HEIGHT, 0);
    drawArm();

    glTranslated(0, ARM_LEN, 0);
    drawJoint();

    glTranslated(0, (float)JOINT_DIAMETER / 2.0f, 0);
    glRotated(joint1_degree, 0, 0, -1);
    glTranslated(0, (float)JOINT_DIAMETER / 2.0f, 0);

    drawArm();

    glTranslated(0, ARM_LEN, 0);

    drawJoint();

    glTranslated(0, (float)JOINT_DIAMETER / 2.0f, 0);
    glRotated(joint2_degree, 0, 0, -1);
    glTranslated(0, (float)JOINT_DIAMETER / 2.0f, 0);

    drawArm();

    glPopMatrix();

#ifdef __APPLE__
    // Some platform need explicit glFlush
    glFlush();
#endif
    glfwSwapBuffers(window);
  }
  return 0;
}
