#pragma once

#include "components/simple_scene.h"

#define SPEED 3.0f
#define OMEGA 2.1f
#define PROP_OMEGA 1.75f
#define DRONE_SIZE 2.0f
#define OVERFLOW 10000000.0f
#define MAX_INCLINE M_PI / 12
#define INCLINATION_SPEED 0.4f

#define NUM_OBJECTS 30
#define ZMAXVAL 20
#define XMAXVAL 20
#define EPS 0.35f

namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
     public:
        struct ViewportArea
        {
            ViewportArea() : x(0), y(0), width(1), height(1) {}
            ViewportArea(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        Tema2();
        ~Tema2();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void collisionCheck();
        void drawCloud(float x, float y, float z, float size);

        void RenderScene();
     protected:
        glm::mat4 modelMatrix;
        float translateX, translateY, translateZ;
        float scaleX, scaleY, scaleZ;
        float angularStepOX, angularStepOY, angularStepOZ;
        float viewPortX, viewPortY, widthPort, heightPort;
        float droneInclinationLR;
        float droneInclinationUD;
        float propellerAngle;
        float droneAngle;
        int objX[NUM_OBJECTS];
        int objZ[NUM_OBJECTS];
        int objType[NUM_OBJECTS];
        float objSize[NUM_OBJECTS];
        glm::vec3 initDrone;
        GLenum polygonMode;
        ViewportArea miniViewportArea;
    };
}   // namespace m1

