//
//  KDTreVisulization.cpp
//  glGAframework
//
//  Created by Margrarita Papaefthimiou on 25/02/2017.
//
//

#include "KDTreeVisulization.hpp"
#include "glGA/glGAHelper.h"
#include <fstream>
#include <sstream>
#include <iostream>
#define USE_PRT 1

GLuint Program_KDTree_Visualize, bufferBBoxes, bufferBBoxesColors;
glm::mat4 model_viewBoxes;
GLuint vaoBBoxes;
std::vector<BBox> Box_coordinates;
std::vector<glm::vec4> Bounding_Boxes;
std::vector<glm::vec4> Bounding_Boxes_Colors;

#if defined USE_PRT || defined USE_MC
void BBoxes_init(bool kdtree_create, std::string kdtree_filename){
    
    if (!kdtree_create){
        
        std::ifstream kdtree_input(kdtree_filename);
        
        if (kdtree_input.is_open()){
            
            double x, y, z;
            
            int i = 0;
            
            while (kdtree_input.good()){
                
                kdtree_input >> x;
                kdtree_input >> y;
                kdtree_input >> z;
                
                Bounding_Boxes.push_back(glm::vec4(x, y, z, 1.0));
                Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
                i++;
            }
            
        }
        else{
            
            cout << "Error while opening the file " << kdtree_filename << " for reading." << endl;
            
        }
        kdtree_input.close();
        
    }
    else{
        for (int i = 0; i < Box_coordinates.size(); i++){
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMin.x, Box_coordinates[i].pMin.y, Box_coordinates[i].pMin.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMax.x, Box_coordinates[i].pMin.y, Box_coordinates[i].pMin.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMin.x, Box_coordinates[i].pMin.y, Box_coordinates[i].pMin.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMin.x, Box_coordinates[i].pMax.y, Box_coordinates[i].pMin.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMin.x, Box_coordinates[i].pMin.y, Box_coordinates[i].pMin.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMin.x, Box_coordinates[i].pMin.y, Box_coordinates[i].pMax.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
            
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMin.x, Box_coordinates[i].pMin.y, Box_coordinates[i].pMax.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMax.x, Box_coordinates[i].pMin.y, Box_coordinates[i].pMax.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMin.x, Box_coordinates[i].pMin.y, Box_coordinates[i].pMax.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMin.x, Box_coordinates[i].pMax.y, Box_coordinates[i].pMax.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
            
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMin.x, Box_coordinates[i].pMax.y, Box_coordinates[i].pMax.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMax.x, Box_coordinates[i].pMax.y, Box_coordinates[i].pMax.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMin.x, Box_coordinates[i].pMax.y, Box_coordinates[i].pMax.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMin.x, Box_coordinates[i].pMax.y, Box_coordinates[i].pMin.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
            
            
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMin.x, Box_coordinates[i].pMax.y, Box_coordinates[i].pMin.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMax.x, Box_coordinates[i].pMax.y, Box_coordinates[i].pMin.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMax.x, Box_coordinates[i].pMax.y, Box_coordinates[i].pMin.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMax.x, Box_coordinates[i].pMin.y, Box_coordinates[i].pMin.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMax.x, Box_coordinates[i].pMax.y, Box_coordinates[i].pMin.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMax.x, Box_coordinates[i].pMax.y, Box_coordinates[i].pMax.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
            
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMax.x, Box_coordinates[i].pMin.y, Box_coordinates[i].pMax.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMax.x, Box_coordinates[i].pMin.y, Box_coordinates[i].pMin.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMax.x, Box_coordinates[i].pMin.y, Box_coordinates[i].pMax.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
            Bounding_Boxes.push_back(glm::vec4(Box_coordinates[i].pMax.x, Box_coordinates[i].pMax.y, Box_coordinates[i].pMax.z, 1.0));
            Bounding_Boxes_Colors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
        }
        
        std::ofstream kdtree_output;
        kdtree_output.open (kdtree_filename);
        if (kdtree_output.is_open()){
            for (int i = 0; i < Bounding_Boxes.size(); i++){
                kdtree_output << Bounding_Boxes[i].x << " " << Bounding_Boxes[i].y << " " << Bounding_Boxes[i].z << " ";
                
            }
            kdtree_output.close();
        }
        else{
            
            cout << "Error while opening file to save the kdtree bounding boxes coordinates." << endl;
            
        }
    }
    
    
}

void initBoundingBoxes(){
    //generate and bind a VAO for the 3D axes
    glGenVertexArrays(1, &vaoBBoxes);
    glBindVertexArray(vaoBBoxes);
    
    //load shaders
    Program_KDTree_Visualize = LoadShaders("./shaders/axes.vert", "./shaders/axes.frag");
    glUseProgram(Program_KDTree_Visualize);
    
    //create vertices VBO on server side GPU
    glGenBuffers(1, &bufferBBoxes);
    glBindBuffer(GL_ARRAY_BUFFER, bufferBBoxes);
    //fill-in VBO with vertices
    glBufferData(GL_ARRAY_BUFFER, Bounding_Boxes.size()*16, &Bounding_Boxes[0], GL_STATIC_DRAW);
    //connect vertex shader with application vertex data
    GLuint  vPosit = glGetAttribLocation(Program_KDTree_Visualize, "vPosition");
    glEnableVertexAttribArray(vPosit);
    glVertexAttribPointer(vPosit, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    //stop using previous VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    //create colors VBO on server side GPU
    glGenBuffers(1, &bufferBBoxesColors);
    glBindBuffer(GL_ARRAY_BUFFER, bufferBBoxesColors);
    glBufferData(GL_ARRAY_BUFFER, Bounding_Boxes_Colors.size()*16, &Bounding_Boxes_Colors[0], GL_STATIC_DRAW);
    GLuint vCol = glGetAttribLocation(Program_KDTree_Visualize, "vColor");
    glEnableVertexAttribArray(vCol);
    glVertexAttribPointer(vCol, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    
    
    GLuint ProjectionBBox = glGetUniformLocation(Program_KDTree_Visualize, "Projection");
    
    glUniformMatrix4fv(ProjectionBBox, 1, GL_FALSE, glm::value_ptr(projection_matrix));
    
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // only one VAO can be bound at a time, so disable it to avoid altering it accidentally
    glBindVertexArray(0);
}

void displayBoundingBoxes(glm::mat4 finalRotation){
    glUseProgram(Program_KDTree_Visualize);
    glBindVertexArray(vaoBBoxes);
    
    glDisable(GL_CULL_FACE);
    //glPushAttrib(GL_ALL_ATTRIB_BITS);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    if (cameraView)
    {
        if (initViewerPosition)
        {
            viewer_pos += glm::vec3(0.0, 0.0, 0.0);
            initViewerPosition = false;
        }
        model_viewBoxes = glm::lookAt(viewer_pos,           // Camera is here
                              viewer_pos + direction, // and looks here : at the same position, plus "direction"
                              up                  // Head is up (set to 0,-1,0 to look upside-down)
                              );
        
        //std::cout<<"\nviewer_pos: "<<glm::to_string(viewer_pos)<<std::endl;
        //std::cout<<"direction: "<<glm::to_string(direction)<<std::endl;
        ///std::cout<<"up: "<<glm::to_string(up)<<std::endl<<std::endl;
    }
    else
    {
        //setup MV matrix including rotation from GUI
        glm::quat quatRot = glm::quat(glm::vec3(g_quat[3], g_quat[0], g_quat[1]));
        glm::mat4 quatToMat = glm::mat4_cast(quatRot);
        // generate new modelView matrix for each frame
        viewer_pos = glm::vec3(0.0, 0.0, -g_Zoom);
        float angle = -90;
        glm::mat4 RotX = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));
        
        glm::mat4 rotation;
#ifdef USE_GEOMETRIC_ALGEBRA
        rotation = quatToMat * view_matrix * finalRotation;
#else
        rotation = quatToMat * view_matrix;
#endif
        model_viewBoxes = rotation * glm::translate(glm::mat4(1.0), viewer_pos);
    }
    // retrieve uniform variables
    GLuint ModelViewBBox = glGetUniformLocation(Program_KDTree_Visualize, "ModelView");
    model_viewBoxes = model_viewBoxes * glm::translate(glm::mat4(), glm::vec3(0.0, -50.0, 0.0));
    
    glUniformMatrix4fv(ModelViewBBox, 1, GL_FALSE, glm::value_ptr(model_viewBoxes));
    GLuint ProjectionBBox = glGetUniformLocation(Program_KDTree_Visualize, "Projection");
    
    glUniformMatrix4fv(ProjectionBBox, 1, GL_FALSE, glm::value_ptr(projection_matrix));
    
    glDrawArrays(GL_LINES, 0, Bounding_Boxes.size());
    
    glBindVertexArray(0);
    
}
#endif
