#include "Parameters.h"
#include <utility>


void SPH::processStep()
{
    //controls how oftern the step count prints to the screen
    if(g_Param.getStepCount()%10 == 0) cout<<"   "<<"\b"<<g_Param.getStepCount()<<flush;

    //-------------------------------------------------------------------
    //       LANCE CREATES 1 OXYGEN RING EVERY SPECIFIND NUMBER OF STEPS
    //-------------------------------------------------------------------
    spawnParticles(g_Param.getStepCount());

    //-------------------------------------------------
    //  STIRRING GAS, NOT FULLY IMPLEMENTED YET
    //-------------------------------------------------
    initializeStirGas();


    //for every grid in the x direction
    for(int x = 0; x < XGRID; x ++)    {
        //for every grid in the  y direction
        for(int y = 0; y < YGRID; y ++)        {
            //for every grid in the z direction
            for(int z = 0; z < ZGRID; z ++)            {
                for(unsigned int i = 0; i < grid[x][y][z].water_particles.size(); i++)
                {
                    //-----------------------------------------------------
                    // FIND W  DENSITIES + PRESSURES (USING SPH) IGNORE AIR
                    //----------------------------------------------------
                    updateDensity(i, x, y, z, true, false, false);
                }

                for(unsigned int i = 0; i < grid[x][y][z].air_particles.size(); i++)
                {
                    //--------------------------------------------------------
                    // FIND A  DENSITIES + PRESSURES (USING SPH) IGNORE WATER
                    //-------------------------------------------------------
                    updateDensity(i, x, y, z, false, true, false);
                }

                for(unsigned int i = 0; i < grid[x][y][z].boundary_particles.size(); i++)
                {   //---------------------------------------------------------
                    // FIND B  DENSITIES + PRESSURES (USING SPH)  IGNORE NOTHING
                    //-----------------------------------------------------------
                    updateDensity(i, x, y, z, false, false, true);
                }
            }
        }
    }
    vector<Particle> new_air_particles[XGRID][YGRID][ZGRID];

    //-------------------------------------------
    // FIND SPH+DEM FORCES + UPDATE POSITIONS
    //-------------------------------------------
    for(int x = 0; x < XGRID; x++)    {
        for(int y = 0; y < YGRID; y++)        {
            for(int z = 0; z < ZGRID; z++)            {
                for(unsigned int i = 0; i < grid[x][y][z].water_particles.size(); i++)
                {
                    updateParticle(i, x, y, z, true,false,false, new_air_particles[x][y][z]); //
                }
                for(unsigned int i = 0; i < grid[x][y][z].air_particles.size(); i++)
                {
                    updateParticle(i, x, y, z, false,true,false, new_air_particles[x][y][z]);
                }
                for(unsigned int i = 0; i < grid[x][y][z].boundary_particles.size(); i++)
                {
                    updateParticle(i, x, y, z, false,false,true, new_air_particles[x][y][z]);
                }
            }
        }
    }

     //-------------------------------------------
    // IGNORE THIS NOT IMPORTANT TO CONVERTER
    //-------------------------------------------
    for(int x = 0; x < XGRID; x++)    {
        for(int y = 0; y < YGRID; y++)        {
            for(int z = 0; z < ZGRID; z++)            {
                grid[x][y][z].air_particles.insert(grid[x][y][z].air_particles.end(),
                                                   new_air_particles[x][y][z].begin(), new_air_particles[x][y][z].end());  // insert the new_air_particles in the usual air particles vector.
            }
        }
    }
     //-------------------------------------------
    // UPDATE SEARCH GRID INDEXES
    //-------------------------------------------
    for(int x = 0; x < XGRID; x++)
    {
        for(int y = 0; y < YGRID; y++)
        {
            for(int z = 0; z < ZGRID; z++)
            {
                updateGrid(x, y, z);
            }
        }
    }
}
void SPH::updateDensity(int particle_id, int id_x, int id_y, int id_z, bool is_water_particle, bool is_air_particle, bool is_boundary_particle)
{
    double H= g_Param.getSmoothingRadius();
    double *pos;
    if (is_water_particle)    pos = grid[id_x][id_y][id_z].water_particles[particle_id].position;
    if (is_air_particle)      pos = grid[id_x][id_y][id_z].air_particles[particle_id].position;
    if (is_boundary_particle) pos = grid[id_x][id_y][id_z].boundary_particles[particle_id].position;

    //the weighted sum of particles that fall within the smoothing radius H
    double sumW_water     = 0.0, sumW_air = 0.0, sumW_boundary=0;    //
    int w_neighbour_count =0, a_neighbour_count=0, b_neighbour_count=0;
    //scalar value to identify what particle is a surface particles. will later be visualized in paraview
    int interface_count=0;

    // the centered particle will only search for neighbours in 27 grids. or 3 in each dimension; the grid it is in [grid index ]+0,
    // the one before it [grid index] -1 and the next grid [grid index]+1
    //id_x is the grid index
    for(int x = -1; x <= 1; x++)    {
        if(id_x+x < 0)   continue;
        if(id_x+x >= XGRID) break;
        for(int y = -1; y <= 1; y++)        {
            if(id_y+y < 0)   continue;
            if(id_y+y >= YGRID) break;
            for(int z = -1; z <= 1; z++)            {
                if(id_z+z < 0)  continue;
                if(id_z+z >= ZGRID) break;

                if (is_water_particle==1)                {
                //------------------------------------------------------------------------------------------
                // GET WATER'S DENSITY USING SPH BY SUMMING WATER AND BOUNDARY PARTICLES (IGNORE AIR)
                //------------------------------------------------------------------------------------------
                    for(unsigned int i = 0; i < grid[id_x+x][id_y+y][id_z+z].water_particles.size(); i++)
                    {
                        //find the distance between the potential neighbor particle and the centred particle
                        double *neighbor_pos = grid[id_x+x][id_y+y][id_z+z].water_particles[i].position;
                        double r[3]            = { pos[0]-neighbor_pos[0], pos[1]-neighbor_pos[1], pos[2]-neighbor_pos[2] };
                        double r_square       = DotProduct(r, r);

                        //if the distance is within the smoothing radius H, add its influence
                        if(r_square <= H*H)                        {
                            sumW_water += Wpoly6(r_square);
                            w_neighbour_count++;
                        }
                    }
                    for(unsigned int i = 0; i < grid[id_x+x][id_y+y][id_z+z].boundary_particles.size(); i++)
                    {
                        //find the distance between the potential neighbor particle and the centred particle
                        double *neighbor_pos = grid[id_x+x][id_y+y][id_z+z].boundary_particles[i].position;
                        double r[3]          = { pos[0]-neighbor_pos[0], pos[1]-neighbor_pos[1], pos[2]-neighbor_pos[2] };
                        double r_square      = DotProduct(r, r);

                        //if the distance is within the smoothing radius H, add its influence
                        if(r_square <= H*H)
                        {
                            sumW_boundary += Wpoly6(r_square);
                            b_neighbour_count++;
                        }
                    }
                }
                if (is_air_particle==1)                {
                //------------------------------------------------------------------------------------------
                // GET AIRS'S DENSITY BY SUMMING AIR AND BOUNDARY PARTICLES (no water)
                //------------------------------------------------------------------------------------------
                    for(unsigned int i = 0; i < grid[id_x+x][id_y+y][id_z+z].air_particles.size(); i++)
                    {
                        //find the distance between the potential neighbor particle and the centred particle
                        double *neighbor_pos = grid[id_x+x][id_y+y][id_z+z].air_particles[i].position;
                        double r[3]          = { pos[0]-neighbor_pos[0], pos[1]-neighbor_pos[1], pos[2]-neighbor_pos[2] };
                        double r_square      = DotProduct(r, r);

                        //if the distance is within the smoothing radius H, add its influence
                        if(r_square <= H*H)
                        {
                            sumW_air += Wpoly6(r_square);
                            a_neighbour_count++;
                        }
                    }

                    for(unsigned int i = 0; i < grid[id_x+x][id_y+y][id_z+z].boundary_particles.size(); i++)
                    {
                        //find the distance between the potential neighbor particle and the centred particle
                        double *neighbor_pos = grid[id_x+x][id_y+y][id_z+z].boundary_particles[i].position;
                        double r[3]          = { pos[0]-neighbor_pos[0], pos[1]-neighbor_pos[1], pos[2]-neighbor_pos[2] };
                        double r_square      = DotProduct(r, r);

                        if(is_air_particle&&r_square<= 0.0625*H*H)  grid[id_x][id_y][id_z].air_particles[particle_id].is_outside   = true;
                        // if the air particle comes with H/4 to the boundary it will be deleted however it will still gain density from it

                        //if the distance is within the smoothing radius H, add its influence
                        if(r_square <= H*H)
                        {
                            sumW_boundary += Wpoly6(r_square);
                            b_neighbour_count++;

                        }
                    }
                    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    // THIS DOES NOT ADD THE WATER PARTICLES TO THE DENSITY IT ONLY IS USED TO SEE
                    // HOW MAY WATER NEIGHBOURS THE AIR PARTICLE HAS. THIS IS NOT VRY IMPORTANT. IGNORE
                    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    for(unsigned int i = 0; i < grid[id_x+x][id_y+y][id_z+z].water_particles.size(); i++)
                    {

                        double *neighbor_pos = grid[id_x+x][id_y+y][id_z+z].water_particles[i].position;
                        double r[3]            = { pos[0]-neighbor_pos[0], pos[1]-neighbor_pos[1], pos[2]-neighbor_pos[2] };
                        double r_square       = DotProduct(r, r);


                        if(r_square <= 4*H*H)
                        {
                            interface_count++;
                            if (interface_count>4)
                            {
                                grid[id_x][id_y][id_z].air_particles[particle_id].is_interface=1;
                            }
                            else  grid[id_x][id_y][id_z].air_particles[particle_id].is_interface=0;
                        }
                    }
                }

                if (is_boundary_particle==1)                {
                //------------------------------------------------------------------------------------------
                // GET BOUNDARY'S DENSITY BY SUMMING ALL OTHER PARTICLES (W,B,A)
                //------------------------------------------------------------------------------------------
                    for(unsigned int i = 0; i < grid[id_x+x][id_y+y][id_z+z].water_particles.size(); i++)
                    {
                        //find the distance between the potential neighbor particle and the centred particle
                        double *neighbor_pos = grid[id_x+x][id_y+y][id_z+z].water_particles[i].position;
                        double r[3]            = { pos[0]-neighbor_pos[0], pos[1]-neighbor_pos[1], pos[2]-neighbor_pos[2] };
                        double r_square       = DotProduct(r, r);

                        //if the distance is within the smoothing radius H, add its influence
                        if(r_square <= H*H)
                        {
                            sumW_water += Wpoly6(r_square);
                            w_neighbour_count++;
                        }
                    }

                    for(unsigned int i = 0; i < grid[id_x+x][id_y+y][id_z+z].boundary_particles.size(); i++)
                    {
                        //find the distance between the potential neighbor particle and the centred particle
                        double *neighbor_pos = grid[id_x+x][id_y+y][id_z+z].boundary_particles[i].position;
                        double r[3]          = { pos[0]-neighbor_pos[0], pos[1]-neighbor_pos[1], pos[2]-neighbor_pos[2] };
                        double r_square      = DotProduct(r, r);


                        //if the distance is within the smoothing radius H, add its influence
                        if(r_square <= H*H)
                        {
                            sumW_boundary += Wpoly6(r_square);
                            b_neighbour_count++;
                        }
                    }
                    for(unsigned int i = 0; i < grid[id_x+x][id_y+y][id_z+z].air_particles.size(); i++)
                    {
                        //find the distance between the potential neighbor particle and the centred particle
                        double *neighbor_pos = grid[id_x+x][id_y+y][id_z+z].air_particles[i].position;
                        double r[3]          = { pos[0]-neighbor_pos[0], pos[1]-neighbor_pos[1], pos[2]-neighbor_pos[2] };
                        double r_square      = DotProduct(r, r);


                        //if the distance is within the smoothing radius H, add its influence
                        if(r_square <= H*H)
                        {
                            sumW_air += Wpoly6(r_square);
                            a_neighbour_count++;
                        }
                    }
                }
            } // for(int z = -1; z <= 1; z++
        } // for(int y = -1; y <= 1; y++){
    } // for(int x = -1; x <= 1; x++){


    //------------------------------------------------------------------------------------------
    // GET DENSITY BY SUMMING CONTRIBUTIONS FROM EACH TYPE W+A+B
    // note: if it is water then the sumW_air=0 and vice versa
    //------------------------------------------------------------------------------------------
    double water_mass=g_Param.getWater_mass(), air_mass = g_Param.getAir_mass(), boundary_mass= g_Param.getBoundary_mass();

    double density = water_mass*sumW_water +air_mass*sumW_air + boundary_mass*sumW_boundary;

    double K=g_Param.getKvalue();
    double Rho0_b =g_Param.getRho0_b(),Rho0_w =g_Param.getRho0_w(),Rho0_a =g_Param.getRho0_a();

    //------------------------------------------------------------------------------------------
    // ASSIGN THE ABOVE CALCULATED DENSITY TO PARTICLE
    //------------------------------------------------------------------------------------------

    if(is_boundary_particle)
    {
        grid[id_x][id_y][id_z].boundary_particles[particle_id].density                                       = density;
        grid[id_x][id_y][id_z].boundary_particles[particle_id].inv_density                                   = 1.0 / density;
        grid[id_x][id_y][id_z].boundary_particles[particle_id].pressure                                      = K*(density - Rho0_b); //k is stiffness
        if(g_Param.getGravityIsOn())grid[id_x][id_y][id_z].boundary_particles[particle_id].gravity_force     = 0;// we don't need gravity because we don't step the boundary particles
        if (g_Param.getGravityIsOn()==0)grid[id_x][id_y][id_z].boundary_particles[particle_id].gravity_force = 0.0;
        grid[id_x][id_y][id_z].boundary_particles[particle_id].buoyant_force                                 = 0.0;
        grid[id_x][id_y][id_z].boundary_particles[particle_id].is_surface                                    = 0;
        grid[id_x][id_y][id_z].boundary_particles[particle_id].b_neighbour_count                             = b_neighbour_count;
        grid[id_x][id_y][id_z].boundary_particles[particle_id].a_neighbour_count                             = a_neighbour_count;
        grid[id_x][id_y][id_z].boundary_particles[particle_id].w_neighbour_count                             = w_neighbour_count;

        //ONLY RELEVANT FOR SPINNING BOUNDARIES (NOT CONVERTER)
        if(g_Param.getStepCount()==1)        {
            double x_arm= abs(pos[0]-0.5*XGRID*H);
            double z_arm = abs(pos[2]-0.5*XGRID*H);
            double radius_orig = sqrt(pow(x_arm,2.0)+pow(z_arm,2.0)); // this is the radius from the center of the converter
            grid[id_x][id_y][id_z].boundary_particles[particle_id].radius_orig                                   = radius_orig;
        }
    }
    if(is_water_particle)
    {
        grid[id_x][id_y][id_z].water_particles[particle_id].density                                          = density;
        grid[id_x][id_y][id_z].water_particles[particle_id].inv_density                                      = 1.0 / density;
        grid[id_x][id_y][id_z].water_particles[particle_id].pressure                                         = K*(density - Rho0_w);
        if(g_Param.getGravityIsOn())grid[id_x][id_y][id_z].water_particles[particle_id].gravity_force        = -density*9.80665;
        if(g_Param.getGravityIsOn()==0)grid[id_x][id_y][id_z].water_particles[particle_id].gravity_force     = 0;
        grid[id_x][id_y][id_z].water_particles[particle_id].buoyant_force                                    = 0.0;
        grid[id_x][id_y][id_z].water_particles[particle_id].is_surface                                       = 0;
        grid[id_x][id_y][id_z].water_particles[particle_id].w_neighbour_count                                = w_neighbour_count;
        grid[id_x][id_y][id_z].water_particles[particle_id].a_neighbour_count                                = a_neighbour_count;
        grid[id_x][id_y][id_z].water_particles[particle_id].b_neighbour_count                                = b_neighbour_count;
    }
    if(is_air_particle)
    {
        grid[id_x][id_y][id_z].air_particles[particle_id].density                                            = density;
        grid[id_x][id_y][id_z].air_particles[particle_id].inv_density                                        = 1.0 / density;
        grid[id_x][id_y][id_z].air_particles[particle_id].pressure                                           = K*(density - Rho0_a);
        if(g_Param.getGravityIsOn())grid[id_x][id_y][id_z].air_particles[particle_id].gravity_force          = -density*9.80665;
        if(g_Param.getGravityIsOn()==0)grid[id_x][id_y][id_z].air_particles[particle_id].gravity_force       = 0;
        grid[id_x][id_y][id_z].air_particles[particle_id].buoyant_force                                     = 0*(density-Rho0_a)*9.80665; //13.0

        grid[id_x][id_y][id_z].air_particles[particle_id].is_surface                                         = 0;
        grid[id_x][id_y][id_z].air_particles[particle_id].a_neighbour_count                                  = a_neighbour_count;
        grid[id_x][id_y][id_z].air_particles[particle_id].w_neighbour_count                                  = w_neighbour_count;
        grid[id_x][id_y][id_z].air_particles[particle_id].b_neighbour_count                                  = b_neighbour_count;
    }
}
void SPH::updateParticle(int particle_id, int id_x, int id_y, int id_z, bool is_water_particle, bool is_air_particle, bool is_boundary_particle, vector<Particle> &new_air_particles)
{
    Particle *pt;
    double viscosity;

    double H = g_Param.getSmoothingRadius();


    // SET THE APPROPRIATE VISCOCITY
    if (is_water_particle)
    {
        pt        = &(grid[id_x][id_y][id_z].water_particles[particle_id]);
        viscosity = g_Param.getMu_w();
        grid[id_x][id_y][id_z].water_particles[particle_id].contact_force= 0;
        grid[id_x][id_y][id_z].water_particles[particle_id].damping_force= 0;
    }
     // SET THE APPROPRIATE VISCOCITY
    if (is_air_particle)
    {
        pt        = &(grid[id_x][id_y][id_z].air_particles[particle_id]);
        viscosity = g_Param.getMu_a();
        grid[id_x][id_y][id_z].air_particles[particle_id].contact_force= 0;
        grid[id_x][id_y][id_z].air_particles[particle_id].damping_force= 0;
    }
     // SET THE APPROPRIATE VISCOCITY
    if (is_boundary_particle)
    {
        pt        = &(grid[id_x][id_y][id_z].boundary_particles[particle_id]);
        viscosity =g_Param.getMu_b() ;
    }

    double f_pressure_w[3] = { 0.0, 0.0, 0.0 }, f_viscosity_w[3] = { 0.0, 0.0, 0.0 };
    double f_pressure_a[3] = { 0.0, 0.0, 0.0 }, f_viscosity_a[3] = { 0.0, 0.0, 0.0 };
    double f_pressure_b[3] = { 0.0, 0.0, 0.0 }, f_viscosity_b[3] = { 0.0, 0.0, 0.0 };
    double f_contact [3]  = { 0.0, 0.0, 0.0 };
    double f_damping [3]  = { 0.0, 0.0, 0.0 };
    double f_friction[3] = { 0.0, 0.0, 0.0 };
    double color_field_normal_w[3] = { 0.0, 0.0, 0.0 }, color_field_laplacian_w = 0.0;
    double color_field_normal_a[3] = { 0.0, 0.0, 0.0 }, color_field_laplacian_a = 0.0;
    double color_field_normal_b[3] = { 0.0, 0.0, 0.0 }, color_field_laplacian_b = 0.0;

    //ACCESS 27 SORTING GRIDS
    for(int x = -1; x <= 1; x++)    {
        if(id_x+x < 0)   continue;
        if(id_x+x >= XGRID) break;
        for(int y = -1; y <= 1; y++)        {
            if(id_y+y < 0)   continue;
            if(id_y+y >= YGRID) break;
            for(int z = -1; z <= 1; z++)            {
                if(id_z+z < 0)  continue;
                if(id_z+z >= ZGRID) break;


                for(unsigned int i = 0; i < grid[id_x+x][id_y+y][id_z+z].water_particles.size(); i++)
                {
                    if(is_water_particle == 1||is_boundary_particle == 1)
                    {

                    //--------------------------------------------------------------------------------------------------------
                    //                  CALCULATE SPH FORCES BETWEEN WATER-WATER OR WATER-BOUNDARY
                    //---------------------------------------------------------------------------------------------------------

                        Particle *neighbour_pt = &(grid[id_x+x][id_y+y][id_z+z].water_particles[i]);
                        //find the distance between the potential neighbor particle and the centred particle
                        double r[3]     = { pt->position[0] - neighbour_pt->position[0],
                                            pt->position[1] - neighbour_pt->position[1],
                                            pt->position[2] - neighbour_pt->position[2]
                                          };
                        double r_square = DotProduct(r, r);

                        //if the distance is within the smoothing radius H, add its forces
                        if(r_square    <= H*H)                     {
                            if(r_square > 0.0)                            {
                                double gradient[3];
                                Wpoly6Grad(r, r_square, gradient);
                                double weight_p = (pt->pressure + neighbour_pt->pressure)*0.5 * neighbour_pt->inv_density; //pressure force
                                for(int j = 0; j < 3; j++) f_pressure_w[j]         += weight_p * gradient[j];
                                for(int j = 0; j < 3; j++) color_field_normal_w[j] += neighbour_pt->inv_density * gradient[j];
                            }
                            double weight_v = neighbour_pt->inv_density * WviscosityLaplacian(r_square);
                            for(int j = 0; j < 3; j++) f_viscosity_w[j] += (neighbour_pt->velocity[j]-pt->velocity[j]) * weight_v;
                            color_field_laplacian_w += neighbour_pt->inv_density * Wpoly6Laplacian(r_square);
                        }
                    }
                    if (is_air_particle==1 )
                    {
                    //--------------------------------------------------------------------------------------------------------
                    //   CALCULATE DEM FORCES.  SCROLL DOWN TO ELSE. DEM TYPE IS USUALLY =2 FOR CONVERTER NOT 1
                    //---------------------------------------------------------------------------------------------------------
                        if(g_Param.getDemType()==1) //usually type 2, so ignore this
                        {
                            double m_k = g_Param.getHertzian();  //spring constant
                            double m_dt = g_Param.getDelta_t() ; // time step
                            double m_ks= 0.4*m_k;                 //shear stiffness (cundell)
                            double m_mu=0.5;                         // coeffient of friction
                            double particle_radius_1 = (1/2)*H;
                            double particle_radius_2 = (1/2)*H;
                            double m_Ffric [3];                      //friction force
                            Particle *neighbour_pt = &(grid[id_x+x][id_y+y][id_z+z].water_particles[i]);
                            //calculate distance
                            double D[3]     = { pt->position[0] - neighbour_pt->position[0],
                                                pt->position[1] - neighbour_pt->position[1],
                                                pt->position[2] - neighbour_pt->position[2]
                                              };
                            double eq_dist = particle_radius_1+particle_radius_2;
                            double D_square = DotProduct(D, D);
                            //check if there is contact
                            if(D_square  <= eq_dist*eq_dist)
                            {
                                //calculate contact forces
                                //elastic force
                                double dist = GetDistance(pt->position, neighbour_pt->position);
                                double D_norm [3]= { D[0]/dist,D[1]/dist,D[2]/dist };
                                double force [3] = { D[0]*(m_k)*(dist-eq_dist)/dist,
                                                     D[1]*(m_k)*(dist-eq_dist)/dist,
                                                     D[2]*(m_k)*(dist-eq_dist)/dist,
                                                   };
                                //---frictional force---
                                //particle movement since last step
                                double d1[3]= { pt->velocity[0]*m_dt,
                                                pt->velocity[1]*m_dt,
                                                pt->velocity[2]*m_dt
                                              };
                                double d2[3]= { neighbour_pt->velocity[0]*m_dt,
                                                neighbour_pt->velocity[1]*m_dt,
                                                neighbour_pt->velocity[2]*m_dt
                                              };
                                double ds[3]= {(d1[0]-d2[0]),
                                               (d1[1]-d2[1]),
                                               (d1[2]-d2[2])
                                              };
                                //compute tangential part by subtracting off normal component
                                for(int j = 0; j < 3; j++) ds[j] -= ((ds[j]*D[j])/D_norm[j])*D[j];
                                //compute Friction Force
                                for(int j = 0; j < 3; j++) m_Ffric[j] += m_ks*ds[j];
                                // normalize
                                double FfricNorm = sqrt(GetLength(m_Ffric));
                                double forceNorm = sqrt(GetLength(force));
                                //tangential force greater than static friction -->dynamic
                               // if( FfricNorm>forceNorm*(m_mu))                                //{
                                for(int j = 0; j < 3; j++) m_Ffric[j]= m_Ffric[j] * ((m_mu*forceNorm)/FfricNorm);
                                for(int j = 0; j < 3; j++) f_friction[j] = m_Ffric[j];
                            }
                        }
                        //--------------------------------------------------------------------------------------------------------
                        //   CALCULATE DEM FORCES BEWTEEN WATER-AIR  IGNORE TYPE 1 ABOVE THIS IS DEFAULT FOR CONVERTER
                        //---------------------------------------------------------------------------------------------------------
                        else //Dem type other
                        {
                            Particle *neighbour_pt = &(grid[id_x+x][id_y+y][id_z+z].water_particles[i]);
                            double r[3]            = { pt->position[0] - neighbour_pt->position[0],
                                                       pt->position[1] - neighbour_pt->position[1],
                                                       pt->position[2] - neighbour_pt->position[2]
                                                     };
                            double r_square        = DotProduct(r, r);
                            double distance        = GetDistance(pt->position, neighbour_pt->position);
                            double unit_vector [3] = {r[0]/distance ,r[1]/distance ,r[2]/distance };

                            if(r_square  <= H*H)
                            {
                                double hertzian_stiff= g_Param.getHertzian();
                                double damping = g_Param.getDamping();

                                //Hertzian Spring Dashpot Model
                                double f_contact_magnitude = ( hertzian_stiff * pow((distance - H/2 ), 3/2)); //50 000
                                double f_damping_magnitude = ( damping * (GetLength(pt->velocity)- GetLength(neighbour_pt->velocity))*pow((distance-H),1/4));

                                f_contact[0] +=  unit_vector[0]*f_contact_magnitude;
                                f_contact[1] +=  unit_vector[1]*f_contact_magnitude;
                                f_contact[2] +=  unit_vector[2]*f_contact_magnitude;

                                f_damping[0] +=  unit_vector[0]*f_damping_magnitude;
                                f_damping[1] +=  unit_vector[1]*f_damping_magnitude;
                                f_damping[2] +=  unit_vector[2]*f_damping_magnitude;

                                //Calculate Frictional Forces
                                //particle movement since last step
                                double deltime = g_Param.getDelta_t();
                                double shear_stiff = 0.3*hertzian_stiff; /*0.4*/
                                double d1[3]= { pt->velocity[0]*deltime,pt->velocity[1]*deltime,pt->velocity[2]*deltime };
                                double d2[3]= { neighbour_pt->velocity[0]*deltime,neighbour_pt->velocity[1]*deltime,neighbour_pt->velocity[2]*deltime };
                                double dFF[3]= {shear_stiff*(d1[0]-d2[0]),shear_stiff*(d1[1]-d2[1]),shear_stiff*(d1[2]-d2[2])};

                                dFF[0] -= ((dFF[0]*distance)/unit_vector[0])*distance;
                                dFF[1] -= ((dFF[1]*distance)/unit_vector[1])*distance;
                                dFF[2] -= ((dFF[2]*distance)/unit_vector[2])*distance;

                                double mu_fric = g_Param.getTangentialFriction();//0.5;
                                double Fric_mag = sqrt( dFF[0]*dFF[0] + dFF[1]*dFF[1] + dFF[2]*dFF[2]);

                                //  if (Fric_mag>f_contact_magnitude* mu_fric ){
                                f_friction[0] += dFF[0] * (f_contact_magnitude* mu_fric) /(Fric_mag);
                                f_friction[1] += dFF[1] * (f_contact_magnitude* mu_fric) /(Fric_mag);
                                f_friction[2] += dFF[2] * (f_contact_magnitude* mu_fric) /(Fric_mag);

                            }
                        } // if(r_square <= H*H){
                    }
                }
                //----------------------------------------------------------------------------------

                for(unsigned int i = 0; i < grid[id_x+x][id_y+y][id_z+z].air_particles.size(); i++)
                {

                    if(is_air_particle==1||is_boundary_particle==1)
                    {
                    //--------------------------------------------------------------------------------------------------------
                    //                  CALCULATE SPH FORCES BETWEEN AIR-AIR OR AIR-BOUNDARY
                    //---------------------------------------------------------------------------------------------------------
                        Particle *neighbour_pt = &(grid[id_x+x][id_y+y][id_z+z].air_particles[i]);
                        double r[3]            = {  pt->position[0] - neighbour_pt->position[0],
                                                    pt->position[1] - neighbour_pt->position[1],
                                                    pt->position[2] - neighbour_pt->position[2]
                                                 };
                        double r_square        = DotProduct(r, r);
                        if(r_square <= H*H)
                        {
                            if(r_square > 0.0)
                            {
                                double gradient[3];
                                Wpoly6Grad(r, r_square, gradient);
                                double weight_p = (pt->pressure + neighbour_pt->pressure)*0.5 * neighbour_pt->inv_density;
                                for(int j = 0; j < 3; j++) f_pressure_a[j]         += weight_p * gradient[j];
                                for(int j = 0; j < 3; j++) color_field_normal_a[j] += neighbour_pt->inv_density * gradient[j];
                            }
                            double weight_v = neighbour_pt->inv_density * WviscosityLaplacian(r_square);
                            for(int j = 0; j < 3; j++) f_viscosity_a[j] += (neighbour_pt->velocity[j]-pt->velocity[j]) * weight_v;

                            color_field_laplacian_a += neighbour_pt->inv_density * Wpoly6Laplacian(r_square);
                        }
                    }
                    if (is_water_particle==1 )
                    {
                    //--------------------------------------------------------------------------------------------------------
                    //   CALCULATE DEM FORCES.  SCROLL DOWN TO ELSE. DEM TYPE IS USUALLY =2 FOR CONVERTER
                    //---------------------------------------------------------------------------------------------------------
                        if(g_Param.getDemType()==1)
                        {
                            double m_k = g_Param.getHertzian();  //spring constant
                            double m_dt = g_Param.getDelta_t() ; // time step
                            double m_ks= 0.4*m_k;                 //shear stiffness (cundell)
                            double m_mu=0.5;                         // coeffient of friction
                            double particle_radius_1 = (1/2)*H;
                            double particle_radius_2 = (1/2)*H;
                            double m_Ffric [3];                      //friction force
                            Particle *neighbour_pt = &(grid[id_x+x][id_y+y][id_z+z].air_particles[i]);
                            //calculate distance
                            double D[3]     = { pt->position[0] - neighbour_pt->position[0],
                                                pt->position[1] - neighbour_pt->position[1],
                                                pt->position[2] - neighbour_pt->position[2]
                                              };
                            double eq_dist = particle_radius_1+particle_radius_2;
                            double D_square = DotProduct(D, D);
                            //check if there is contact
                            if(D_square  <= eq_dist*eq_dist)                            {
                                //calculate contact forces
                                //elastic force
                                double dist = GetDistance(pt->position, neighbour_pt->position);
                                double D_norm [3]= { D[0]/dist,D[1]/dist,D[2]/dist };
                                double force [3] = { D[0]*(m_k)*(dist-eq_dist)/dist,
                                                     D[1]*(m_k)*(dist-eq_dist)/dist,
                                                     D[2]*(m_k)*(dist-eq_dist)/dist,
                                                   };
                                //---frictional force---
                                //particle movement since last step
                                double d1[3]= { pt->velocity[0]*m_dt,
                                                pt->velocity[1]*m_dt,
                                                pt->velocity[2]*m_dt
                                              };
                                double d2[3]= { neighbour_pt->velocity[0]*m_dt,
                                                neighbour_pt->velocity[1]*m_dt,
                                                neighbour_pt->velocity[2]*m_dt
                                              };
                                double ds[3]= {(d1[0]-d2[0]),
                                               (d1[1]-d2[1]),
                                               (d1[2]-d2[2])
                                              };
                                //compute tangential part by subtracting off normal component
                                for(int j = 0; j < 3; j++) ds[j] -= ((ds[j]*D[j])/D_norm[j])*D[j];
                                //compute Friction Force
                                for(int j = 0; j < 3; j++) m_Ffric[j] += m_ks*ds[j];
                                // normalize
                                double FfricNorm = sqrt(GetLength(m_Ffric));
                                double forceNorm = sqrt(GetLength(force));
                                for(int j = 0; j < 3; j++) m_Ffric[j]= m_Ffric[j] * ((m_mu*forceNorm)/FfricNorm);
                                for(int j = 0; j < 3; j++) f_friction[j] = m_Ffric[j];
                            }
                        }
                        else //Dem type other
                        //--------------------------------------------------------------------------------------------------------
                        //   CALCULATE DEM FORCES BETWEEN AIR-WATER.  IGNORE TYPE 1, THIS IS DEFAULT FOR CONVERTER
                        //---------------------------------------------------------------------------------------------------------
                        {
                            Particle *neighbour_pt = &(grid[id_x+x][id_y+y][id_z+z].air_particles[i]);
                            double r[3]            = { pt->position[0] - neighbour_pt->position[0],
                                                       pt->position[1] - neighbour_pt->position[1],
                                                       pt->position[2] - neighbour_pt->position[2]
                                                     };
                            double r_square        = DotProduct(r, r);
                            double distance        = GetDistance(pt->position, neighbour_pt->position);
                            double unit_vector [3] = {r[0]/distance ,r[1]/distance ,r[2]/distance };
                            if(r_square  <= H*H)                            {
                                double hertzian_stiff= g_Param.getHertzian();
                                double damping = g_Param.getDamping();
                                double f_contact_magnitude = ( hertzian_stiff * pow((distance - H/2 ), 3/2)); //50 000
                                double f_damping_magnitude = ( damping * (GetLength(pt->velocity)- GetLength(neighbour_pt->velocity))*pow((distance-H),1/4));
                                //-----------------------------------------------------
                                //Calculate Normal(contact) and Damping Forces
                                //-----------------------------------------------------
                                f_contact[0] +=  unit_vector[0]*f_contact_magnitude;
                                f_contact[1] +=  unit_vector[1]*f_contact_magnitude;
                                f_contact[2] +=  unit_vector[2]*f_contact_magnitude;
                                f_damping[0] +=  unit_vector[0]*f_damping_magnitude;
                                f_damping[1] +=  unit_vector[1]*f_damping_magnitude;
                                f_damping[2] +=  unit_vector[2]*f_damping_magnitude;
                                //---------------------------------
                                //Calculate Frictional Forces
                                //---------------------------------
                                double deltime = g_Param.getDelta_t();
                                double shear_stiff = 0.3*hertzian_stiff;//0.4

                                double d1[3]= { pt->velocity[0]*deltime,pt->velocity[1]*deltime,pt->velocity[2]*deltime };
                                double d2[3]= { neighbour_pt->velocity[0]*deltime,neighbour_pt->velocity[1]*deltime,neighbour_pt->velocity[2]*deltime };
                                double dFF[3]= {shear_stiff*(d1[0]-d2[0]),shear_stiff*(d1[1]-d2[1]),shear_stiff*(d1[2]-d2[2])};

                                //compute tangental part by subtracting normal component
                                dFF[0] -= ((dFF[0]*distance)/unit_vector[0])*distance;
                                dFF[1] -= ((dFF[1]*distance)/unit_vector[1])*distance;
                                dFF[2] -= ((dFF[2]*distance)/unit_vector[2])*distance;

                                double mu_fric = 0.5;
                                double Fric_mag = sqrt( dFF[0]*dFF[0] + dFF[1]*dFF[1] + dFF[2]*dFF[2]);
                                f_friction[0] += dFF[0] * (f_contact_magnitude* mu_fric) /(Fric_mag);
                                f_friction[1] += dFF[1] * (f_contact_magnitude* mu_fric) /(Fric_mag);
                                f_friction[2] += dFF[2] * (f_contact_magnitude* mu_fric) /(Fric_mag);
                            }
                        } // if(r_square <= H*H){
                    }
                }
                //-------------------------------------------------------------------------------------

                for(unsigned int i = 0; i < grid[id_x+x][id_y+y][id_z+z].boundary_particles.size(); i++)
                {
                //--------------------------------------------------------------------------------------------------------
                //                  CALCULATE SPH FORCES BEWTEEN BOUNDARY-BOUNDARY OR BOUNDARY-AIR OR BOUNDARY-WATER
                //---------------------------------------------------------------------------------------------------------

                    Particle *neightbor_pt = &(grid[id_x+x][id_y+y][id_z+z].boundary_particles[i]);
                    double r[3] = { pt->position[0] - neightbor_pt->position[0],
                                    pt->position[1] - neightbor_pt->position[1],
                                    pt->position[2] - neightbor_pt->position[2]
                                  };
                    double r_square = DotProduct(r, r);

                    if(r_square <= H*H)
                    {
                        if(r_square > 0.0)
                        {
                            double gradient[3];
                            Wpoly6Grad(r, r_square, gradient);

                            double weight_p = (pt->pressure + neightbor_pt->pressure)*0.5 * neightbor_pt->inv_density;
                            for(int j = 0; j < 3; j++) f_pressure_b[j] += weight_p * gradient[j];
                            for(int j = 0; j < 3; j++) color_field_normal_b[j] += neightbor_pt->inv_density * gradient[j];
                        }
                        double weight_v = neightbor_pt->inv_density * WviscosityLaplacian(r_square);
                        for(int j = 0; j < 3; j++) f_viscosity_b[j] += (neightbor_pt->velocity[j]-pt->velocity[j]) * weight_v;

                        color_field_laplacian_b += neightbor_pt->inv_density * Wpoly6Laplacian(r_square);

                    } // if(r_square <= H*H){
                }
            } // for(int z = -1; z <= 1; z++
        } // for(int y = -1; y <= 1; y++){
    } // for(int x = -1; x <= 1; x++){

    double f_pressure[3], f_viscosity[3] ;
    double f_surface[3] = { 0.0, 0.0, 0.0 };

    // SET VISCOCITIES AND MASSES
    double W_mass=g_Param.getWater_mass();
    double A_mass=g_Param.getAir_mass();
    double B_mass=g_Param.getBoundary_mass();
    double Mu_w=g_Param.getMu_w();
    double Mu_a=g_Param.getMu_a();
    double Mu_b=g_Param.getMu_b();

    // COMBINE THE PRESSURE FORCES (FOR W,A,B)
    for(int j = 0; j < 3; j++) {
    f_pressure[j]= - W_mass*f_pressure_w[j]  - A_mass*f_pressure_a[j] - B_mass*f_pressure_b[j];
    }
    // COMBINE THE VISCOCITY FORCES (FOR W,A,B)
    for(int j = 0; j < 3; j++){
     f_viscosity[j]= (viscosity+Mu_w)*0.5*W_mass*f_viscosity_w[j] + (viscosity+Mu_a)*0.5*A_mass*f_viscosity_a[j]+ (viscosity+Mu_b)*0.5*B_mass*f_viscosity_b[j];
     }

    // COMBINE THE COLORFIELD (FOR W,A,B) -> RELATED TO SURFACE TENSION
    double MassXcolor_field_normal_w[3]= {0,0,0};
    for(int j = 0; j < 3; j++)
    {
        MassXcolor_field_normal_w[j] = (color_field_normal_w[j])*(g_Param.getWater_mass());
    }
    double normal_norm_w = sqrt( DotProduct(MassXcolor_field_normal_w,MassXcolor_field_normal_w)) ; // magnitude of colorfield

    if (is_boundary_particle)
    {
        //the boundary particle will not move for the converter situation
        pt->acceleration[0] = 0;
        pt->acceleration[1] = 0;
        pt->acceleration[2] = 0;

        //NOT USED FOR CONVERTER,
        if (g_Param.getSpinOn()==1)
        {
            double yaxis[3]= {0,1,0};
            double radius[3]= {0,0,0};
            if ( pt->position[0] > 0)
            {
                radius[0] = pt->position[0] - 0.5*XGRID*g_Param.getSmoothingRadius();
            }
            else
            {
                radius[0] = - (abs(pt->position[0]) - 0.5*XGRID*g_Param.getSmoothingRadius());
            }
            if ( pt->position[1] > 0)
            {
                radius[1] = pt->position[1] - 0.5*XGRID*g_Param.getSmoothingRadius();
            }
            else
            {
                radius[1] = - (abs(pt->position[1]) - 0.5*XGRID*g_Param.getSmoothingRadius());
            }
            if ( pt->position[2] > 0)
            {
                radius[2] = pt->position[2] - 0.5*XGRID*g_Param.getSmoothingRadius();
            }
            else
            {
                radius[2] = - (abs(pt->position[2]) - 0.5*XGRID*g_Param.getSmoothingRadius());
            }
            double tangent[3]= {radius[1] * yaxis [2] - radius [2] * yaxis[1],
                                radius[2] * yaxis [0] - radius [0] * yaxis[2],
                                radius[0] * yaxis [1] - radius [1] * yaxis[0],
                               };
            Normalize(tangent);
            double speed_factor = g_Param.getSpinSpeed();
            double spin_speed = speed_factor * sqrt( pow(radius[0],2)+pow(radius[2],2));
            pt->velocity [0] = spin_speed* tangent[0];
            pt->velocity [1] = 0;
            pt->velocity [2] = spin_speed* tangent[2];
        }
    }
    if (is_water_particle )
    {
    //--------------------------------------------
    //     CALULATE SURFACE TENSION FORCE
    //--------------------------------------------
        double f_surface_w[3] = { 0.0, 0.0, 0.0 };
        if (normal_norm_w > 10 )
        {
            grid[id_x][id_y][id_z].water_particles[particle_id].is_surface = 1;
            Normalize(MassXcolor_field_normal_w);
            for(int s = 0; s < 3; s++)
            {
                f_surface_w[s]= (-g_Param.getSigma_s())*((color_field_laplacian_w)*(g_Param.getWater_mass()))*(MassXcolor_field_normal_w[s]);//removed minus
            }
        }
        //------------------------------------------------------------------
        //     SET GRAVITY AND BUOYANCY DEPENDING IF ITS A CONVERTER OF MILL
        //-------------------------------------------------------------------
        double f_gravity_y = 0;
        double f_gravity_z = 0;
        if(g_Param.getGravityIsOn()&&g_Param.getConverterOn()) f_gravity_y= pt->gravity_force;
        if(g_Param.getGravityIsOn()&&g_Param.getMillOn()) f_gravity_z= pt->gravity_force;
        double buoyant_force_y =0;
        double buoyant_force_z =0;
        if(g_Param.getConverterOn()) buoyant_force_y= pt->buoyant_force;
        if(g_Param.getMillOn()) buoyant_force_z= pt->buoyant_force;
        //-------------------------------------------------------------------
        //     FIND ACCELRATION WITH SUM OF FORCES
        //     F=MA---> A= F/M M=DENSITY*VOLUME
        //--------------------------------------------------------------------
        pt->acceleration[0] = (f_pressure[0]+f_viscosity[0]+f_surface_w[0]+f_contact[0]-f_damping[0]+f_friction[0])* pt->inv_density;
        pt->acceleration[1] = (f_pressure[1]+f_viscosity[1]+f_surface_w[1]+f_contact[1]-f_damping[1]+f_friction[1]+f_gravity_y+buoyant_force_y) * pt->inv_density;
        pt->acceleration[2] = (f_pressure[2]+f_viscosity[2]+f_surface_w[2]+f_contact[2]-f_damping[2]+f_friction[2]+f_gravity_z+buoyant_force_z)* pt->inv_density;
    }
    if (is_air_particle)
    {
        //------------------------------------------------------------------
        //     SET GRAVITY AND BUOYANCY DEPENDING IF ITS A CONVERTER OF MILL
        //-------------------------------------------------------------------
        double f_gravity_y = 0;
        double f_gravity_z = 0;
        if(g_Param.getGravityIsOn()&&g_Param.getConverterOn()) f_gravity_y= pt->gravity_force;
        if(g_Param.getGravityIsOn()&&g_Param.getMillOn()) f_gravity_z= pt->gravity_force;
        double buoyant_force_y =0;
        double buoyant_force_z =0;
        if(g_Param.getConverterOn()) buoyant_force_y= pt->buoyant_force;
        if(g_Param.getMillOn()) buoyant_force_z= pt->buoyant_force;
        //-------------------------------------------------------------------
        //     FIND ACCELRATION WITH SUM OF FORCES
        //     F=MA---> A= F/M M=DENSITY*VOLUME
        //--------------------------------------------------------------------
        pt->acceleration[0] = (f_pressure[0]+f_viscosity[0]+f_surface[0]+f_contact[0]-f_damping[0]+f_friction[0]) * pt->inv_density;
        pt->acceleration[1] = (f_pressure[1]+f_viscosity[1]+f_surface[1]+f_contact[1]-f_damping[1]+f_friction[1]+f_gravity_y+buoyant_force_y) * pt->inv_density;
        pt->acceleration[2] = (f_pressure[2]+f_viscosity[2]+f_surface[2]+f_contact[2]-f_damping[2]+f_friction[2]+f_gravity_z+buoyant_force_z) * pt->inv_density;
    }
    //---------------------------------------------------------------------------
    //     UPDATE POSITIONS AND VELOCITIES OF ALL PARTICLES BY ACCELERATING THEM
    //---------------------------------------------------------------------------
    for(int j = 0; j < 3; j++) pt->position[j] += (pt->velocity[j] + pt->acceleration[j]*0.75*g_Param.getDelta_t()) * g_Param.getDelta_t();
    for(int j = 0; j < 3; j++) pt->velocity[j] += pt->acceleration[j] * g_Param.getDelta_t();

    //NOT USED FOR CONVERTER
    if(is_boundary_particle==1&&g_Param.getSpinOn()==1) //expansion correction
    {
        if(pt->position[0] > (0.5*XGRID*H) &&  pt->position[2]  >   (0.5*ZGRID*H) )            // 4th QUADRANT
        {
            //find the angle at which the expand particle is at and scale it back to orignial radius
            double theta    = atan( abs(pt->position[2]-(0.5*XGRID*H))/abs(pt->position[0]-(0.5*XGRID*H)));
            pt->position[0] = (0.5*XGRID*H) + (cos(theta)* pt->radius_orig) ;
            pt->position[2] = (0.5*ZGRID*H) + (sin(theta)* pt->radius_orig) ;
        }
        if(pt->position[0] < (0.5*XGRID*H) &&  pt->position[2]  >   (0.5*ZGRID*H) )           // 3rd QUADRANT
        {
            double theta    = atan( abs((pt->position[2])-0.5*XGRID*H)/abs((0.5*XGRID*H)-pt->position[0]));
            pt->position[0] = (0.5*XGRID*H) - (cos(theta)* pt->radius_orig) ;
            pt->position[2] = (0.5*ZGRID*H) + (sin(theta)* pt->radius_orig) ;
        }
        if(pt->position[0] < (0.5*XGRID*H) &&  pt->position[2]  <   (0.5*ZGRID*H) )           // 2nd QUADRANT
        {
            double theta    = atan( abs((0.5*XGRID*H)-pt->position[2])/abs((0.5*XGRID*H)-pt->position[0])) ;
            pt->position[0] = (0.5*XGRID*H) - (cos(theta)* pt->radius_orig) ;
            pt->position[2] = (0.5*ZGRID*H) - (sin(theta)* pt->radius_orig) ;
        }
        if(pt->position[0] > (0.5*XGRID*H) &&  pt->position[2]  <   (0.5*ZGRID*H) )            // 1st QUADRANT
        {
            double theta    = atan( abs((0.5*XGRID*H)-pt->position[2])/abs(pt->position[0]-(0.5*XGRID*H)));
            pt->position[0] = (0.5*XGRID*H) + (cos(theta)* pt->radius_orig) ;
            pt->position[2] = (0.5*ZGRID*H) - (sin(theta)* pt->radius_orig) ;
        }
    }

    //-----------------------------------------------------------------------
    //     MARK PARTICLES THAT HAVE LEFT CONVERTER DOMAIN . WILL DELETE LATER
    //-----------------------------------------------------------------------
    if((      pt->position[0] < 0.0001 || pt->position[0]>0.99*XGRID*g_Param.getSmoothingRadius()) //roughly the extent of the cylinder boundary
            ||(pt->position[1] < 0.0001 || pt->position[1]>0.99*YGRID*g_Param.getSmoothingRadius())
            ||(pt->position[2] < 0.0001 || pt->position[2]>0.99*ZGRID*g_Param.getSmoothingRadius()))
    {
        if(is_water_particle) grid[id_x][id_y][id_z].water_particles[particle_id].is_outside = true;
        if(is_air_particle)   grid[id_x][id_y][id_z].air_particles[particle_id].is_outside   = true;
        if(is_boundary_particle) grid[id_x][id_y][id_z].boundary_particles[particle_id].is_outside   = true;
    }
}
void SPH::updateGrid(int id_x, int id_y, int id_z)
{
    double H = g_Param.getSmoothingRadius();
    static double inv_x_step = 1.0/H;
    static double inv_y_step = 1.0/H;
    static double inv_z_step = 1.0/H;


    for(int i = 0; i < (int)grid[id_x][id_y][id_z].boundary_particles.size(); i++)
        {
        if(g_Param.getParticleDeletion() && grid[id_x][id_y][id_z].boundary_particles[i].is_outside)
           {
            //-----------------------------------------------------------------------
            //     DELETE BOUNDARY PARTICLES THAT LEAVE THE DOMAIN
            //-----------------------------------------------------------------------
            grid[id_x][id_y][id_z].boundary_particles[i] = grid[id_x][id_y][id_z].boundary_particles.back();
            grid[id_x][id_y][id_z].boundary_particles.pop_back();
            i--;
            continue;
        }
    }

    for(int i = 0; i < (int)grid[id_x][id_y][id_z].water_particles.size(); i++)    {

        if(g_Param.getParticleDeletion() && grid[id_x][id_y][id_z].water_particles[i].is_outside)
            {
            //-----------------------------------------------------------------------
            //     DELETE WATER PARTICLES THAT LEAVE THE DOMAIN
            //-----------------------------------------------------------------------
            grid[id_x][id_y][id_z].water_particles[i] = grid[id_x][id_y][id_z].water_particles.back();
            grid[id_x][id_y][id_z].water_particles.pop_back();
            i--;
            continue;
        }
        double *pos = grid[id_x][id_y][id_z].water_particles[i].position;


        //---------------------------------------------------------------------------
        //     FIND THE NEW GRID INDEXES. AKA THE GRID WHICH THE PARTICLES LIE IN.
        //     IN UPDATEPARTICLE THE POSITION CHANGES SO THE GRID INDEX WILL ALSO
        //     CHANGE
        //---------------------------------------------------------------------------
        int current_id_x = (int)floor( (pos[0])     *inv_x_step );//current_id_x is the id inside of a grid block
        int current_id_y = (int)floor( (pos[1])     *inv_y_step );
        int current_id_z = (int)floor( (pos[2])     *inv_z_step );

        //THE GRID INDEX WILL BE CHANGED TO ZERO IF LESS THAN ZERO
        //IF THE GRID INDEX IS GREATER THAN THE MAX NUMBER OR INDEXES, IT WILL BE CHANGED TO THE MAX NUMBER
        if      (current_id_x < 0)           current_id_x = 0;
        else if (current_id_x >= XGRID)      current_id_x = XGRID-1;
        if      (current_id_y < 0)           current_id_y = 0;
        else if (current_id_y >= YGRID)      current_id_y = YGRID-1;
        if      (current_id_z < 0)           current_id_z = 0;
        else if (current_id_z >= ZGRID)      current_id_z = ZGRID-1;

        if(id_x != current_id_x || id_y != current_id_y || id_z != current_id_z)        {
            // move particle to new grid
            grid[current_id_x][current_id_y][current_id_z].water_particles.push_back( grid[id_x][id_y][id_z].water_particles[i] );
            // delete the particle from this grid
            grid[id_x][id_y][id_z].water_particles[i] = grid[id_x][id_y][id_z].water_particles.back();
            grid[id_x][id_y][id_z].water_particles.pop_back();
            i--;

        }
    }

    for(int i = 0; i < (int)grid[id_x][id_y][id_z].air_particles.size(); i++)
    {
        if(grid[id_x][id_y][id_z].air_particles[i].is_outside)
        {
            //-----------------------------------------------------------------------
            //     DELETE AIR PARTICLES THAT LEAVE THE DOMAIN
            //-----------------------------------------------------------------------
            grid[id_x][id_y][id_z].air_particles[i] = grid[id_x][id_y][id_z].air_particles.back();
            grid[id_x][id_y][id_z].air_particles.pop_back();
            i--;
            continue;
        }

        double *pos = grid[id_x][id_y][id_z].air_particles[i].position;
        //-----------------------------------------------------------------------
        //     FIND NEW GRID INDEXES
        //-----------------------------------------------------------------------
        int current_id_x = (int)floor( (pos[0])     *inv_x_step );
        int current_id_y = (int)floor( (pos[1])     *inv_y_step );
        int current_id_z = (int)floor( (pos[2])     *inv_z_step );

        if(current_id_x < 0)           current_id_x = 0;
        else if(current_id_x >= XGRID) current_id_x = XGRID-1;
        if(current_id_y < 0)           current_id_y = 0;
        else if(current_id_y >= YGRID) current_id_y = YGRID-1;
        if(current_id_z < 0)           current_id_z = 0;
        else if(current_id_z >= ZGRID) current_id_z = ZGRID-1;

        if(id_x != current_id_x || id_y != current_id_y || id_z != current_id_z)
        {
            // move particle to new grid
            grid[current_id_x][current_id_y][current_id_z].air_particles.push_back( grid[id_x][id_y][id_z].air_particles[i] );
            // delete the particle from this grid
            grid[id_x][id_y][id_z].air_particles[i] = grid[id_x][id_y][id_z].air_particles.back();
            grid[id_x][id_y][id_z].air_particles.pop_back();
            i--;
        }
    }
}














void SPH::spawnParticles(int step)
{



    double H = g_Param.getSmoothingRadius();
    if(step==0)
    //-----------------------------------------------------------------------
    //     MAIN MENU SETUP, TO VIEW LANCE SETTINGS GO FURTHER DOWN TO "ELSE"
     //-----------------------------------------------------------------------
    {
        bool return_home = 0;
        do
        {
            char option = 's';
            bool bool_num=0;
            double double_num=0.0;
            int int_num=0;

            cout<<"Select An Option. Click Z to return home: ";
            cin>>option;

            switch (option)
            {
            case 'z':
            case 'Z':
            {
                return_home=1;
                break;
            }
            case 'a':
            case 'A':
            {
                cout<< "Lance On? 0=NO  1=YES"<<endl;
                for(;;)
                {
                    if(cin>>bool_num)
                    {
                        g_Param.setLanceOn(bool_num);
                        viewLanceProperties();
                        break;
                    }
                    else
                    {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(),'\n');
                        viewLanceProperties();
                        cout<< "Lance On? 0=NO  1=YES"<<endl;
                        cout<<" Please Type In a Valid Input: ";
                    }
                }
                break;
            }
            case 'b':
            case 'B':
            {
                getchar();
                cout<< "Enter New Lance Postions : "<<endl;
                cout<< "Enter X postion: ";
                for(;;)
                {
                    if(cin>>double_num)
                    {
                        g_Param.setLancePosX(double_num);
                        viewLanceProperties();
                        break;
                    }
                    else
                    {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(),'\n');
                        viewLanceProperties();
                        cout<< "Enter X postion: ";
                        cout<<" Please Type In a Valid Input: ";
                    }
                }
                cout<< "Enter Y postion: ";
                for(;;)
                {
                    if(cin>>double_num)
                    {
                        g_Param.setLancePosY(double_num);
                        viewLanceProperties();
                        break;
                    }
                    else
                    {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(),'\n');
                        viewLanceProperties();
                        cout<< "Enter Y postion: ";
                        cout<<" Please Type In a Valid Input: ";
                    }
                }
                cout<< "Enter Z postion: ";
                for(;;)
                {
                    if(cin>>double_num)
                    {
                        g_Param.setLancePosZ(double_num);
                        viewLanceProperties();
                        break;
                    }
                    else
                    {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(),'\n');
                        viewLanceProperties();
                        cout<< "Enter Z postion: ";
                        cout<<" Please Type In a Valid Input: ";
                    }
                }
                break;
            }
            case 'c':
            case 'C':
            {
                cout<< "Set Lance Flow Current: Making Air Every "<<g_Param.getRateOfAir()<<" Steps. Enter New Rate: ";
                for(;;)
                {
                    if(cin>>int_num)
                    {
                        g_Param.setRateOfAir(int_num);
                        viewLanceProperties();
                        break;
                    }
                    else
                    {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(),'\n');
                        viewLanceProperties();
                        cout<< "Set Rate of Air:"<<endl;
                        cout<<" Please Type In a Valid Input: ";
                    }
                }
                break;
            }
            case 'd':
            case 'D':
            {
                cout<< "Set Lance Opening Radius "<<g_Param.getLanceOpeningRadius()<<"m Enter New Radius: ";
                for(;;)
                {
                    if(cin>>double_num)
                    {
                        g_Param.setLanceOpeningRadius(double_num);
                        viewLanceProperties();
                        break;
                    }
                    else
                    {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(),'\n');
                        viewLanceProperties();
                        cout<< "Set Lance Opening Radius: "<<endl;
                        cout<<" Please Type In a Valid Input: ";
                    }
                }
                break;
            }
            case 'n':
            case 'N':
            {
                cout<< "Set Water Seed : ";
                for(;;)
                {
                    if(cin>>int_num)
                    {
                        g_Param.setWater_per_step(int_num);
                        viewLanceProperties();
                        break;
                    }
                    else
                    {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(),'\n');
                        viewLanceProperties();
                        cout<< "Set Water Seed  : "<<endl;;
                        cout<<" Please Type In a Valid Input: ";
                    }
                }
                break;
            }
            case 'o':
            case 'O':
            {
                cout<< "Set Air Seed : ";
                for(;;)
                {
                    if(cin>>int_num)
                    {
                        g_Param.setAir_per_step(int_num);
                        viewLanceProperties();
                        break;
                    }
                    else
                    {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(),'\n');
                        viewLanceProperties();
                        cout<< "Set Air Seed  : "<<endl;;
                        cout<<" Please Type In a Valid Input: ";
                    }
                }
                break;
            }
            case 'p':
            case 'P':
            {
                cout<< "Set Lance Type [ 1 or 2] : ";
                for(;;)
                {
                    if(cin>>int_num)
                    {
                        g_Param.setLanceType(int_num);
                        viewLanceProperties();
                        break;
                    }
                    else
                    {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(),'\n');
                        viewLanceProperties();
                        cout<< "Set Lance Type [ 1 or 2] : "<<endl;;
                        cout<<" Please Type In a Valid Input: ";
                    }
                }
                break;
            }
            case 'q':
            case 'Q':
            {
                cout<< "Set Lance Air Rings Current: "<<g_Param.getNumberOfRings();
                for(;;)
                {
                    if(cin>>int_num)
                    {
                        g_Param.setNumberOfRings(int_num);
                        viewLanceProperties();
                        break;
                    }
                    else
                    {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(),'\n');
                        viewLanceProperties();
                        cout<< "Set Lance Air Rings Current: "<<g_Param.getNumberOfRings();
                        cout<<" Please Type In a Valid Input: ";
                    }
                }
                break;
            }
            case 'r':
            case 'R':
            {
                cout<< "Set Lance Air Seeds. Current: "<<g_Param.getNumberofSeeds();
                for(;;)
                {
                    if(cin>>int_num)
                    {
                        g_Param.setNumberOfSeeds(int_num);
                        viewLanceProperties();
                        break;
                    }
                    else
                    {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(),'\n');
                        viewLanceProperties();
                        cout<< "Set Lance Air Seeds. Current: "<<g_Param.getNumberofSeeds();
                        cout<<" Please Type In a Valid Input: ";
                    }
                }
                break;
            }

            case 'e':
            case 'E':
            {
                getchar();
                cout<< "Enter New Lance Output Velocity Ranges : "<<endl;
                cout<< "Enter Min X Velocity: ";
                for(;;)
                {
                    if(cin>>double_num)
                    {
                        g_Param.setLanceOutputVelocityXMin(double_num);
                        viewLanceProperties();
                        break;
                    }
                    else
                    {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(),'\n');
                        viewLanceProperties();
                        cout<< "Enter X Max Velocity: ";
                        cout<<" Please Type In a Valid Input: ";
                    }
                }
                cout<< "Enter New Lance Output Velocity Ranges : "<<endl;
                cout<< "Enter Max X Velocity: ";
                for(;;)
                {
                    if(cin>>double_num)
                    {
                        g_Param.setLanceOutputVelocityXMax(double_num);
                        viewLanceProperties();
                        break;
                    }
                    else
                    {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(),'\n');
                        viewLanceProperties();
                        cout<< "Enter X Max Velocity: ";
                        cout<<" Please Type In a Valid Input: ";
                    }
                }
                cout<< "Enter New Lance Output Velocity Ranges : "<<endl;
                cout<< "Enter Min Y Velocity: ";
                for(;;)
                {
                    if(cin>>double_num)
                    {
                        g_Param.setLanceOutputVelocityYMin(double_num);
                        viewLanceProperties();
                        break;
                    }
                    else
                    {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(),'\n');
                        viewLanceProperties();
                        cout<< "Enter Y Min Velocity: ";
                        cout<<" Please Type In a Valid Input: ";
                    }
                }
                cout<< "Enter New Lance Output Velocity Ranges : "<<endl;
                cout<< "Enter Max Y Velocity: ";
                for(;;)
                {
                    if(cin>>double_num)
                    {
                        g_Param.setLanceOutputVelocityYMax(double_num);
                        viewLanceProperties();
                        break;
                    }
                    else
                    {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(),'\n');
                        viewLanceProperties();
                        cout<< "Enter Y Max Velocity: ";
                        cout<<" Please Type In a Valid Input: ";
                    }
                }
                cout<< "Enter New Lance Output Velocity Ranges : "<<endl;
                cout<< "Enter Min Z Velocity: ";
                for(;;)
                {
                    if(cin>>double_num)
                    {
                        g_Param.setLanceOutputVelocityZMin(double_num);
                        viewLanceProperties();
                        break;
                    }
                    else
                    {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(),'\n');
                        viewLanceProperties();
                        cout<< "Enter Z Min Velocity: ";
                        cout<<" Please Type In a Valid Input: ";
                    }
                }
                cout<< "Enter New Lance Output Velocity Ranges : "<<endl;
                cout<< "Enter Max Z Velocity: ";
                for(;;)
                {
                    if(cin>>double_num)
                    {
                        g_Param.setLanceOutputVelocityZMax(double_num);
                        viewLanceProperties();
                        break;
                    }
                    else
                    {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(),'\n');
                        viewLanceProperties();
                        cout<< "Enter Z Max Velocity: ";
                        cout<<" Please Type In a Valid Input: ";
                    }
                }
            }

            }
        }
        while(!return_home);
    }
    else
    {
     //-----------------------------------------------------------------------
     //  SCROLL DOWN TO LANCE TYPE 2 TO VIEW DEFAULT CONVERTER LANCE
     //  BELOW ARE NON DEFAULT AIR SEEDING OPTIONS
     //-----------------------------------------------------------------------
        //---------------  ------------------------------------------
        //  NON-DEFAULT |  CREATE NEW AIR PARTICLES FROM SINGLE POINT
        //---------------  -------------------------------------------
        double spawn_point[3] = {g_Param.getLancePosX()*XGRID*H, g_Param.getLancePosY()*YGRID*H , g_Param.getLancePosZ()*ZGRID*H}; // where the air rings are created

        for(int j = 0; j < g_Param.getAir_per_step(); j++)
        {
            double init_position[3] = {spawn_point[0], spawn_point[1] , spawn_point[2]};
            double init_velocity[3] = { get_random(-2, 2 ) //x direction
                                        ,   get_random(-7,-6.9)       //y direction
                                        ,   get_random(-2, 2)  //z direciton
                                      };
            int grid_index_x = ((int)floor(init_position[0]/g_Param.getSmoothingRadius()));
            int grid_index_y = ((int)floor(init_position[1]/g_Param.getSmoothingRadius()));
            int grid_index_z = ((int)floor(init_position[2]/g_Param.getSmoothingRadius()));                                            //H is the smoothing radius, aka the width of each cell

            grid[grid_index_x][grid_index_y][grid_index_z].air_particles.push_back( Particle(init_position, init_velocity) );       // 0.6/0.045=13.333 floor makes it 13 so the paraticle is located in this block
        }
        //----------------   -----------------------------------------
        //  NON-DEFAULT  |   CREATE NEW WATER PARTICLES FROM SINGLE POINT
        //----------------   -----------------------------------------
        for(int j = 0; j < g_Param.getWater_per_step(); j++)
        {
            double init_position[3] = {spawn_point[0]*abs(cos(g_Param.getTimesteps())), spawn_point[1], spawn_point[2]*abs(cos(g_Param.getTimesteps()))};
            double init_velocity[3] = { get_random(-1, 1 ) //x direction
                                        ,   get_random(-0.8, 0)       //y direction
                                        ,   get_random(-1, 1)  //z direciton
                                      };
            int grid_index_x = ((int)floor(init_position[0]/g_Param.getSmoothingRadius()));
            int grid_index_y = ((int)floor(init_position[1]/g_Param.getSmoothingRadius()));
            int grid_index_z = ((int)floor(init_position[2]/g_Param.getSmoothingRadius()));                                            //H is the smoothing radius, aka the width of each cell

            grid[grid_index_x][grid_index_y][grid_index_z].water_particles.push_back( Particle(init_position, init_velocity) );       // 0.6/0.045=13.333 floor makes it 13 so the paraticle is located in this block
        }
         //--------------    ---------------------------------------
        //  NON-DEFAULT |    CREATE NEW WATER PARTICLES IN CIRCLE
        //---------------    ---------------------------------------

        bool make_water=0;
        if (make_water)
        {
            double w_center_of_ring[3] = {0.5*XGRID*g_Param.getSmoothingRadius(),0.5*YGRID*g_Param.getSmoothingRadius() ,0.5*ZGRID*g_Param.getSmoothingRadius()}; // where the air rings are created
            double w_radius = 4*g_Param.getSmoothingRadius();//H
            double w_circumferance = TwoPI*w_radius;
            int w_n_points = (int)(floor(w_circumferance/(1.5*g_Param.getSmoothingRadius()))); //H/5
            double w_theta = 0;

            if(step == 1)
            {
                double w_init_position[3] = { w_center_of_ring[0], w_center_of_ring[1], w_center_of_ring[2]  };// the center where the air rings will be generated
                double w_init_velocity[3] = {0, 0, 0 };

                int grid_index_x = ((int)floor(w_center_of_ring[0]/g_Param.getSmoothingRadius()));
                int grid_index_y = ((int)floor(w_center_of_ring[1]/g_Param.getSmoothingRadius()));
                int grid_index_z = ((int)floor(w_center_of_ring[2]/g_Param.getSmoothingRadius()));

                grid[grid_index_x][grid_index_y][grid_index_z].water_particles.push_back( Particle(w_init_position, w_init_velocity) );
            }

            int number_w_rings=5;
            if(step==1||step%20==0)   //make new water particles every nth step
            {
                for(int n=0; n< w_n_points; n++)
                {
                    for(int ring = 0; ring < number_w_rings; ring++)
                    {
                        w_theta     = n*(TwoPI/w_n_points); // we want theta to go from 0 to 2PI within n_points_in_circle times. for each point of the circle that is created theta will adjust accordingly.
                        double x  = (w_radius*cos(w_theta))*(ring*(1/number_w_rings)) + (w_center_of_ring[0]); //the +(x) is the translation factor
                        double y  = w_center_of_ring[1];
                        double z  = (w_radius *sin( w_theta ))*(ring*(1/number_w_rings))+ (w_center_of_ring[2]); // 0.05=1/20 meaning there will be 20 rings each one with radius 0.05 large than the next

                        double w_init_position[3] = { x, y, z  };
                        double w_init_velocity[3] = { 0,get_random(-4,-3.9),0 };// find out the actual values needed for velocities //VEL SHOULD BE APROX-0.2

                        int grid_index_x = ((int)floor(x/g_Param.getSmoothingRadius()));
                        int grid_index_y = ((int)floor(y/g_Param.getSmoothingRadius()));
                        int grid_index_z = ((int)floor(z/g_Param.getSmoothingRadius()));

                        grid[grid_index_x][grid_index_y][grid_index_z].water_particles.push_back( Particle(w_init_position, w_init_velocity) );
                    }
                }

            }
        }
        //------------------------------------------------------------
        //                      LANCE TYPES 1,2,3
        //------------------------------------------------------------

        double H= g_Param.getSmoothingRadius();

        if (g_Param.getLanceOn())
        {
            double center_of_ring[3] = {g_Param.getLancePosX()*XGRID*H, g_Param.getLancePosY()*YGRID*H , g_Param.getLancePosZ()*ZGRID*H}; // where the air rings are created
            double radius = g_Param.getLanceOpeningRadius(); //1//H
            double circumferance = TwoPI*radius;
            int n_points = (int)(floor(circumferance/(1.0*H))); //H/5 6 points
            double theta = 0;

            int number_of_rings=g_Param.getNumberOfRings();
            double d_between_rings = H;//1/((double)number_of_rings);
            double Vx_min= g_Param.getLanceOutputVelocityXMin(), Vx_max = g_Param.getLanceOutputVelocityXMax();
            double Vy_min= g_Param.getLanceOutputVelocityYMin(), Vy_max = g_Param.getLanceOutputVelocityYMax();
            double Vz_min= g_Param.getLanceOutputVelocityZMin(), Vz_max = g_Param.getLanceOutputVelocityZMax();
            //-----------------------    --------------------------------------------
            //    NON - DEFAULT     |    CONVERTER LANCE TYPE 1
           //------------------------    -------------------------------------------
            if(g_Param.getLanceType()==1)
            {
                if(step==1||(step%g_Param.getRateOfAir()==0))
                {
                    for(int n=0; n< n_points; n++)
                    {
                        for(int rings = 0; rings < number_of_rings; rings++)
                        {
                            theta     = n*(TwoPI/n_points);
                            double x  = (radius*cos(theta))*(rings*d_between_rings) + (center_of_ring[0]);
                            double y  = center_of_ring[1];
                            double z  = (radius *sin( theta ))*(rings*d_between_rings)+ (center_of_ring[2]);

                            double init_position[3] = { x, y, z  };
                            double init_velocity[3] = { get_random(Vx_min,Vx_max), get_random(Vy_min,Vy_max), get_random(Vz_min,Vz_max) };

                            int grid_index_x = ((int)floor(x/H));
                            int grid_index_y = ((int)floor(y/H));
                            int grid_index_z = ((int)floor(z/H));

                            grid[grid_index_x][grid_index_y][grid_index_z].air_particles.push_back( Particle(init_position, init_velocity) );
                        }
                    }
                }
            }
            // !!!!!!!!!!!!!!!!!!!!!!       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            // !!!--------------!!!!!      ---------------------------------------------------
            // !!!   DEFAULT    !!!!!             CONVERTER LANCE TYPE 2
            // !!!--------------!!!!!      ---------------------------------------------------
           //  !!!!!!!!!!!!!!!!!!!!!!      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            if(g_Param.getLanceType()==2)
            {
                if(step==1||(step%g_Param.getRateOfAir()==0))
                {
                    int  n_nozzles = 6;

                    for(int n=0; n< n_nozzles; n++)
                    {
                        theta     = n*(TwoPI/n_nozzles);
                        double x  = (radius*cos(theta)) + (center_of_ring[0]);
                        double y  = center_of_ring[1];
                        double z  = (radius *sin( theta ))+ (center_of_ring[2]);

                        double nozzle_position[3] = { x, y, z  };
                        double throat_radius =0.007;//OF THE LANCE
                        //-----------------------------------------------------------------------
                        //      THE LANCE HAS j NUMBER OF NOZZLES
                        //      EACH NOZZLE HAS k NUMBER OF RINGS
                        //      EACH RINGS IS MADE UP OF l NUMBER OF SEEDS
                        //-----------------------------------------------------------------------
                        int number_of_rings = g_Param.getNumberOfRings(); // ONCE LANCE HAS X NUMBER OF BIZZ
                        int n_seeds = g_Param.getNumberofSeeds();
                        double theta2 = 0;


                        for (int rings=1; rings< number_of_rings; rings++)
                        {
                            for(int m=0; m< n_seeds; m++)
                            {
                                theta2     = (double)m*(TwoPI/(double)n_seeds);
                                //if(rings%2==0) theta2=+0.5*(TwoPI/(double)n_seeds);
                                double x2  = (throat_radius*cos(theta2))*(rings*(1/((double)number_of_rings))) + (nozzle_position[0]);
                                double y2  = nozzle_position[1];
                                double z2  = (throat_radius *sin( theta2 ))*(rings*(1/((double)number_of_rings))) + (nozzle_position[2]);

                                double init_position[3] = { x2,y2,z2};
                                double init_velocity[3] = { get_random(Vx_min,Vx_max), get_random(Vy_min,Vy_max), get_random(Vz_min,Vz_max) };

                                int grid_index_x = ((int)floor(x2/H));
                                int grid_index_y = ((int)floor(y2/H));
                                int grid_index_z = ((int)floor(z2/H));
                                //-----------------------------------------------------------------------
                                //    CREATE THE AIR PARTICLES AT INIT POSITION WITH INIT VELOCITY
                                //-----------------------------------------------------------------------
                                grid[grid_index_x][grid_index_y][grid_index_z].air_particles.push_back( Particle(init_position, init_velocity) );
                            }
                        }
                    }
                }
            }
            //-----------------------------------------------------------------------
            //    NON - DEFAULT CONVERTER LANCE TYPE 3
           //-----------------------------------------------------------------------
            if (g_Param.getLanceType()==3)
            {
                //double center_of_bottom_ring[3]={(0.5*XGRID*g_Param.getSmoothingRadius()),4*g_Param.getSmoothingRadius(),0.5*ZGRID*g_Param.getSmoothingRadius()};
                double throat_radius =0.007;

                for(int cycles =0; cycles<10; cycles++)
                {
                    for(int n=0; n< 10; n++)
                    {
                        double theta     = (double)n*(TwoPI/10);
                        double x  = (get_random(0.01,1)*throat_radius*cos(theta)) + (center_of_ring[0]);// get_random(0.01,1)* makes it so that that the particles can be seeded at any part of the circle
                        double y  = center_of_ring[1];
                        double z  = (get_random(0.01,1)*throat_radius *sin( theta ))+ (center_of_ring[2]);

                        double init_position[3] = { x, y, z  };
                        double init_velocity[3] = { get_random(-0.02,0.02), get_random(8,9), get_random(-0.02,0.02) };

                        int grid_index_x = ((int)floor(x/H));
                        int grid_index_y = ((int)floor(y/H));
                        int grid_index_z = ((int)floor(z/H));

                        grid[grid_index_x][grid_index_y][grid_index_z].air_particles.push_back( Particle(init_position, init_velocity) );
                    }
                }
            }
        }
    }//else
}

inline void Normalize(double *a)
{
    double length = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
    if(!length) return;
    double inv_length = 1.0/length;
    a[0] *= inv_length;
    a[1] *= inv_length;
    a[2] *= inv_length;
}
inline double GetLength(double *a)
{
    return sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
}

