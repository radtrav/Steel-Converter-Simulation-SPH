
#include "Parameters.h"


inline void Normalize(double *a)
{
    double length = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);

    if(!length) return;

    double inv_length = 1.0/length;

    a[0] *= inv_length;
    a[1] *= inv_length;
    a[2] *= inv_length;

}
extern double DotProduct(double *a, double *b)//================replaced inline with extern
{
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

extern double GetDistance(double *a, double *b)
{
    return sqrt( pow(a[0]-b[0], 2.0) + pow(a[1]-b[1], 2.0) + pow(a[2]-b[2], 2.0) );
}

inline double GetLength(double *a)
{
    return sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
}

double random0to1()
{
    static double invRAND_MAX = 1.0/RAND_MAX;
    return rand()*invRAND_MAX;
}

double get_random(double min, double max)
{


    srand((unsigned)time(0));
    return  min + (double)rand()/((double)RAND_MAX/(max-min));

    /* Returns a random double between min and max */
    //return (max - min) * ( (double)rand() / (double)RAND_MAX ) + min;
}


int SPH::getWaterCount()
{
    int nWaterParticles=0;
    for(int x = 0; x < XGRID; x++)
    {
        for(int y = 0; y < YGRID; y++)
        {
            for(int z = 0; z < ZGRID; z++)
            {
                for(unsigned int i = 0; i < grid[x][y][z].water_particles.size(); i++)
                {
                    nWaterParticles++;
                }
            }
        }
    }
    return nWaterParticles;
}
int SPH::getAirCount()
{
    int nAirParticles=0;
    for(int x = 0; x < XGRID; x++)
    {
        for(int y = 0; y < YGRID; y++)
        {
            for(int z = 0; z < ZGRID; z++)
            {
                for(unsigned int i = 0; i < grid[x][y][z].air_particles.size(); i++)
                {
                    nAirParticles++;
                }
            }
        }
    }
    return nAirParticles;
}
int SPH::getBoundaryCount()
{
    int nBoundaryParticles=0;
    for(int x = 0; x < XGRID; x++)
    {
        for(int y = 0; y < YGRID; y++)
        {
            for(int z = 0; z < ZGRID; z++)
            {
                for(unsigned int i = 0; i < grid[x][y][z].boundary_particles.size(); i++)
                {
                    nBoundaryParticles++;
                }
            }
        }
    }
    return nBoundaryParticles;
}
