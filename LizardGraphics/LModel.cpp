#include "LModel.h"
#include "LModelBuffer.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "include/tinyobjloader/tiny_obj_loader.h"

LGraphics::LModel::LModel(LApp* app, const char * path)
    :LRectangleShape(app)
{
    loadModel(path);
}

void LGraphics::LModel::loadModel(const char * path)
{
    //tinyobj::attrib_t attrib;
    //std::vector<tinyobj::shape_t> shapes;
    //std::vector<tinyobj::material_t> materials;
    //std::string warn, err;

    //if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path)) {
    //    throw std::runtime_error(warn + err);
    //}

    //buffer = new LModelBuffer()
}
