#include <halfedge.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>

int main(int argc, char * argv[])
{
    if(argc < 3)
        printf("ERROR: Not enough arguments.\n");
    else
    {
        Mesh m;

        m.generateHalfEdge(argv[1]);

        int count = atoi(argv[2]);

        auto s1 = std::chrono::high_resolution_clock::now();
        for(int i = 0; i < count; i++)
        {
            auto t1 = std::chrono::high_resolution_clock::now();
            m.subdivide();
            auto t2 = std::chrono::high_resolution_clock::now();
            double duration = (double)std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() * 0.000000001;
            printf("Subdiv number %d took: %f seconds\n", i + 1, duration);
        }
        auto s2 = std::chrono::high_resolution_clock::now();
        double duration = (double)std::chrono::duration_cast<std::chrono::nanoseconds>(s2 - s1).count() * 0.000000001;
        printf("All subdivs took: %f seconds\n", duration);
        
    }
    return 0;
}
