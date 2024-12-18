#include "lab_m1/lab_tema/Tema1.h"

#include <vector>
#include <iostream>
#include <cmath>

#include "lab_m1/lab_tema/transform2D.h"
#include "lab_m1/lab_tema/object2D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
    delete[] heightMap;
}


void Tema1::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    logicSpace.x = 0;       // logic x
    logicSpace.y = 0;       // logic y
    logicSpace.width = 500;   // logic width
    logicSpace.height = 281;  // logic height

    // Initialize tx and ty (the translation steps)
    translateX = 0;
    translateY = 0;

    // Initialize sx and sy (the scale factors)
    scaleX = 1;
    scaleY = 1;

    // Initialize angularStep
    angularStep = 0;

    orientation = 1;
    // Terrain data, function: 0.8 * sin(0.4 * x) + sin(0.7 * x)
    heightMap = new float[HEIGHT_MAP_LEVEL];
    stepX = 0.03f;

    float xCoord = 0, minNegativeHeight = 0;
    for (int i = 0; i < HEIGHT_MAP_LEVEL; i++) {
        heightMap[i] = (float)(0.8 * sin(0.4 * xCoord) + sin(0.7 * xCoord));
        xCoord += stepX;
        if (heightMap[i] < minNegativeHeight) {
            minNegativeHeight = heightMap[i];
        }
    }

    // Only the first quadrant will be displayed, without losing the terrain properties
    for (int i = 0; i < HEIGHT_MAP_LEVEL; i++) {
        heightMap[i] -= minNegativeHeight + LIFT_UP;
        heightMap[i] *= SCALE_FACTOR_Y;
    }

    // Add objects
    Mesh* terrainUnit = object2D::CreateSquare("tUnit", glm::vec3(0, -1, 0), 1.0f, glm::vec3(0.76f, 0.7f, 0.5f), true);
    AddMeshToList(terrainUnit);

    // Initializing tank movement variables
    tank1X = TANK_1_OFF * stepX * SCALE_FACTOR_X, tank2X = TANK_2_OFF * stepX * SCALE_FACTOR_X;
    shooter1Rotate = SHOOTER_OFF, shooter2Rotate = -SHOOTER_OFF;
    tank1Y = heightMap[TANK_1_OFF], tank2Y = heightMap[TANK_2_OFF];
    map1Index = TANK_1_OFF, map2Index = TANK_2_OFF;
    d1X = 0, d2X = 0;

    // Initialize tank hp
    tank1HP = HP, tank2HP = HP;

    // Initializing bullet object
    render1Bullet = false, render2Bullet = false;
    bullet1X = 0, bullet2X = 0, bullet1Y = 0, bullet2Y = 0;
    bullet1X = 0, bullet1Y = 0;
    launched1Bullet = false, launched2Bullet = false;
    max1Reached = false, max2Reached = false;
    stepLaunch1 = 0, stepLaunch2 = 0;

    Mesh* bullet = object2D::CreateBullet("bullet", glm::vec3(0, 0, 0), BULLET_RADIUS, glm::vec3(0.02f, 0.02f, 0.02f), true);

    AddMeshToList(bullet);

    // Health bar
    Mesh* emptyBar = object2D::CreateSquare("emptybar",
                                            glm::vec3(TANK_1_OFF * stepX * SCALE_FACTOR_X, heightMap[TANK_1_OFF], 0),
                                            1.3f * TANK_LENGTH, glm::vec3(0, 0, 0), false);

    AddMeshToList(emptyBar);

    Mesh* filledBar = object2D::CreateSquare("filledbar",
                                            glm::vec3(TANK_1_OFF * stepX * SCALE_FACTOR_X, heightMap[TANK_1_OFF], 0),
                                            1.3f * TANK_LENGTH, glm::vec3(1.0f, 0.0f, 0.0f), true);

    AddMeshToList(filledBar);

    Mesh* empty2Bar = object2D::CreateSquare("empty2bar",
        glm::vec3(TANK_2_OFF * stepX * SCALE_FACTOR_X, heightMap[TANK_2_OFF], 0),
        1.3f * TANK_LENGTH, glm::vec3(0, 0, 0), false);

    AddMeshToList(empty2Bar);

    Mesh* filled2Bar = object2D::CreateSquare("filled2bar",
        glm::vec3(TANK_2_OFF * stepX * SCALE_FACTOR_X, heightMap[TANK_2_OFF], 0),
        1.3f * TANK_LENGTH, glm::vec3(1.0f, 0.0f, 0.0f), true);

    AddMeshToList(filled2Bar);

    // Trajectory
    Mesh* tParticle = object2D::CreateSquare("tparticle", glm::vec3(0, 0, 0), 0.5f,
                                             glm::vec3(0.91f, 0.91f, 0.91f), true);
    AddMeshToList(tParticle);

    // Blood Moon
    Mesh* moon = object2D::CreateBullet("moon", glm::vec3(100, 230, 0), 30, glm::vec3(0.533f, 0.0313f, 0.0313f), true);

    AddMeshToList(moon);
}

// 2D visualization matrix
glm::mat3 Tema1::VisualizationTransf2D(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
    float sx, sy, tx, ty;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    tx = viewSpace.x - sx * logicSpace.x;
    ty = viewSpace.y - sy * logicSpace.y;

    return glm::transpose(glm::mat3(
        sx, 0.0f, tx,
        0.0f, sy, ty,
        0.0f, 0.0f, 1.0f));
}

// Uniform 2D visualization matrix (same scale factor on x and y axes)
glm::mat3 Tema1::VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
    float sx, sy, tx, ty, smin;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    if (sx < sy)
        smin = sx;
    else
        smin = sy;
    tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
    ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

    return glm::transpose(glm::mat3(
        smin, 0.0f, tx,
        0.0f, smin, ty,
        0.0f, 0.0f, 1.0f));
}


void Tema1::SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear)
{
    glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    glEnable(GL_SCISSOR_TEST);
    glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
    GetSceneCamera()->Update();
}

void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void Tema1::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->GetResolution();

    // Sets the screen area where to draw - the left half of the window
    viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
    SetViewportArea(viewSpace, glm::vec3(0, 0, 0.33f), true);

    // Compute the 2D visualization matrix
    visMatrix = glm::mat3(1);
    visMatrix *= VisualizationTransf2D(logicSpace, viewSpace);

    DrawScene(visMatrix, deltaTimeSeconds);
}

// Retreives the coordinates on the top shooter
glm::vec3 Tema1::startBulletCoord(glm::vec3 tankCoordinates, float tankLength, float cannonAngle) {
    float x = 0, y = 0;
    float topRadius = 0.33f * tankLength;

    x = tankCoordinates[0] - ((10 - topRadius) * cos(M_PI / 2 - cannonAngle) - tankLength / 2) - 0.3f;
    y = tankCoordinates[1] + tankLength * 0.37f * 1.5f + (10 - topRadius) * sin(M_PI / 2 - cannonAngle);

    return glm::vec3(x, y, 0);
}

void Tema1::DrawScene(glm::mat3 visMatrix, float deltaTimeSeconds)
{
    // Displaying winning color
    if (tank1HP <= 0) {
        printf("\033[31mPlayer II WON!\033[0m\n");
        exit(0);
    } else if (tank2HP <= 0) {
        printf("\033[38;5;208mPlayer I WON!\033[0m\n");
        exit(0);
    }

    // Render Moon
    RenderMesh(meshes["moon"], shaders["VertexColor"], visMatrix);

    // Add object transformations, multiplying with visMatrix, invert order
    float scaleFactorY = 0, scaleFactorX = 0, shearFactor = 0;
    for (int i = 0; i < HEIGHT_MAP_LEVEL - 1; i++) {
        modelMatrix = visMatrix;

        // Translation with the xCoord = stepX
        modelMatrix *= transform2D::Translate(i * stepX * SCALE_FACTOR_X, heightMap[i]);

        // Shear
        shearFactor = (heightMap[i + 1] - heightMap[i]) / scaleX;
        modelMatrix *= transform2D::Shear(shearFactor);

        // Scaling
        scaleFactorY = (heightMap[i] < heightMap[i + 1])? heightMap[i] : heightMap[i + 1];
        scaleFactorX = stepX;
        modelMatrix *= transform2D::Scale(scaleFactorX * SCALE_FACTOR_X, scaleFactorY);

        RenderMesh2D(meshes["tUnit"], shaders["VertexColor"], modelMatrix);
    }

    // Creating the tanks with oriented cannons
    Mesh* tank1 = object2D::CreateTank("tank1", glm::vec3(TANK_1_OFF * stepX * SCALE_FACTOR_X, heightMap[TANK_1_OFF], 0), TANK_LENGTH,
                                        glm::vec3(0.929f, 0.529f, 0.267f), glm::vec3(0.412f, 0.286f, 0.231f),
                                         shooter1Rotate, true);
    AddMeshToList(tank1);

    Mesh* tank2 = object2D::CreateTank("tank2", glm::vec3(TANK_2_OFF * stepX * SCALE_FACTOR_X, heightMap[TANK_2_OFF], 0), TANK_LENGTH,
                                        glm::vec3(0.69f, 0.204f, 0.133f), glm::vec3(0.831f, 0.325f, 0.286f), shooter2Rotate, true);
    AddMeshToList(tank2);

    float newTank2X = tank2X + ((int)d2X) * stepX * SCALE_FACTOR_X;
    map2Index = (int)(newTank2X / (stepX * SCALE_FACTOR_X));
    float newTank2Y = heightMap[map2Index];

    float cTank2X = newTank2X + TANK_LENGTH / 2;
    float cTank2Y = newTank2Y;
    ///////////////////////////////////// Tank 1: //////////////////////////////////////

    // tank movement update
    float newTankX = tank1X + ((int)d1X) * stepX * SCALE_FACTOR_X;
    map1Index = (int)(newTankX / (stepX * SCALE_FACTOR_X));
    float newTankY = heightMap[map1Index];

    float cTankX = newTankX + TANK_LENGTH / 2;
    float cTankY = newTankY;

    modelMatrix = visMatrix;

    float tankLineP1X = newTankX + ((map1Index + 15) * stepX) * SCALE_FACTOR_X;
    float tankLineP1Y = heightMap[map1Index + 15];
    float tankLineP2Y = heightMap[map1Index + 25];
    float tankLineP2X = newTankX + ((map1Index + 25) * stepX) * SCALE_FACTOR_X;

    float angle = atan2(tankLineP2Y - tankLineP1Y, tankLineP2X - tankLineP1X);
    float t = (tankLineP2Y - tankLineP1Y) / (tankLineP2X - tankLineP1X);

    // geometrically calculated the dy after the rotation
    modelMatrix *= transform2D::Translate(0, t * TANK_LENGTH / 2 + ADJUST_ORIENTATION);
    modelMatrix *= transform2D::Translate(cTankX, cTankY);
    modelMatrix *= transform2D::Rotate(-angle);
    modelMatrix *= transform2D::Translate(-cTankX, -cTankY);
    modelMatrix *= transform2D::Translate(newTankX - tank1X, newTankY - tank1Y);

    RenderMesh2D(meshes["tank1"], shaders["VertexColor"], modelMatrix);
    modelMatrix *= transform2D::Translate(-4, 7 * TANK_LENGTH);
    modelMatrix *= transform2D::Scale(1.0, 0.25f);
    RenderMesh2D(meshes["emptybar"], shaders["VertexColor"], modelMatrix);
    modelMatrix *= transform2D::Scale((1.0 * tank1HP) / (HP * 1.0), 1.0);
    modelMatrix *= transform2D::Translate(((HP * 1.0) / (1.0 * tank1HP) - 1) * 2.25f * TANK_LENGTH, 0);
    RenderMesh2D(meshes["filledbar"], shaders["VertexColor"], modelMatrix);

    if (!launched1Bullet) {
        prevAngle1 = angle;
        stepLaunch1 = 0.01f;
        bullet1X = INIT_SPEED * cos(M_PI / 2 - shooter1Rotate + angle) * 0.01f;
        bullet1Y = INIT_SPEED * sin(M_PI / 2 - shooter1Rotate + angle) * 0.01f - 0.5f * g * 0.01f * 0.01f;

        // Trajectory Drawing
        for (int i = 0; i < TRAJECTORY; i++) {
            modelMatrix = visMatrix;
            glm::vec3 bulletCoord = startBulletCoord(glm::vec3(newTankX, newTankY, 0), TANK_LENGTH, shooter1Rotate);
            modelMatrix *= transform2D::Translate(bullet1X, bullet1Y);
            modelMatrix *= transform2D::Translate(0, t * TANK_LENGTH / 2 + ADJUST_ORIENTATION + 1);
            modelMatrix *= transform2D::Translate(cTankX, cTankY);
            modelMatrix *= transform2D::Rotate(-angle);
            modelMatrix *= transform2D::Translate(-cTankX, -cTankY);
            modelMatrix *= transform2D::Translate(bulletCoord[0], bulletCoord[1]);
            RenderMesh2D(meshes["tparticle"], shaders["VertexColor"], modelMatrix);

            stepLaunch1 += 0.005f;
            bullet1X = INIT_SPEED * cos(M_PI / 2 - shooter1Rotate + prevAngle1) * stepLaunch1;
            bullet1Y = INIT_SPEED * sin(M_PI / 2 - shooter1Rotate + prevAngle1) * stepLaunch1 - 0.5f * g * stepLaunch1 * stepLaunch1;
        }
    }

    // Verify if bullet is launched:
    if (render1Bullet || launched1Bullet) {
        if (!launched1Bullet) {
            prevAngle1 = angle;
            stepLaunch1 = deltaTimeSeconds;
            bullet1X = INIT_SPEED * cos(M_PI / 2 - shooter1Rotate + angle) * deltaTimeSeconds;
            bullet1Y = INIT_SPEED * sin(M_PI / 2 - shooter1Rotate + angle) * deltaTimeSeconds - 0.5f * g * deltaTimeSeconds * deltaTimeSeconds;
        } else {
            stepLaunch1 += deltaTimeSeconds;
            bullet1X = INIT_SPEED * cos(M_PI / 2 - shooter1Rotate + prevAngle1) * stepLaunch1;
            bullet1Y = INIT_SPEED * sin(M_PI / 2 - shooter1Rotate + prevAngle1) * stepLaunch1 - 0.5f * g * stepLaunch1 * stepLaunch1;
        }

        modelMatrix = visMatrix;
        glm::vec3 bulletCoord = startBulletCoord(glm::vec3(newTankX, newTankY, 0), TANK_LENGTH, shooter1Rotate);
        modelMatrix *= transform2D::Translate(bullet1X, bullet1Y);
        modelMatrix *= transform2D::Translate(0, t * TANK_LENGTH / 2 + ADJUST_ORIENTATION + 1);
        modelMatrix *= transform2D::Translate(cTankX, cTankY);
        modelMatrix *= transform2D::Rotate(-angle);
        modelMatrix *= transform2D::Translate(-cTankX, -cTankY);
        modelMatrix *= transform2D::Translate(bulletCoord[0], bulletCoord[1]);

        RenderMesh2D(meshes["bullet"], shaders["VertexColor"], modelMatrix);

        render1Bullet = false;
        launched1Bullet = true;

        glm::vec3 newBCoord = modelMatrix * glm::vec3(0, 0, 1);
        glm::vec3 tank2Center = visMatrix * glm::vec3(cTank2X, cTank2Y, 1);
        // Verify if the bullet hit the enemy tank
        if (newBCoord[0] >= tank2Center[0] - HIT_RADIUS_X && newBCoord[0] <= tank2Center[0] + HIT_RADIUS_X &&
            newBCoord[1] >= tank2Center[1] - HIT_RADIUS_Y && newBCoord[1] <= tank2Center[1] + HIT_RADIUS_Y) {
            tank2HP--;
            launched1Bullet = false;
        }

        float x, y;
        for (int i = 0; i < HEIGHT_MAP_LEVEL - 1; i++) {
            x = i * stepX * SCALE_FACTOR_X;
            y = heightMap[i];

            glm::vec3 terrainCoord = visMatrix * glm::vec3(x, y, 1);
            if (abs(terrainCoord[0] - newBCoord[0]) < TERRAIN_EPS && abs(terrainCoord[1] - newBCoord[1]) < TERRAIN_EPS
                || ((i == HEIGHT_MAP_LEVEL - 2) && newBCoord[0] >= terrainCoord[0] - TERRAIN_EPS) ||
                (i == 0 && newBCoord[0] <= terrainCoord[0] + TERRAIN_EPS)) {
                launched1Bullet = false;
                break;
            }
        }
    }

    ///////////////////////////////////// Tank 2: //////////////////////////////////////
    modelMatrix = visMatrix;

    tankLineP1X = newTank2X + ((map2Index + 15) * stepX) * SCALE_FACTOR_X;
    tankLineP1Y = heightMap[map2Index + 15];
    tankLineP2Y = heightMap[map2Index + 25];
    tankLineP2X = newTank2X + ((map2Index + 25) * stepX) * SCALE_FACTOR_X;

    angle = atan2(tankLineP2Y - tankLineP1Y, tankLineP2X - tankLineP1X);
    t = (tankLineP2Y - tankLineP1Y) / (tankLineP2X - tankLineP1X);
    // geometrically calculated the dy after the rotation
    modelMatrix *= transform2D::Translate(0, t * TANK_LENGTH / 2 + ADJUST_ORIENTATION);
    modelMatrix *= transform2D::Translate(cTank2X, cTank2Y);
    modelMatrix *= transform2D::Rotate(-angle);
    modelMatrix *= transform2D::Translate(-cTank2X, -cTank2Y);
    modelMatrix *= transform2D::Translate(newTank2X - tank2X, newTank2Y - tank2Y);

    RenderMesh2D(meshes["tank2"], shaders["VertexColor"], modelMatrix);
    modelMatrix *= transform2D::Translate(-4, 4.3f * TANK_LENGTH);
    modelMatrix *= transform2D::Scale(1.0, 0.25f);
    RenderMesh2D(meshes["empty2bar"], shaders["VertexColor"], modelMatrix);
    modelMatrix *= transform2D::Scale((1.0 * tank2HP) / (HP * 1.0), 1.0);
    modelMatrix *= transform2D::Translate(((HP * 1.0) / (1.0 * tank2HP) - 1) * (TANK_LENGTH * TANK_LENGTH - 0.7 * TANK_LENGTH), 0);
    RenderMesh2D(meshes["filled2bar"], shaders["VertexColor"], modelMatrix);

    if (!launched2Bullet) {
        prevAngle2 = angle;
        stepLaunch2 = 0.01f;
        bullet2X = INIT_SPEED * cos(M_PI / 2 - shooter2Rotate + angle) * 0.01f;
        bullet2Y = INIT_SPEED * sin(M_PI / 2 - shooter2Rotate + angle) * 0.01f - 0.5f * g * 0.01f * 0.01f;

        // Trajectory Drawing
        for (int i = 0; i < TRAJECTORY; i++) {
            modelMatrix = visMatrix;
            glm::vec3 bulletCoord = startBulletCoord(glm::vec3(newTank2X, newTank2Y, 0), TANK_LENGTH, shooter2Rotate);
            modelMatrix *= transform2D::Translate(bullet2X, bullet2Y);
            modelMatrix *= transform2D::Translate(0, t * TANK_LENGTH / 2 + ADJUST_ORIENTATION + 1);
            modelMatrix *= transform2D::Translate(cTank2X, cTank2Y);
            modelMatrix *= transform2D::Rotate(-angle);
            modelMatrix *= transform2D::Translate(-cTank2X, -cTank2Y);
            modelMatrix *= transform2D::Translate(bulletCoord[0], bulletCoord[1]);
            RenderMesh2D(meshes["tparticle"], shaders["VertexColor"], modelMatrix);

            stepLaunch2 += 0.005f;
            bullet2X = INIT_SPEED * cos(M_PI / 2 - shooter2Rotate + prevAngle2) * stepLaunch2;
            bullet2Y = INIT_SPEED * sin(M_PI / 2 - shooter2Rotate + prevAngle2) * stepLaunch2 - 0.5f * g * stepLaunch2 * stepLaunch2;
        }
    }

    // Verify if bullet is launched:
    if (render2Bullet || launched2Bullet) {
        if (!launched2Bullet) {
            prevAngle2 = angle;
            stepLaunch2 = deltaTimeSeconds;
            bullet2X = -INIT_SPEED * cos(M_PI / 2 - shooter2Rotate + angle) * deltaTimeSeconds;
            bullet2Y = INIT_SPEED * sin(M_PI / 2 - shooter2Rotate + angle) * deltaTimeSeconds - 0.5f * g * deltaTimeSeconds * deltaTimeSeconds;
        }
        else {
            stepLaunch2 += deltaTimeSeconds;
            bullet2X = -INIT_SPEED * cos(M_PI / 2 - shooter2Rotate + prevAngle2) * stepLaunch2;
            bullet2Y = INIT_SPEED * sin(M_PI / 2 - shooter2Rotate + prevAngle2) * stepLaunch2 - 0.5f * g * stepLaunch2 * stepLaunch2;
        }

        modelMatrix = visMatrix;
        glm::vec3 bulletCoord = startBulletCoord(glm::vec3(newTank2X, newTank2Y, 0), TANK_LENGTH, shooter2Rotate);
        bullet2X = -bullet2X;
        modelMatrix *= transform2D::Translate(bullet2X, bullet2Y);
        modelMatrix *= transform2D::Translate(0, t * TANK_LENGTH / 2 + ADJUST_ORIENTATION + 1);
        modelMatrix *= transform2D::Translate(cTank2X, cTank2Y);
        modelMatrix *= transform2D::Rotate(-angle);
        modelMatrix *= transform2D::Translate(-cTank2X, -cTank2Y);
        modelMatrix *= transform2D::Translate(bulletCoord[0], bulletCoord[1]);

        RenderMesh2D(meshes["bullet"], shaders["VertexColor"], modelMatrix);

        render2Bullet = false;
        launched2Bullet = true;

        glm::vec3 newBCoord = modelMatrix * glm::vec3(0, 0, 1);
        glm::vec3 tank1Center = visMatrix * glm::vec3(cTankX, cTankY, 1);

        // Verify if the bullet hit the enemy tank
        if (newBCoord[0] >= tank1Center[0] - HIT_RADIUS_X && newBCoord[0] <= tank1Center[0] + HIT_RADIUS_X + 10 &&
            newBCoord[1] >= tank1Center[1] - HIT_RADIUS_Y && newBCoord[1] <= tank1Center[1] + HIT_RADIUS_Y + 10) {
            tank1HP--;
            launched2Bullet = false;
        }

        float x, y;
        for (int i = 0; i < HEIGHT_MAP_LEVEL - 1; i++) {
            x = i * stepX * SCALE_FACTOR_X;
            y = heightMap[i];

            glm::vec3 terrainCoord = visMatrix * glm::vec3(x, y, 1);
            if (abs(terrainCoord[0] - newBCoord[0]) < TERRAIN_EPS && abs(terrainCoord[1] - newBCoord[1]) < TERRAIN_EPS
                || ((i == HEIGHT_MAP_LEVEL - 2) && newBCoord[0] >= terrainCoord[0] - TERRAIN_EPS) ||
                (i == 0 && newBCoord[0] <= terrainCoord[0] + TERRAIN_EPS)) {
                launched2Bullet = false;
                break;
            }
        }
    }

    RemoveMeshFromList(tank1);
    RemoveMeshFromList(tank2);
}

void Tema1::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    if (window->KeyHold(GLFW_KEY_LEFT)) {
        d1X -= deltaTime * TANK_SPEED;
        if (tank1X + d1X <= LEFT_BOUND_1) {
            d1X += deltaTime * TANK_SPEED;
        }
    }

    if (window->KeyHold(GLFW_KEY_RIGHT)) {
        d1X += deltaTime * TANK_SPEED;
        if (tank1X + d1X >= RIGHT_BOUND_1) {
            d1X -= deltaTime * TANK_SPEED;
        }
    }

    if (window->KeyHold(GLFW_KEY_UP)) {
        shooter1Rotate -= deltaTime * SHOOTER_SPEED;
        if (shooter1Rotate <= -M_PI / 2 + 0.2f) {
            shooter1Rotate = -M_PI / 2 + 0.2f;
        }
    }

    if (window->KeyHold(GLFW_KEY_DOWN)) {
        shooter1Rotate += deltaTime * SHOOTER_SPEED;
        if (shooter1Rotate >= M_PI / 2 - 0.2f) {
            shooter1Rotate = M_PI / 2 - 0.2f;
        }
    }

    if (window->KeyHold(GLFW_KEY_A)) {
        d2X -= deltaTime * TANK_SPEED;
        if (tank2X + d2X <= LEFT_BOUND_2) {
            d2X += deltaTime * TANK_SPEED;
        }
    }

    if (window->KeyHold(GLFW_KEY_D)) {
        d2X += deltaTime * TANK_SPEED;
        if (tank2X + d2X >= RIGHT_BOUND_2) {
            d2X -= deltaTime * TANK_SPEED;
        }
    }

    if (window->KeyHold(GLFW_KEY_W)) {
        shooter2Rotate -= deltaTime * SHOOTER_SPEED;
        if (shooter2Rotate <= -M_PI / 2 + 0.2f) {
            shooter2Rotate = -M_PI / 2 + 0.2f;
        }
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        shooter2Rotate += deltaTime * SHOOTER_SPEED;
        if (shooter2Rotate >= M_PI / 2 - 0.2f) {
            shooter2Rotate = M_PI / 2 - 0.2f;
        }
    }
}


void Tema1::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_ENTER) {
        if (!launched1Bullet) {
            render1Bullet = true;
        }
    }

    if (key == GLFW_KEY_SPACE) {
        if (!launched2Bullet) {
            render2Bullet = true;
        }
    }
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}
