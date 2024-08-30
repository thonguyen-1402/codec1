#pragma once
#ifndef OCTREE_H
#define OCTREE_H

#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <string>
#include<iostream>

template <class type >
struct vector3d {
    type x , y, z;
    vector3d () {};
    vector3d(type x_, type y_, type z_) : x(x_), y(y_), z(z_) {};
};
typedef vector3d<float> point;
typedef vector3d<unsigned char> color;
template <class type>
vector3d <type> operator + ( const vector3d<type> &a, const vector3d<type> &b) {
    return vector3d<type>(a.x + b.x, a.y + b.y,a.z + b.z);
}
template <class type>
vector3d <type> operator - ( const vector3d<type> &a, vector3d<type> &b) {
    return vector3d <type> ( a.x - b.x, a.y - b.y, a.z - b.z);
}
template <class type, class type2>
vector3d <type> operator * ( const vector3d<type> &a, type2 scalar) {
    return vector3d<type> (a.x *scalar, a.y*scalar, a.z*scalar);
}
struct point_cloud {
    std::vector<point> vertices;
    std::vector<color> colors;
    size_t size;
   
};

class octree {
private: 
point maxbounds;
point minbounds;
color color_;
octree*children[8];
public :
octree() {};
octree(const point &maxbounds_, const point &minbounds_ ) : maxbounds(maxbounds_), minbounds(minbounds_) {
    for ( int i = 0 ; i < 8; i++) {
        children[i] = nullptr;
    }
}
~ octree() {
    for ( int i = 0 ; i < 8; i++) {
        for ( int i = 0  ; i < 8; i++) {
            delete children[i];
        }
    }
}
color get_color();

void insert (octree*node, const point&p, const color&c , float erorr);
void from_pc_to_octree(const point_cloud *p, octree*node, float error);
void write_octree_stream(octree*node, std :: ofstream &Outstream);
void write_octree_into_file (octree*node, const char*filename);
octree*reconstruct_octree(std :: ifstream &InStream, point &maxbounds, point &minbounds);
octree*read_octree_from_file(char*filename);
point_cloud* octree_into_pc (octree*root);
void write_octree_stream2(octree*node, std::ofstream &outstream);

};
float get_distance ( const point &p1, const point &p2);
unsigned char get_octant(const point &p, const point &centre);

void read_pc_from_ply (const std :: string filename, point_cloud *pc);
#endif



