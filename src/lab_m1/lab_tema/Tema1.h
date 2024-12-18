#pragma once

#include "components/simple_scene.h"

constexpr auto HEIGHT_MAP_LEVEL = 1112;
constexpr auto g = 6000;

#define SCALE_FACTOR_X 15
#define SCALE_FACTOR_Y 27
#define LIFT_UP -2.3f

#define TANK_1_OFF 100
#define TANK_2_OFF 800
#define SHOOTER_OFF 0.785f

#define ADJUST_ORIENTATION -0.5f

#define TANK_LENGTH 20.0f
#define TANK_SPEED 130.0f

#define LEFT_BOUND_1 -35.0f
#define LEFT_BOUND_2 -420.0f
#define RIGHT_BOUND_1 995.0f
#define RIGHT_BOUND_2 610.0f

#define BULLET_RADIUS 2.0f
#define INIT_SPEED 1000

#define HIT_RADIUS_X 30.0f
#define HIT_RADIUS_Y 50.0f
#define TERRAIN_EPS 20.0f
#define EXPLOSION 20
#define TRAJECTORY 100

#define HP 5

#define SHOOTER_SPEED 1

namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
     public:
        struct ViewportSpace
        {
            ViewportSpace() : x(0), y(0), width(1), height(1) {}
            ViewportSpace(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        struct LogicSpace
        {
            LogicSpace() : x(0), y(0), width(1), height(1) {}
            LogicSpace(float x, float y, float width, float height)
                : x(x), y(y), width(width), height(height) {}
            float x;
            float y;
            float width;
            float height;
        };

     public:
        Tema1();
        ~Tema1();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void DrawScene(glm::mat3 visMatrix, float deltaTimeSeconds);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        glm::mat3 VisualizationTransf2D(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);
        glm::mat3 VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);
        glm::vec3 startBulletCoord(glm::vec3 tankCoordinates, float tankLength, float cannonAngle);

        void SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);

    protected:
        float translateX, translateY;
        float scaleX, scaleY;
        float angularStep;
        int orientation;
        float length;
        ViewportSpace viewSpace;
        LogicSpace logicSpace;
        glm::mat3 modelMatrix, visMatrix;
        float* heightMap;
        float stepX;
        float shooter1Rotate, shooter2Rotate;
        float tank1X, tank1Y, tank2Y, tank2X;
        int map1Index, map2Index;
        float d1X, d2X;
        bool render1Bullet, render2Bullet;
        float bullet1X, bullet1Y, bullet2X, bullet2Y;
        float stepLaunch1, stepLaunch2, prevAngle1, prevAngle2;
        bool launched1Bullet, launched2Bullet, max1Reached, max2Reached;
        int tank1HP, tank2HP;
    };
}   // namespace m1
