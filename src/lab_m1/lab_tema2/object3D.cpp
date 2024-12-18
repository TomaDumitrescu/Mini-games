#include "object3D.h"
#include "transform3D.h"

#include <vector>
#include <iostream>

#include "core/engine.h"
#include "utils/gl_utils.h"

Mesh* object3D::CreateDepo(const std::string& name, glm::vec3 center, float l, glm::vec3 color, glm::mat4 modelMatrix)
{
    std::vector<VertexFormat> vertices =
    {
        VertexFormat(center + glm::vec3(-l / 2, - l/2, -l/2), color),
        VertexFormat(center + glm::vec3(l / 2, -l / 2, -l / 2), color),
        VertexFormat(center + glm::vec3(l / 2, -l / 2, l / 2), color),
        VertexFormat(center + glm::vec3(-l / 2, -l / 2, l / 2), color),
        VertexFormat(center + glm::vec3(-l / 2, l / 2, -l / 2), color),
        VertexFormat(center + glm::vec3(l / 2, l / 2, -l / 2), color),
        VertexFormat(center + glm::vec3(l / 2, l / 2, l / 2), color),
        VertexFormat(center + glm::vec3(-l / 2, l / 2, l / 2), color),
    };

    Mesh* cube = new Mesh(name);
    std::vector<unsigned int> indices = {
        0, 1, 5, 0, 5, 4,
        2, 3, 6, 6, 3, 7,
        0, 3, 4, 4, 3, 7,
        1, 5, 2, 5, 6, 2,
        0, 1, 3, 1, 2, 3,
        4, 7, 5, 7, 6, 5
    };

    for (VertexFormat& v : vertices) {
        glm::vec4 new_v = modelMatrix * glm::vec4(v.position, 1.0f);
        v.position = glm::vec3(new_v);
    }

    cube->InitFromData(vertices, indices);
    return cube;
}

void UsePrimitive(std::vector<VertexFormat>& vertices, std::vector<unsigned int>& indices,
                    glm::mat4 modelMatrix, glm::vec3 color) {
    std::vector<glm::vec3> cubeV = {
        {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, 0.5f}, {-0.5f, -0.5f, 0.5f},
        {-0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}
    };

    for (auto& v : cubeV) {
        vertices.emplace_back(glm::vec3(modelMatrix * glm::vec4(v, 1.0f)), color);
    }

    std::vector<unsigned int> newIndices = {
        0, 1, 5, 0, 5, 4, 1, 2, 6, 1, 6, 5,
        2, 3, 7, 2, 7, 6, 3, 0, 4, 3, 4, 7,
        0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7
    };

    for (auto idx : newIndices) {
        indices.push_back(vertices.size() - cubeV.size() + idx);
    }
}

Mesh* object3D::CreateDrone(const std::string& name, glm::vec3 location, float alpha)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 XColor = glm::vec3(0.941f, 0.941f, 0.941f);
    glm::vec3 propellersColor = glm::vec3(0.0f, 0.0f, 0.0f);

    // Creating the drone skeleton
    glm::mat4 XModelMatrix = glm::mat4(1);
    XModelMatrix *= transform3D::Translate(location.x, location.y, location.z);
    XModelMatrix *= transform3D::RotateOY(M_PI / 4);
    UsePrimitive(vertices, indices, XModelMatrix * transform3D::Scale(2.0f, 0.15f, 0.3f), XColor);

    glm::mat4 X2ModelMatrix = XModelMatrix;
    X2ModelMatrix *= transform3D::RotateOY(M_PI / 2);
    X2ModelMatrix *= transform3D::Scale(2.0f, 0.15f, 0.3f);
    UsePrimitive(vertices, indices, X2ModelMatrix, XColor);

    // Adding top elements
    glm::vec3 cubeCoords[4] = {
    {-0.88f, 0.15f, 0.0f}, {0.88f, 0.15f, 0.0f},
    {0.0f, 0.15f, -0.88f}, {0.0f, 0.15f, 0.88f}
    };

    for (auto& coord : cubeCoords) {
        glm::mat4 primitiveModelMatrix = XModelMatrix;
        primitiveModelMatrix *= transform3D::Translate(coord.x, coord.y, coord.z);
        primitiveModelMatrix *= transform3D::Scale(0.25f, 0.25f, 0.25f);
        UsePrimitive(vertices, indices, primitiveModelMatrix, XColor);

        glm::mat4 propellerModelMatrix = primitiveModelMatrix;
        propellerModelMatrix *= transform3D::Translate(0.0f, 0.5f, 0.0f);
        // The drone will be loaded and cleared from the meshes hashmap every scene
        propellerModelMatrix *= transform3D::RotateOY(glm::radians(alpha));
        propellerModelMatrix *= transform3D::Scale(0.3f, 0.2f, 2.4f);
        UsePrimitive(vertices, indices, propellerModelMatrix, propellersColor);
    }

    Mesh* drone = new Mesh(name);
    drone->InitFromData(vertices, indices);
    return drone;
}

Mesh* object3D::CreateTree(const std::string& name, glm::vec3 center, float size) {
    int detailLevel = 36;
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 baseColor = glm::vec3(0.341f, 0.176f, 0.024f);
    glm::vec3 leaveColor = glm::vec3(0.2823f, 0.3686f, 0.3215f);

    float trunkH = size * 0.5f;
    float trunkR = size * 0.1f;
    float coneH = size * 0.6f;
    float coneR = size * 0.4f;

    unsigned int start = 0;

    // Cylinder
    vertices.emplace_back(center, baseColor);
    for (int i = 0; i <= detailLevel; ++i) {
        float alpha = 2.0f * M_PI * (1.0 * i) / detailLevel;
        vertices.emplace_back(center + glm::vec3(trunkR * cos(alpha), 0.0f, trunkR * sin(alpha)), baseColor);
    }

    glm::vec3 trunkTop = center + glm::vec3(0.0f, trunkH, 0.0f);
    vertices.emplace_back(trunkTop, baseColor);
    for (int i = 0; i <= detailLevel; ++i) {
        float alpha = 2.0f * M_PI * (1.0 * i) / detailLevel;
        vertices.emplace_back(trunkTop + glm::vec3(trunkR * cos(alpha), 0.0f, trunkR * sin(alpha)), baseColor);
    }


    for (int i = 1; i <= detailLevel; ++i) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    for (int i = 1; i <= detailLevel; ++i) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(detailLevel + 2 + i);

        indices.push_back(i + 1);
        indices.push_back(detailLevel + 2 + i);
        indices.push_back(detailLevel + 2 + i + 1);
    }

    start = vertices.size();

    // First Cone
    glm::vec3 circle1 = trunkTop;
    glm::vec3 top1 = circle1 + glm::vec3(0.0f, coneH, 0.0f);
    vertices.emplace_back(top1, leaveColor);
    for (int i = 0; i <= detailLevel; ++i) {
        float alpha = 2.0f * M_PI * (1.0 * i) / detailLevel;
        vertices.emplace_back(circle1 + glm::vec3(coneR * cos(alpha), 0.0f, coneR * sin(alpha)), leaveColor);
    }

    for (int i = 1; i <= detailLevel; ++i) {
        indices.push_back(start);
        indices.push_back(start + i);
        indices.push_back(start + i + 1);
    }

    start = vertices.size();

    // Second Cone
    glm::vec3 circle2 = top1 * 0.8f;
    glm::vec3 top2 = circle2 + glm::vec3(0.0f, coneH, 0.0f);
    vertices.emplace_back(top2, leaveColor);
    for (int i = 0; i <= detailLevel; ++i) {
        float alpha = 2.0f * glm::pi<float>() * float(i) / float(detailLevel);
        vertices.emplace_back(circle2 + glm::vec3(coneR * cos(alpha), 0.0f, coneR * sin(alpha)), leaveColor);
    }
    for (int i = 1; i <= detailLevel; ++i) {
        indices.push_back(start);
        indices.push_back(start + i);
        indices.push_back(start + i + 1);
    }


    Mesh* tree = new Mesh(name);
    tree->InitFromData(vertices, indices);
    return tree;
}

#define NUM_ROWS 100
#define NUM_COLUMNS 100

Mesh* object3D::drawTerrain(const std::string& name, float size, glm::vec3 color)
{
    float halfWidth = (NUM_COLUMNS * size) / 2.0f;
    float halfLength = (NUM_ROWS * size) / 2.0f;
    std::vector<VertexFormat> vertices;
    for (int r = 0; r <= NUM_ROWS; ++r) {
        for (int col = 0; col <= NUM_COLUMNS; ++col) {
            vertices.push_back(VertexFormat(glm::vec3(col * size - halfWidth, 0.0f, r * size - halfLength), color));
        }
    }

    std::vector<unsigned int> indices;
    for (int r = 0; r < NUM_ROWS; ++r) {
        for (int c = 0; c < NUM_COLUMNS; ++c) {
            int cadrane2 = r * (NUM_COLUMNS + 1) + c;
            int cadrane1 = cadrane2 + 1;
            int cadrane3 = (r + 1) * (NUM_COLUMNS + 1) + c;
            int cadrane4 = cadrane3 + 1;

            // First tralpha
            indices.push_back(cadrane2);
            indices.push_back(cadrane3);
            indices.push_back(cadrane1);

            // Second tralpha
            indices.push_back(cadrane1);
            indices.push_back(cadrane3);
            indices.push_back(cadrane4);
        }
    }

    Mesh* terrain = new Mesh(name);
    terrain->InitFromData(vertices, indices);
    return terrain;
}

Mesh* object3D::CreateSphere(const std::string& name, glm::vec3 center, float r, glm::vec3 color)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    int detailLevel = 35;
    for (int i = 0; i <= detailLevel; ++i) {
        float alpha = M_PI / detailLevel * i - M_PI / 2.0f;

        for (int j = 0; j <= detailLevel; ++j) {
            float gamma = 2 * M_PI / detailLevel * j;
            vertices.emplace_back(center + glm::vec3(r * cos(alpha) * cos(gamma), r * sin(alpha),
                                    r * cos(alpha) * sin(gamma)), color);
        }
    }

    for (int i = 0; i < detailLevel; ++i) {
        for (int j = 0; j < detailLevel; j += 1) {
            int first = i * (detailLevel + 1) + j;
            int second = first + detailLevel + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    Mesh* sphere = new Mesh(name);
    sphere->InitFromData(vertices, indices);
    return sphere;
}
