#pragma once

#include <stdlib.h>
#include <cmath>
#include <cglm/cglm.h>

struct Camera
{
    vec3 position;
    vec3 lookat;
    vec3 up;

    float hAngle;
    float vAngle;
};

float relHAngle;
float relVAngle;
float relLookAtAngle;

void initializeCamera(vec3 position, vec3 lookat, vec3 up, Camera *c)
{
    glm_vec3_copy(position, c->position);
    glm_vec3_copy(lookat, c->lookat);
    glm_vec3_copy(up, c->up);

    glm_vec3_normalize(c->lookat);
    glm_vec3_normalize(c->up);

    vec3 Htarget = {c->lookat[0], 0, c->lookat[2]};
    glm_vec3_normalize(Htarget);

    float Angle = asin(abs(Htarget[2]));

    if (Htarget[2] >= 0)
    {
        if (Htarget[0] >= 0)
        {
            c->hAngle = 360 - glm_deg(Angle);
        }
        else
        {
            c->hAngle = 180 + glm_deg(Angle);
        }
    }
    else
    {
        if (Htarget[0] >= 0)
        {
            c->hAngle = glm_deg(Angle);
        }
        else
        {
            c->hAngle = 180 - glm_deg(Angle);
        }
    }

    c->vAngle = -glm_deg(asin(c->lookat[1]));
}

struct Quaternion {
    float x;
    float y;
    float z;
    float w;
};

void initializeQuaternion(Quaternion *q, vec3 v, float Angle)
{
    Angle = Angle / 2;
    float sinHalf = sin(glm_rad(Angle));
    float cosHalf = cos(glm_rad(Angle));
    q -> x = v[0] * sinHalf;
    q -> y = v[1] * sinHalf;
    q -> z = v[2] * sinHalf;
    q -> w = cosHalf;
}

void getQuaternionConjugate(Quaternion *q, Quaternion *qc)
{
    qc->x = -q->x;
    qc->y = -q->y;
    qc->z = -q->z;
    qc->w = q->w;
}

void QuatMulVector(Quaternion *q, vec3 v, Quaternion *result)
{
    float w = - (q->x * v[0]) - (q->y * v[1]) - (q->z * v[2]);
    float x =   (q->w * v[0]) + (q->y * v[2]) - (q->z * v[1]);
    float y =   (q->w * v[1]) + (q->z * v[0]) - (q->x * v[2]);
    float z =   (q->w * v[2]) + (q->x * v[1]) - (q->y * v[0]);

    result->w = w;
    result->x = x;
    result->y = y;
    result->z = z;
}

void QuatMulQuaternion(Quaternion *l, Quaternion *r, Quaternion *result)
{
    float w = (l->w * r->w) - (l->x * r->x) - (l->y * r->y) - (l->z * r->z);
    float x = (l->x * r->w) + (l->w * r->x) + (l->y * r->z) - (l->z * r->y);
    float y = (l->y * r->w) + (l->w * r->y) + (l->z * r->x) - (l->x * r->z);
    float z = (l->z * r->w) + (l->w * r->z) + (l->x * r->y) - (l->y * r->x);

    result->w = w;
    result->x = x;
    result->y = y;
    result->z = z;
}

void Rotate(vec3 V, vec3 axis, float Angle)
{
    Quaternion result;
    Quaternion rotationQ;
    Quaternion rotationQConjugate;
    initializeQuaternion(&rotationQ, axis, Angle);
    getQuaternionConjugate(&rotationQ, &rotationQConjugate);

    QuatMulVector(&rotationQ, V, &result);
    QuatMulQuaternion(&result, &rotationQConjugate, &result);

    V[0] = result.x;
    V[1] = result.y;
    V[2] = result.z;
}

void generateViewMatrix(Camera *c, mat4 view)
{
    mat4 cameraAxis;
    mat4 cameraTranslate;
    glm_mat4_identity(cameraAxis);
    glm_mat4_identity(cameraTranslate);

    vec3 U;
    vec3 V;
    vec3 N;

    glm_vec3_normalize_to(c->lookat, N);
    glm_vec3_cross(c->up, N, U);
    glm_vec3_normalize(U);

    glm_vec3_cross(N, U, V);

    cameraAxis[0][0] = U[0]; cameraAxis[0][1] = U[1]; cameraAxis[0][2] = U[2]; cameraAxis[0][3] = 0.0f;
    cameraAxis[1][0] = V[0]; cameraAxis[1][1] = V[1]; cameraAxis[1][2] = V[2]; cameraAxis[1][3] = 0.0f;
    cameraAxis[2][0] = N[0]; cameraAxis[2][1] = N[1]; cameraAxis[2][2] = N[2]; cameraAxis[2][3] = 0.0f;
    cameraAxis[3][0] = 0.0f; cameraAxis[3][1] = 0.0f; cameraAxis[3][2] = 0.0f; cameraAxis[3][3] = 1.0f;

    glm_translate(cameraTranslate, c->position);
    glm_mat4_mul(cameraAxis, cameraTranslate, view);
}

void cameraUpdate(Camera *c)
{
    vec3 yAxis = {0.0f, 1.0f, 0.0f};

    // rotate horizontally
    vec3 lookat = {1.0f, 0.0f, 0.0f};
    Rotate(lookat, yAxis, c->hAngle);
    glm_vec3_normalize(lookat);

    vec3 U;
    vec3 V;

    // rotate vertically
    glm_vec3_cross(yAxis, lookat, U);
    glm_vec3_normalize(U);
    Rotate(lookat, U, c->vAngle);

    glm_vec3_copy(lookat, c->lookat);
    glm_vec3_normalize(c->lookat);

    glm_vec3_cross(c->lookat, U, c->up);
    glm_vec3_normalize(c->up);
}

float relativePosition(Camera *c1, Camera *c2)
{
    return glm_vec3_distance(c1->lookat, c2->lookat);
}

void relativeOrientation(Camera *c1, Camera *c2)
{
    vec3 U_1;
    vec3 V_1;
    vec3 N_1;

    vec3 U_2;
    vec3 V_2;
    vec3 N_2;

    glm_vec3_normalize_to(c1->lookat, N_1);
    glm_vec3_cross(c1->up, N_1, U_1);
    glm_vec3_normalize(U_1);

    glm_vec3_cross(N_1, U_1, V_1);


    glm_vec3_normalize_to(c2->lookat, N_2);
    glm_vec3_cross(c2->up, N_2, U_2);
    glm_vec3_normalize(U_2);

    glm_vec3_cross(N_2, U_2, V_2);

    relHAngle = glm_vec3_angle(U_1, U_2);
    relVAngle = glm_vec3_angle(V_1, V_2);
    relLookAtAngle = glm_vec3_angle(N_1, N_2);
}
