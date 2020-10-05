//
//  KDTreVisulization.hpp
//  glGAframework
//
//  Created by Margrarita Papaefthimiou on 25/02/2017.
//
//

#ifndef KDTreeVisulization_hpp
#define KDTreeVisulization_hpp

#include <stdio.h>
#include <GL/glew.h>

#define GLM_SWIZZLE
#if defined _WIN32 || defined _WIN64
#define GLM_FORCE_INLINE
#endif
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include <stdio.h>
#include <iostream>

#include "geometry.h"

extern glm::vec3           viewer_pos;
extern glm::mat4           projection_matrix;
extern glm::vec3           direction;
extern glm::vec3           viewer_pos;
extern float               g_quat[3];
extern int                 cameraView;
extern bool                initViewerPosition;
extern glm::vec3           up;
extern float               g_Zoom;
extern glm::mat4           view_matrix;

void initBoundingBoxes();
void BBoxes_init(bool kdtree_create, std::string kdtree_filename);
void displayBoundingBoxes(glm::mat4 finalRotation);

#endif /* KDTreVisulization_hpp */
