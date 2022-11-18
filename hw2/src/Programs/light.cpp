#include <iostream>
#include "context.h"
#include "program.h"

//struct Material {
//  glm::vec3 ambient;
//  glm::vec3 diffuse;
//  glm::vec3 specular;
//  float shininess;
//};
//
//struct DirectionLight {
//  int enable;
//  glm::vec3 direction;
//  glm::vec3 lightColor;
//};
//
//struct PointLight {
//  int enable;
//  glm::vec3 position;
//  glm::vec3 lightColor;
//
//  float constant;
//  float linear;
//  float quadratic;
//};
//
//struct Spotlight {
//  int enable;
//  glm::vec3 position;
//  glm::vec3 direction;
//  glm::vec3 lightColor;
//  float cutOff;
//
//  // Paramters for attenuation formula
//  float constant;
//  float linear;
//  float quadratic;
//}; 

bool LightProgram::load() {
  /* TODO#4-2: Pass model vertex data to vertex buffer
   *           1. Generate and bind vertex array object (VAO) for each model
   *           2. Generate and bind three vertex buffer objects (VBOs) for each model
   *           3. Pass model positions, normals and texcoords to to VBOs
   * Note:
   *           If you implament BasicProgram properly, You might inherent BasicProgram's load function
   */
  programId = quickCreateProgram(vertProgramFile, fragProgramFIle);
  int num_model = (int)ctx->models.size();
  VAO = new GLuint[num_model];

  glGenVertexArrays(num_model, VAO);

  for (int i = 0; i < num_model; i++) {
    glBindVertexArray(VAO[i]);
    Model* model = ctx->models[i];

    GLuint VBO[3];  // position, normal, texCoord
    glGenBuffers(3, VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model->positions.size(), model->positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model->normals.size(), model->normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model->texcoords.size(), model->texcoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
  return programId != 0;
}

void LightProgram::doMainLoop() {
  /* TODO#4-3: Render objects with shader
   *           1. use and bind program (BasicProgram::programId)
   *           2. Iterate all objects (ctx->objects)
   *           3. Load currect model VAO for object
   *           4. Pass projection, view, model matrix to shaders
   *           5. Pass light and material parameters to shader
   *           6. Pass model texture to shaders
   *           7. Draw with glDrawArrays
   * Note:
   *           1. light paramters are providered in context.h
   *           2. material parameter for each object get be found in ctx->objects[i]->material
   */
  glUseProgram(programId);

  GLint scaleLoc = glGetUniformLocation(programId, "scaleEnable");
  glUniform1i(scaleLoc, ctx->scaleEnable);
  GLint camDirLoc = glGetUniformLocation(programId, "cameraDir");
  glUniform3fv(camDirLoc, 1, ctx->camera->getFront());
  
  //setting Light
  // direction light
  GLint dlOnLoc = glGetUniformLocation(programId, "dl.enable");
  GLint dlDirLoc = glGetUniformLocation(programId, "dl.direction");
  GLint dlColLoc = glGetUniformLocation(programId, "dl.lightColor");
  glUniform1i(dlOnLoc, ctx->directionLightEnable);
  glUniform3f(dlDirLoc, ctx->directionLightDirection.x, ctx->directionLightDirection.y, ctx->directionLightDirection.z);
  glUniform3f(dlColLoc, ctx->directionLightColor.x, ctx->directionLightColor.y, ctx->directionLightColor.z);

  // point light
  GLint plOnLoc = glGetUniformLocation(programId, "pl.enable");
  GLint plPosLoc = glGetUniformLocation(programId, "pl.position");
  GLint plColLoc = glGetUniformLocation(programId, "pl.lightColor");
  GLint plConLoc = glGetUniformLocation(programId, "pl.constant");
  GLint plLinLoc = glGetUniformLocation(programId, "pl.linear");
  GLint plQuaLoc = glGetUniformLocation(programId, "pl.quadratic");
  glUniform1i(plOnLoc, ctx->pointLightEnable);
  glUniform3f(plPosLoc, ctx->pointLightPosition.x, ctx->pointLightPosition.y, ctx->pointLightPosition.z);
  glUniform3f(plColLoc, ctx->pointLightColor.x, ctx->pointLightColor.y, ctx->pointLightColor.z);
  glUniform1f(plConLoc, ctx->pointLightConstant);
  glUniform1f(plLinLoc, ctx->pointLightLinear);
  glUniform1f(plQuaLoc, ctx->pointLightQuardratic);

  // spot light
  GLint slOnLoc = glGetUniformLocation(programId, "sl.enable");
  GLint slPosLoc = glGetUniformLocation(programId, "sl.position");
  GLint slDirLoc = glGetUniformLocation(programId, "sl.direction");
  GLint slColLoc = glGetUniformLocation(programId, "sl.lightColor");
  GLint slCutLoc = glGetUniformLocation(programId, "sl.cutOff");
  GLint slConLoc = glGetUniformLocation(programId, "sl.constant");
  GLint slLinLoc = glGetUniformLocation(programId, "sl.linear");
  GLint slQuaLoc = glGetUniformLocation(programId, "sl.quadratic");
  glUniform1i(slOnLoc, ctx->spotLightEnable);
  glUniform3f(slPosLoc, ctx->spotLightPosition.x, ctx->spotLightPosition.y, ctx->spotLightPosition.z);
  glUniform3f(slDirLoc, ctx->spotLightDirection.x, ctx->spotLightDirection.y, ctx->spotLightDirection.z);
  glUniform3f(slColLoc, ctx->spotLightColor.x, ctx->spotLightColor.y, ctx->spotLightColor.z);
  glUniform1f(slCutLoc, ctx->spotLightCutOff);
  glUniform1f(slConLoc, ctx->spotLightConstant);
  glUniform1f(slLinLoc, ctx->spotLightLinear);
  glUniform1f(slQuaLoc, ctx->spotLightQuardratic);

  // projection view 
  const float* projectMat = ctx->camera->getProjectionMatrix();
  GLint pmatLoc = glGetUniformLocation(programId, "Projection");
  glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, projectMat);
  
  const float* viewMat = ctx->camera->getViewMatrix();
  GLint vmatLoc = glGetUniformLocation(programId, "ViewMatrix");
  glUniformMatrix4fv(vmatLoc, 1, GL_FALSE, viewMat);
  int obj_num = (int)ctx->objects.size();

  // view pos
  GLint vpLoc = glGetUniformLocation(programId, "viewPos");
  glUniform3fv(vpLoc, 1, ctx->camera->getPosition());
  
  for (int i = 0; i < obj_num; i++) {
    int modelIndex = ctx->objects[i]->modelIndex;
    glBindVertexArray(VAO[modelIndex]);

    Model* model = ctx->models[modelIndex];
    glm::mat4 m = ctx->objects[i]->transformMatrix * model->modelMatrix;
    const float* modelMat = glm::value_ptr(m);
    GLint mmatLoc = glGetUniformLocation(programId, "ModelMatrix");
    glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, modelMat);

    const float* modelNormalMat = glm::value_ptr(glm::transpose(glm::inverse(m)));
    GLint mnmatLoc = glGetUniformLocation(programId, "ModelNormalMatrix");
    glUniformMatrix4fv(mnmatLoc, 1, GL_FALSE, modelNormalMat);

    // Setting Material
    Material *material = &ctx->objects[i]->material;
    GLint matAmbLoc = glGetUniformLocation(programId, "material.ambient");
    glUniform3f(matAmbLoc, material->ambient.x, material->ambient.y, material->ambient.z);
    GLint matDiffLoc = glGetUniformLocation(programId, "material.diffuse");
    glUniform3f(matDiffLoc, material->diffuse.x, material->diffuse.y, material->diffuse.z);
    GLint matSpecLoc = glGetUniformLocation(programId, "material.specular");
    glUniform3f(matSpecLoc, material->specular.x, material->specular.y, material->specular.z);
    GLint matShinLoc = glGetUniformLocation(programId, "material.shininess");
    glUniform1f(matShinLoc, material->shininess);

    // bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, model->textures[ctx->objects[i]->textureIndex]);
    glDrawArrays(model->drawMode, 0, model->numVertex);
  }
  glUseProgram(0);
}
