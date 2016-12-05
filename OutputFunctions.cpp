#include "Parameters.h"
#include <utility>




char SPH::writeGrid()
{

    ofstream grid_out;
    grid_out.open("data/csv/grid.csv");
    if( !    grid_out.is_open() )
    {
        cout << "  Couldn't Open data/csv/grid.csv "  << endl;
        return 1;
    }
    for(int x = 0; x < XGRID; x++)
    {
        for(int y = 0; y < YGRID; y++)
        {
            for(int z = 0; z < ZGRID; z++)
            {
                double bbmin[3] = {     x*g_Param.getSmoothingRadius(),     y*g_Param.getSmoothingRadius(),     z*g_Param.getSmoothingRadius() };                 //H is the smoothing length aka the width of a cell
                double bbmax[3] = { (x+1)*g_Param.getSmoothingRadius(), (y+1)*g_Param.getSmoothingRadius(), (z+1)*g_Param.getSmoothingRadius() };

                int grid_index_x = ((int)floor(bbmin[0]/g_Param.getSmoothingRadius()));
                int grid_index_y = ((int)floor(bbmin[1]/g_Param.getSmoothingRadius()));
                int grid_index_z = ((int)floor(bbmin[2]/g_Param.getSmoothingRadius()));

                int grid_index_x2 = ((int)floor(bbmax[0]/g_Param.getSmoothingRadius()));
                int grid_index_y2 = ((int)floor(bbmax[1]/g_Param.getSmoothingRadius()));
                int grid_index_z2 = ((int)floor(bbmax[2]/g_Param.getSmoothingRadius()));

                grid_out<< bbmin [0]<<","<<bbmin[1]<<","<<bbmin[2]<<","<<"["<<grid_index_x<<"]"<<"["<<grid_index_y<<"]"<<"["<<grid_index_z<<"]"<<","<<endl;
                grid_out<< bbmax [0]<<","<<bbmax[1]<<","<<bbmax[2]<<","<<"["<<grid_index_x2<<"]"<<"["<<grid_index_y2<<"]"<<"["<<grid_index_z2<<"]"<<","<<endl;

                for(int i = 0; i < 3; i++) grid[x][y][z].bbmin[i] = bbmin[i];
                for(int i = 0; i < 3; i++) grid[x][y][z].bbmax[i] = bbmax[i];
            }
        }
    }
    grid_out.close();
    return 0;
}
char SPH::writeVTK()
{
//------------------------------------------------------------------------WATER--------------------------------------------------------------------------------
    if(g_Param.getVtkWater())
    {
        char w_data_out[100];
        static int w_datafile_part = 0;
        snprintf(w_data_out, 100, "data/vtk/water/w_%s_%d.vtk",g_Param.getRunName().c_str(),w_datafile_part++);
        ofstream w_data_outobj ( w_data_out );
        int nWater=getWaterCount();
        if( !w_data_outobj.is_open() )
        {
            cout << "  Couldn't open WaterDataPart_.vtk"  << endl;
            return 1;
        }
        double velocity_mag=0;
        double accel_mag=0;

        w_data_outobj<<"# vtk DataFile Version 5.0 "<<endl;
        w_data_outobj<<"Water Particle Positions "<<endl;
        w_data_outobj<<"ASCII "<<endl;
        w_data_outobj<<"DATASET UNSTRUCTURED_GRID "<<endl;
        w_data_outobj<<"POINTS "<<nWater<<" double"<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {

                    for(unsigned int i = 0; i < grid[x][y][z].water_particles.size(); i++)
                    {
                        double *pos = grid[x][y][z].water_particles[i].position;
                        w_data_outobj<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<" "<<endl;
                    }
                }
            }
        }
//OUTPUT VELOCITY
        w_data_outobj<<"POINT_DATA "<<nWater<<endl;

        w_data_outobj<<"SCALARS "<<"Velocity "<<"double "<<endl;
        w_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].water_particles.size(); i++)
                    {
                        double *vel = grid[x][y][z].water_particles[i].velocity;
                        velocity_mag= GetLength(vel);
                        w_data_outobj<<velocity_mag<<endl;
                    }
                }
            }
        }

//OUTPUT ACCELERATION
        w_data_outobj<<"SCALARS "<<"Acceleration "<<"double"<<endl;
        w_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {

                    for(unsigned int i = 0; i < grid[x][y][z].water_particles.size(); i++)
                    {
                        double   *accel = grid[x][y][z].water_particles[i].acceleration;
                        accel_mag=GetLength(accel);
                        w_data_outobj<<accel_mag<<endl;
                    }
                }
            }
        }
//OUTPUT DENSITY
        w_data_outobj<<"SCALARS "<<"Density "<<"double "<<endl;
        w_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].water_particles.size(); i++)    w_data_outobj << grid[x][y][z].water_particles[i].density<<endl;
                }
            }
        }
//OUTPUT PRESSURE
        w_data_outobj<<"SCALARS "<<"Pressure "<<"double "<<endl;
        w_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].water_particles.size(); i++)    w_data_outobj << grid[x][y][z].water_particles[i].pressure<<endl;
                }
            }
        }
        //OUTPUT CONTACT
        w_data_outobj<<"SCALARS "<<"Contact "<<"double "<<endl;
        w_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].water_particles.size(); i++)    w_data_outobj << grid[x][y][z].water_particles[i].contact_force<<endl;
                }
            }
        }
        //OUTPUT DAMPING
        w_data_outobj<<"SCALARS "<<"Contact "<<"double "<<endl;
        w_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].water_particles.size(); i++)    w_data_outobj << grid[x][y][z].water_particles[i].damping_force<<endl;
                }
            }
        }
        //OUTPUT WHICH PARTICLE, the particles will later be assigned colours to distinguish them.
        w_data_outobj<<"SCALARS "<<"Surface "<<"int"<<endl;
        w_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].water_particles.size();    i++) w_data_outobj << grid[x][y][z].water_particles[i].is_surface<<endl; //1 for surface
                }
            }
        }

//OUTPUT WHICH PARTICLE NEIGHBOURS COUNT. THIS IS DPENDANT ON THE SMOOTHING RADIUS
        w_data_outobj<<"SCALARS "<<"WaterNeighbours "<<"int"<<endl;
        w_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].water_particles.size();    i++) w_data_outobj << grid[x][y][z].water_particles[i].w_neighbour_count<<endl;
                }
            }
        }

        w_data_outobj<<"SCALARS "<<"BoundaryNeighbours "<<"int"<<endl;
        w_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].water_particles.size();    i++) w_data_outobj << grid[x][y][z].water_particles[i].b_neighbour_count<<endl;
                }
            }
        }
        w_data_outobj<<"SCALARS "<<"AirNeighbours "<<"int"<<endl;
        w_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].water_particles.size();    i++) w_data_outobj << grid[x][y][z].water_particles[i].a_neighbour_count<<endl;
                }
            }
        }
        w_data_outobj<<"SCALARS "<<"TotalNeighbours "<<"int"<<endl;
        w_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].water_particles.size();    i++) w_data_outobj << grid[x][y][z].water_particles[i].w_neighbour_count+grid[x][y][z].water_particles[i].b_neighbour_count+grid[x][y][z].water_particles[i].a_neighbour_count<<endl;
                }
            }
        }
        w_data_outobj.close();
    }
//-------------------------------------------------------------------------AIR--------------------------------------------------------------------------------
    if(g_Param.getVtkAir())
    {
        char a_data_out[100];
        static int a_datafile_part = 0;
        snprintf(a_data_out, 100, "data/vtk/air/a_%s_%d.vtk",g_Param.getRunName().c_str(), a_datafile_part++);
        ofstream a_data_outobj ( a_data_out );
        int nAir=getAirCount();
        if( !a_data_outobj.is_open() )
        {
            cout << "  Couldn't open AirDataPart_.vtk"  << endl;
            return 1;
        }
        double velocity_mag=0;
        double accel_mag=0;
        a_data_outobj<<"# vtk DataFile Version 5.0 "<<endl;
        a_data_outobj<<"Air Particle Positions "<<endl;
        a_data_outobj<<"ASCII "<<endl;
        a_data_outobj<<"DATASET UNSTRUCTURED_GRID "<<endl;
        a_data_outobj<<"POINTS "<<nAir<<"double"<<endl;
        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {

                    for(unsigned int i = 0; i < grid[x][y][z].air_particles.size(); i++)
                    {
                        double *pos = grid[x][y][z].air_particles[i].position;
                        a_data_outobj<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<" "<<endl;
                    }
                }
            }
        }
        a_data_outobj<<"POINT_DATA "<<nAir<<endl;
//OUTPUT VELOCITY

        a_data_outobj<<"SCALARS "<<"Velocity "<<"double"<<endl;
        a_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].air_particles.size(); i++)
                    {
                        double *vel = grid[x][y][z].air_particles[i].velocity;
                        velocity_mag=GetLength(vel);
                        a_data_outobj<<velocity_mag<<endl;

                    }
                }
            }
        }
//OUTPUT ACCELERATION
        a_data_outobj<<"SCALARS "<<"Acceleration "<<"double"<<endl;
        a_data_outobj<<"LOOKUP_TABLE default "<<endl;


        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].air_particles.size(); i++)
                    {
                        double   *accel = grid[x][y][z].air_particles[i].acceleration;
                        accel_mag=GetLength(accel);
                        a_data_outobj<<accel_mag<<endl;
                    }
                }
            }
        }
//OUTPUT PRESSURE

        a_data_outobj<<"SCALARS "<<"Density "<<"double"<<endl;
        a_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].air_particles.size(); i++)    a_data_outobj << grid[x][y][z].air_particles[i].density<<endl;
                }
            }
        }
        a_data_outobj<<"SCALARS "<<"Pressure "<<"double"<<endl;
        a_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].air_particles.size(); i++)    a_data_outobj << grid[x][y][z].air_particles[i].pressure<<endl;
                }
            }
        }
        a_data_outobj<<"SCALARS "<<"Contact "<<"double"<<endl;
        a_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].air_particles.size(); i++)    a_data_outobj << grid[x][y][z].air_particles[i].contact_force<<endl;
                }
            }
        }
        a_data_outobj<<"SCALARS "<<"Damping "<<"double"<<endl;
        a_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].air_particles.size(); i++)    a_data_outobj << grid[x][y][z].air_particles[i].damping_force<<endl;
                }
            }
        }
        a_data_outobj<<"SCALARS "<<"Surface "<<"int"<<endl;
        a_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].air_particles.size();    i++) a_data_outobj << grid[x][y][z].air_particles[i].is_surface<<endl; //1 for surface
                }
            }
        }


        a_data_outobj<<"SCALARS "<<"Interface "<<"int"<<endl;
        a_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].air_particles.size();    i++) a_data_outobj << grid[x][y][z].air_particles[i].is_interface<<endl; //1 for surface
                }
            }
        }
        a_data_outobj<<"SCALARS "<<"AirNeighbours "<<"int"<<endl;
        a_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].air_particles.size();    i++) a_data_outobj << grid[x][y][z].air_particles[i].a_neighbour_count<<endl; //1 for surface

                }
            }
        }
        a_data_outobj<<"SCALARS "<<"WaterNeighbours "<<"int"<<endl;
        a_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].air_particles.size();    i++) a_data_outobj << grid[x][y][z].air_particles[i].w_neighbour_count<<endl; //1 for surface
                }
            }
        }
        a_data_outobj<<"SCALARS "<<"BoundaryNeighbours "<<"int"<<endl;
        a_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].air_particles.size();    i++) a_data_outobj << grid[x][y][z].air_particles[i].b_neighbour_count<<endl; //1 for surface
                }
            }
        }
        a_data_outobj<<"SCALARS "<<"TotalNeighbours "<<"int"<<endl;
        a_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].air_particles.size();    i++) a_data_outobj << grid[x][y][z].air_particles[i].b_neighbour_count+grid[x][y][z].air_particles[i].a_neighbour_count+grid[x][y][z].air_particles[i].w_neighbour_count<<endl; //1 for surface

                }
            }
        }

        a_data_outobj.close();

    }
//------------------------------------------------------------------------BOUNDARY---------------------------------------------------------------------------
    if(g_Param.getVtkBoundary())
    {
        char b_data_out[100];
        static int b_datafile_part = 0;
        snprintf(b_data_out, 100, "data/vtk/boundary/b_%s_%d.vtk",g_Param.getRunName().c_str(),b_datafile_part++);
        ofstream b_data_outobj ( b_data_out );
        int nBoundary=getBoundaryCount();
        if( !b_data_outobj.is_open() )
        {
            cout << "  Couldn't open BoundaryDataPart_.vtk"  << endl;
            return 1;
        }
        b_data_outobj<<"# vtk DataFile Version 5.0 "<<endl;
        b_data_outobj<<"Boundary Particle Positions "<<endl;
        b_data_outobj<<"ASCII "<<endl;
        b_data_outobj<<"DATASET UNSTRUCTURED_GRID "<<endl;
        b_data_outobj<<"POINTS "<<nBoundary<<" double"<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {

                    for(unsigned int i = 0; i < grid[x][y][z].boundary_particles.size(); i++)
                    {
                        double *pos = grid[x][y][z].boundary_particles[i].position;
                        b_data_outobj<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<endl;
                    }
                }
            }
        }

        b_data_outobj<<"POINT_DATA "<<nBoundary<<endl;
//OUTPUT PRESSURE
        b_data_outobj<<"SCALARS "<<"Density "<<"double"<<endl;
        b_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].boundary_particles.size(); i++)    b_data_outobj << grid[x][y][z].boundary_particles[i].density<<endl;
                }
            }
        }
        b_data_outobj<<"SCALARS "<<"Pressure "<<"double"<<endl;
        b_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].boundary_particles.size(); i++)    b_data_outobj << grid[x][y][z].boundary_particles[i].pressure<<endl;
                }
            }
        }

        b_data_outobj<<"SCALARS "<<"BoundaryNeighbours"<<"int"<<endl;
        b_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].boundary_particles.size();    i++) b_data_outobj << grid[x][y][z].boundary_particles[i].b_neighbour_count<<endl;
                }
            }
        }

        b_data_outobj<<"SCALARS "<<"WaterNeighbours"<<"int"<<endl;
        b_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].boundary_particles.size();    i++) b_data_outobj << grid[x][y][z].boundary_particles[i].w_neighbour_count<<endl;
                }
            }
        }


        b_data_outobj<<"SCALARS "<<"AirNeighbours"<<"int"<<endl;
        b_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].boundary_particles.size();    i++) b_data_outobj << grid[x][y][z].boundary_particles[i].a_neighbour_count<<endl;
                }
            }
        }

        b_data_outobj<<"SCALARS "<<"TotalNeighbours"<<"int"<<endl;
        b_data_outobj<<"LOOKUP_TABLE default "<<endl;

        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].boundary_particles.size();    i++) b_data_outobj << grid[x][y][z].boundary_particles[i].b_neighbour_count+grid[x][y][z].boundary_particles[i].w_neighbour_count+grid[x][y][z].boundary_particles[i].a_neighbour_count<<endl;
                }
            }
        }
        b_data_outobj.close();

    }

    return 0;
}
char SPH::writeCSV()
{
//--------------------------------------------------------------------WATER-------------------------------------------------------------------------------

    if(g_Param.getCsvWater()==1)
    {
        char water_out[100];
        static int Wfile_part = 0;
        snprintf(water_out, 100, "data/csv/water/w_%s_%d.csv", g_Param.getRunName().c_str(),Wfile_part++);
        ofstream out_water( water_out );
        if( !out_water )
        {
            if(g_Param.getCsvWater()) cout<< "ERROR: Could not open water cvs file. Make sure there the following location exists: data/csv/water/NameOfWaterFile.csv "  << endl
                                              << "If it does not exist, please create the appropriate folders"<<endl;
            return 1;
        }

        int count=0;
        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].water_particles.size(); i++)
                    {
                        double *pos = grid[x][y][z].water_particles[i].position;
                        count++;
                        out_water <<pos[0]<<","<<pos[1]<<","<<pos[2]<<","<<"1"<<","<<"["<<x<<"]"<<"["<<y<<"]""["<<z<<"]"<<endl;
                    }
                }
            }
        }
        out_water.close();
    }
//--------------------------------------------------------------------AIR----------------------------------------------------------------------
    if (g_Param.getCsvAir()==1)
    {
        char air_out[100];
        static int Afile_part = 0;
        snprintf(air_out, 100, "data/csv/air/a_%s_%d.csv",g_Param.getRunName().c_str(), Afile_part++);
        ofstream out_air( air_out );
        if( !out_air )
        {
            if(g_Param.getCsvAir())cout<< "ERROR: Could not open air cvs file. Make sure there the following location exists: data/csv/air/NameOfAirFile.csv "  << endl
                                           << "If it does not exist, please create the appropriate folders"<<endl;
            return 1;
        }
        int count=0;
        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].air_particles.size(); i++)
                    {
                        double *pos = grid[x][y][z].air_particles[i].position;
                        count++;
                        out_air<<pos[0]<<","<<pos[1]<<","<<pos[2]<<","<<"2"<<","<<"["<<x<<"]"<<"["<<y<<"]""["<<z<<"]"<<endl;
                    }
                }
            }
        }
        out_air.close();
    }
    //------------------------------------------------------------------BOUNDARY-------------------------------------------------------------------------
    if(g_Param.getCsvBoundary()==1&&g_Param.getSpinSpeed()>0)
    {
        char boundary_out[100];
        static int Bfile_part = 0;
        snprintf(boundary_out, 100, "data/csv/boundary/b_%s_%d.csv",g_Param.getRunName().c_str(), Bfile_part++);
        ofstream out_boundary( boundary_out );
        if( !out_boundary )
        {
            if(g_Param.getCsvBoundary())  cout<< "ERROR: Could not open boundary cvs file. Make sure the following location exists: data/csv/boundary/NameOfBoundaryFile.csv "  << endl
                                                  << "If it does not exist, please create the appropriate folders"<<endl;
            return 1;
        }
        int count=0;
        for(int x = 0; x < XGRID; x++)
        {
            for(int y = 0; y < YGRID; y++)
            {
                for(int z = 0; z < ZGRID; z++)
                {
                    for(unsigned int i = 0; i < grid[x][y][z].boundary_particles.size(); i++)
                    {
                        double *pos = grid[x][y][z].boundary_particles[i].position;
                        count++;
                        out_boundary<<pos[0]<<","<<pos[1]<<","<<pos[2]<<","<<"3"<<","<<"["<<x<<"]"<<"["<<y<<"]"<<"["<<z<<"]"<<endl;
                    }
                }
            }
        }
        out_boundary.close();
    }
    return 0;
}
char SPH::printSettings()
{


    char run_out[100];
    snprintf(run_out, 100, "data/run/%s.txt", g_Param.getRunName().c_str());
    //ofstream out_water_air( water_air_out );
    ofstream out_run( run_out );

    if( !out_run )
    {
        cout<< "ERROR: Could not open run file. Does folder: 'data/run/' exist?"  << endl;
        return 1;
    }

    out_run<<"-------------------------------------------------------------------------------"<<endl
           <<"                                  JFE SPH"<<endl
           <<"-------------------------------------------------------------------------------"<<endl
           <<"    [A] Run Name:                 "<<g_Param.getRunName()<<endl<<endl

           <<"    [B] Notes                     [F] Time Step Settings     "<<endl
           <<"    [C] Particle Settings         [G] Read In Positions  "<<endl
           <<"    [D] Particle Block Settings   [H] Environment Settings     "<<endl
           <<"    [E] Lance Settings            [I] Print Settings    "<<endl;

    out_run  <<endl
             <<"    QUICK SETTINGS"<<endl
             <<"    --------------"<<endl
             <<"    [I] Print Every:        "<<"["<<g_Param.getPrintEvery()<<"]"<<endl;

    out_run <<"    [E] Lance:              " ;
    if(g_Param.getLanceOn() ==1) out_run<<  "[ON]"<<endl;
    if(g_Param.getLanceOn() ==0) out_run<< "[OFF]"<<endl;
    out_run <<"    [D] Particle Block:     ";
    if(g_Param.getParticleBlockOn() ==1) out_run<< "[ON]"<<endl;
    if(g_Param.getParticleBlockOn() ==0) out_run<< "[OFF]"<<endl;
    out_run <<"    [H] Gravity:            ";
    if(g_Param.getGravityIsOn() ==1) out_run<< "[ON]"<<endl;
    if(g_Param.getGravityIsOn() ==0) out_run<< "[OFF]"<<endl;
    out_run <<"    [H] Converter:          ";
    if(g_Param.getConverterOn() ==1) out_run<< "[ON]"<<endl;
    if(g_Param.getConverterOn() ==0) out_run<< "[OFF]"<<endl;
    out_run <<"    [H] Mill:               ";
    if(g_Param.getMillOn() ==1) out_run<< "[ON]"<<endl;
    if(g_Param.getMillOn() ==0) out_run<< "[OFF]"<<endl;
    out_run <<"    [G] Initialized Fluid:  "<<"["<<getWaterCount()<<"]"<<endl;
    out_run<<"-------------------------------------------------------------------------------"
           <<"                                 Lance Settings "<<endl
           <<endl
           <<"    [A] Lance On:         "<<g_Param.getLanceOn()<<""<<endl
           <<"    [B] Lance Position:   "<<"["<<g_Param.getLancePosX()<<","<<g_Param.getLancePosY()<<","<<g_Param.getLancePosZ()<<"] x (CONVERTER DIMENSIONS)"<<endl
           <<"    [C] Flow Rate         "<<"Air Outputted Every "<<g_Param.getRateOfAir()<<" Steps"<<endl
           <<"    [D] Radius of Flow:   "<<g_Param.getLanceOpeningRadius() <<endl
           <<"    [E] Velocity of Flow: "<<"X: ["<<g_Param.getLanceOutputVelocityXMin()<<","<<g_Param.getLanceOutputVelocityXMax()<<"]  Y: ["<<g_Param.getLanceOutputVelocityYMin()<<","<<g_Param.getLanceOutputVelocityYMax()<<"]  Z: ["<<g_Param.getLanceOutputVelocityZMin()<<","<<g_Param.getLanceOutputVelocityZMax()<<"]"<<endl
           <<"    [N] Water per Step:         "<< g_Param.getWater_per_step()<<endl
           <<"    [O] Air per Step:           "<<g_Param.getAir_per_step()<<endl
           <<"    [Q] Number Of Air Rings: "<<g_Param.getNumberOfRings()<<endl
           <<"    [R] Number Of Air Seeds: "<<g_Param.getNumberofSeeds()<<endl
           <<"-------------------------------------------------------------------------------"<<endl
           <<"                       Particle Block Settings"<<endl
           <<endl
           <<"    [A] Length: "<<"("<<g_Param.getBlockLength()<<")"<<"x(H)"<<endl
           <<"    [B] Width: "<<"("<<g_Param.getBlockWidth()<<")"<<"x(H)"<<endl
           <<"    [C] Height: "<<"("<<g_Param.getBlockHeight()<<")"<<"x(H)"<<endl
           <<"    [D] Location [RELATIVE TO GRID SIZE]: "<<"("<<g_Param.getParticleBlockLocationX()<<","<<g_Param.getParticleBlockLocationY()<<","<<g_Param.getParticleBlockLocationZ()<<")"<<"x(Grid Dimensions)"<<endl
           <<"    [E] SPACING "<<g_Param.getParticleBlockSpacing()<<endl
           <<"    [F] Initialize Block = "<<g_Param.getParticleBlockOn() <<endl<<endl
           <<"    Number of Particles In Block = "<<g_Param.getBlockLength()*g_Param.getBlockHeight()*g_Param.getBlockWidth()<<endl
           <<"    Smoothing Length H: "<<g_Param.getSmoothingRadius()<<endl
           <<"-------------------------------------------------------------------------------"<<endl
           <<"                       Timestep Settings"<<endl
           <<endl
           <<"    [A] Size of Timesteps:   "<< g_Param.getDelta_t()<<endl
           <<"    [B] Number of Timesteps: "<<g_Param.getTimesteps()<<endl
           <<"-------------------------------------------------------------------------------"<<endl
           <<"                      ENVIRONMENT PROPERITIES"<<endl
           <<endl
           <<"    [A] Gravity:            "<<g_Param.getGravityIsOn() <<endl
           <<"    [B] Converter Boundary  "<<g_Param.getConverterOn() <<endl
           <<"    [C] Particle Deletion:  "<<g_Param.getParticleDeletion() <<endl
           <<"    [D] Mill Boundary       "<<g_Param.getMillOn() <<endl
           <<"    [E] Mill Speed          "<<g_Param.getSpinSpeed()<<endl
           <<"-------------------------------------------------------------------------------"<<endl

           <<"    [Z] <--Home            PARTICLE PROPERTIES"<<endl
           <<endl
           <<"    [A] Water Rest Density:     "<<g_Param.getRho0_w() <<endl
           <<"    [B] Air Rest Density:       "<<g_Param.getRho0_a() <<endl
           <<"    [C] Boundary Rest Density:  "<<g_Param.getRho0_b() <<endl<<endl

           <<"    [D] Water Particle Mass:    "<<g_Param.getWater_mass()<<endl
           <<"    [E] Air Particle Mass:      "<<g_Param.getAir_mass()<<endl
           <<"    [F] Boundary Particle Mass: "<<g_Param.getBoundary_mass()<<endl<<endl

           <<"    [G] Water Viscocity:        "<<g_Param.getMu_w()<<endl
           <<"    [H] Air Viscocity:          "<<g_Param.getMu_a()<<endl
           <<"    [I] Boundary Viscocity:     "<<g_Param.getMu_b()<<endl<<endl

           <<"    [J] Smoothing Radius:       "<<g_Param.getSmoothingRadius()<<endl
           <<"    [K] Gas Stiffness           "<<g_Param.getKvalue()<<endl
           <<"    [L] Hertzian Stiffness      "<<g_Param.getHertzian()<<endl
           <<"    [M] Hertzian Damping        "<<g_Param.getDamping()<<endl
           <<"    [N] Tangential Friction     "<<g_Param.getTangentialFriction()<<endl

           <<"-------------------------------------------------------------------------------"<<endl
           <<"    NOTES:"<<g_Param.getNotes()

           <<"    "<<endl
           <<"-------------------------------------------------------------------------------"<<endl;
    out_run.close();
    return 0;
}

//





















