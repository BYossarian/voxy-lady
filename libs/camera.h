
#pragma once

#include <cmath>
#include <algorithm>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "./window.h"
#include "./aabb.h"

// a typical FPS Camera (i.e. there's no roll)
class Camera {

public:

    Camera(glm::vec3 initialPosition, GLfloat initialYaw, GLfloat initialPitch, 
                GLfloat initialMovementSpeed, GLfloat initialTurnSpeed, GLfloat initialFovYAngle,
                GLfloat initialAspectRatio, GLfloat initialNearPlaneDistance, GLfloat initialFarPlaneDistance);

    glm::mat4 calcualateViewMatrix();
    glm::mat4 calcualateProjectionMatrix();

    void update(double deltaTime, Window &window);

    bool canSee(const AABB &box) const;

    const glm::vec3& getPosition() const { return position; };
    const glm::vec3& getDirection() const { return front; };

    // prevent copy and copy-assignment
    Camera& operator=(const Camera&) = delete;
    Camera(const Camera&) = delete;

private:

    // limit the pitch to -89 -> 89 degrees
    static constexpr GLfloat MAX_PITCH = M_PI * (89.0f / 180.0f);
    static constexpr GLfloat MIN_PITCH = M_PI * (-89.0f / 180.0f);
    static constexpr GLfloat TWO_PI = 2 * M_PI;

    static constexpr glm::vec3 WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up;

    GLfloat yaw;
    GLfloat pitch;

    GLfloat movementSpeed;
    GLfloat turnSpeed;

    GLfloat fovYAngle;
    GLfloat aspectRatio;
    GLfloat nearPlaneDistance;
    GLfloat farPlaneDistance;
    AABB frustrumAABB;

    void updateDirectionVectors();
    void updateFrustrumAABB();

};

Camera::Camera(glm::vec3 initialPosition, GLfloat initialYaw, GLfloat initialPitch, 
                GLfloat initialMovementSpeed, GLfloat initialTurnSpeed, GLfloat initialFovYAngle,
                GLfloat initialAspectRatio, GLfloat initialNearPlaneDistance, GLfloat initialFarPlaneDistance)
    : position(initialPosition), yaw(initialYaw), pitch(initialPitch), 
        movementSpeed(initialMovementSpeed), turnSpeed(initialTurnSpeed),
        fovYAngle(initialFovYAngle), aspectRatio(initialAspectRatio), nearPlaneDistance(initialNearPlaneDistance), 
        farPlaneDistance(initialFarPlaneDistance) {

            updateDirectionVectors();
            updateFrustrumAABB();

        }

glm::mat4 Camera::calcualateViewMatrix() {
    // NB: in documentation/tutorials, there's seems to be some confusion about 
    // whether the third argument to lookAt should be the camera's up or the 
    // world's up. However - after looking at the glm::lookAt source code - in 
    // the case of our Camera it doesn't actually make a difference because we
    // don't use/allow roll (which is probably why there's confusion)
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::calcualateProjectionMatrix() {
    return glm::perspective(fovYAngle, aspectRatio, nearPlaneDistance, farPlaneDistance);
}

void Camera::updateDirectionVectors() {

    front.x = cos(yaw) * cos(pitch);
    front.y = sin(pitch);
    front.z = sin(yaw) * cos(pitch);
    front = glm::normalize(front);

    right = glm::normalize(glm::cross(front, WORLD_UP));

    up = glm::normalize(glm::cross(right, front));

}

void Camera::updateFrustrumAABB() {

    frustrumAABB.xMin = position.x;
    frustrumAABB.xMax = position.x;
    frustrumAABB.yMin = position.y;
    frustrumAABB.yMax = position.y;
    frustrumAABB.zMin = position.z;
    frustrumAABB.zMax = position.z;

    GLfloat farFaceHalfHeight = farPlaneDistance * tan(fovYAngle);
    glm::vec3 farFaceHalfUp = farFaceHalfHeight * up;
    glm::vec3 farFaceHalfRight = farFaceHalfHeight * aspectRatio * right;
    glm::vec3 farFaceCentre = position + farPlaneDistance * front;

    // iterate over the four corners of the far face of the view frustrum, and 
    // expand the frustrumAABB to include them:
    for (int i = -1; i <= 1; i += 2) {
        for (int j = -1; j <= 1; j += 2) {

            glm::vec3 corner = farFaceCentre + static_cast<float>(i) * farFaceHalfUp + static_cast<float>(j) * farFaceHalfRight;

            frustrumAABB.xMin = std::min(frustrumAABB.xMin, corner.x);
            frustrumAABB.xMax = std::max(frustrumAABB.xMax, corner.x);
            frustrumAABB.yMin = std::min(frustrumAABB.yMin, corner.y);
            frustrumAABB.yMax = std::max(frustrumAABB.yMax, corner.y);
            frustrumAABB.zMin = std::min(frustrumAABB.zMin, corner.z);
            frustrumAABB.zMax = std::max(frustrumAABB.zMax, corner.z);

        }
    }

}

void Camera::update(double deltaTime, Window &window) {

    const bool* keys = window.getKeyStates();

    GLfloat displacementMagnitude = movementSpeed * deltaTime;

    if (keys[GLFW_KEY_W]) {
        position += front * displacementMagnitude;
    }

    if (keys[GLFW_KEY_S]) {
        position -= front * displacementMagnitude;
    }

    if (keys[GLFW_KEY_A]) {
        position -= right * displacementMagnitude;
    }

    if (keys[GLFW_KEY_D]) {
        position += right * displacementMagnitude;
    }

    if (keys[GLFW_KEY_Q]) {
        position += up * displacementMagnitude;
    }

    if (keys[GLFW_KEY_E]) {
        position -= up * displacementMagnitude;
    }

    GLfloat xMouseChange = window.getMouseXChange();
    GLfloat yMouseChange = window.getMouseYChange();

    yaw += xMouseChange * turnSpeed;
    pitch -= yMouseChange * turnSpeed;

    if (pitch > MAX_PITCH) {
        pitch = MAX_PITCH;
    } else if (pitch < MIN_PITCH) {
        pitch = MIN_PITCH;
    }

    if (yaw < 0) {
        yaw += TWO_PI;
    } else if (yaw > TWO_PI) {
        yaw -= TWO_PI;
    }

    updateDirectionVectors();
    updateFrustrumAABB();

}

bool Camera::canSee(const AABB &box) const {

    // TODO: use SAT for tighter frustum/box intersection
    return frustrumAABB.intersects(box);

}
