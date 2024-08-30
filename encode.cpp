#include "time.h"
#include "octree.h"

int main (int agrc , char * agrv[]) {
struct timespec start, stop;
clock_gettime(CLOCK_REALTIME, &start);

char*filename = agrv[1];
point maxbounds(1000000.0f,1000000.0f,1000000.0f);
point minbounds(0.0, 0.0f, 0.0f);
float error = atof(agrv[2]);
point_cloud *pc;
read_pc_from_ply(filename,pc);
for ( int i = 0; i < pc->size ; i++ ) {
    if ( maxbounds.x < pc->vertices[i].x) maxbounds.x = pc->vertices[i].x;
    if ( maxbounds.y < pc->vertices[i].y) maxbounds.y = pc->vertices[i].y;
    if ( maxbounds.z < pc->vertices[i].z) maxbounds.z = pc->vertices[i].z;
    if ( minbounds.x > pc->vertices[i].x) minbounds.x = pc->vertices[i].x;
    if ( minbounds.y > pc->vertices[i].y) minbounds.y = pc->vertices[i].y;
    if ( minbounds.z > pc->vertices[i].z) minbounds.z = pc->vertices[i].z;
}
octree root (maxbounds, minbounds);
root.from_pc_to_octree (pc, &root, error );
root.write_octree_into_file(&root,agrv[3]);
clock_gettime(CLOCK_REALTIME, &stop);
double time_ms = (stop.tv_sec - start.tv_sec) * 1000.0 + (stop.tv_nsec - start.tv_nsec) / 1000000.0;
    std::cout << "time: " << time_ms << " ms" << std::endl;  

    return 0;

}