#include "Parameters.h"
#include <utility>

void SPH::initializeConverterBoundary()
{
   /* ofstream cyl_boundaries;
    cyl_boundaries.open("data/csv/converter.csv");*/

    //-----------------------------
    char converter_out[100];
    snprintf(converter_out, 100, "data/csv/boundary/b_%s_i.csv", g_Param.getRunName().c_str());
    ofstream conv_boundaries( converter_out );
    if( !conv_boundaries)
    {
        cout<< "ERROR: Could not open boundary cvs file. Make sure there the following directory exists: data/csv/boundary/ "  << endl
                                              << "If it does not exist, please create the appropriate directories"<<endl;

    }
    //---------------------------------------------------------------------------------------
    //  THE CONVERTER INVOLVES THREE SHAPES: THE FLOOR, THE CONE AND THE CYLINDER
    //---------------------------------------------------------------------------------------
    // BC_SPACING = TO THE DISTANCE BETWEEN BOUNDARY PARTICLES


    double novelocity [3]= {0,0,0};             // VELOCITY=0
    double H= g_Param.getSmoothingRadius();  // SMOOTHING RADIUS
    //cone
    double h_cone = 0.500;                      // HEIGHT OF CONE
    double radius_initial  = 0.4125; //the initial radius of the cone (layer 1)
    double radius_initial2 = radius_initial-(0.5*BC_SPACING); //the initial radius of the cone (layer 2)
    double radius_cone=0; // will set later
    double radius_cone2=0;// will set later
    double theta_cone  = 0;// will set later
    double theta_cone2 = 0;// will set later
    int last_height_step_cone = (int)float((h_cone/BC_SPACING));// the height at which the cone stops and the cylinder begins

    //floor
    int startx =  (int)floor((0.5*XGRID*H-radius_initial)/BC_SPACING)-2;
    int endx = 2+(int)floor((0.5*XGRID*H+radius_initial)/BC_SPACING);

    //----------------------------------------------------------Converter Floor----------------------------------------------------------------------

    for(int x = startx ; x < endx; x++)     //360= 0.9/0.0025   xmax/BC_SPACING   the number of bc particles that can fit in [0<x<0.9] you can view the domain range (0.9) in paraview.// original 360
    {
        for(int z= startx; z <endx; z++)      //576=1.44/0.0025   the number of bc particles that can fit in [0<y<1.44] // orig 576
        {

            //----------------------------------------------------------------
            // DEFINE THE GEOMETRY OF THE FLOOR OF THE CONVERTER
            //  NOTE: ALL BOUNDARIES MUST BE 2 LAYERS THICK AND
            //        OFFSET SO THAT THE BOUNDARIES ARE STAGGERED. IN A
            //------------------------------------------------------------------
            double xy_wall1[3] = {x*BC_SPACING + (0*H), (0.5*H)  , (z*BC_SPACING)+(0.0*H)  }; //x*BC_SPACING= the distance that the last placed particle if is from x=0. orig 0
            double xy_wall1b[3] = {x*BC_SPACING+0.5*BC_SPACING+(0*H), 0.5*BC_SPACING+(0.5*H)  , (z*BC_SPACING+0.5*BC_SPACING )+(0*H) };//double walls

            //-----------------------------------------------------------
            //            FIND THE INDEXES FOR THE TWO LAYERS
            //-----------------------------------------------------------

            int grid_index_x = ((int)floor(xy_wall1[0]/H));
            int grid_index_y = ((int)floor(xy_wall1[1]/H));
            int grid_index_z = ((int)floor(xy_wall1[2]/H));

            int grid_index_x2 = ((int)floor(xy_wall1b[0]/H));
            int grid_index_y2 = ((int)floor(xy_wall1b[1]/H));
            int grid_index_z2 = ((int)floor(xy_wall1b[2]/H));

           //---------------------------------------------------------------------------------------------------------------------------------------------------
            //           PRINT, VIEWED AS TE CONVERTER FILE IN CSV/CONVERTER. HOWEVERY THE BOUNDARIES ALSO PRINT TO CSV/BOUNDARIES
            //              THE converter.csv file only shows the last used converter geometry
            //--------------------------------------------------------------------------------------------------------------------------------------------------

            if(g_Param.getCsvBoundary()==1) conv_boundaries<<xy_wall1[0]<<","<<xy_wall1[1]<<","<<xy_wall1[2]<<","<<"["<<grid_index_x<<"]"<<"["<<grid_index_y<<"]"<<"["<<grid_index_z<<"]"<<","<<endl; //print out to csv file
            if(g_Param.getCsvBoundary()==1) conv_boundaries<<xy_wall1b[0]<<","<<xy_wall1b[1]<<","<<xy_wall1b[2]<<","<<"["<<grid_index_x<<"]"<<"["<<grid_index_y<<"]"<<"["<<grid_index_z<<"]"<<","<<endl;
            //-----------------------------------------------------------
            //            CREATE PARTICLES AND INSERT IN GRID
            //-----------------------------------------------------------
            grid[grid_index_x][grid_index_y][grid_index_z].boundary_particles.push_back( Particle(xy_wall1, novelocity) );
            grid[grid_index_x2][grid_index_y2][grid_index_z2].boundary_particles.push_back( Particle(xy_wall1b, novelocity) );


        }
    }
    //-----------------------------------------------------------Converter Cone--------------------------------------------------

    for(int height_step=0; height_step<last_height_step_cone; height_step++)  // height_step[0,17] ** the height of the cone is 0.146 the height of the ful converter.
    {
        //-----------------------------------------------------------
        //           GEOMETRY
        //----------------------------------------------------------
        double y  = (height_step *BC_SPACING) + (0.5*H+BC_SPACING);
        double y2 = y+0.5*BC_SPACING;

        radius_cone  = 0.625*y+radius_initial;
        radius_cone2 = 0.625*y+radius_initial2;

        double circumferance_cone= TwoPI*radius_cone;

        double n_points_in_cone= circumferance_cone/BC_SPACING;// inner and outer have same amount of points
        double n_points_in_cone2= circumferance_cone/BC_SPACING;

        for(int n=0; n<n_points_in_cone; n++)
        {
            theta_cone     = n*(TwoPI/n_points_in_cone); // we want theta to go from 0 to 2PI within n_points_in_circle times. for each point of the circle that is created theta will adjust accordingly.
            double x  = (radius_cone *cos(theta_cone))+0.5*XGRID*H + (0*H); //the +(x*H) is the translation factor
            double z  = (radius_cone *sin( theta_cone))+0.5*ZGRID*H + (0*H);

            double bound_position_cone[3] = {     x,     y,     z };
            double bound_velocity_cone[3] = {     0,     0,     0 };

              //-----------------------------------------------------------
            //            FIND THE INDEXES FOR THE LAYER 1
            //-----------------------------------------------------------

            int grid_index_x_cone=((int)floor(bound_position_cone[0]/H));
            int grid_index_y_cone=((int)floor(bound_position_cone[1]/H));
            int grid_index_z_cone=((int)floor(bound_position_cone[2]/H));

            if(g_Param.getCsvBoundary()==1) conv_boundaries<<bound_position_cone[0]<<","<<bound_position_cone[1]<<","<<bound_position_cone[2]<<","<<"["<<grid_index_x_cone<<"]"<<"["<<grid_index_y_cone<<"]"<<"["<<grid_index_z_cone<<"]"<<","<<endl; //print out to csv file
            //-----------------------------------------------------------
            //            CREATE PARTICLES AND INSERT IN GRID
            //-----------------------------------------------------------
            grid[grid_index_x_cone][grid_index_y_cone][grid_index_z_cone].boundary_particles.push_back( Particle(bound_position_cone, bound_velocity_cone) );
        }
        for(int n=0; n<n_points_in_cone2; n++)
        {
            theta_cone2     = n*(TwoPI/n_points_in_cone2); // we want theta to go from 0 to 2PI within n_points_in_circle times. for each point of the circle that is created theta will adjust accordingly.
            double x2  = (radius_cone2 *cos(theta_cone2))+0.5*XGRID*H + (0*H); //the +(x*H) is the translation factor
            double z2  = (radius_cone2 *sin( theta_cone2))+0.5*ZGRID*H + (0*H);

            double bound_position_cone2[3] = {     x2,     y2,     z2 };
            double bound_velocity_cone2[3] = {     0,     0,     0 };
             //-----------------------------------------------------------
            //            FIND THE INDEXES FOR THE LAYER 2
            //-----------------------------------------------------------
            int grid_index_x_cone2=((int)floor(bound_position_cone2[0]/H));
            int grid_index_y_cone2=((int)floor(bound_position_cone2[1]/H));
            int grid_index_z_cone2=((int)floor(bound_position_cone2[2]/H));

            if(g_Param.getCsvBoundary()==1) conv_boundaries<<bound_position_cone2[0]<<","<<bound_position_cone2[1]<<","<<bound_position_cone2[2]<<","<<"["<<grid_index_x_cone2<<"]"<<"["<<grid_index_y_cone2<<"]"<<"["<<grid_index_z_cone2<<"]"<<","<<endl; //print out to csv file
            //-----------------------------------------------------------
            //            CREATE PARTICLES AND INSERT IN GRID
            //-----------------------------------------------------------
            grid[grid_index_x_cone2][grid_index_y_cone2][grid_index_z_cone2].boundary_particles.push_back( Particle(bound_position_cone2, bound_velocity_cone2) );
        }
    }
//----------------------------------------------------------Converter Cylinder-------------------------------------------
    //cylinder
    double h_conv = H*YGRID ;  //1.850;//remember that it is better to alter the value of YGRID than h_cyl to maximize computational efficiency. we can probably make the height small than 0.95 to to speed up the system especially if we have particle deletion. however we make want air particles to physically leave the converter before
    double radius_outer= 0.725;
    double radius_inner= (radius_outer)-(0.5*BC_SPACING);
    double circumferance_outer= TwoPI*radius_outer;
    double n_points_in_circle_outer= circumferance_outer/BC_SPACING;// inner and outer have same amount of points
    double theta = 0;
    // radius= 0.92592*z+radius_initial        radius_intial = radius_final/1.758  radius initial refers to to the small open of the cone at z=0. radius final is the final open of the cone where it connects with the cylinder.

    for(int height_step=last_height_step_cone-2; height_step<(h_conv/BC_SPACING); height_step++)  // height_step means on which level of y we are placing the circle, bp must be a specific spacing distance away. inital height step - 0.39/BC_Spacing- 1*H---> this is the last height on the cone
    {
        for(int n=0; n<n_points_in_circle_outer; n++)
        {

            theta     = n*(TwoPI/n_points_in_circle_outer); // we want theta to go from 0 to 2PI within n_points_in_circle times. for each point of the circle that is created theta will adjust accordingly.
            double x  = (radius_outer *cos(theta))+0.5*XGRID*H + (0*H); //the +(x*H) is the translation factor
            double y  = (height_step *BC_SPACING) + (1*H);
            double z  = (radius_outer *sin( theta ))+0.5*ZGRID*H + (0*H);

            double x2  = (radius_inner *cos(1.5*theta))+0.5*XGRID*H + (0*H);
            double y2  = (height_step *BC_SPACING)+(0.5*BC_SPACING)+ (1*H);
            double z2  = (radius_inner *sin(1.5*theta ))+0.5*ZGRID*H+ (0*H);

            double bound_position_outer[3] = {     x,     y,     z };
            double bound_velocity_outer[3] = {     0,     0,     0 };

            double bound_position_inner[3] = {     x2,    y2,   z2};
            double bound_velocity_inner[3] = {     0,     0,     0 };

            int grid_index_x_outer=((int)floor(bound_position_outer[0]/H));
            int grid_index_y_outer=((int)floor(bound_position_outer[1]/H));
            int grid_index_z_outer=((int)floor(bound_position_outer[2]/H));

            int grid_index_x_inner=((int)floor(bound_position_inner[0]/H));
            int grid_index_y_inner=((int)floor(bound_position_inner[1]/H));
            int grid_index_z_inner=((int)floor(bound_position_inner[2]/H));

            if(g_Param.getCsvBoundary()==1) conv_boundaries<<bound_position_outer[0]<<","<<bound_position_outer[1]<<","<<bound_position_outer[2]<<","<<"["<<grid_index_x_outer<<"]"<<"["<<grid_index_y_outer<<"]"<<"["<<grid_index_z_outer<<"]"<<","<<endl; //print out to csv file
            if(g_Param.getCsvBoundary()==1) conv_boundaries<<bound_position_inner[0]<<","<<bound_position_inner[1]<<","<<bound_position_inner[2]<<","<<"["<<grid_index_x_inner<<"]"<<"["<<grid_index_y_inner<<"]"<<"["<<grid_index_z_inner<<"]"<<","<<endl; //print out to csv file

            grid[grid_index_x_outer][grid_index_y_outer][grid_index_z_outer].boundary_particles.push_back( Particle(bound_position_outer, bound_velocity_outer) );

            grid[grid_index_x_inner][grid_index_y_inner][grid_index_z_inner].boundary_particles.push_back( Particle(bound_position_inner, bound_velocity_inner) );
        }
    }
    conv_boundaries.close();
}
void SPH::initializeMillBoundary()
{



    char mill_out[100];
    snprintf(mill_out, 100, "data/csv/boundary/b_%s_i.csv", g_Param.getRunName().c_str());
    ofstream mill_boundaries( mill_out );
    if( !mill_boundaries)
    {
        cout<< "ERROR: Could not open boundary cvs file. Make sure there the following directory exists: data/csv/boundary/ "  << endl
                                              << "If it does not exist, please create the appropriate directories"<<endl;

    }

    double novelocity [3]= {0,0,0};
    double H= g_Param.getSmoothingRadius();
    double radius_outer= 0.725;
    double layers=2;

    if (layers==2)
    {
        //floor
        int startx =  (int)floor((0.5*XGRID*g_Param.getSmoothingRadius()-radius_outer)/BC_SPACING)-2;
        int endx = 2+(int)floor((0.5*XGRID*g_Param.getSmoothingRadius()+radius_outer)/BC_SPACING);

        //----------------------------------------------------------BM Floor----------------------------------------------------------------------

        for(int x = startx ; x < endx; x++)     //360= 0.9/0.0025   xmax/BC_SPACING   the number of bc particles that can fit in [0<x<0.9] you can view the domain range (0.9) in paraview.// original 360
        {
            for(int z= startx; z <endx; z++)      //576=1.44/0.0025   the number of bc particles that can fit in [0<y<1.44] // orig 576
            {
                double xy_wall1[3] = {x*BC_SPACING + (0*g_Param.getSmoothingRadius()), (0.5*H)  , (z*BC_SPACING)+(0.0*H)  }; //x*BC_SPACING= the distance that the last placed particle if is from x=0. orig 0
                double xy_wall1b[3] = {x*BC_SPACING+0.5*BC_SPACING+(0*H), 0.5*BC_SPACING+(0.5*H)  , (z*BC_SPACING+0.5*BC_SPACING )+(0*H) };//double walls


                int grid_index_x = ((int)floor(xy_wall1[0]/H));
                int grid_index_y = ((int)floor(xy_wall1[1]/H));
                int grid_index_z = ((int)floor(xy_wall1[2]/H));

                int grid_index_x2 = ((int)floor(xy_wall1b[0]/H));
                int grid_index_y2 = ((int)floor(xy_wall1b[1]/H));
                int grid_index_z2 = ((int)floor(xy_wall1b[2]/H));


                if(g_Param.getCsvBoundary()==1)mill_boundaries<<xy_wall1[0]<<","<<xy_wall1[1]<<","<<xy_wall1[2]<<","<<"["<<grid_index_x<<"]"<<"["<<grid_index_y<<"]"<<"["<<grid_index_z<<"]"<<","<<endl; //print out to csv file
                if(g_Param.getCsvBoundary()==1)mill_boundaries<<xy_wall1b[0]<<","<<xy_wall1b[1]<<","<<xy_wall1b[2]<<","<<"["<<grid_index_x<<"]"<<"["<<grid_index_y<<"]"<<"["<<grid_index_z<<"]"<<","<<endl;

                grid[grid_index_x][grid_index_y][grid_index_z].boundary_particles.push_back( Particle(xy_wall1, novelocity) );
                grid[grid_index_x2][grid_index_y2][grid_index_z2].boundary_particles.push_back( Particle(xy_wall1b, novelocity) );
            }
        }

//----------------------------------------------------------BM Cylinder-------------------------------------------

        //cylinder
        double h_conv = 0.95*H*YGRID ;  //1.850;//remember that it is better to alter the value of YGRID than h_cyl to maximize computational efficiency. we can probably make the height small than 0.95 to to speed up the system especially if we have particle deletion. however we make want air particles to physically leave the converter before

        double radius_inner= (radius_outer)-(0.5*BC_SPACING);
        double circumferance_outer= TwoPI*radius_outer;
        double n_points_in_circle_outer= circumferance_outer/BC_SPACING;// inner and outer have same amount of points
        double theta = 0;
        // radius= 0.92592*z+radius_initial        radius_intial = radius_final/1.758  radius initial refers to to the small open of the cone at z=0. radius final is the final open of the cone where it connects with the cylinder.

        for(int height_step=0; height_step<(h_conv/BC_SPACING); height_step++)  // height_step means on which level of y we are placing the circle, bp must be a specific spacing distance away. inital height step - 0.39/BC_Spacing- 1*H---> this is the last height on the cone
        {
            for(int n=0; n<n_points_in_circle_outer; n++)
            {

                theta     = n*(TwoPI/n_points_in_circle_outer); // we want theta to go from 0 to 2PI within n_points_in_circle times. for each point of the circle that is created theta will adjust accordingly.
                double x  = (radius_outer *cos(theta))+0.5*XGRID*g_Param.getSmoothingRadius() + (0*g_Param.getSmoothingRadius()); //the +(x*H) is the translation factor
                double y  = (height_step *BC_SPACING) + (0*g_Param.getSmoothingRadius());
                double z  = (radius_outer *sin( theta ))+0.5*ZGRID*g_Param.getSmoothingRadius() + (0*g_Param.getSmoothingRadius());

                double x2  = (radius_inner *cos(1.5*theta))+0.5*XGRID*g_Param.getSmoothingRadius() + (0*g_Param.getSmoothingRadius());
                double y2  = (height_step *BC_SPACING)+(0.5*BC_SPACING)+ (1*g_Param.getSmoothingRadius());
                double z2  = (radius_inner *sin(1.5*theta ))+0.5*ZGRID*g_Param.getSmoothingRadius() + (0*g_Param.getSmoothingRadius());

                double bound_position_outer[3] = {     x,     y,     z };
                double bound_velocity_outer[3] = {     0,     0,     0 };

                double bound_position_inner[3] = {     x2,    y2,   z2};
                double bound_velocity_inner[3] = {     0,     0,     0 };

                int grid_index_x_outer=((int)floor(bound_position_outer[0]/g_Param.getSmoothingRadius()));
                int grid_index_y_outer=((int)floor(bound_position_outer[1]/g_Param.getSmoothingRadius()));
                int grid_index_z_outer=((int)floor(bound_position_outer[2]/g_Param.getSmoothingRadius()));

                int grid_index_x_inner=((int)floor(bound_position_inner[0]/g_Param.getSmoothingRadius()));
                int grid_index_y_inner=((int)floor(bound_position_inner[1]/g_Param.getSmoothingRadius()));
                int grid_index_z_inner=((int)floor(bound_position_inner[2]/g_Param.getSmoothingRadius()));

                if(g_Param.getCsvBoundary()==1)mill_boundaries<<bound_position_outer[0]<<","<<bound_position_outer[1]<<","<<bound_position_outer[2]<<","<<"["<<grid_index_x_outer<<"]"<<"["<<grid_index_y_outer<<"]"<<"["<<grid_index_z_outer<<"]"<<","<<endl; //print out to csv file
                if(g_Param.getCsvBoundary()==1)mill_boundaries<<bound_position_inner[0]<<","<<bound_position_inner[1]<<","<<bound_position_inner[2]<<","<<"["<<grid_index_x_inner<<"]"<<"["<<grid_index_y_inner<<"]"<<"["<<grid_index_z_inner<<"]"<<","<<endl; //print out to csv file

                grid[grid_index_x_outer][grid_index_y_outer][grid_index_z_outer].boundary_particles.push_back( Particle(bound_position_outer, bound_velocity_outer) );

                grid[grid_index_x_inner][grid_index_y_inner][grid_index_z_inner].boundary_particles.push_back( Particle(bound_position_inner, bound_velocity_inner) );

                if(theta>PI*0.97 && theta <PI*1.03 )
                {
                    for(int n=1; n<10 ; n++)
                    {


                        double x  = (radius_outer *cos(theta))+0.5*XGRID*g_Param.getSmoothingRadius() + n*BC_SPACING; //the +(x*H) is the translation factor
                        double y  = (height_step *BC_SPACING) + (0*g_Param.getSmoothingRadius());
                        double z  = (radius_outer *sin( theta ))+0.5*ZGRID*g_Param.getSmoothingRadius() ;



                        double bound_position_outer[3] = {     x,     y,     z };


                        int grid_index_x_outer=((int)floor(bound_position_outer[0]/g_Param.getSmoothingRadius()));
                        int grid_index_y_outer=((int)floor(bound_position_outer[1]/g_Param.getSmoothingRadius()));
                        int grid_index_z_outer=((int)floor(bound_position_outer[2]/g_Param.getSmoothingRadius()));


                        if(g_Param.getCsvBoundary()==1)mill_boundaries<<bound_position_outer[0]<<","<<bound_position_outer[1]<<","<<bound_position_outer[2]<<","<<"["<<grid_index_x_outer<<"]"<<"["<<grid_index_y_outer<<"]"<<"["<<grid_index_z_outer<<"]"<<","<<endl; //print out to csv file
                        //       cyl_boundaries<<bound_position_inner[0]<<","<<bound_position_inner[1]<<","<<bound_position_inner[2]<<","<<"["<<grid_index_x_inner<<"]"<<"["<<grid_index_y_inner<<"]"<<"["<<grid_index_z_inner<<"]"<<","<<endl; //print out to csv file
//
                        grid[grid_index_x_outer][grid_index_y_outer][grid_index_z_outer].boundary_particles.push_back( Particle(bound_position_outer, bound_velocity_outer) );
                        //       grid[grid_index_x_inner][grid_index_y_inner][grid_index_z_inner].boundary_particles.push_back( Particle(bound_position_inner, bound_velocity_inner) );
                    }
                }
                if(theta>(PI/2)*0.97 && theta <(1/2)*PI*1.03 )
                {
                    for(int n=1; n<10 ; n++)
                    {
                        // cout<<"1/2 PI     n="<<n<<endl;
                        double x  = (radius_outer *cos(theta))+0.5*XGRID*H; //the +(x*H) is the translation factor
                        double y  = (height_step *BC_SPACING) + (0*H);
                        double z  = (radius_outer *sin( theta ))+0.5*ZGRID*H - n*BC_SPACING;

                        double x2  = (radius_inner *cos(1.5*theta))+0.5*XGRID*H;
                        double y2  = (height_step *BC_SPACING)+(0.5*BC_SPACING)+ (1*H);
                        double z2  = (radius_inner *sin(1.5*theta ))+0.5*ZGRID*H ;

                        double bound_position_outer[3] = {     x,     y,     z };
                        double bound_velocity_outer[3] = {     0,     0,     0 };

                        double bound_position_inner[3] = {     x2,    y2,   z2};
                        double bound_velocity_inner[3] = {     0,     0,     0 };

                        int grid_index_x_outer=((int)floor(bound_position_outer[0]/H));
                        int grid_index_y_outer=((int)floor(bound_position_outer[1]/H));
                        int grid_index_z_outer=((int)floor(bound_position_outer[2]/H));

                        int grid_index_x_inner=((int)floor(bound_position_inner[0]/H));
                        int grid_index_y_inner=((int)floor(bound_position_inner[1]/H));
                        int grid_index_z_inner=((int)floor(bound_position_inner[2]/H));

                        if(g_Param.getCsvBoundary()==1)mill_boundaries<<bound_position_outer[0]<<","<<bound_position_outer[1]<<","<<bound_position_outer[2]<<","<<"["<<grid_index_x_outer<<"]"<<"["<<grid_index_y_outer<<"]"<<"["<<grid_index_z_outer<<"]"<<","<<endl; //print out to csv file
                        if(g_Param.getCsvBoundary()==1)mill_boundaries<<bound_position_inner[0]<<","<<bound_position_inner[1]<<","<<bound_position_inner[2]<<","<<"["<<grid_index_x_inner<<"]"<<"["<<grid_index_y_inner<<"]"<<"["<<grid_index_z_inner<<"]"<<","<<endl; //print out to csv file

                        grid[grid_index_x_outer][grid_index_y_outer][grid_index_z_outer].boundary_particles.push_back( Particle(bound_position_outer, bound_velocity_outer) );
                        grid[grid_index_x_inner][grid_index_y_inner][grid_index_z_inner].boundary_particles.push_back( Particle(bound_position_inner, bound_velocity_inner) );
                    }
                }
                if(theta>(3/2)*PI*0.97 && theta <(3/2)*PI*1.03 )
                {
                    for(int n=1; n<10 ; n++)
                    {
                        // cout<<"3/2 PI     n="<<n<<endl;
                        double x  = (radius_outer *cos(theta))+0.5*XGRID*H ; //the +(x*H) is the translation factor
                        double y  = (height_step *BC_SPACING) + (0*H);
                        double z  = (radius_outer *sin(theta))+0.5*ZGRID*H + BC_SPACING;


                        double x2  = (radius_inner *cos(1.5*theta))+0.5*XGRID*H + (0*H);
                        double y2  = (height_step *BC_SPACING)+(0.5*BC_SPACING)+ (1*H);
                        double z2  = (radius_inner *sin(1.5*theta ))+0.5*ZGRID*H+ (0*H);

                        double bound_position_outer[3] = {     x,     y,     z };
                        double bound_velocity_outer[3] = {     0,     0,     0 };

                        double bound_position_inner[3] = {     x2,    y2,   z2};
                        double bound_velocity_inner[3] = {     0,     0,     0 };

                        int grid_index_x_outer=((int)floor(bound_position_outer[0]/H));
                        int grid_index_y_outer=((int)floor(bound_position_outer[1]/H));
                        int grid_index_z_outer=((int)floor(bound_position_outer[2]/H));

                        int grid_index_x_inner=((int)floor(bound_position_inner[0]/H));
                        int grid_index_y_inner=((int)floor(bound_position_inner[1]/H));
                        int grid_index_z_inner=((int)floor(bound_position_inner[2]/H));

                        if(g_Param.getCsvBoundary()==1)mill_boundaries<<bound_position_outer[0]<<","<<bound_position_outer[1]<<","<<bound_position_outer[2]<<","<<"["<<grid_index_x_outer<<"]"<<"["<<grid_index_y_outer<<"]"<<"["<<grid_index_z_outer<<"]"<<","<<endl; //print out to csv file
                        if(g_Param.getCsvBoundary()==1)mill_boundaries<<bound_position_inner[0]<<","<<bound_position_inner[1]<<","<<bound_position_inner[2]<<","<<"["<<grid_index_x_inner<<"]"<<"["<<grid_index_y_inner<<"]"<<"["<<grid_index_z_inner<<"]"<<","<<endl; //print out to csv file

                        grid[grid_index_x_outer][grid_index_y_outer][grid_index_z_outer].boundary_particles.push_back( Particle(bound_position_outer, bound_velocity_outer) );
                        grid[grid_index_x_inner][grid_index_y_inner][grid_index_z_inner].boundary_particles.push_back( Particle(bound_position_inner, bound_velocity_inner) );
                    }
                }
                if(theta>(2)*PI*0.97 /*|| theta <PI*0.03*/ )
                {
                    for(int n=1; n <10 ; n++)
                    {

                        double x  = (radius_outer *cos(theta))+0.5*XGRID*g_Param.getSmoothingRadius() - n*BC_SPACING; //the +(x*H) is the translation factor
                        double y  = (height_step *BC_SPACING) + (0*g_Param.getSmoothingRadius());
                        double z  = (radius_outer *sin( theta ))+0.5*ZGRID*g_Param.getSmoothingRadius() + (0*g_Param.getSmoothingRadius());

                        double x2  = (radius_inner *cos(1.5*theta))+0.5*XGRID*g_Param.getSmoothingRadius() + (0*g_Param.getSmoothingRadius());
                        double y2  = (height_step *BC_SPACING)+(0.5*BC_SPACING)+ (1*g_Param.getSmoothingRadius());
                        double z2  = (radius_inner *sin(1.5*theta ))+0.5*ZGRID*g_Param.getSmoothingRadius() + (0*g_Param.getSmoothingRadius());

                        double bound_position_outer[3] = {     x,     y,     z };
                        double bound_velocity_outer[3] = {     0,     0,     0 };

                        double bound_position_inner[3] = {     x2,    y2,   z2};
                        double bound_velocity_inner[3] = {     0,     0,     0 };

                        int grid_index_x_outer=((int)floor(bound_position_outer[0]/g_Param.getSmoothingRadius()));
                        int grid_index_y_outer=((int)floor(bound_position_outer[1]/g_Param.getSmoothingRadius()));
                        int grid_index_z_outer=((int)floor(bound_position_outer[2]/g_Param.getSmoothingRadius()));

                        int grid_index_x_inner=((int)floor(bound_position_inner[0]/g_Param.getSmoothingRadius()));
                        int grid_index_y_inner=((int)floor(bound_position_inner[1]/g_Param.getSmoothingRadius()));
                        int grid_index_z_inner=((int)floor(bound_position_inner[2]/g_Param.getSmoothingRadius()));

                        if(g_Param.getCsvBoundary()==1)mill_boundaries<<bound_position_outer[0]<<","<<bound_position_outer[1]<<","<<bound_position_outer[2]<<","<<"["<<grid_index_x_outer<<"]"<<"["<<grid_index_y_outer<<"]"<<"["<<grid_index_z_outer<<"]"<<","<<endl; //print out to csv file
                        if(g_Param.getCsvBoundary()==1)mill_boundaries<<bound_position_inner[0]<<","<<bound_position_inner[1]<<","<<bound_position_inner[2]<<","<<"["<<grid_index_x_inner<<"]"<<"["<<grid_index_y_inner<<"]"<<"["<<grid_index_z_inner<<"]"<<","<<endl; //print out to csv file

                        grid[grid_index_x_outer][grid_index_y_outer][grid_index_z_outer].boundary_particles.push_back( Particle(bound_position_outer, bound_velocity_outer) );
                        grid[grid_index_x_inner][grid_index_y_inner][grid_index_z_inner].boundary_particles.push_back( Particle(bound_position_inner, bound_velocity_inner) );
                    }
                }
            }
        }  //roof

        //----------------------------------------------------------BM Roof----------------------------------------------------------------------

        for(int x = startx ; x < endx; x++)     //360= 0.9/0.0025   xmax/BC_SPACING   the number of bc particles that can fit in [0<x<0.9] you can view the domain range (0.9) in paraview.// original 360
        {
            for(int z= startx; z <endx; z++)      //576=1.44/0.0025   the number of bc particles that can fit in [0<y<1.44] // orig 576
            {
                double xy_wall1[3] = {x*BC_SPACING + (0*g_Param.getSmoothingRadius()), h_conv+ BC_SPACING  , (z*BC_SPACING)+(0.0*g_Param.getSmoothingRadius())  }; //x*BC_SPACING= the distance that the last placed particle if is from x=0. orig 0
                double xy_wall1b[3] = {x*BC_SPACING+0.5*BC_SPACING+(0*g_Param.getSmoothingRadius()), h_conv+ 2*BC_SPACING , (z*BC_SPACING+0.5*BC_SPACING )+(0*g_Param.getSmoothingRadius()) };//double walls

                int grid_index_x = ((int)floor(xy_wall1[0]/H));
                int grid_index_y = ((int)floor(xy_wall1[1]/H));
                int grid_index_z = ((int)floor(xy_wall1[2]/H));

                int grid_index_x2 = ((int)floor(xy_wall1b[0]/H));
                int grid_index_y2 = ((int)floor(xy_wall1b[1]/H));
                int grid_index_z2 = ((int)floor(xy_wall1b[2]/H));

                if(g_Param.getCsvBoundary()==1)mill_boundaries<<xy_wall1[0]<<","<<xy_wall1[1]<<","<<xy_wall1[2]<<","<<"["<<grid_index_x<<"]"<<"["<<grid_index_y<<"]"<<"["<<grid_index_z<<"]"<<","<<endl; //print out to csv file
                if(g_Param.getCsvBoundary()==1)mill_boundaries<<xy_wall1b[0]<<","<<xy_wall1b[1]<<","<<xy_wall1b[2]<<","<<"["<<grid_index_x<<"]"<<"["<<grid_index_y<<"]"<<"["<<grid_index_z<<"]"<<","<<endl;

                grid[grid_index_x][grid_index_y][grid_index_z].boundary_particles.push_back( Particle(xy_wall1, novelocity) );
                grid[grid_index_x2][grid_index_y2][grid_index_z2].boundary_particles.push_back( Particle(xy_wall1b, novelocity) );
            }
        }
    }
    if (layers==3)
    {
        //floor
        int startx =  (int)floor((0.5*XGRID*g_Param.getSmoothingRadius()-radius_outer)/BC_SPACING)-2;
        int endx = 2+(int)floor((0.5*XGRID*g_Param.getSmoothingRadius()+radius_outer)/BC_SPACING);

        //----------------------------------------------------------BM Floor----------------------------------------------------------------------

        for(int x = startx ; x < endx; x++)     // xmax/BC_SPACING   the number of bc particles that can fit in [0<x<0.9] you can view the domain range (0.9) in paraview.// original 360
        {
            for(int z= startx; z <endx; z++)      // the number of bc particles that can fit in [0<y<1.44] // orig 576
            {
                double xy_wall1[3] = {x*BC_SPACING + (0*g_Param.getSmoothingRadius()), (0.5*H)  , (z*BC_SPACING)+(0.0*H)  }; //x*BC_SPACING= the distance that the last placed particle if is from x=0. orig 0
                double xy_wall1b[3] = {x*BC_SPACING+0.25*BC_SPACING+(0*H), 0.25*BC_SPACING+(0.5*H)  , (z*BC_SPACING+0.25*BC_SPACING )+(0*H) };//double walls
                double xy_wall1c[3] = {x*BC_SPACING+0.5*BC_SPACING+(0*H), 0.5*BC_SPACING+(0.5*H)  , (z*BC_SPACING+0.5*BC_SPACING )+(0*H) };//double walls


                int grid_index_x = ((int)floor(xy_wall1[0]/H));
                int grid_index_y = ((int)floor(xy_wall1[1]/H));
                int grid_index_z = ((int)floor(xy_wall1[2]/H));

                int grid_index_x2 = ((int)floor(xy_wall1b[0]/H));
                int grid_index_y2 = ((int)floor(xy_wall1b[1]/H));
                int grid_index_z2 = ((int)floor(xy_wall1b[2]/H));

                int grid_index_x3 = ((int)floor(xy_wall1c[0]/H));
                int grid_index_y3 = ((int)floor(xy_wall1c[1]/H));
                int grid_index_z3 = ((int)floor(xy_wall1c[2]/H));


                if(g_Param.getCsvBoundary()==1)mill_boundaries<<xy_wall1[0]<<","<<xy_wall1[1]<<","<<xy_wall1[2]<<","<<"["<<grid_index_x<<"]"<<"["<<grid_index_y<<"]"<<"["<<grid_index_z<<"]"<<","<<endl; //print out to csv file
                if(g_Param.getCsvBoundary()==1)mill_boundaries<<xy_wall1b[0]<<","<<xy_wall1b[1]<<","<<xy_wall1b[2]<<","<<"["<<grid_index_x<<"]"<<"["<<grid_index_y<<"]"<<"["<<grid_index_z<<"]"<<","<<endl;
                if(g_Param.getCsvBoundary()==1)mill_boundaries<<xy_wall1c[0]<<","<<xy_wall1c[1]<<","<<xy_wall1c[2]<<","<<"["<<grid_index_x<<"]"<<"["<<grid_index_y<<"]"<<"["<<grid_index_z<<"]"<<","<<endl;

                grid[grid_index_x][grid_index_y][grid_index_z].boundary_particles.push_back( Particle(xy_wall1, novelocity) );
                grid[grid_index_x2][grid_index_y2][grid_index_z2].boundary_particles.push_back( Particle(xy_wall1b, novelocity) );
                grid[grid_index_x3][grid_index_y3][grid_index_z3].boundary_particles.push_back( Particle(xy_wall1c, novelocity) );
            }
        }

//----------------------------------------------------------BM Cylinder-------------------------------------------

        //cylinder
        double h_conv = 0.98*H*YGRID ; //0.98 //1.850;// it is better to alter the value of YGRID than h_conv to maximize computational efficiency. we can probably make the height small than 0.95 to to speed up the system especially if we have particle deletion. however we make want air particles to physically leave the converter before

        double radius_inner= (radius_outer)-(0.5*BC_SPACING);
        //third layer radius
        double radius_inner_inner= (radius_outer)-(0.25*BC_SPACING);
        double circumferance_outer= TwoPI*radius_outer;
        double n_points_in_circle_outer= circumferance_outer/BC_SPACING;// inner and outer have same amount of points
        double theta = 0;

        for(int height_step=0; height_step<(h_conv/BC_SPACING); height_step++)  // height_step means on which level of y we are placing the circle, bp must be a specific spacing distance away. inital height step - 0.39/BC_Spacing- 1*H---> this is the last height on the cone
        {
            for(int n=0; n<n_points_in_circle_outer; n++)
            {
                theta     = n*(TwoPI/n_points_in_circle_outer); // we want theta to go from 0 to 2PI within n_points_in_circle times. for each point of the circle that is created theta will adjust accordingly.
                double x  = (radius_outer *cos(theta))+0.5*XGRID*g_Param.getSmoothingRadius() + (0*g_Param.getSmoothingRadius()); //the +(x*H) is the translation factor
                double y  = (height_step *BC_SPACING) + (0*g_Param.getSmoothingRadius());
                double z  = (radius_outer *sin( theta ))+0.5*ZGRID*g_Param.getSmoothingRadius() + (0*g_Param.getSmoothingRadius());

                double x2  = (radius_inner *cos(1.666*theta))+0.5*XGRID*g_Param.getSmoothingRadius() + (0*g_Param.getSmoothingRadius());
                double y2  = (height_step *BC_SPACING)+(0.666*BC_SPACING)+ (1*g_Param.getSmoothingRadius());
                double z2  = (radius_inner *sin(1.666*theta ))+0.5*ZGRID*g_Param.getSmoothingRadius() + (0*g_Param.getSmoothingRadius());

                //third layer between 1 and 2
                double x3  = (radius_inner_inner *cos(1.333*theta))+0.5*XGRID*g_Param.getSmoothingRadius() + (0*g_Param.getSmoothingRadius());
                double y3  = (height_step *BC_SPACING)+(0.333*BC_SPACING)+ (1*g_Param.getSmoothingRadius());
                double z3  = (radius_inner *sin(1.333*theta ))+0.5*ZGRID*g_Param.getSmoothingRadius() + (0*g_Param.getSmoothingRadius());

                double bound_position_outer[3] = {     x,     y,     z };
                double bound_velocity_outer[3] = {     0,     0,     0 };

                double bound_position_inner[3] = {     x2,    y2,   z2};
                double bound_velocity_inner[3] = {     0,     0,     0 };

                //third layer between 1 and 2
                double bound_position_inner_inner[3] = {     x3,    y3,   z3};
                double bound_velocity_inner_inner[3] = {     0,     0,     0 };

                int grid_index_x_outer=((int)floor(bound_position_outer[0]/H));
                int grid_index_y_outer=((int)floor(bound_position_outer[1]/H));
                int grid_index_z_outer=((int)floor(bound_position_outer[2]/H));

                int grid_index_x_inner=((int)floor(bound_position_inner[0]/H));
                int grid_index_y_inner=((int)floor(bound_position_inner[1]/H));
                int grid_index_z_inner=((int)floor(bound_position_inner[2]/H));

                //third layer between 1 and 2
                int grid_index_x_inner_inner((int)floor(bound_position_inner_inner[0]/H));
                int grid_index_y_inner_inner=((int)floor(bound_position_inner_inner[1]/H));
                int grid_index_z_inner_inner=((int)floor(bound_position_inner_inner[2]/H));

                if(g_Param.getCsvBoundary()==1)mill_boundaries<<bound_position_outer[0]<<","<<bound_position_outer[1]<<","<<bound_position_outer[2]<<","<<"["<<grid_index_x_outer<<"]"<<"["<<grid_index_y_outer<<"]"<<"["<<grid_index_z_outer<<"]"<<","<<endl; //print out to csv file
                if(g_Param.getCsvBoundary()==1)mill_boundaries<<bound_position_inner[0]<<","<<bound_position_inner[1]<<","<<bound_position_inner[2]<<","<<"["<<grid_index_x_inner<<"]"<<"["<<grid_index_y_inner<<"]"<<"["<<grid_index_z_inner<<"]"<<","<<endl; //print out to csv file
                if(g_Param.getCsvBoundary()==1)mill_boundaries<<bound_position_inner_inner[0]<<","<<bound_position_inner_inner[1]<<","<<bound_position_inner_inner[2]<<","<<"["<<grid_index_x_inner_inner<<"]"<<"["<<grid_index_y_inner_inner<<"]"<<"["<<grid_index_z_inner_inner<<"]"<<","<<endl; //print out to csv file

                grid[grid_index_x_outer][grid_index_y_outer][grid_index_z_outer].boundary_particles.push_back( Particle(bound_position_outer, bound_velocity_outer) );
                grid[grid_index_x_inner][grid_index_y_inner][grid_index_z_inner].boundary_particles.push_back( Particle(bound_position_inner, bound_velocity_inner) );
                grid[grid_index_x_inner_inner][grid_index_y_inner_inner][grid_index_z_inner_inner].boundary_particles.push_back( Particle(bound_position_inner_inner, bound_velocity_inner_inner) );

                if(theta>PI*0.97 && theta <PI*1.03 )
                {
                    for(int n=1; n<10 ; n++)
                    {


                        double x  = (radius_outer *cos(theta))+0.5*XGRID*g_Param.getSmoothingRadius() + n*BC_SPACING; //the +(x*H) is the translation factor
                        double y  = (height_step *BC_SPACING) + (0*g_Param.getSmoothingRadius());
                        double z  = (radius_outer *sin( theta ))+0.5*ZGRID*g_Param.getSmoothingRadius() ;

                        //  double x2  = (radius_inner *cos(1.5*theta))+0.5*XGRID*g_Param.getSmoothingRadius() ;
                        // double y2  = (height_step *BC_SPACING)+(0.5*BC_SPACING)+ (1*g_Param.getSmoothingRadius());
                        // double z2  = (radius_inner *sin(1.5*theta ))+0.5*ZGRID*g_Param.getSmoothingRadius() ;

                        double bound_position_outer[3] = {     x,     y,     z };
                        //  double bound_velocity_outer[3] = {     0,     0,     0 };
                        //    double bound_position_inner[3] = {     x2,    y2,   z2};
                        //   double bound_velocity_inner[3] = {     0,     0,     0 };

                        int grid_index_x_outer=((int)floor(bound_position_outer[0]/g_Param.getSmoothingRadius()));
                        int grid_index_y_outer=((int)floor(bound_position_outer[1]/g_Param.getSmoothingRadius()));
                        int grid_index_z_outer=((int)floor(bound_position_outer[2]/g_Param.getSmoothingRadius()));

                        //  int grid_index_x_inner=((int)floor(bound_position_inner[0]/g_Param.getSmoothingRadius()));
                        //  int grid_index_y_inner=((int)floor(bound_position_inner[1]/g_Param.getSmoothingRadius()));
                        //  int grid_index_z_inner=((int)floor(bound_position_inner[2]/g_Param.getSmoothingRadius()));

                        if(g_Param.getCsvBoundary()==1)mill_boundaries<<bound_position_outer[0]<<","<<bound_position_outer[1]<<","<<bound_position_outer[2]<<","<<"["<<grid_index_x_outer<<"]"<<"["<<grid_index_y_outer<<"]"<<"["<<grid_index_z_outer<<"]"<<","<<endl; //print out to csv file
                        //       cyl_boundaries<<bound_position_inner[0]<<","<<bound_position_inner[1]<<","<<bound_position_inner[2]<<","<<"["<<grid_index_x_inner<<"]"<<"["<<grid_index_y_inner<<"]"<<"["<<grid_index_z_inner<<"]"<<","<<endl; //print out to csv file
//
                        grid[grid_index_x_outer][grid_index_y_outer][grid_index_z_outer].boundary_particles.push_back( Particle(bound_position_outer, bound_velocity_outer) );
                        //       grid[grid_index_x_inner][grid_index_y_inner][grid_index_z_inner].boundary_particles.push_back( Particle(bound_position_inner, bound_velocity_inner) );
                    }
                }
                if(theta>(PI/2)*0.97 && theta <(1/2)*PI*1.03 )
                {
                    for(int n=1; n<10 ; n++)
                    {
                        // cout<<"1/2 PI     n="<<n<<endl;
                        double x  = (radius_outer *cos(theta))+0.5*XGRID*H; //the +(x*H) is the translation factor
                        double y  = (height_step *BC_SPACING) + (0*H);
                        double z  = (radius_outer *sin( theta ))+0.5*ZGRID*H - n*BC_SPACING;

                        double x2  = (radius_inner *cos(1.5*theta))+0.5*XGRID*H;
                        double y2  = (height_step *BC_SPACING)+(0.5*BC_SPACING)+ (1*H);
                        double z2  = (radius_inner *sin(1.5*theta ))+0.5*ZGRID*H ;

                        double bound_position_outer[3] = {     x,     y,     z };
                        double bound_velocity_outer[3] = {     0,     0,     0 };

                        double bound_position_inner[3] = {     x2,    y2,   z2};
                        double bound_velocity_inner[3] = {     0,     0,     0 };

                        int grid_index_x_outer=((int)floor(bound_position_outer[0]/H));
                        int grid_index_y_outer=((int)floor(bound_position_outer[1]/H));
                        int grid_index_z_outer=((int)floor(bound_position_outer[2]/H));

                        int grid_index_x_inner=((int)floor(bound_position_inner[0]/H));
                        int grid_index_y_inner=((int)floor(bound_position_inner[1]/H));
                        int grid_index_z_inner=((int)floor(bound_position_inner[2]/H));

                        if(g_Param.getCsvBoundary()==1)mill_boundaries<<bound_position_outer[0]<<","<<bound_position_outer[1]<<","<<bound_position_outer[2]<<","<<"["<<grid_index_x_outer<<"]"<<"["<<grid_index_y_outer<<"]"<<"["<<grid_index_z_outer<<"]"<<","<<endl; //print out to csv file
                        if(g_Param.getCsvBoundary()==1)mill_boundaries<<bound_position_inner[0]<<","<<bound_position_inner[1]<<","<<bound_position_inner[2]<<","<<"["<<grid_index_x_inner<<"]"<<"["<<grid_index_y_inner<<"]"<<"["<<grid_index_z_inner<<"]"<<","<<endl; //print out to csv file

                        grid[grid_index_x_outer][grid_index_y_outer][grid_index_z_outer].boundary_particles.push_back( Particle(bound_position_outer, bound_velocity_outer) );
                        grid[grid_index_x_inner][grid_index_y_inner][grid_index_z_inner].boundary_particles.push_back( Particle(bound_position_inner, bound_velocity_inner) );
                    }
                }
                if(theta>(3/2)*PI*0.97 && theta <(3/2)*PI*1.03 )
                {
                    for(int n=1; n<10 ; n++)
                    {
                        // cout<<"3/2 PI     n="<<n<<endl;
                        double x  = (radius_outer *cos(theta))+0.5*XGRID*H ; //the +(x*H) is the translation factor
                        double y  = (height_step *BC_SPACING) + (0*H);
                        double z  = (radius_outer *sin(theta))+0.5*ZGRID*H + BC_SPACING;


                        double x2  = (radius_inner *cos(1.5*theta))+0.5*XGRID*H + (0*H);
                        double y2  = (height_step *BC_SPACING)+(0.5*BC_SPACING)+ (1*H);
                        double z2  = (radius_inner *sin(1.5*theta ))+0.5*ZGRID*H+ (0*H);

                        double bound_position_outer[3] = {     x,     y,     z };
                        double bound_velocity_outer[3] = {     0,     0,     0 };

                        double bound_position_inner[3] = {     x2,    y2,   z2};
                        double bound_velocity_inner[3] = {     0,     0,     0 };

                        int grid_index_x_outer=((int)floor(bound_position_outer[0]/H));
                        int grid_index_y_outer=((int)floor(bound_position_outer[1]/H));
                        int grid_index_z_outer=((int)floor(bound_position_outer[2]/H));

                        int grid_index_x_inner=((int)floor(bound_position_inner[0]/H));
                        int grid_index_y_inner=((int)floor(bound_position_inner[1]/H));
                        int grid_index_z_inner=((int)floor(bound_position_inner[2]/H));

                        if(g_Param.getCsvBoundary()==1)mill_boundaries<<bound_position_outer[0]<<","<<bound_position_outer[1]<<","<<bound_position_outer[2]<<","<<"["<<grid_index_x_outer<<"]"<<"["<<grid_index_y_outer<<"]"<<"["<<grid_index_z_outer<<"]"<<","<<endl; //print out to csv file
                        if(g_Param.getCsvBoundary()==1)mill_boundaries<<bound_position_inner[0]<<","<<bound_position_inner[1]<<","<<bound_position_inner[2]<<","<<"["<<grid_index_x_inner<<"]"<<"["<<grid_index_y_inner<<"]"<<"["<<grid_index_z_inner<<"]"<<","<<endl; //print out to csv file

                        grid[grid_index_x_outer][grid_index_y_outer][grid_index_z_outer].boundary_particles.push_back( Particle(bound_position_outer, bound_velocity_outer) );
                        grid[grid_index_x_inner][grid_index_y_inner][grid_index_z_inner].boundary_particles.push_back( Particle(bound_position_inner, bound_velocity_inner) );
                    }
                }
                if(theta>(2)*PI*0.97 /*|| theta <PI*0.03*/ )
                {
                    for(int n=1; n <10 ; n++)
                    {

                        double x  = (radius_outer *cos(theta))+0.5*XGRID*g_Param.getSmoothingRadius() - n*BC_SPACING; //the +(x*H) is the translation factor
                        double y  = (height_step *BC_SPACING) + (0*g_Param.getSmoothingRadius());
                        double z  = (radius_outer *sin( theta ))+0.5*ZGRID*g_Param.getSmoothingRadius() + (0*g_Param.getSmoothingRadius());

                        double x2  = (radius_inner *cos(1.5*theta))+0.5*XGRID*g_Param.getSmoothingRadius() + (0*g_Param.getSmoothingRadius());
                        double y2  = (height_step *BC_SPACING)+(0.5*BC_SPACING)+ (1*g_Param.getSmoothingRadius());
                        double z2  = (radius_inner *sin(1.5*theta ))+0.5*ZGRID*g_Param.getSmoothingRadius() + (0*g_Param.getSmoothingRadius());

                        double bound_position_outer[3] = {     x,     y,     z };
                        double bound_velocity_outer[3] = {     0,     0,     0 };

                        double bound_position_inner[3] = {     x2,    y2,   z2};
                        double bound_velocity_inner[3] = {     0,     0,     0 };

                        int grid_index_x_outer=((int)floor(bound_position_outer[0]/g_Param.getSmoothingRadius()));
                        int grid_index_y_outer=((int)floor(bound_position_outer[1]/g_Param.getSmoothingRadius()));
                        int grid_index_z_outer=((int)floor(bound_position_outer[2]/g_Param.getSmoothingRadius()));

                        int grid_index_x_inner=((int)floor(bound_position_inner[0]/g_Param.getSmoothingRadius()));
                        int grid_index_y_inner=((int)floor(bound_position_inner[1]/g_Param.getSmoothingRadius()));
                        int grid_index_z_inner=((int)floor(bound_position_inner[2]/g_Param.getSmoothingRadius()));

                        if(g_Param.getCsvBoundary()==1)mill_boundaries<<bound_position_outer[0]<<","<<bound_position_outer[1]<<","<<bound_position_outer[2]<<","<<"["<<grid_index_x_outer<<"]"<<"["<<grid_index_y_outer<<"]"<<"["<<grid_index_z_outer<<"]"<<","<<endl; //print out to csv file
                        if(g_Param.getCsvBoundary()==1)mill_boundaries<<bound_position_inner[0]<<","<<bound_position_inner[1]<<","<<bound_position_inner[2]<<","<<"["<<grid_index_x_inner<<"]"<<"["<<grid_index_y_inner<<"]"<<"["<<grid_index_z_inner<<"]"<<","<<endl; //print out to csv file

                        grid[grid_index_x_outer][grid_index_y_outer][grid_index_z_outer].boundary_particles.push_back( Particle(bound_position_outer, bound_velocity_outer) );
                        grid[grid_index_x_inner][grid_index_y_inner][grid_index_z_inner].boundary_particles.push_back( Particle(bound_position_inner, bound_velocity_inner) );
                    }
                }
            }
        }  //roof

        //----------------------------------------------------------BM Roof----------------------------------------------------------------------

        for(int x = startx ; x < endx; x++)     //360= 0.9/0.0025   xmax/BC_SPACING   the number of bc particles that can fit in [0<x<0.9] you can view the domain range (0.9) in paraview.// original 360
        {
            for(int z= startx; z <endx; z++)      //576=1.44/0.0025   the number of bc particles that can fit in [0<y<1.44] // orig 576
            {
                double xy_wall1[3] = {x*BC_SPACING + (0*g_Param.getSmoothingRadius()), h_conv+ BC_SPACING  , (z*BC_SPACING)+(0.0*g_Param.getSmoothingRadius())  }; //x*BC_SPACING= the distance that the last placed particle if is from x=0. orig 0
                double xy_wall1b[3] = {x*BC_SPACING+0.5*BC_SPACING+(0*g_Param.getSmoothingRadius()), h_conv+ 2*BC_SPACING , (z*BC_SPACING+0.5*BC_SPACING )+(0*g_Param.getSmoothingRadius()) };//double walls

                int grid_index_x = ((int)floor(xy_wall1[0]/H));
                int grid_index_y = ((int)floor(xy_wall1[1]/H));
                int grid_index_z = ((int)floor(xy_wall1[2]/H));

                int grid_index_x2 = ((int)floor(xy_wall1b[0]/H));
                int grid_index_y2 = ((int)floor(xy_wall1b[1]/H));
                int grid_index_z2 = ((int)floor(xy_wall1b[2]/H));

                if(g_Param.getCsvBoundary()==1)mill_boundaries<<xy_wall1[0]<<","<<xy_wall1[1]<<","<<xy_wall1[2]<<","<<"["<<grid_index_x<<"]"<<"["<<grid_index_y<<"]"<<"["<<grid_index_z<<"]"<<","<<endl; //print out to csv file
                if(g_Param.getCsvBoundary()==1)mill_boundaries<<xy_wall1b[0]<<","<<xy_wall1b[1]<<","<<xy_wall1b[2]<<","<<"["<<grid_index_x<<"]"<<"["<<grid_index_y<<"]"<<"["<<grid_index_z<<"]"<<","<<endl;

                grid[grid_index_x][grid_index_y][grid_index_z].boundary_particles.push_back( Particle(xy_wall1, novelocity) );
                grid[grid_index_x2][grid_index_y2][grid_index_z2].boundary_particles.push_back( Particle(xy_wall1b, novelocity) );
            }
        }
    }

    if(g_Param.getCsvBoundary()==1)mill_boundaries.close();
}
