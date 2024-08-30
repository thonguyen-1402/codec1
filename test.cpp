#include "octree.h"
//this the read_pc_from_ply does not work as it prints out nothing
void read_pc_from_ply (const std::string filename, point_cloud *pc ) {
        std::ifstream inStream(filename);
        if (!inStream.is_open()) {
            throw std::runtime_error("Could not open file for reading");
        }

        std::string line;
        std::getline(inStream, line); // Read the first line (should be "ply")
        if (line != "ply") {
            throw std::runtime_error("Not a valid PLY file");
        }

        size_t num_vertices = 0;
        bool has_colors = false;

        // Read header to determine the number of vertices, faces, and color information
        while (std::getline(inStream, line)) {
            if (line.substr(0, 10) == "element v") {
                num_vertices = std::stoi(line.substr(11));
            } else if (line.substr(0, 15) == "property uchar") {
                has_colors = true;
            } else if (line == "end_header") {
                break;
            }
        }

        // Read vertices and colors
        pc->vertices.resize(num_vertices);
        if (has_colors) {
            pc->colors.resize(num_vertices);
        }
        pc->size = num_vertices;

        for (size_t i = 0; i < num_vertices; ++i) {
            std::getline(inStream, line);
            std::istringstream iss(line);
            iss >> pc->vertices[i].x >> pc->vertices[i].y >> pc->vertices[i].z;
            if (has_colors) {
                int r, g, b;
                iss >> r >> g >> b;
                pc->colors[i] = {static_cast<unsigned char>(r), static_cast<unsigned char>(g), static_cast<unsigned char>(b)};
            }
        }

        inStream.close();
    }

int main ( int agrc, char*agrv[]) {
    char * file = agrv[1];
    point_cloud *pc;
    read_pc_from_ply(file,pc);
    for ( int i = 0 ; i < pc->size; i++) {
        std :: cout << pc->vertices[i].x << std :: endl; 
    }

}