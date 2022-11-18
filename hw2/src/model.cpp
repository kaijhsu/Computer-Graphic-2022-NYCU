#include "model.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <glm/vec3.hpp>

Model* Model::fromObjectFile(const char* obj_file) {
  Model* m = new Model();

  std::ifstream ObjFile(obj_file);
  std::cout << "open file: " << obj_file << std::endl;

  if (!ObjFile.is_open()) {
    std::cout << "Can't open File !" << std::endl;
    return NULL;
  }

  /* TODO#1: Load model data from OBJ file
   *         You only need to handle v, vt, vn, f
   *         Other fields you can directly ignore
   *         Fill data into m->positions, m->texcoords m->normals and m->numVertex
   *         Data format:
   *           For positions and normals
   *         | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    | 8    | 9    | 10   | 11   | ...
   *         | face 1                                                       | face 2               ...
   *         | v1x  | v1y  | v1z  | v2x  | v2y  | v2z  | v3x  | v3y  | v3z  | v1x  | v1y  | v1z  | ...
   *         | vn1x | vn1y | vn1z | vn2x | vn2y | vn2z | vn3x | vn3y | vn3z | vn1x | vn1y | vn1z | ...
   *           For texcoords
   *         | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    | ...
   *         | face 1                                  | face 2        ...
   *         | v1x  | v1y  | v2x  | v2y  | v3x  | v3y  | v1x  | v1y  | ...
   * Note:
   *        OBJ File Format (https://en.wikipedia.org/wiki/Wavefront_.obj_file)
   *        For simplification, all faces from OBJ file are triangle (vertex per face = 3)
   */

  std::string line = "";
  std::string prefix = "";
  std::stringstream ss;

  std::vector<glm::vec3> v;
  v.push_back(glm::vec3());
  std::vector<glm::vec3> vn;
  vn.push_back(glm::vec3());
  std::vector<glm::vec2> vt;
  vt.push_back(glm::vec2());
  m->numVertex = 0;

  while (getline(ObjFile, line)) {
    ss.clear();
    ss.str(line);
    ss >> prefix;
    if (prefix == "v") {
      glm::vec3 data;
      ss >> data.x >> data.y >> data.z;
      v.push_back(data);
    } else if (prefix == "vn") {
      glm::vec3 data;
      ss >> data.x >> data.y >> data.z;
      vn.push_back(data);
    } else if (prefix == "vt") {
      glm::vec2 data;
      ss >> data.x >> data.y;
      vt.push_back(data);
    } else if (prefix == "f") {
      int vertex_index;
      int texture_index;
      int normal_index;
      for (int i = 0; i < 3; ++i) {
        std::string tmp;
        ss >> tmp;
        size_t del1 = tmp.find_first_of("/");
        size_t del2 = tmp.find_last_of("/");
        vertex_index = stoi(tmp.substr(0, del1));
        texture_index = stoi(tmp.substr(del1 + 1, del2));
        normal_index = stoi(tmp.substr(del2 + 1));
        m->positions.push_back(v[vertex_index].x);
        m->positions.push_back(v[vertex_index].y);
        m->positions.push_back(v[vertex_index].z);
        m->normals.push_back(vn[normal_index].x);
        m->normals.push_back(vn[normal_index].y);
        m->normals.push_back(vn[normal_index].z);
        m->texcoords.push_back(vt[texture_index].x);
        m->texcoords.push_back(vt[texture_index].y);
        m->numVertex += 1;
      } 
    } else {
      continue;
    }
  }
  if (ObjFile.is_open()) ObjFile.close();

  return m;
}
