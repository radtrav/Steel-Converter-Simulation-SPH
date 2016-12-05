#include "Parameters.h"
#include <utility>

char SPH::homeScreen()
{
    ofstream run_log;
    run_log.open("data/run/run.txt",ios::app);
    string notes = "no notes";
    //char info_out[100];
    char click;
    //double double_val=0;
    // int int_val;
    string name_string;

    viewSettings();
    bool start_program=0;
    do
    {
        cout<<"    [1] Run Simulation            [ctrl+c] Exit Program"<< endl;
        cout<<"-------------------------------------------------------------------------------"<<endl;
        cout<<"    ==> CHOOSE AN OPTION          [LETTER+ENTER]   ";
        cin>>click;
        cin.ignore();

        switch (click)
        {
        case '1':
        {
            viewSettings();
            cout<<"    Beginning Simulation...Click [Enter] Twice"<<endl;
            start_program=1;
            break;
        }
        case 'g':
        case 'G':
        {
            //cin.ignore(1000, '\n') ;
            readInData();
            viewSettings();
            break;
        }
        case 'a':
        case 'A':
        {
            cout<<" Type in a Run Name With No Spaces:   ";
            for(;;)
            {
                if(cin>>name_string)
                {
                    g_Param.SetRunName(name_string);
                    viewSettings();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewSettings();
                    cout<<" Type in a Run Name With No Spaces:   ";
                    cout<<" Please Type In a Valid Input:   ";
                }
            }
            break;
        }
        case 'c':
        case 'C':
        {
            setupParticleProperties();
            viewSettings();
            break;
        }
        case 'f':
        case 'F':
        {
            setupTimestepProperties();
            viewSettings();
            break;
        }
        case 'd':
        case 'D':
        {

            setupParticleBlock();
            viewSettings();
            break;
        }
        case 'e':
        case 'E':
        {
            viewLanceProperties();
            spawnParticles(g_Param.getStepCount());
            viewSettings();
            break;
        }
        case 'h':
        case 'H':
        {
            setupEnvironmentProperties();
            viewSettings();
            break;
        }
        case 'i':
        case 'I':
        {
            setupPrintProperties();
            viewSettings();
            break;
        }
        case 'b':
        case 'B':
        {
            noteScreen();
            char which;
            bool return_home =0;

            do
            {
                cout<<"    Click A and Then [ENTER] to Enter Notes:  ";
                cin>> which;
                switch (which)
                {

                case 'z':
                case 'Z':
                {
                    return_home=1;
                    break;
                }
                default:
                {
                    getchar();
                    cout<<"    Go Ahead and Enter Your Notes"<<endl;
                    getline(cin, notes);
                    g_Param.setNotes(notes);
                    g_Param.setIsNotes(1);
                    noteScreen();
                    break;
                }
                }
            }
            while(!return_home);
            viewSettings();
            break;
        }
        default:
        {
            viewSettings();
            break;
        }
        }//end of switch
    }
    while(!start_program==1);

    cin.ignore(1000, '\n') ;
    cout<<endl;
    run_log.close();
    return 0;
}
void SPH::viewSettings()
{


    printf("\033[2J\033[1;1H");
    cout<<"-------------------------------------------------------------------------------"<<endl
        <<"                                  JFE-DEMSPH"<<endl
        <<"-------------------------------------------------------------------------------"<<endl
        <<"    [A] Run Name:                 "<<g_Param.getRunName()<<endl<<endl

        <<"    [B] Notes                     [F] Time Step Settings     "<<endl
        <<"    [C] Particle Settings         [G] Read In Positions  "<<endl
        <<"    [D] Particle Block Settings   [H] Environment Settings     "<<endl
        <<"    [E] Lance Settings            [I] Print Settings   "<<endl;

    cout  <<endl
          <<"    Quick Settings"<<endl
          <<"    --------------"<<endl
          <<"    [I] Print Every:              "<<"("<<g_Param.getPrintEvery()<<")"<<endl;

    cout <<"    [E] Lance:                    " ;
    if(g_Param.getLanceOn() ==1) cout<<  "(ON)"<<endl;
    if(g_Param.getLanceOn() ==0) cout<< "(OFF)"<<endl;
    cout <<"    [D] Particle Block:           ";
    if(g_Param.getParticleBlockOn() ==1) cout<< "(ON)"<<endl;
    if(g_Param.getParticleBlockOn() ==0) cout<< "(OFF)"<<endl;
    cout <<"    [H] Gravity:                  ";
    if(g_Param.getGravityIsOn() ==1) cout<< "(ON)"<<endl;
    if(g_Param.getGravityIsOn() ==0) cout<< "(OFF)"<<endl;
    cout <<"    [H] Converter:                ";
    if(g_Param.getConverterOn() ==1) cout<< "(ON)"<<endl;
    if(g_Param.getConverterOn() ==0) cout<< "(OFF)"<<endl;
    cout <<"    [H] Mill:                     ";
    if(g_Param.getMillOn() ==1) cout<< "(ON)"<<endl;
    if(g_Param.getMillOn() ==0) cout<< "(OFF)"<<endl;
    cout <<"    [G] Initialized Fluid:        "<<"("<<getWaterCount()<<")"<<endl;
    cout<<"-------------------------------------------------------------------------------"<<endl;
}
void SPH::viewParticleProperties()
{
    printf("\033[2J\033[1;1H");
    cout<<"-------------------------------------------------------------------------------"<<endl
        <<"    [Z] <--Home            PARTICLE PROPERTIES"<<endl
        <<"-------------------------------------------------------------------------------"<<endl
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

        <<"-------------------------------------------------------------------------------"<<endl;
}


void SPH::viewEnvironmentProperties()
{
    printf("\033[2J\033[1;1H");
    cout<<"-------------------------------------------------------------------------------"<<endl
        <<"    [Z] <--Home           ENVIRONMENT PROPERTIES"<<endl
        <<"-------------------------------------------------------------------------------"<<endl
        <<"    [A] Gravity:            "<<g_Param.getGravityIsOn() <<endl
        <<"    [B] Converter Boundary  "<<g_Param.getConverterOn() <<endl
        <<"    [C] Particle Deletion:  "<<g_Param.getParticleDeletion() <<endl
        <<"    [D] Mill Boundary       "<<g_Param.getMillOn() <<endl
        <<"    [E] Mill Speed          "<<g_Param.getSpinSpeed()<<endl
        <<"    [F] System              ";
        if (g_Param.getSteelO2()==1){
          cout <<"Steel/O2"<<endl;
        }else {
            cout <<"Water/Air"<<endl;
        };


        cout<<"  "<<endl
        <<"   "<<endl
        <<"    "<<endl

        <<"   "<<endl
        <<"   "<<endl<<endl<<endl<<endl<<endl
        <<"-------------------------------------------------------------------------------"<<endl;
}

void SPH::viewPrintProperties()
{
    printf("\033[2J\033[1;1H");
    cout<<"-------------------------------------------------------------------------------"<<endl
        <<"    [Z] <--Home           PRINT PROPERTIES"<<endl
        <<"-------------------------------------------------------------------------------"<<endl
        <<"    [A] Print Water CSV:          "<<g_Param.getCsvWater()<<endl
        <<"    [B] Print Air CSV:            "<<g_Param.getCsvAir()<<endl
        <<"    [C] Print Boundary CSV:       "<<g_Param.getCsvBoundary()<<endl
        <<"    [D] Print Water VTK:          "<<g_Param.getVtkWater()<<endl
        <<"    [E] Print Air VTK:            "<<g_Param.getVtkAir()<<endl
        <<"    [F] Print Boundary VTK:       "<<g_Param.getVtkBoundary()<<endl
        <<"    [G] Print Every __ Steps:     "<<g_Param.getPrintEvery()<<endl


        <<"    "<<endl
        <<"    "<<endl
        <<"    "<<endl
        <<"    "<<endl
        <<"    "<<endl
        <<"    "<<endl
        <<"    "<<endl
        <<"    "<<endl
        <<"-------------------------------------------------------------------------------"<<endl;
}
void SPH::viewTimestepProperties()
{
    printf("\033[2J\033[1;1H");
    cout<<"-------------------------------------------------------------------------------"<<endl
        <<"    [Z] <--Home            TIMESTEP SETTINGS"<<endl
        <<"-------------------------------------------------------------------------------"<<endl
        <<"    [A] Number of Timesteps: "<<g_Param.getTimesteps()<<endl
        <<"    [B] Size of Timesteps:   "<< g_Param.getDelta_t()<<endl
        <<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl
        <<"-------------------------------------------------------------------------------"<<endl;
}
void SPH::viewParticleBlock()
{

    printf("\033[2J\033[1;1H");
    cout<<"-------------------------------------------------------------------------------"<<endl
        <<"    [Z] <--Home      INITIALIZE PARTICLE BLOCK SETTINGS"<<endl
        <<"-------------------------------------------------------------------------------"<<endl

        <<"    [A] Length: "<<"("<<g_Param.getBlockLength()<<")"<<"x(H)"<<endl
        <<"    [B] Width: "<<"("<<g_Param.getBlockWidth()<<")"<<"x(H)"<<endl
        <<"    [C] Height: "<<"("<<g_Param.getBlockHeight()<<")"<<"x(H)"<<endl
        <<"    [D] Location [RELATIVE TO GRID SIZE]: "<<"("<<g_Param.getParticleBlockLocationX()<<","<<g_Param.getParticleBlockLocationY()<<","<<g_Param.getParticleBlockLocationZ()<<")"<<"x(Grid Dimensions)"<<endl
        <<"    [E] SPACING "<<g_Param.getParticleBlockSpacing()<<endl
        <<"    [F] Initialize Block = "<<g_Param.getParticleBlockOn() <<endl<<endl
        <<"    Number of Particles In Block = "<<g_Param.getBlockLength()*g_Param.getBlockHeight()*g_Param.getBlockWidth()<<endl
        <<"    Smoothing Length H: "<<g_Param.getSmoothingRadius()<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl
        <<"-------------------------------------------------------------------------------"<<endl;

}
void SPH::viewLanceProperties()
{
    printf("\033[2J\033[1;1H");
    cout<<"-------------------------------------------------------------------------------"<<endl
        <<"    [Z] <--Home                    LANCE SETTINGS"<<endl
        <<"-------------------------------------------------------------------------------"<<endl
        <<"    [A] Lance On:            "<<g_Param.getLanceOn()<<""<<endl
        <<"    [B] Lance Position:      "<<"["<<g_Param.getLancePosX()<<","<<g_Param.getLancePosY()<<","<<g_Param.getLancePosZ()<<"] x (CONVERTER DIMENSIONS)"<<endl
        <<"    [C] Flow Rate            "<<"Air Outputted Every "<<g_Param.getRateOfAir()<<" Steps"<<endl
        <<"    [D] Radius of Flow:      "<<g_Param.getLanceOpeningRadius() <<endl
        <<"    [E] Velocity of Flow:    "<<"X: ["<<g_Param.getLanceOutputVelocityXMin()<<","<<g_Param.getLanceOutputVelocityXMax()<<"]  Y: ["<<g_Param.getLanceOutputVelocityYMin()<<","<<g_Param.getLanceOutputVelocityYMax()<<"]  Z: ["<<g_Param.getLanceOutputVelocityZMin()<<","<<g_Param.getLanceOutputVelocityZMax()<<"]"<<endl
        <<"    [N] Water per Step:      "<< g_Param.getWater_per_step()<<endl
        <<"    [O] Air per Step:        "<<g_Param.getAir_per_step()<<endl
        <<"    [P] Lance Type:          "<<g_Param.getLanceType()<<endl
        <<"    [Q] Number Of Air Rings: "<<g_Param.getNumberOfRings()<<endl
        <<"    [R] Number Of Air Seeds: "<<g_Param.getNumberofSeeds()<<endl
        <<"                           "<<endl
        <<"     "<<endl
        <<"     "<<endl
        <<"  "<<endl
        <<"   "<<endl<<endl
        <<"-------------------------------------------------------------------------------"<<endl;
}
void SPH::noteScreen()
{


    printf("\033[2J\033[1;1H");
    cout<<"-------------------------------------------------------------------------------"<<endl
        <<"    [Z] <--Home                NOTES"<<endl
        <<"-------------------------------------------------------------------------------"<<endl
        <<"    NOTES:  "<<g_Param.getNotes()

        <<"    "<<endl
        <<"-------------------------------------------------------------------------------"<<endl;

}

