#include "MeshData.h"
#include "../pugixml/src/pugixml.hpp"
#include <iostream>

using namespace Savitar;

MeshData::MeshData()
{

}

MeshData::~MeshData()
{

}

void MeshData::fillByXMLNode(pugi::xml_node xml_node)
{
    this->vertices.clear();
    this->faces.clear();

    // TODO: Add error handling (what to do if there is no vertices object, etc)
    // Add every vertex.
    pugi::xml_node xml_vertices = xml_node.child("vertices");
    for(pugi::xml_node vertex = xml_vertices.child("vertex"); vertex; vertex = vertex.next_sibling("vertex"))
    {
        Vertex temp_vertex = Vertex(vertex.attribute("x").as_float(), vertex.attribute("y").as_float(), vertex.attribute("z").as_float());
        this->vertices.push_back(temp_vertex);
    }

    // Add every face.
    pugi::xml_node xml_triangles = xml_node.child("triangles");
    for(pugi::xml_node face = xml_triangles.child("triangle"); face; face = face.next_sibling("triangle"))
    {
        Face temp_face = Face(face.attribute("v1").as_int(), face.attribute("v2").as_int(), face.attribute("v3").as_int());
        this->faces.push_back(temp_face);
    }
}

void MeshData::clear()
{
    this->faces.clear();
    this->vertices.clear();
}

PyObject* MeshData::getVerticesAsBytes()
{
    std::string vertices_data;

    for(int i = 0; i < vertices.size(); i++)
    {
        float x = vertices.at(i).getX();
        float y = vertices.at(i).getY();
        float z = vertices.at(i).getZ();
        vertices_data.append(reinterpret_cast<const char*>(&x), sizeof(float));
        vertices_data.append(reinterpret_cast<const char*>(&y), sizeof(float));
        vertices_data.append(reinterpret_cast<const char*>(&z), sizeof(float));
    }
    return PyBytes_FromStringAndSize(vertices_data.c_str(), vertices_data.size());
}

PyObject* MeshData::getFlatVerticesAsBytes()
{
    std::string vertices_data;
    for(int i = 0; i < faces.size(); i++)
    {
        int v1 = faces.at(i).getV1();
        int v2 = faces.at(i).getV2();
        int v3 = faces.at(i).getV3();
        
        // Add vertices for face 1
        float x = vertices.at(v1).getX();
        float y = vertices.at(v1).getY();
        float z = vertices.at(v1).getZ();
        vertices_data.append(reinterpret_cast<const char*>(&x), sizeof(float));
        vertices_data.append(reinterpret_cast<const char*>(&y), sizeof(float));
        vertices_data.append(reinterpret_cast<const char*>(&z), sizeof(float));

        // Add vertices for face 2
        x = vertices.at(v2).getX();
        y = vertices.at(v2).getY();
        z = vertices.at(v2).getZ();
        vertices_data.append(reinterpret_cast<const char*>(&x), sizeof(float));
        vertices_data.append(reinterpret_cast<const char*>(&y), sizeof(float));
        vertices_data.append(reinterpret_cast<const char*>(&z), sizeof(float));

        // Add vertices for face 3
        x = vertices.at(v3).getX();
        y = vertices.at(v3).getY();
        z = vertices.at(v3).getZ();
        vertices_data.append(reinterpret_cast<const char*>(&x), sizeof(float));
        vertices_data.append(reinterpret_cast<const char*>(&y), sizeof(float));
        vertices_data.append(reinterpret_cast<const char*>(&z), sizeof(float));
    }
    return PyBytes_FromStringAndSize(vertices_data.c_str(), vertices_data.size());
}

PyObject* MeshData::getFacesAsBytes()
{
    std::string face_data;

    for(int i = 0; i < faces.size(); i++)
    {
        int v1 = faces.at(i).getV1();
        int v2 = faces.at(i).getV2();
        int v3 = faces.at(i).getV3();
        face_data.append(reinterpret_cast<const char*>(&v1), sizeof(int));
        face_data.append(reinterpret_cast<const char*>(&v2), sizeof(int));
        face_data.append(reinterpret_cast<const char*>(&v3), sizeof(int));
    }
    return PyBytes_FromStringAndSize(face_data.c_str(), face_data.size());
}

void MeshData::toXmlNode(pugi::xml_node& node)
{
    pugi::xml_node vertices_node = node.append_child("vertices");
    for(int i = 0; i < vertices.size(); i++)
    {
        pugi::xml_node vertex_node = vertices_node.append_child("vertex");
        vertex_node.append_attribute("x") = vertices.at(i).getX();
        vertex_node.append_attribute("y") = vertices.at(i).getY();
        vertex_node.append_attribute("z") = vertices.at(i).getZ();
    }

    pugi::xml_node triangles_node = node.append_child("triangles");
    for(int i = 0; i < faces.size(); i++)
    {
        pugi::xml_node triangle_node = triangles_node.append_child("triangle");
        triangle_node.append_attribute("v1") = faces.at(i).getV1();
        triangle_node.append_attribute("v2") = faces.at(i).getV2();
        triangle_node.append_attribute("v3") = faces.at(i).getV3();
    }
}

void MeshData::setVerticesFromBytes(PyObject* py_bytes)
{
    if(py_bytes == nullptr)
    {
        return;
    }

    vertices.clear();
    char* bytes = PyBytes_AsString(py_bytes);
    int num_bytes = PyBytes_Size(py_bytes);
    int num_floats = num_bytes / sizeof(float);

    //Interpret byte array as array of floats.
    float* float_array = reinterpret_cast<float*>(bytes);

    for(int i = 0; i < num_floats; i +=3)
    {
        Vertex temp_vertex = Vertex(float_array[i], float_array[i + 1], float_array[i + 2]);
        this->vertices.push_back(temp_vertex);
    }
}

void MeshData::setFacesFromBytes(PyObject* py_bytes)
{
    if(py_bytes == nullptr)
    {
        return; 
    }

    faces.clear();
    char* bytes = PyBytes_AsString(py_bytes);
    int num_bytes = PyBytes_Size(py_bytes);
    int num_ints = num_bytes / sizeof(int);

    //Interpret byte array as array of ints.
    int* int_array = reinterpret_cast<int*>(bytes);

    for(int i = 0; i < num_ints; i +=3)
    {
        Face temp_face = Face(int_array[i], int_array[i + 1], int_array[i + 2]);
        this->faces.push_back(temp_face);
    }
}

std::vector< Vertex > MeshData::getVertices()
{
    return vertices;
}



