#include "octree.h"
#include <queue>
float get_distance ( const point &p1, const point &p2) {
    return (p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y) +(p1.z-p2.z)*(p1.z-p2.z);
}
unsigned char get_octant(const point &p, const point &centre) {
    unsigned char octant = 0;
    if ( p.x > centre.x) octant |= 4;
    if ( p.y > centre.y) octant |= 2;
    if ( p.z > centre.z) octant |= 1;
    return octant;
}
void octree :: insert (octree*node, const point &p, const color &c, float error) {
    float errorsq = error * error;
    if (!node) {
        return;
    }
    point centre = (node->maxbounds + node->minbounds)*0.5f;
    std :: cout << "currrent bounds " << "centre x " << centre.x << std::endl;
    unsigned char child_octant = get_octant(p, centre);
    if ( get_distance(p, centre) <= errorsq) {
        node->color_ = c;
        return;
    }
    if ( !node->children[child_octant]) {
    point new_max = centre;
    point new_min = node->minbounds;
    if(child_octant & 4) {new_max.x = node->maxbounds.x; new_min.x = centre.x;}
    if(child_octant & 2) {new_max.y = node->maxbounds.y; new_min.y = centre.y;}
    if(child_octant & 1) {new_max.z = node->maxbounds.z; new_min.z = centre.z;}
    node->children[child_octant] = new octree( new_max, new_min);
    }
    insert (node->children[child_octant],  p  ,  c  , error );
}


//for encoding :
void octree :: from_pc_to_octree( const point_cloud *p , octree*node, float error){ //function to construct an octree in DFS order
    for ( int i = 0 ; p->size ; i++) {
        insert(node, p->vertices[i], p->colors[i], error );
        std :: cout << " successfully constructed an octree" << std :: endl;
    }
}


 //funcntion to write the octree into an outstream : each branch is represented by a byte of 8 bits ( each bit represent a node ), if a node has children, flag it as 1 , otherwise flag as 0
 void octree :: write_octree_stream (octree*node, std :: ofstream&Outstream) { 
    if (!node) return;
    unsigned char branch_info = 0;
    for (int i = 0; i < 8; i++) {
        if (node->children[i]) branch_info |= 1 << i;  
    }
    Outstream.write(reinterpret_cast<const char*> (&branch_info), sizeof(unsigned char));
    if (!branch_info) {
    Outstream.write (reinterpret_cast<const char*>(&node->color_), sizeof(color));}
    for ( int i = 0 ; i < 8; i++) {
            if(node->children[i]) write_octree_stream(node->children[i], Outstream);
        }
        
 }
 
 void octree:: write_octree_into_file (octree*root, const char*filename) {
    std :: ofstream outfile (filename, std :: ios ::binary);
    if (!outfile.is_open()) std :: cerr << "Couldn't open file for writing";
    outfile.write (reinterpret_cast<char*>(&root->maxbounds), sizeof(point) );
    outfile.write (reinterpret_cast<char*>(&root->maxbounds), sizeof(point));
    write_octree_stream(root, outfile);
    outfile.close();
 }
 //work but seeems incorrect
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

/*(segment fault )void read_pc_from_ply(const std::string filename, point_cloud *pc) {
    std::ifstream inStream(filename);
    if (!inStream.is_open()) {
        throw std::runtime_error("Could not open file for reading");
    }

    std::string line;
    std:: string substring = "element vertex";
    std::getline(inStream, line);
    if (line != "ply") {
        throw std::runtime_error("Not a valid PLY file");
    }

    size_t number_vertices = 0;
    bool has_colors = false;

     while (std::getline(inStream, line)) {
         if(line.find(substring,0) != std :: string :: npos){
                number_vertices = std :: stoi(line.substr(15,6));
         } else if (line.find ("property uchar red")) {
            has_colors = true;
         }
        else if (line == "end_header") {
                break;
            }
        }

    std::cout << "Reading PLY file: " << filename << std::endl;
    std::cout << "Number of vertices: " << number_vertices << std::endl;
    std::cout << "Has colors: " << (has_colors ? "Yes" : "No") << std::endl;

    pc->vertices.resize(number_vertices);
    std :: cout << "size of the vertices is : " << number_vertices << std :: endl;
    if (has_colors) {
        pc->colors.resize(number_vertices);
            std :: cout << "size of the colors is : " << number_vertices << std :: endl;
    }
    pc->size = number_vertices;
    bool end_of_header = false;
   
int index = 0;
     while (std::getline(inStream, line)) {

        if (!end_of_header) {
            if (line == "end_header") {
                end_of_header = true;
            }
            continue;
        }
        std::stringstream ss(line);

        ss >>pc->vertices[index].x >> pc->vertices[index].y >> pc->vertices[index].z;
        if ( has_colors) {
        int tempColor;
        ss >> tempColor;
        pc->colors[index].x = static_cast<unsigned char>(tempColor);

        // Repeat for the other color components (y, z) if necessary
        ss >> tempColor;
        pc->colors[index].y = static_cast<unsigned char>(tempColor);

        ss >> tempColor;
        pc->colors[index].z = static_cast<unsigned char>(tempColor);
    }
index++;
}

    std :: cout << "successfully wwritten to pc " << std :: endl;

    inStream.close();
}
*/
