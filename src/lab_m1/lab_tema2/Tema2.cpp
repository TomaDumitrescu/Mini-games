#include "lab_m1/lab_tema2/Tema2.h"

#include <vector>
#include <string>
#include <iostream>

#include "lab_m1/lab_tema2/transform3D.h"
#include "lab_m1/lab_tema2/object3D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    polygonMode = GL_FILL;

    // Initialize tx, ty and tz (the translation steps)
    translateX = 0;
    translateY = 0;
    translateZ = 0;

    // Initialize sx, sy and sz (the scale factors)
    scaleX = 1;
    scaleY = 1;
    scaleZ = 1;

    // Initialize angular steps
    angularStepOX = 0;
    angularStepOY = 0;
    angularStepOZ = 0;

    initDrone = glm::vec3(0, 4, 0);

    propellerAngle = 0;
    droneAngle = 0;
    droneInclinationLR = 0;
    droneInclinationUD = 0;

    viewPortX = 0;
    viewPortY = 0;
    widthPort = 1;
    heightPort = 1;

    // Sets the resolution of the small viewport
    glm::ivec2 resolution = window->GetResolution();
    miniViewportArea = ViewportArea(50, 50, resolution.x / 5.f, resolution.y / 5.f);

    // Initializing the objects
    Mesh* depo = object3D::CreateDepo("depo", glm::vec3(0, 0, 0), 1, glm::vec3(0.58f, 0.58f, 0.58f), glm::mat4(1));
    AddMeshToList(depo);
    Mesh* tree = object3D::CreateTree("tree", glm::vec3(0, 0, 0), 1);
    AddMeshToList(tree);

    int deltaPos, cnt = 0, sign1, sign2;
    for (int z = -ZMAXVAL; z <= ZMAXVAL && cnt < NUM_OBJECTS; z += max(rand() % 10, 6)) {
        for (int x = -XMAXVAL; x <= XMAXVAL && cnt < NUM_OBJECTS; x += max(rand() % 10, 7)) {
            objType[cnt] = rand() % 3;
            deltaPos = rand() % 3, sign1 = rand() % 2, sign2 = rand() % 2;
            sign1 = (sign1 == 0) ? 1 : -1;
            sign2 = (sign2 == 0) ? 1 : -1;
            objX[cnt] = x + deltaPos;
            objZ[cnt] = z - deltaPos;
            if (objType[cnt] == 0) {
                objSize[cnt] = max(rand() % 6, 2);
            } else {
                objSize[cnt] = 2. / (rand() % 3 + 1);
            }
            cnt++;
        }
    }

    // Initializing terrain
    Mesh* terrain = object3D::drawTerrain("terrain", 0.5f, glm::vec3(0.6078f, 0.4627f, 0.3254f));
    AddMeshToList(terrain);
}

void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0.749f, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Tema2::collisionCheck() {
    if (translateX <= -22) {
        translateX = -22;
    }

    if (translateX >= 22) {
        translateX = 22;
    }

    if (translateZ <= -22) {
        translateZ = -22;
    }

    if (translateZ >= 22) {
        translateZ = 22;
    }

    if (translateY <= -3.9f + 0.55f * DRONE_SIZE * (abs(sin(droneInclinationLR)) + abs(sin(droneInclinationUD)))) {
        translateY = -3.9f + 0.55f * DRONE_SIZE * (abs(sin(droneInclinationLR)) + abs(sin(droneInclinationUD)));
    }

    if (translateY >= 20) {
        translateY = 20;
    }

    int x, y, z, len;
    float newTranslateX = translateX, newTranslateY = translateY, newTranslateZ = translateZ;
    bool collision = true;
    for (int i = 0; i < NUM_OBJECTS; i++) {
        collision = true;
        if (objType[i] == 0) {
            x = objSize[i], y = (1.0 * objSize[i]) / 2, z = (1.0 * objSize[i]) / 3;
            if (translateX - objX[i] - (1.0 * x) / 1.80 - 1.1f < EPS && objX[i] + (1.0 * x) / 1.80 <= translateX + 1.1f) {
                newTranslateX = 3 * EPS;
            } else if (objX[i] - (1.0 * x) / 1.80 - translateX - 1.1f < EPS && objX[i] - (1.0 * x) / 1.80 >= translateX - 1.1f) {
                newTranslateX = -3 * EPS;
            } else {
                collision = false;
            }

            if (translateY - 1.0 * y + 3.5f < EPS) {
                newTranslateY = 3 * EPS;
            } else {
                collision = false;
            }

            if (translateZ - objZ[i] - (1.0 * z) / 1.80 - 1.1f < EPS && objZ[i] + (1.0 * z) / 1.80 <= translateZ + 1.1f) {
                newTranslateZ = 3 * EPS;
            } else if (objZ[i] - (1.0 * z) / 1.80 - translateZ - 1.1f < EPS && objZ[i] - (1.0 * x) / 1.80 >= translateZ - 1.1f) {
                newTranslateZ = -3 * EPS;
            } else {
                collision = false;
            }
        } else {
            len = objSize[i];
            if (translateX - objX[i] - 0.35 * len - 1.1f < EPS && objX[i] + 0.35 * len <= translateX + 1.1f) {
                newTranslateX = 3 * EPS;
            } else if (objX[i] - 0.35 * len - translateX - 1.1f < EPS && objX[i] - 0.35 * len >= translateX - 1.1f) {
                newTranslateX = -3 * EPS;
            } else {
                collision = false;
            }

            if (translateZ - objZ[i] - 0.35 * len - 1.1f < EPS && objZ[i] + 0.35 * len <= translateZ + 1.1f) {
                newTranslateZ = 3 * EPS;
            } else if (objZ[i] - 0.35 * len - translateZ - 1.1f < EPS && objZ[i] - 0.35 * len >= translateZ - 1.1f) {
                newTranslateZ = -3 * EPS;
            } else {
                collision = false;
            }

            if (translateY - 1.2 * len + 3.5f < EPS) {
                newTranslateY = 3 * EPS;
            } else {
                collision = false;
            }
        }

        if (collision) {
            translateX += newTranslateX, translateY += newTranslateY, translateZ += newTranslateZ;
        }
    }
}

void Tema2::RenderScene() {
    // Generating objects
    for (int i = 0; i < NUM_OBJECTS; i++) {
        modelMatrix = glm::mat4(1);
        if (objType[i] == 0) {
            modelMatrix *= transform3D::Translate(objX[i], objSize[i] * 1.0 / 4, objZ[i]);
            modelMatrix *= transform3D::Scale(objSize[i], objSize[i] * 1.0 / 2, objSize[i] * 1.0 / 3);
            RenderMesh(meshes["depo"], shaders["VertexColor"], modelMatrix);
            continue;
        } else {
            modelMatrix *= transform3D::Translate(objX[i], 0, objZ[i]);
            modelMatrix *= transform3D::Scale(objSize[i], objSize[i], objSize[i]);
            RenderMesh(meshes["tree"], shaders["VertexColor"], modelMatrix);
        }
    }

    // Terrain
    RenderMesh(meshes["terrain"], shaders["VertexColor"], glm::mat4(1));
    // Render drone
    Mesh* drone = object3D::CreateDrone("drone", initDrone, propellerAngle);
    AddMeshToList(drone);

    collisionCheck();

    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(translateX, translateY, translateZ);
    modelMatrix *= transform3D::RotateOY(angularStepOY);
    modelMatrix *= transform3D::RotateOZ(-droneInclinationLR);
    modelMatrix *= transform3D::RotateOX(droneInclinationUD);
    RenderMesh(meshes["drone"], shaders["VertexColor"], modelMatrix);
    RemoveMeshFromList(drone);
    propellerAngle += PROP_OMEGA;
    if (propellerAngle >= OVERFLOW) {
        propellerAngle = 0;
    }
}

void Tema2::Update(float deltaTimeSeconds)
{
    glLineWidth(3);
    glPointSize(5);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);

    RenderScene();

    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(miniViewportArea.x - viewPortX, miniViewportArea.y - viewPortY, miniViewportArea.width * widthPort,
                miniViewportArea.height * heightPort);

    RenderScene();
}

void Tema2::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    // Add key holding events
    if (window->KeyHold(GLFW_KEY_W)) {
        translateZ -= deltaTime * SPEED;
        droneInclinationUD -= deltaTime * INCLINATION_SPEED;
        if (droneInclinationUD <= -MAX_INCLINE) {
            droneInclinationUD = -MAX_INCLINE;
        }
    }

    if (window->KeyHold(GLFW_KEY_A)) {
        translateX -= deltaTime * SPEED;
        droneInclinationLR -= deltaTime * INCLINATION_SPEED;
        if (droneInclinationLR <= -MAX_INCLINE) {
            droneInclinationLR = -MAX_INCLINE;
        }
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        translateZ += deltaTime * SPEED;
        droneInclinationUD += deltaTime * INCLINATION_SPEED;
        if (droneInclinationUD >= MAX_INCLINE) {
            droneInclinationUD = MAX_INCLINE;
        }
    }

    if (window->KeyHold(GLFW_KEY_D)) {
        translateX += deltaTime * SPEED;
        droneInclinationLR += deltaTime * INCLINATION_SPEED;
        if (droneInclinationLR >= MAX_INCLINE) {
            droneInclinationLR = MAX_INCLINE;
        }
    }

    if (window->KeyHold(GLFW_KEY_E)) {
        translateY += deltaTime * SPEED;
    }

    if (window->KeyHold(GLFW_KEY_Q)) {
        translateY -= deltaTime * SPEED;
    }

    if (window->KeyHold(GLFW_KEY_RIGHT)) {
        angularStepOY += deltaTime * OMEGA;
    }

    if (window->KeyHold(GLFW_KEY_LEFT)) {
        angularStepOY -= deltaTime * OMEGA;
    }
}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
