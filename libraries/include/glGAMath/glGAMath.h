//
//  glGAMath.h
//
//
//  Created by George Papagiannakis on 23/1/13.
//  Copyright (c) 2013 UoC & FORTH. All rights reserved.
//

#define glGACharacterApp_glGAMath_h

#include <assimp/anim.h>
#include <assimp/vector3.h>
#include <assimp/quaternion.h>
#include <assimp/matrix4x4.h>
#include <assimp/matrix3x3.h>
#ifndef GLM_SWIZZLE
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/constants.hpp>

#define QUAT 0
#define DQUAT 1

#define GA_GAIGEN 2
#define CGA_GAIGEN 3

#define GA_VERSOR 4
#define CGA_VERSOR 5

#define GA_GAALOP 6
#define CGA_GAALOP 7

#define INTERPOLATION QUAT

#if INTERPOLATION==GA_VERSOR || INTERPOLATION==CGA_VERSOR
#include "space/vsr_cga3D_types.h"
#endif

#define ROTATIONX 0
#define ROTATIONY 1
#define ROTATIONZ 2

/*
 Dorst, L., Fontijne, D., and Mann, S. 2010. Geometric Algebra for Computer Science. Morgan Kaufmann.
 */
#define USE_LIBGASANDBOX
#ifdef USE_LIBGASANDBOX

#include <glGAMath/c3ga.h>
#include <glGAMath/c3ga_util.h>
#endif

struct aiRotor2
{
    
};
struct aiRotor
{
    float w;
    float x;
    float y;
    float z;
    
    aiRotor()
    {
        this->w = 0;
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }
    
    aiRotor(float w, float x, float y, float z)
    {
        this->w = w;
        this->x = x;
        this->y = y;
        this->z = z;
    }
    
    aiRotor& operator=(aiRotor a) {
        this->w = a.w;
        this->x = a.x;
        this->y = a.y;
        this->z = a.z;
        return *this;
    }
};

struct RotorDef
{
    float scalar; //0
    float e1e2; //6;
    float e1e3; //7;
    float e1einf; //8;
    float e2e3; //10;
    float e2einf; //11;
    float e3einf; //13;
    float e2e3einf;  //26;
    
    RotorDef()
    {
        
    }
    RotorDef(float scalar,
             float e1e2,
             float e1e3,
             float e1einf,
             float e2e3,
             float e2einf,
             float e3einf,
             float e2e3einf)
    {
        this->scalar=scalar;
        this->e1e2=e1e2;
        this->e1e3=e1e3;
        this->e1einf=e1einf;
        this->e2e3=e2e3;
        this->e2einf=e2einf;
        this->e3einf=e3einf;
        this->e2e3einf=e2e3einf;
    }
    
    RotorDef& operator=(const RotorDef& other) // copy assignment
    {
        this->scalar=other.scalar;
        this->e1e2=other.e1e2;
        this->e1e3=other.e1e3;
        this->e1einf=other.e1einf;
        this->e2e3=other.e2e3;
        this->e2einf=other.e2einf;
        this->e3einf=other.e3einf;
        this->e2e3einf=other.e2e3einf;
        return *this;
    }
};


struct RotorDefScale
{
    float scalar; //0
    float e1e2; //6;
    float e1e3; //7;
    float e1einf; //8;
    float e2e3; //10;
    float e2einf; //11;
    float e3einf; //13;
    float e15; //15;
    float e2e3einf;  //26;
    float e28; //28
    float e29; //29
    float e30; //30
    
    RotorDefScale()
    {
        
    }
    RotorDefScale(float scalar,
                  float e1e2,
                  float e1e3,
                  float e1einf,
                  float e2e3,
                  float e2einf,
                  float e3einf,
                  float e2e3einf,float e15, float e28,
                  float e29,
                  float e30)
    {
        this->scalar=scalar;
        this->e1e2=e1e2;
        this->e1e3=e1e3;
        this->e1einf=e1einf;
        this->e2e3=e2e3;
        this->e2einf=e2einf;
        this->e3einf=e3einf;
        this->e2e3einf=e2e3einf;
        this->e15=e15;
        this->e28=e28; //28
        this->e29=e29; //29
        this->e30=e30; //30
    }
    
    RotorDefScale& operator=(const RotorDefScale& other) // copy assignment
    {
        this->scalar=other.scalar;
        this->e1e2=other.e1e2;
        this->e1e3=other.e1e3;
        this->e1einf=other.e1einf;
        this->e2e3=other.e2e3;
        this->e2einf=other.e2einf;
        this->e3einf=other.e3einf;
        this->e2e3einf=other.e2e3einf;
        this->e15=other.e15;
        this->e28=other.e28; //28
        this->e29=other.e29; //29
        this->e30=other.e30; //30
        return *this;
    }
};

/*
   ********************************** GLM Helper functions **********************
*/

void    printVec4GLM(std::string vecName,glm::vec4& vec);
void    printQuatfGML(std::string  quatName,glm::quat& quat);
void    printMat4GML(std::string  matName,glm::mat4& mat);

/*
 Gaigen 3D homogeneous model GA classes
 from Fontijne, D. 2006. Gaigen 2: a Geometric Algebra Implementation Generator. Proceedings of ACM GPCE 2006, 141–151.
 and
 Dorst, L., Fontijne, D., and Mann, S. 2010. Geometric Algebra for Computer Science. Morgan Kaufmann.
 */


/**  A GA Euclidean model interpolation method: 
 input: Start, End RotationQ quaternions
 output: Out quaternion
 method converts i/o quaternions to Euclidean model GA rotors (based on Gaigen and libGASandbox) and factor 'n' interpolation steps.
 
 Papagiannakis, G. 2013. Geometric algebra rotors for skinned character animation blending. Technical Brief, ACM SIGGRAPH ASIA 2013, Hong Kong, November 2013, 1–6.
 **/

glm::mat4 makeMat4(c3ga::flatPoint imageOfE1NI, c3ga::flatPoint imageOfE2NI, c3ga::flatPoint imageOfE3NI, c3ga::flatPoint imageOfNONI);

/**************  Euclidean Geometric Algebra (GA) Interpolation - Gaigen  *******************/
glm::mat4 interpolateGA_GAIGEN(e3ga::rotor StartRotationQ, e3ga::rotor  EndRotationQ, float Factor, float GAfactorDenominator, glm::quat start);
/***********************************************************************/

/**************  Conformal Geometric Algebra (CGA) Interpolation - Gaigen  *******************/
c3ga::TRSversor interpolateCGA1Trs2Rot(glm::vec3 translation1, c3ga::rotor srcQ, c3ga::rotor destQ, float Factor, float GAfactorDenominator);
c3ga::TRSversor interpolateCGA(glm::vec3 translation1, glm::vec3 translation2, c3ga::rotor srcQ, c3ga::rotor destQ, float Factor, float GAfactorDenominator);
c3ga::TRSversor interpolateCGA2Trs1Rot(glm::vec3 translation1, glm::vec3 translation2, c3ga::rotor srcQ, float Factor, float GAfactorDenominator);
c3ga::TRversor trRotGaigen(glm::vec3 translation, c3ga::rotor StartRotationQ);
/***********************************************************************/

/********************* Interpolation GA_GAALOP ************************/
void interpolateGA_GAALOP(aiQuaternion& Out, aiRotor src, aiRotor dst, float Factor);
/***********************************************************************/

/********************* Interpolation CGA_GAALOP ************************/
float* mulVersors(float* motor1, float* motor2);
RotorDef interpolateCGA_GAALOP(glm::vec3 translation1, glm::vec3 translation2, aiRotor srcQ, aiRotor dstQ, float alpha);
RotorDef TrRotToVersor(glm::vec3 translation1, aiRotor srcQ);
RotorDef translationInterpolateRotationCGA_GAALOP(glm::vec3 translation1, aiRotor srcQ, aiRotor destQ, float Factor);
RotorDef rotationInterpolateTranslationCGA_GAALOP(glm::vec3 translation1, glm::vec3 translation2, aiRotor srcQ, float alpha);

//geometric product motor and dilator
RotorDefScale gpMotorDilator(RotorDef motor, float Dilator);

glm::mat4 GAALOPVersorToMat4(float* finalR);
glm::mat4 GAALOPVersorToMat4Scale(float* finalR);
glm::mat4 RotorDefToMat4(RotorDef finalR);
/***********************************************************************/

#if INTERPOLATION==GA_VERSOR || INTERPOLATION==CGA_VERSOR
/********************* Interpolation GA_VERSOR ************************/
glm::mat4 interpolateGAVersor(vsr::cga::Rot srcQ, vsr::cga::Rot destQ, float Factor, float GAfactorDenominator);
/***********************************************************************/

/********************* Interpolation CGA_VERSOR ************************/
vsr::cga::Mot interpolateCGAVersor1Pos2Rot(glm::vec3 translation1, vsr::cga::Rot srcR, vsr::cga::Rot dstR, float Factor, float GAfactorDenominator);
vsr::cga::Mot interpolateCGAVersor(glm::vec3 translation1, glm::vec3 translation2, vsr::cga::Rot srcR, vsr::cga::Rot dstR, float Factor, float GAfactorDenominator);
vsr::cga::Mot interpolateCGAVersor11(glm::vec3 translation1, vsr::cga::Rot srcQ);
vsr::cga::Mot interpolateCGAVersor1Rot(glm::vec3 translation1, glm::vec3 translation2, vsr::cga::Rot srcQ, float Factor, float GAfactorDenominator);

glm::mat4 getMat4Versor(vsr::cga::Mot result);
/***********************************************************************/
#endif

#ifdef USE_LIBGASANDBOX

// Local axis rotation
glm::mat4 rotorToMat4(e3ga::mv rotor);
e3ga::mv getRotor(glm::quat srcQ, glm::quat destQ);
void rotateCoordinateSystem(e3ga::mv rotor, e3ga::mv &newe1, e3ga::mv &newe2, e3ga::mv &newe3);
void rotateX(float angle, int currentRot, e3ga::mv &rotor, e3ga::mv newe1,  e3ga::mv newe2,  e3ga::mv newe3, glm::mat4 &finalRotation);
void rotateY(float angle, int currentRot, e3ga::mv &rotor, e3ga::mv newe1,  e3ga::mv newe2,  e3ga::mv newe3, glm::mat4 &finalRotation);
void rotateZ(float angle, int currentRot, e3ga::mv &rotor, e3ga::mv newe1,  e3ga::mv newe2,  e3ga::mv newe3, glm::mat4 &finalRotation);

void aiMat2glmMat(const aiMatrix4x4 *from, glm::mat4 &to);
#endif


#endif //glGACharacterApp_glGAMath_h
