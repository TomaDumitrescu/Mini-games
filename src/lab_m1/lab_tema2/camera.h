#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"


namespace implemented
{
    class DroneCamera
    {
    public:
        DroneCamera()
        {
            position = glm::vec3(0, 2, 5);
            forward = glm::vec3(0, 0, -1);
            up = glm::vec3(0, 1, 0);
            right = glm::vec3(1, 0, 0);
            distanceToTarget = 2;
        }

        DroneCamera(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up)
        {
            Set(position, center, up);
        }

        ~DroneCamera()
        { }

        // Update DroneCamera
        void Set(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up)
        {
            this->position = position;
            forward = glm::normalize(center - position);
            right = glm::cross(forward, up);
            this->up = glm::cross(right, forward);
        }

        void MoveForward(float distance)
        {
            glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
            position += dir * distance;
        }

        void TranslateForward(float distance)
        {
            // Translate the DroneCamera using the `forward` vector.
            // What's the difference between `TranslateForward()` and
            // `MoveForward()`?
            position += glm::normalize(forward) * distance;
        }

        void TranslateUpward(float distance)
        {
            // Translate the DroneCamera using the `up` vector.
            position += glm::normalize(up) * distance;

        }

        void TranslateRight(float distance)
        {
            glm::vec3 projected_xoz = glm::vec3(right.x, 0, right.z);
            position += glm::normalize(projected_xoz) * distance;
        }

        void RotateFirstPerson_OX(float angle)
        {
            // Compute the new `forward` and `up` vectors.
            // Don't forget to normalize the vectors! Use `glm::rotate()`.
            glm::mat4 rotate_world_ox = glm::rotate(glm::mat4(1.0f), angle, right);
            forward = glm::normalize(glm::vec3(rotate_world_ox * glm::vec4(forward, 1.0f)));
            up = glm::normalize(glm::cross(right, forward));
        }

        void RotateFirstPerson_OY(float angle)
        {
            // Compute the new `forward`, `up` and `right`
            // vectors. Use `glm::rotate()`. Don't forget to normalize the
            // vectors!
            glm::mat4 rotate_world_oy = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
            forward = glm::normalize(glm::vec3(rotate_world_oy * glm::vec4(forward, 1.0f)));
            right = glm::normalize(glm::vec3(rotate_world_oy * glm::vec4(right, 1.0f)));
            up = glm::normalize(glm::cross(right, forward));
        }

        void RotateFirstPerson_OZ(float angle)
        {
            // Compute the new `right` and `up`. This time,
            // `forward` stays the same. Use `glm::rotate()`. Don't forget
            // to normalize the vectors!
            glm::mat4 rotate_world_oz = glm::rotate(glm::mat4(1.0f), angle, forward);
            right = glm::normalize(glm::vec3(rotate_world_oz * glm::vec4(right, 1.0f)));
            up = glm::normalize(glm::cross(right, forward));
        }

        void RotateThirdPerson_OX(float angle)
        {
            // Rotate the DroneCamera in third-person mode around the OX axis.
            // Use `distanceToTarget` as translation distance.
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OX(angle);
            TranslateForward(-distanceToTarget);
        }

        void RotateThirdPerson_OY(float angle)
        {
            // Rotate the DroneCamera in third-person mode around the OY axis.
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OY(angle);
            TranslateForward(-distanceToTarget);
        }

        void RotateThirdPerson_OZ(float angle)
        {
            // Rotate the DroneCamera in third-person mode around the OZ axis.
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OZ(angle);
            TranslateForward(-distanceToTarget);
        }

        glm::mat4 GetViewMatrix()
        {
            // Returns the view matrix
            return glm::lookAt(position, position + forward, up);
        }

        glm::vec3 GetTargetPosition()
        {
            return position + forward * distanceToTarget;
        }

    public:
        float distanceToTarget;
        glm::vec3 position;
        glm::vec3 forward;
        glm::vec3 right;
        glm::vec3 up;
    };
}   // namespace implemented
