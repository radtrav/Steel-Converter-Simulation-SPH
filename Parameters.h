#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <vector>
#include <algorithm>
#include <list>
#include <stack>
#include <utility>
#include <math.h>

#include <cstdlib>
#include <string>
#include <deque>
#include "strtk.hpp"
//constants
#define PI    3.14159265
#define TwoPI 6.28318531
#define InvPI 0.318309886

#define XGRID 30//34//25//20//15#define YGRID 20//32
#define YGRID 20//20//25//40//46//48//46//37//32
#define ZGRID 30//25//20//15
#define XRANGE XGRID*H //1.53 for example x is between [0,1.53)
#define YRANGE YGRID*H// 2.07
#define ZRANGE ZGRID*H// 1.53
#define BC_SPACING 0.01//0.007//0.01//0.021//0.0200//0.01 //0.0025x8
using namespace std;

struct Particle
{

    double position[3], velocity[3], acceleration[3];
    double density;
    double inv_density;
    double pressure;
    double gravity_force;
    double buoyant_force;
    bool   is_surface;
    bool   is_outside;
    int    a_neighbour_count;
    int    b_neighbour_count;
    int    w_neighbour_count;
    bool   is_interface;
    double contact_force;
    double damping_force;
    double radius_orig;

    Particle(double *p_in, double *v_in)
    {
        for(int i = 0; i < 3; i++) position[i]     = p_in[i];
        for(int i = 0; i < 3; i++) velocity[i]     = v_in[i];
        for(int i = 0; i < 3; i++) acceleration[i] = 0.0;

        is_outside = false;
    }
};
struct Grid
{
    double bbmin[3], bbmax[3];

    vector<Particle> water_particles;
    vector<Particle> air_particles;
    vector<Particle> boundary_particles;

    Grid() {};
    Grid(double *bbmin_in, double *bbmax_in)
    {
        for(int i = 0; i < 3; i++) bbmin[i] = bbmin_in[i];
        for(int i = 0; i < 3; i++) bbmax[i] = bbmax_in[i];
    }
};
struct CellVertex  // where is this defined?
{
    double color_field;
    double color_field_normal[3];
    double color_field_normal_norm;
    double color_field_laplacian;
    CellVertex() {};
};
//#define GRID_Y 1000
class SPH
{
private:
    Grid grid[XGRID][YGRID][ZGRID];
    CellVertex cell_vertex[XGRID+1][YGRID+1][ZGRID+1];
    void updateDensity(int particle_id, int id_x, int id_y, int id_z, bool is_water_particle, bool is_air_particle, bool is_boundary_particle);
    void updateParticle(int particle_id, int id_x, int id_y, int id_z, bool is_water_particle, bool is_boundary_particle, bool is_air_particle, vector<Particle> &new_air_particles);
    void updateGrid(int id_x, int id_y, int id_z);
    void spawnParticles(int step);
    void computeColorField(CellVertex &cell_vertex, int id_x, int id_y, int id_z);

    //MAIN MENU FUNCTIONS
    void noteScreen();
    void lanceSetUpScreen();
    void setupParticleBlock();
    void viewSettings();
    void viewParticleBlock();
    void readInData();
    void runOptions();
    void initializeLance();
    void viewParticleProperties();
    void setupParticleProperties();
    void viewTimestepProperties();
    void setupTimestepProperties();
    void viewLanceProperties();
    void viewEnvironmentProperties();
    void setupEnvironmentProperties();
    void initializeStirGas();
    void setupPrintProperties();
    void viewPrintProperties();

public:

    SPH() {}
    char writeGrid();
    void processStep();
    char homeScreen();

    char writeCSV();
    char writeVTK();
    void initializeParticleBlock();
    void setFluidSystem();
    void initializeConverterBoundary();
    void initializeMillBoundary();
    char printSettings();

    int getWaterCount();
    int getAirCount();
    int getBoundaryCount();
};
class Param
{
private:
    //PARTICLE PROPERTIES
    double Rho0_w;
    double Rho0_a;
    double Rho0_b;
    double Water_mass;
    double Air_mass;
    double Boundary_mass;
    double Sigma_s;
    double Mu_w;
    double Mu_a;
    double Mu_b;
    double Kvalue;
    double smoothing_radius;

    // Dashpot
    double hertzian_spring;
    double damping;
    double tangential_friction;
    int dem_type;

    //STEP PROPERTIES
    double Delta_t;
    int Water_per_step;
    int Air_per_step;
    int step_count;
    int Timesteps;

    //NOTES
    bool   isNotes;
    string RunName;
    string notes;
    bool   csv_air;
    bool   csv_water;
    bool   csv_boundary;
    bool   vtk_air;
    bool   vtk_water;
    bool   vtk_boundary;

    //ENVIRONMENT PROPERTIES
    bool gravity_on;
    bool converter_boundaries_on;
    bool mill_boundaries_on;
    bool particle_deletion_on;
    bool spin_on;
    double spin_speed;
    bool steel_02;

    //BLOCK PROPERTIES
    bool particle_block_on;
    int block_length;
    int block_width;
    int block_height;
    double block_locationX;
    double block_locationY;
    double block_locationZ;
    double particle_block_spacing;

    //LANCE PROPERTIES
    bool   lance_on;
    int     lance_type;
    double lance_posX;
    double lance_posY;
    double lance_posZ;
    int     rate_of_air;
    double lance_opening_radius;
    double lance_output_velocityXmin;
    double lance_output_velocityXmax;
    double lance_output_velocityYmin;
    double lance_output_velocityYmax;
    double lance_output_velocityZmin;
    double lance_output_velocityZmax;
    int     n_air_rings;
    int     number_seeds;

    //STIRRING GAS
    bool    stirring_gas_on;
    int     stir_type;
    double  stir_posX;
    double  stir_posY;
    double stir_posZ;
    int     rate_of_stir;
    double stir_opening_radius;
    double stir_output_velocityXmin;
    double stir_output_velocityXmax;
    double stir_output_velocityYmin;
    double stir_output_velocityYmax;
    double stir_output_velocityZmin;
    double stir_output_velocityZmax;

    //PRINT PROPERTIES
    int print_every;
public:
    Param()
    {
        //PARTICLE PROPERTIES
        Rho0_w = 7050;//1000;//1000.0;
        Rho0_a = 10.00;
        Rho0_b = 7050;//1000.0;
        Water_mass =0.038;//0.038// 0.048;//0.01;//0.048
        Air_mass =  0.00038;//0.0038;//0.0038;
        Boundary_mass =0.038;//0.038//0.048;//0.001;//0.048;
        Sigma_s= 12;//6;//15;//0.6; steel 25*water
        Mu_w= 28;//28.0;//5.0; steel 5.6*water
        Mu_a=1.0;
        Mu_b= 40;//5.0;
        smoothing_radius=0.05;


        //Dashpot
        hertzian_spring = 500000;//500000;//50 000
        damping=5;//5;
        tangential_friction=0.5;
        dem_type = 2;

        //STEP PROPERTIES
        Delta_t= 0.0005;//0.0005
        Water_per_step=0;
        Air_per_step = 0 ;
        Kvalue= 12;//10;
        Timesteps= 50000; //12000 should give abou 6 seconds
        step_count=0;

        //ENVIRONMENT PROPERTIES
        gravity_on = 1;
        converter_boundaries_on = 1;
        mill_boundaries_on=0;
        particle_deletion_on = 1;
        spin_on=0;
        spin_speed=0;
        steel_02=1;

        //NOTES
        RunName= "No_Name";
        notes=" NO NOTES";
        isNotes=0;


        //BLOCK PROPERTIES
        particle_block_on = 0; // FILL SETTINGS 1,40,120,120,0,0,0,0
        block_height=60;
        block_length=40;   //120;
        block_width =40;   //120;
        block_locationX =0.1;//0.3;//0.3;// 0.12;
        block_locationY =0.2;//0.05;//0.10;// 0.12;
        block_locationZ =0.25;//0.3;//0.30;// 0.12;
        particle_block_spacing = 0.32;//0.15;//0.225;//0.038mass= 0.32;//0.72;

        //LANCE PROPERTIES
        lance_on     =1;
        lance_type  =2;
        lance_posX  =0.5;
        lance_posY  =0.8;//0.75;//0.39;//0.32//0.6;//0.39;20
        lance_posZ  =0.5;
        rate_of_air = 2;//1;
        lance_opening_radius      = 0.035;//0.04;//from komagata
        lance_output_velocityXmin=-0.08;//0.2
        lance_output_velocityXmax= 0.08;
        lance_output_velocityYmin= -7.0;//6//-9.0;
        lance_output_velocityYmax= -6.9;//5.9//-8.9;
        lance_output_velocityZmin=-0.08;
        lance_output_velocityZmax= 0.08;
        n_air_rings = 5;
        number_seeds =5;

        //STIRRING GAS
        stirring_gas_on=0;
        stir_type=3;
        stir_posX=0.5;
        stir_posY=0.05;//0.39;
        stir_posZ=0.5;
        rate_of_stir= 1;
        stir_opening_radius=0.50;//
        stir_output_velocityXmin=-0.04;//0.2
        stir_output_velocityXmax= 0.04;
        stir_output_velocityYmin= 2.9;
        stir_output_velocityYmax= 3.0;
        stir_output_velocityZmin=-0.04;
        stir_output_velocityZmax= 0.04;

        //PRINT PROPERTIES
        csv_air=0;
        csv_water=0;
        csv_boundary=1;
        vtk_air=1;
        vtk_water=1;
        vtk_boundary=0;
        print_every= 30;
    }
    //PARTICLE PROPERTIES
    double getRho0_w() const{
        return Rho0_w;
    };
    void   setRho0_w(double value)
    {
        Rho0_w = value;
    };
    double getRho0_a() const
    {
        return Rho0_a;
    };
    void   setRho0_a(double value)
    {
        Rho0_a = value;
    };
    double getRho0_b()const
    {
        return Rho0_b;
    };
    void   setRho0_b(double value)
    {
        Rho0_b =value;
    };
    double getWater_mass() const
    {
        return Water_mass;
    };
    void   setWater_mass(double value)
    {
        Water_mass=value;
    };
    double getAir_mass()const
    {
        return Air_mass;
    };
    void   setAir_mass(double value)
    {
        Air_mass=value;
    };
    double getBoundary_mass()const
    {
        return Boundary_mass;
    };
    void   setBoundary_mass(double value)
    {
        Boundary_mass=value;
    };
    double getSigma_s ()const
    {
        return Sigma_s;
    };
    void   setSigma_s(double value)
    {
        Sigma_s=value;
    };
    double getMu_w ()const
    {
        return Mu_w;
    };
    void   setMu_w(double value)
    {
        Mu_w=value;
    };
    double getMu_a ()const
    {
        return Mu_a;
    };
    void   setMu_a(double value)
    {
        Mu_a=value;
    };
    double getMu_b ()const
    {
        return Mu_b;
    };
    void   setMu_b (double value)
    {
        Mu_b=value;
    };
    double getKvalue ()const
    {
        return Kvalue;
    };
    void   setKvalue(double value)
    {
        Kvalue=value;
    };
    double getSmoothingRadius()
    {
        return smoothing_radius;
    };
    void   setSmoothingRadius(double h)
    {
        smoothing_radius=h;
    };

    //DASHPOT
    double getHertzian()
    {
        return hertzian_spring;
    };
    void   setHertzian(double value)
    {
        hertzian_spring=value;
    };
    double getDamping()
    {
        return damping;
    };
    void   setDamping(double value)
    {
        damping=value;
    };
    double getTangentialFriction()
    {
        return tangential_friction;
    };
    void   setTangentialFriction(double value)
    {
        tangential_friction=value;
    };


    int    getDemType()
    {
        return dem_type;
    };
    void   setDemType(int value)
    {
        dem_type=value;
    };

    //STEP PROPERTIES
    double getDelta_t()const
    {
        return Delta_t ;
    };
    void   setDelta_t(double value)
    {
        Delta_t=value;
    };
    int    getWater_per_step()const
    {
        return Water_per_step ;
    };
    void   setWater_per_step(int value)
    {
        Water_per_step=value;
    };
    int    getAir_per_step ()const
    {
        return Air_per_step;
    };
    void   setAir_per_step(int value)
    {
        Air_per_step=value;
    };
    int    getTimesteps()const
    {
        return Timesteps ;
    };
    void   setTimesteps(int value)
    {
        Timesteps=value;
    };
    int    getStepCount()
    {
        return step_count;
    };
    void   accumulateStepCount()
    {
        step_count++;
    };

    //NOTES
    string getRunName()const
    {
        return RunName;
    };
    void   SetRunName(string name)
    {
        RunName=name;
    };
    string getNotes()const
    {
        return notes;
    };
    void   setNotes(string note)
    {
        notes=note;
    };
    bool   getIsNotes()
    {
        return isNotes;
    };
    void   setIsNotes(bool notes)
    {
        isNotes=notes;
    };


    //ENVIRONMENT PROPERTIES
    bool   getGravityIsOn()const
    {
        return gravity_on;
    };
    void   setGravityIsOn(bool on_off)
    {
        gravity_on=on_off;
    };
    bool   getConverterOn()
    {
        return converter_boundaries_on;
    };
    void   setConverterOn(bool on_off)
    {
        converter_boundaries_on=on_off;
    };
    bool   getMillOn()
    {
        return mill_boundaries_on;
    };
    void   setMillOn(bool on_off)
    {
        mill_boundaries_on=on_off;
    };
    bool   getParticleDeletion()
    {
        return particle_deletion_on;
    };
    void   setParticleDeletion(bool on_off)
    {
        particle_deletion_on=on_off;
    };
    bool   getSpinOn()
    {
        return spin_on;
    };
    void   setSpinOn(bool on_off)
    {
        spin_on=on_off;
    };
    double getSpinSpeed()
    {
        return spin_speed;
    };
    void   setSpinSpeed(double speed)
    {
        spin_speed=speed;
    };
    bool   getSteelO2()
    {
        return steel_02;
    };
    void   setSteelO2(bool on_off)
    {
        steel_02=on_off;
    };

    //BLOCK PROPERTIES
    bool   getParticleBlockOn()const
    {
        return particle_block_on;
    };
    void   setParticleBlockOn(bool on_off)
    {
        particle_block_on=on_off;
    };
    int    getBlockLength ()
    {
        return block_length;
    };
    void   setBlockLength (int length)
    {
        block_length=length;
    };
    int    getBlockWidth ()
    {
        return block_width;
    };
    void   setBlockWidth (int width)
    {
        block_width=width;
    };
    int    getBlockHeight ()
    {
        return block_height;
    };
    void   setBlockHeight (int height)
    {
        block_height=height;
    };
    double getParticleBlockLocationX ()
    {
        return block_locationX;
    };
    void   setParticleBlockLocationX(double location)
    {
        block_locationX=location;
    };
    double getParticleBlockLocationY ()
    {
        return block_locationY;
    };
    void   setParticleBlockLocationY(double location)
    {
        block_locationY=location;
    };
    double getParticleBlockLocationZ ()
    {
        return block_locationZ;
    };
    void   setParticleBlockLocationZ(double location)
    {
        block_locationZ=location;
    };
    double getParticleBlockSpacing()
    {
        return particle_block_spacing;
    };
    void   setParticleBlockSpacing(double spacing)
    {
        particle_block_spacing=spacing;
    };

    //LANCE PROPERTIES
    bool   getLanceOn()const
    {
        return lance_on;
    };
    void   setLanceOn(bool on_off)
    {
        lance_on=on_off;
    };
    int    getLanceType()
    {
        return lance_type;
    };
    void   setLanceType(int type)
    {
        lance_type=type;
    };
    double getLancePosX()
    {
        return lance_posX;
    };
    void   setLancePosX(double lancepos)
    {
        lance_posX=lancepos;
    };
    double getLancePosY()
    {
        return lance_posY;
    };
    void   setLancePosY(double lancepos)
    {
        lance_posY=lancepos;
    };
    double getLancePosZ()
    {
        return lance_posZ;
    };
    void   setLancePosZ(double lancepos)
    {
        lance_posZ=lancepos;
    };
    int    getRateOfAir()
    {
        return rate_of_air;
    };
    void   setRateOfAir(int rate)
    {
        rate_of_air=rate;
    };
    double getLanceOpeningRadius()
    {
        return lance_opening_radius;
    };
    void   setLanceOpeningRadius(double radius)
    {
        lance_opening_radius=radius;
    };
    double getLanceOutputVelocityXMin()
    {
        return lance_output_velocityXmin;
    };
    void   setLanceOutputVelocityXMin(double velocity)
    {
        lance_output_velocityXmin=velocity;
    };
    double getLanceOutputVelocityXMax()
    {
        return lance_output_velocityXmax;
    };
    void   setLanceOutputVelocityXMax(double velocity)
    {
        lance_output_velocityXmax=velocity;
    };
    double getLanceOutputVelocityYMin()
    {
        return lance_output_velocityYmin;
    };
    void   setLanceOutputVelocityYMin(double velocity)
    {
        lance_output_velocityYmin=velocity;
    };
    double getLanceOutputVelocityYMax()
    {
        return lance_output_velocityYmax;
    };
    void   setLanceOutputVelocityYMax(double velocity)
    {
        lance_output_velocityYmax=velocity;
    };
    double getLanceOutputVelocityZMin()
    {
        return lance_output_velocityZmin;
    };
    void   setLanceOutputVelocityZMin(double velocity)
    {
        lance_output_velocityZmin=velocity;
    };
    double getLanceOutputVelocityZMax()
    {
        return lance_output_velocityZmax;
    };
    void   setLanceOutputVelocityZMax(double velocity)
    {
        lance_output_velocityZmax=velocity;
    };
    int getNumberOfRings()
    {
        return n_air_rings;
    };
    void   setNumberOfRings(int rings)
    {
        n_air_rings=rings;
    };
    int     getNumberofSeeds()
    {
        return number_seeds;
    };
    void   setNumberOfSeeds(int seeds)
    {
        number_seeds=seeds;
    };

    //STIRRINGGAS
    bool   getStirringGasOn()
    {
        return stirring_gas_on;
    };
    void   setStirringGasOn(bool on_off)
    {
        stirring_gas_on=on_off;
    };
    int    getStirType()
    {
        return stir_type;
    };
    void   setStirType(int type)
    {
        stir_type=type;
    };
    double getStirPosX()
    {
        return stir_posX;
    };
    void   setStirPosX(double stirpos)
    {
        stir_posX=stirpos;
    };
    double getStirPosY()
    {
        return stir_posY;
    };
    void   setStirPosY(double stirpos)
    {
        stir_posY=stirpos;
    };
    double getStirPosZ()
    {
        return stir_posZ;
    };
    void   setStirPosZ(double stirpos)
    {
        stir_posZ=stirpos;
    };
    int    getRateOfStir()
    {
        return rate_of_stir;
    };
    void   setRateOfStir(int rate)
    {
        rate_of_stir=rate;
    };
    double getStirOpeningRadius()
    {
        return stir_opening_radius;
    };
    void   setStirOpeningRadius(double radius)
    {
        stir_opening_radius=radius;
    };
    double getStirOutputVelocityXMin()
    {
        return stir_output_velocityXmin;
    };
    void   setStirOutputVelocityXMin(double velocity)
    {
        stir_output_velocityXmin=velocity;
    };
    double getStirOutputVelocityXMax()
    {
        return stir_output_velocityXmax;
    };
    void   setStirOutputVelocityXMax(double velocity)
    {
        stir_output_velocityXmax=velocity;
    };
    double getStirOutputVelocityYMin()
    {
        return stir_output_velocityYmin;
    };
    void   setStirOutputVelocityYMin(double velocity)
    {
        stir_output_velocityYmin=velocity;
    };
    double getStirOutputVelocityYMax()
    {
        return stir_output_velocityYmax;
    };
    void   setStirOutputVelocityYMax(double velocity)
    {
        stir_output_velocityYmax=velocity;
    };
    double getStirOutputVelocityZMin()
    {
        return stir_output_velocityZmin;
    };
    void   setStirOutputVelocityZMin(double velocity)
    {
        stir_output_velocityZmin=velocity;
    };
    double getStirOutputVelocityZMax()
    {
        return stir_output_velocityZmax;
    };
    void   setStirOutputVelocityZMax(double velocity)
    {
        stir_output_velocityZmax=velocity;
    };

    //PRINTPROPERTIES
    bool   getCsvAir()
    {
        return csv_air;
    };
    void   setCsvAir(bool on_off)
    {
        csv_air=on_off;
    };
    bool   getCsvWater()
    {
        return csv_water;
    };
    void   setCsvWater(bool on_off)
    {
        csv_water=on_off;
    };
    bool   getCsvBoundary()
    {
        return csv_boundary;
    };
    void   setCsvBoundary(bool on_off)
    {
        csv_boundary=on_off;
    };
    bool   getVtkAir()
    {
        return vtk_air;
    };
    void   setVtkAir(bool on_off)
    {
        vtk_air=on_off;
    };
    bool   getVtkWater()
    {
        return vtk_water;
    };
    void   setVtkWater(bool on_off)
    {
        vtk_water=on_off;
    };
    bool   getVtkBoundary()
    {
        return vtk_boundary;
    };
    void   setVtkBoundary(bool on_off)
    {
        vtk_boundary=on_off;
    };
    int     getPrintEvery()
    {
        return print_every;
    };
    void    setPrintEvery(int num)
    {
        print_every=num;
    };

};
extern Param g_Param;


extern void Normalize(double *a);
extern double DotProduct(double *a, double *b);
extern double GetDistance(double *a, double *b);
extern double GetLength(double *a);
extern double random0to1();
extern double get_random(double min, double max);
extern double Wpoly6(double r_square);
extern void   WspikyGrad(double *r, double r_square, double *gradient);
extern double WviscosityLaplacian(double r_square);
extern void   Wpoly6Grad(double *r, double r_square, double *gradient);
extern double Wpoly6Laplacian(double r_square);







