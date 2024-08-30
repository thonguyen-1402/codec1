#include "octree.h"
#include <queue>

//read the octree (in bit stream) and reconstruct the octree => write the octree to an input stream=> load write the stream into a file 
octree* octree :: reconstruct_octree(std :: ifstream &InStream, point&maxbounds, point&minbounds) { //reconstruct the tree in DFS order
octree*node = new octree(maxbounds, minbounds);
unsigned char branch_info;
InStream.read(reinterpret_cast<char*> (&branch_info), sizeof(unsigned char));
if(!branch_info) {
    InStream.read(reinterpret_cast<char*> (&node->color_), sizeof(color)); //a pointer to the adress of color_ (& here indicate the adresss of the color_ not the node )
}
point centre = (minbounds + maxbounds) *0.5f;
point new_max = centre;
point new_min = minbounds;
    for (int octant_no = 0 ; octant_no < 8; octant_no++) {
        if  (branch_info & (1 << octant_no)) { //if the octant matches the branch info => this indicates that the octant at the index i has children
         if (octant_no & 4) new_max.x = maxbounds.x, new_min.x = centre.x;
         if (octant_no & 2 ) new_max.y = maxbounds.y, new_min.y = centre.y;
         if (octant_no & 1) new_max.z = maxbounds.z, new_min.z = centre.z;

    node->children[octant_no] = reconstruct_octree(InStream, new_max, new_min);
    }
} 
return node;
}
octree* octree :: read_octree_from_file(char *filename) {
    std::ifstream input(filename, std:: ios :: binary);
     if (!input.is_open()){
        std::cerr << "Can't open file for reading";
    }
    point maxbounds, minbounds;
    input.read(reinterpret_cast<char*> (&maxbounds), sizeof(point));
    input.read(reinterpret_cast<char*> (&minbounds), sizeof(point));
    octree*root = reconstruct_octree(input, maxbounds, minbounds);
    input.close();
    return root;
}
point_cloud *octree::octree_into_pc(octree* root) {  
    // Since the octree is encoded in BFS order, we must decode it in BFS order.

    int leaf_count = 0;
    
    // First pass to count the number of leaf nodes.
    std::queue<octree*> node_queue;
    node_queue.push(root);   
    while (!node_queue.empty()) {
        octree* node = node_queue.front();
        node_queue.pop();
        bool has_child = false;
        
        // Check if the current node has any children.
        for (int i = 0; i < 8; i++) {
            if (node->children[i]) {
                has_child = true;
                node_queue.push(node->children[i]);
            }
        }
        
        // If the node is a leaf (has no children), increment the leaf counter.
        if (!has_child) {
            leaf_count++;
        }
    }
    
    // Resize the point cloud to accommodate the vertices and colors.
    point_cloud * pc = new point_cloud();
    pc->size = leaf_count;
    pc->vertices.resize(leaf_count);
    pc->colors.resize(leaf_count);
    
    int index = 0;
    
    // Second pass to populate the point cloud with vertices and colors.
    std::queue<octree*> decode_queue;
    decode_queue.push(root);   
    while (!decode_queue.empty()) {
        octree* node = decode_queue.front();
        decode_queue.pop();
        bool has_child = false;
        
        // Check if the current node has any children.
        for (int i = 0; i < 8; i++) {
            if (node->children[i]) {
                has_child = true;
                decode_queue.push(node->children[i]);
            }
        }
        
        // If the node is a leaf, add the vertex and color to the point cloud.
        if (!has_child) {
            pc->vertices[index] = (maxbounds + minbounds) * 0.5f; // Assuming maxbounds and minbounds are defined appropriately.
            pc->colors[3 * index + 0].x = node->color_.x;  // Red component
            pc->colors[3 * index + 1].y = node->color_.y;  // Green component
            pc->colors[3 * index + 2].z = node->color_.z;  // Blue component
            index++;
        }
    }
    
    return pc;
}

void pc_to_fileply (point_cloud *pc, char*filename) {
    std:: ofstream output(filename);
    if (!output.is_open()) {
        std::cerr << "Cannot open file for writing";
        exit(1);
    }
    output << "ply" << std :: endl;
    output << "format ascii 1.0" << std :: endl;
    output << "comment Version 2, Copyright 2017, 8i Labs, Inc." << std :: endl;
    output << "comment frame_to_world_scale 0.181985" << std :: endl;
    output << "comment frame_to_world_translation -31.8478 1.0016 -32.6788" << std:: endl;
    output << "comment width 1023" << std :: endl;
    output << "element vertex" << pc->vertices.size() << std :: endl;
    output << "property float x" << std :: endl;
    output << "property float y" << std :: endl;
    output << "property float z" << std :: endl;
    output << "property uchar red" << std :: endl;
    output << "property uchar green" << std :: endl;
    output << "property uchar blue" << std :: endl;
    output << "end_header" << std :: endl;

   // Write point cloud data in ASCII format
    for (size_t i = 0; i < pc->size; ++i) {
        output << pc->vertices[3 * i].x << " "   // x
               << pc->vertices[3 * i + 1].y << " " // y
               << pc->vertices[3 * i + 2].z << " " // z
               << static_cast<int>(pc->colors[3 * i].x) << " "   // red
               << static_cast<int>(pc->colors[3 * i + 1].y) << " " // green
               << static_cast<int>(pc->colors[3 * i + 2].z) << std::endl; // blue
    }
    output.close();
    if (!output) std :: cout << "writing unsuccessfully";
    /*for (size_t i = 0; i < pc->size; i++) {
      output.write(reinterpret_cast< const char*>(&pc->vertices[3*i]), sizeof(float));
      output.write(reinterpret_cast<const char*>(&pc->colors[3*i]), sizeof(unsigned char));
*/
}
int main(int agrc, char*agrv[]) {
    struct timespec start, stop;
    clock_gettime(CLOCK_REALTIME, &start);
    octree *root = new octree();
    root->read_octree_from_file(agrv[1]);
    point_cloud *pc = root->octree_into_pc(root);
    pc_to_fileply(pc, agrv[2]);
    clock_gettime(CLOCK_REALTIME, &stop);
    double time_ms = (stop.tv_sec - start.tv_sec) * 1000.0 + (stop.tv_nsec - start.tv_nsec) / 1000000.0;
    std::cout << "time: " << time_ms << " ms" << std::endl;

    return 0;

}

