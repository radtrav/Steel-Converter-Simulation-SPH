#include "Parameters.h"
#include <utility>

void SPH::readInData()
{
    char filename [50];
    ifstream fileobject;
    bool file_good=0;
    bool return_home=0;
    char ParticleType='w';
    do
    {
        cout<<"    Particle Data Type? W = Water, B = Boundary, A = Air  Z= Cancel:   ";
        cin>> ParticleType;
        cin.ignore();
        if(!(ParticleType=='w'||ParticleType=='W'||ParticleType=='a'||ParticleType=='A'||ParticleType=='b'||ParticleType=='B'||ParticleType=='z'||ParticleType=='Z')) cout<<"    Invalid Input: Try again."<<endl;

    }
    while(!(ParticleType=='w'||ParticleType=='W'||ParticleType=='a'||ParticleType=='A'||ParticleType=='b'||ParticleType=='B'||ParticleType=='z'||ParticleType=='Z'));


    switch (ParticleType)
    {
    case 'w':
    case 'W':
    {
        cout<<"    Enter Name Of Water File to Read Position From:   ";
        break;
    }
    case 'a':
    case 'A':
    {
        cout<<"    Enter Name Of Air File to Read Position From:   ";
        break;
    }
    case 'b':
    case 'B':
    {
        cout<<"    Enter Name Of Boundary File to Read Position From:   ";
        break;
    }
    case 'z':
    case 'Z':
    {
        cout<<"    No Positional Data Initiated"<<endl;
        file_good=1;
        return_home=1;
        break;
    }
    }

    do
    {
        cin.getline(filename, 50);
        fileobject.open(filename);

        if (!fileobject.is_open())
        {
            cout<<"  No File... Try Again"<<endl;
            //exit(EXIT_FAILURE);
        }
        else(file_good=1);
    }
    while(!file_good);

    string double_string;
    fileobject>>double_string;
    while(fileobject.good())
    {
        if (return_home==0)
        {
            double init_position[3];
            double init_velocity[3]= {0,0,0};

            strtk::parse(double_string, ",", init_position[0], init_position[1], init_position[2]); // note the StrTk toolkit needs to be set up http://www.codeproject.com/Articles/23198/C-String-Toolkit-StrTk-Tokenizer


            int grid_index_x = ((int)floor(init_position[0]/g_Param.getSmoothingRadius()));
            int grid_index_y = ((int)floor(init_position[1]/g_Param.getSmoothingRadius()));
            int grid_index_z = ((int)floor(init_position[2]/g_Param.getSmoothingRadius()));

            switch (ParticleType)
            {
            case 'w':
            case 'W':
            {
                grid[grid_index_x][grid_index_y][grid_index_z].water_particles.push_back( Particle(init_position, init_velocity) );
                break;
            }
            case 'a':
            case 'A':
            {
                grid[grid_index_x][grid_index_y][grid_index_z].air_particles.push_back( Particle(init_position, init_velocity) );
                break;
            }
            case 'b':
            case 'B':
            {
                grid[grid_index_x][grid_index_y][grid_index_z].boundary_particles.push_back( Particle(init_position, init_velocity) );
                break;
            }

            }
            fileobject>>double_string;
        }
    }

}
void SPH::setupParticleBlock()
{

//WATER BLOCK

    viewParticleBlock();

    char change_settings ='x';
    double double_in= 0.0;
    int int_in;
    bool returnHome=0;
    bool bool_val=1;


    do
    {
        cout<<" Select A Key [A=F] to Change a Value "<<endl<<" Enter 'Z' to Return to Home"<<endl<<endl;
        cin>>change_settings;
        switch (change_settings)
        {

        case 'a':
        case 'A':
        {
            cout<<" Please Enter A New Length: ";
            for(;;)
            {
                if(cin>>int_in)
                {
                    g_Param.setBlockLength(int_in);
                    viewParticleBlock();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewParticleBlock();
                    cout<<" Please Enter A New Length: "<<endl;;
                    cout<<" Please enter a valid integer: "<<endl;
                }
            }
            break;
        }
        case 'b':
        case 'B':
        {
            cout<<" Please Enter A New Width: ";
            for(;;)
            {
                if(cin>>int_in)
                {
                    g_Param.setBlockWidth(int_in);
                    viewParticleBlock();
                    break;
                }
                else
                {

                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewParticleBlock();
                    cout<<" Please Enter A New Length: "<<endl;;
                    cout<<" Please enter a valid integer: "<<endl;
                }
            }
            break;
        }
        case 'c':
        case 'C':
        {
            cout<<" Please Enter A New Height: ";
            for(;;)
            {
                if(cin>>int_in)
                {
                    g_Param.setBlockHeight(int_in);
                    viewParticleBlock();
                    break;
                }
                else
                {

                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewParticleBlock();
                    cout<<" Please Enter A New Length: "<<endl;;
                    cout<<" Please enter a valid integer: "<<endl;
                }
            }
            break;
        }
        case 'd':
        case 'D':
        {
            cout<<" Please Enter the New X Location: ";
            for(;;)
            {
                if(cin>>double_in)
                {
                    g_Param.setParticleBlockLocationX(double_in);
                    viewParticleBlock();
                    break;
                }
                else
                {

                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewParticleBlock();
                    cout<<" Please Enter the New X Location: "<<endl;;
                    cout<<" Please enter a valid number: "<<endl;
                }

            }

            cout<<" Please Enter the New Y Location: ";
            for(;;)
            {
                if(cin>>double_in)
                {
                    g_Param.setParticleBlockLocationY(double_in);
                    viewParticleBlock();
                    break;
                }
                else
                {
                    cout<<" Please enter a valid number"<<endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewParticleBlock();
                    cout<<" Please Enter the New Y Location: "<<endl;;
                    cout<<" Please enter a valid number: "<<endl;
                }
            }

            cout<<" Please Enter the New Z Location: ";
            for(;;)
            {
                if(cin>>double_in)
                {
                    g_Param.setParticleBlockLocationZ(double_in);
                    viewParticleBlock();
                    break;
                }
                else
                {

                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewParticleBlock();
                    cout<<" Please Enter the New Z Location: "<<endl;;
                    cout<<" Please enter a valid number: "<<endl;
                }
            }
            break;
        }
        case 'e':
        case 'E':
        {
            cout<<" Please Enter A Block Particle Spacing: ";
            for(;;)
            {
                if(cin>>double_in)
                {
                    g_Param.setParticleBlockSpacing(double_in);
                    viewParticleBlock();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewParticleBlock();
                    cout<<" Please Enter A Block Particle Spacing: "<<endl;
                    cout<<" Please enter a valid number: "<<endl;
                }
            }
            break;
        }
        case 'f':
        case 'F':
        {
            cout<<" Initialize Particle Block? ON=1 OFF=0: ";
            for(;;)
            {
                if(cin>>bool_val)
                {
                    g_Param.setParticleBlockOn(bool_val);
                    viewParticleBlock();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewParticleBlock();
                    cout<<" Initialize Particle Block? ON=1 OFF=0: ";
                    cout<<" Please enter a valid number: "<<endl;
                }
            }
            break;
        }
        case 'z':
        case 'Z':
        {
            returnHome=1;
            break;
        }
        default:
        {
            cout<<" Invalid Input"<<endl;
            viewParticleBlock();
            break;
        }
        }
    }
    while(returnHome==0);
}
void SPH::setupEnvironmentProperties()
{
//WATER BLOCK

    viewEnvironmentProperties();
    char change_settings ='x';
    bool bool_val=1;
    double double_val=0;
    bool returnHome=0;

    do
    {
        cout<<" Select A Key to Change a Value "<<endl<<" Enter 'Z' to Return to Home"<<endl<<endl;
        cin>>change_settings;
        switch (change_settings)
        {
        case 'a':
        case 'A':
        {
            cout<<"Set Number Gravity ON=1 OFF=0 : ";
            for(;;)
            {
                if( cin>>bool_val)
                {
                    g_Param.setGravityIsOn(bool_val);
                    viewEnvironmentProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewEnvironmentProperties();
                    cout<<"Set Number Gravity ON=1 OFF=0 : "<<endl;
                    cout<<"Please Type In a Valid Number";
                }
            }
            break;
        }
        case 'b':
        case 'B':
        {
            cout<<"Set Converter Boundaries ON=1 OFF-0: ";
            for(;;)
            {
                if( cin>>bool_val)
                {
                    g_Param.setConverterOn(bool_val);
                    if(bool_val==1)g_Param.setMillOn(!bool_val);
                    viewEnvironmentProperties();

                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewEnvironmentProperties();
                    cout<<"Set Converter Boundaries ON=1 OFF=0: ";
                    cout<<"Please Type In a Valid Number";
                }
            }
            break;
        }
        case 'c':
        case 'C':
        {
            cout<<"Set Particle Deletion ON=1 OFF=0: ";
            for(;;)
            {
                if( cin>>bool_val)
                {
                    g_Param.setParticleDeletion(bool_val);
                    viewEnvironmentProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewEnvironmentProperties();
                    cout<<"Set Particle Deletion ON=1 OFF-0: ";
                    cout<<"Please Type In a Valid Number";
                }
            }
            break;
        }
        case 'd':
        case 'D':
        {
            cout<<"Set Mill Boundaries ON=1 OFF-0: ";
            for(;;)
            {
                if( cin>>bool_val)
                {
                    g_Param.setMillOn(bool_val);
                    if(bool_val==1)g_Param.setConverterOn(!bool_val);
                    viewEnvironmentProperties();

                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewEnvironmentProperties();
                    cout<<"Set Converter Mill ON=1 OFF=0: ";
                    cout<<"Please Type In a Valid Number";
                }
            }
            break;
        }
        case 'e':
        case 'E':
        {
            cout<<"Set Mill Speed : ";
            for(;;)
            {
                if( cin>>double_val)
                {
                    g_Param.setSpinSpeed(double_val);
                    viewEnvironmentProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewEnvironmentProperties();
                    cout<<"Set Mill Speed: ";
                    cout<<"Please Type In a Valid Number";
                }
            }
            break;
        }
        case 'f':
        case 'F':
        {
            cout<<"Set Fluid System: Steel-O2 =1      Water-Air=0 ";
            for(;;)
            {
                if( cin>>bool_val)
                {
                    g_Param.setSteelO2(bool_val);
                    viewEnvironmentProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewEnvironmentProperties();
                    cout<<"Set Fluid System: Steel-O2 =1      Water-Air=0 ";
                    cout<<"Please Type In a Valid Number";
                }
            }
            break;
        }

        case 'z':
        case 'Z':
        {
            returnHome=1;
            break;
        }
        default:
        {
            cout<<" Invalid Input"<<endl;
            viewParticleProperties();
            break;
        }
        }
    }
    while(returnHome==0);
}

void SPH::setFluidSystem()
{

    if(g_Param.getSteelO2()==1)
    {
        g_Param.setRho0_w(7500);
        g_Param.setRho0_a(10);
        g_Param.setRho0_b(7500);

        g_Param.setWater_mass(0.038);
        g_Param.setAir_mass(0.0038);
        g_Param.setBoundary_mass(0.038);

        g_Param.setSigma_s(12);

        g_Param.setMu_w(28);
        g_Param.setMu_a(1);
        g_Param.setMu_b(28);

        g_Param.setKvalue(12);
    }
    else
    {
        g_Param.setRho0_w(1000);
        g_Param.setRho0_a(10);
        g_Param.setRho0_b(1000);

        g_Param.setWater_mass(0.038);
        g_Param.setAir_mass(0.0038);
        g_Param.setBoundary_mass(0.038);

        g_Param.setSigma_s(12);

        g_Param.setMu_w(5);
        g_Param.setMu_a(1);
        g_Param.setMu_b(5);

        g_Param.setKvalue(12);
    }

}

void SPH::setupPrintProperties()
{
//WATER BLOCK

    viewPrintProperties();
    char change_settings ='x';
    bool bool_val=1;
    int int_val=0;
    bool returnHome=0;

    do
    {

        cout<<" Select A Key to Change a Value "<<endl<<" Enter 'Z' to Return to Home"<<endl<<endl;
        cin>>change_settings;
        switch (change_settings)
        {
        case 'a':
        case 'A':
        {
            cout<<"Print Water CSV ON=1 OFF=0 : ";
            for(;;)
            {
                if( cin>>bool_val)
                {
                    g_Param.setCsvWater(bool_val);
                    viewPrintProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewPrintProperties();
                    cout<<"Print Water CSV ON=1 OFF=0 : ";
                    cout<<"Please Type In a Valid Number";
                }
            }
            break;
        }
        case 'b':
        case 'B':
        {
            cout<<"Print Air CSV ON=1 OFF=0 : ";
            for(;;)
            {
                if( cin>>bool_val)
                {
                    g_Param.setCsvAir(bool_val);
                    viewPrintProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewPrintProperties();
                    cout<<"Print Air CSV ON=1 OFF=0 : ";
                    cout<<"Please Type In a Valid Number";
                }
            }
            break;
        }
        case 'c':
        case 'C':
        {
            cout<<"Print Boundary CSV ON=1 OFF=0 : ";
            for(;;)
            {
                if( cin>>bool_val)
                {
                    g_Param.setCsvBoundary(bool_val);
                    viewPrintProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewPrintProperties();
                    cout<<"Print Boundary CSV ON=1 OFF=0 : ";
                    cout<<"Please Type In a Valid Number";
                }
            }
            break;
        }
        case 'd':
        case 'D':
        {
            cout<<"Print Water VTK ON=1 OFF=0 : ";
            for(;;)
            {
                if( cin>>bool_val)
                {
                    g_Param.setVtkWater(bool_val);
                    viewPrintProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewPrintProperties();
                    cout<<"Print Water VTK ON=1 OFF=0 : ";
                    cout<<"Please Type In a Valid Number";
                }
            }
            break;
        }
        case 'e':
        case 'E':
        {
            cout<<"Print Air VTK ON=1 OFF=0 : ";
            for(;;)
            {
                if( cin>>bool_val)
                {
                    g_Param.setVtkAir(bool_val);
                    viewPrintProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewPrintProperties();
                    cout<<"Print Air VTK ON=1 OFF=0 : ";
                    cout<<"Please Type In a Valid Number";
                }
            }
            break;
        }
        case 'f':
        case 'F':
        {
            cout<<"Print Boundary VTK ON=1 OFF=0 : ";
            for(;;)
            {
                if( cin>>bool_val)
                {
                    g_Param.setVtkBoundary(bool_val);
                    viewPrintProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewPrintProperties();
                    cout<<"Print Boundary VTK ON=1 OFF=0 : ";
                    cout<<"Please Type In a Valid Number";
                }
            }
            break;
        }
        case 'g':
        case 'G':
        {
            cout<<"Print Files Every [] Steps : ";
            for(;;)
            {
                if( cin>>int_val)
                {
                    g_Param.setPrintEvery(int_val);
                    viewPrintProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewPrintProperties();
                    cout<<"Print Files Every [] Steps : ";
                    cout<<"Please Type In a Valid Number";
                }
            }
            break;
        }


        case 'z':
        case 'Z':
        {
            returnHome=1;
            break;
        }
        default:
        {
            cout<<" Invalid Input"<<endl;
            viewPrintProperties();
            break;
        }
        }
    }
    while(returnHome==0);


}

void SPH::setupTimestepProperties()
{

//WATER BLOCK

    viewTimestepProperties();

    char change_settings ='x';
    double double_val= 0.0;
    int int_val= 0;
    bool returnHome=0;

    do
    {
        cout<<" Select A Key to Change a Value "<<endl<<" Enter 'Z' to Return to Home"<<endl<<endl;
        cin>>change_settings;
        switch (change_settings)
        {
        case 'a':
        case 'A':
        {
            cout<<"Set Number of Timesteps. Default: 50000 Current: "<<g_Param.getTimesteps()<<endl;
            for(;;)
            {
                if( cin>>int_val)
                {
                    g_Param.setTimesteps(int_val);
                    viewTimestepProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewTimestepProperties();
                    cout<<"Set Number of Timesteps. Default: 50000 Current: "<<g_Param.getTimesteps()<<endl;
                    cout<<"Please Type In a Valid Number"<<endl;
                }
            }
            break;
        }
        case 'b':
        case 'B':
        {
            cout<<"Set Size of Timesteps. Default: 0.001 Current: "<<g_Param.getDelta_t()<<endl;
            for(;;)
            {
                if( cin>>double_val)
                {
                    g_Param.setDelta_t(double_val);
                    viewTimestepProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewTimestepProperties();
                    cout<<"Set Size of Timesteps. Default: 0.001 Current: "<<g_Param.getDelta_t()<<endl;
                    cout<<"Please Type In a Valid Number"<<endl;
                }
            }
            break;
        }
        case 'z':
        case 'Z':
        {
            returnHome=1;
            break;
        }
        default:
        {
            cout<<" Invalid Input"<<endl;
            viewParticleProperties();
            break;
        }
        }
    }
    while(returnHome==0);
}
void SPH::setupParticleProperties()
{

//WATER BLOCK

    viewParticleProperties();
    char change_settings ='z';
    double double_val= 0.0;
    bool returnHome=0;
    do
    {
        cout<<" Select A Key to Change a Value "<<endl<<" Enter 'Z' to Return to Home"<<endl<<endl;
        cin>>change_settings;
        switch (change_settings)
        {
        case 'a':
        case 'A':
        {
            cout<<" Set Water Rest Density. Default: 1000 Current: "<<g_Param.getRho0_w()<<endl;
            for(;;)
            {
                if(cin>>double_val)
                {
                    g_Param.setRho0_w(double_val);
                    viewParticleProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewParticleProperties();
                    cout<<" Set Water Rest Density. Default: 1000 Current: "<<g_Param.getRho0_w()<<endl;
                    cout<<" Please Type In a Valid Number"<<endl;
                }
            }
            break;
        }
        case 'b':
        case 'B':
        {
            cout<<" Set Air Rest Density. Default: 10  Current: "<<g_Param.getRho0_a()<<endl;
            for(;;)
            {
                if(cin>>double_val)
                {
                    g_Param.setRho0_a(double_val);
                    viewParticleProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewParticleProperties();
                    cout<<" Set Air Rest Density. Default: 1000 Current: "<<g_Param.getRho0_a()<<endl;
                    cout<<" Please Type In a Valid Number"<<endl;
                }
            }
            break;
        }
        case 'c':
        case 'C':
        {
            cout<<" Set Boundary Rest Density. Default: 1000  Current: "<<g_Param.getRho0_b()<<endl;
            for(;;)
            {
                if(cin>>double_val)
                {
                    g_Param.setRho0_b(double_val);
                    viewParticleProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewParticleProperties();
                    cout<<" Set Boundary Rest Density. Default: 1000 Current: "<<g_Param.getRho0_b()<<endl;
                    cout<<" Please Type In a Valid Number"<<endl;
                }
            }
            break;
        }
        case 'd':
        case 'D':
        {
            cout<<"Set Water Particle Mass. Default: 0.048 Current: "<<g_Param.getWater_mass()<<endl;
            for(;;)
            {
                if( cin>>double_val)
                {
                    g_Param.setWater_mass(double_val);
                    viewParticleProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewParticleProperties();
                    cout<<"Set Water Particle Mass. Default: 0.048 Current: "<<g_Param.getWater_mass()<<endl;
                    cout<<"Please Type In a Valid Number"<<endl;
                }
            }
            break;
        }
        case 'e':
        case 'E':
        {
            cout<<"Set Air Particle Mass. Default: 0.0027 Current: "<<g_Param.getAir_mass()<<endl;
            for(;;)
            {
                if( cin>>double_val)
                {
                    g_Param.setAir_mass(double_val);
                    viewParticleProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewParticleProperties();
                    cout<<"Set Air Particle Mass. Default: 0.0048 Current: "<<g_Param.getAir_mass()<<endl;
                    cout<<"Please Type In a Valid Number"<<endl;
                }
            }
            break;
        }
        case 'f':
        case 'F':
        {

            cout<<"Set Boundary Particle Mass. Default: 0.048 Current: "<<g_Param.getBoundary_mass()<<endl;
            for(;;)
            {
                if( cin>>double_val)
                {
                    g_Param.setBoundary_mass(double_val);
                    viewParticleProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewParticleProperties();
                    cout<<"Set Boundary Particle Mass. Default: 0.048 Current: "<<g_Param.getBoundary_mass()<<endl;
                    cout<<"Please Type In a Valid Number"<<endl;
                }
            }
            break;
        }
        case 'g':
        case 'G':
        {
            cout<<"Set Water Viscocity. Default: 20 Current: "<<g_Param.getMu_w()<<endl;
            for(;;)
            {
                if( cin>>double_val)
                {
                    g_Param.setMu_w(double_val);
                    viewParticleProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewParticleProperties();
                    cout<<"Set Water Viscocity. Default: 20 Current: "<<g_Param.getMu_w()<<endl;
                    cout<<"Please Type In a Valid Number"<<endl;
                }
            }
            break;
        }
        case 'h':
        case 'H':
        {
            cout<<"Set Air Viscocity. Default: 1 Current: "<<g_Param.getMu_a()<<endl;
            for(;;)
            {
                if( cin>>double_val)
                {
                    g_Param.setMu_a(double_val);
                    viewParticleProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewParticleProperties();
                    cout<<"Set Air Viscocity. Default: 1 Current: "<<g_Param.getMu_a()<<endl;
                    cout<<"Please Type In a Valid Number"<<endl;
                }
            }
            break;
        }
        case 'i':
        case 'I':
        {
            cout<<"Set Boundary Viscocity. Default: 20 Current: "<<g_Param.getMu_b()<<endl;
            for(;;)
            {
                if( cin>>double_val)
                {
                    g_Param.setMu_b(double_val);
                    viewParticleProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewParticleProperties();
                    cout<<"Set Boundary Viscocity. Default: 1 Current: "<<g_Param.getMu_b()<<endl;
                    cout<<"Please Type In a Valid Number"<<endl;
                }
            }
            break;
        }
        case 'j':
        case 'J':
        {
            cout<<"Set Gas Stiffness. Current: "<<g_Param.getKvalue()<<endl;
            for(;;)
            {
                if( cin>>double_val)
                {
                    g_Param.setKvalue(double_val);
                    viewParticleProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewParticleProperties();
                    cout<<"Set Gas Stiffness. Current: "<<g_Param.getKvalue()<<endl;
                    cout<<"Please Type In a Valid Number"<<endl;
                }
            }
            break;
        }
        case 'k':
        case 'K':
        {
            cout<<"Set Smoothing Radius. Default: 0.05 Current: "<<g_Param.getSmoothingRadius()<<endl;
            for(;;)
            {
                if( cin>>double_val)
                {
                    g_Param.setSmoothingRadius(double_val);
                    viewParticleProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewParticleProperties();
                    cout<<"Set Smoothing Radius. Default: 0.05 Current: "<<g_Param.getSmoothingRadius()<<endl;
                    cout<<"Please Type In a Valid Number"<<endl;
                }
            }
            break;
        }
        case 'l':
        case 'L':
        {
            cout<<"Set Hertzian Spring Stiffness: Current "<<g_Param.getHertzian()<<endl;
            for(;;)
            {
                if( cin>>double_val)
                {
                    g_Param.setHertzian(double_val);
                    viewParticleProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewParticleProperties();
                    cout<<"Set Hertzian Spring Stiffness: Current "<<g_Param.getHertzian()<<endl;
                    cout<<"Please Type In a Valid Number"<<endl;
                }
            }
            break;
        }
        case 'm':
        case 'M':
        {
            cout<<"Set Hertzian Spring Damping: Current "<<g_Param.getDamping()<<endl;
            for(;;)
            {
                if( cin>>double_val)
                {
                    g_Param.setDamping(double_val);
                    viewParticleProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewParticleProperties();
                    cout<<"Set Hertzian Spring Damping: Current "<<g_Param.getDamping()<<endl;
                    cout<<"Please Type In a Valid Number"<<endl;
                }
            }
            break;
        }
        case 'n':
        case 'N':
        {
            cout<<"Set Hertzian Tangential Friction: Current "<<g_Param.getTangentialFriction()<<endl;
            for(;;)
            {
                if( cin>>double_val)
                {
                    g_Param.setTangentialFriction(double_val);
                    viewParticleProperties();
                    break;
                }
                else
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    viewParticleProperties();
                    cout<<"Set Hertzian Tangential Friction: Current "<<g_Param.getTangentialFriction()<<endl;
                    cout<<"Please Type In a Valid Number"<<endl;
                }
            }
            break;
        }

        case 'z':
        case 'Z':
        {
            returnHome=1;
            break;
        }
        default:
        {
            cout<<" Invalid Input"<<endl;
            viewParticleProperties();
            break;
        }
        }
    }
    while(returnHome==0);
}
void SPH::initializeParticleBlock()
{
    cout<<"Initializing Particle Block"<<endl;
    double H = g_Param.getSmoothingRadius();
    double spacing = g_Param.getParticleBlockSpacing();

    for(int x = 0; x <g_Param.getBlockLength(); x++)
    {
        for(int y = 0; y <g_Param.getBlockHeight(); y++)
        {
            for(int z = 0; z < g_Param.getBlockWidth(); z++)
            {
                double init_position[3] = {x*(spacing*H) + (g_Param.getParticleBlockLocationX()*XGRID*H),
                                           y*(spacing*H) + (g_Param.getParticleBlockLocationY()*YGRID*H),
                                           z*(spacing*H) + (g_Param.getParticleBlockLocationZ()*ZGRID*H)
                                          };//0.72
                double init_velocity[3] = {        0,      0.0,        0, };//get_random(-0.2,0.2)}


                int grid_index_x = ((int)floor(init_position[0]/H));
                int grid_index_y = ((int)floor(init_position[1]/H));
                int grid_index_z = ((int)floor(init_position[2]/H));

                grid[grid_index_x][grid_index_y][grid_index_z].water_particles.push_back( Particle(init_position, init_velocity) );
            }
        }
    }

}
void SPH::initializeLance()
{
    ofstream lance_boundaries;
    lance_boundaries.open("data/csv/lance.csv");
    double center_of_ring[3] = {0.5*XGRID*g_Param.getSmoothingRadius(),0.4*YGRID*g_Param.getSmoothingRadius() ,0.5*ZGRID*g_Param.getSmoothingRadius()}; // where the air rings are created

//cylinder
    double h_lance = 0.25;//remember that it is better to alter the value of YGRID than h_cyl to maximize computational efficiency. we can probably make the height small than 0.95 to to speed up the system especially if we have particle deletion. however we make want air particles to physically leave the converter before
    double radius_outer= 0.05;
    double radius_inner= (radius_outer)-(0.5*BC_SPACING);
    double circumferance_outer= TwoPI*radius_outer;
    double n_points_in_circle_outer= circumferance_outer/BC_SPACING;// inner and outer have same amount of points
    double theta = 0;

    // radius= 0.92592*z+radius_initial        radius_intial = radius_final/1.758  radius initial refers to to the small open of the cone at z=0. radius final is the final open of the cone where it connects with the cylinder.

    for(int height_step=1; height_step<(h_lance/BC_SPACING); height_step++) // height_step means on which level of y we are placing the circle, bp must be a specific spacing distance away. inital height step - 0.39/BC_Spacing- 1*H---> this is the last height on the cone
    {
        for(int n=0; n<n_points_in_circle_outer; n++)
        {
            theta     = n*(TwoPI/n_points_in_circle_outer); // we want theta to go from 0 to 2PI within n_points_in_circle times. for each point of the circle that is created theta will adjust accordingly.
            double x  = (radius_outer *cos(theta))+0.5*XGRID*g_Param.getSmoothingRadius() + (0*g_Param.getSmoothingRadius()); //the +(x*H) is the translation factor
            double y  = (height_step *BC_SPACING) + (center_of_ring[1]-2*g_Param.getSmoothingRadius());
            double z  = (radius_outer *sin( theta ))+0.5*ZGRID*g_Param.getSmoothingRadius() + (0*g_Param.getSmoothingRadius());

            double x2  = (radius_inner *cos(1.5*theta))+0.5*XGRID*g_Param.getSmoothingRadius() + (0*g_Param.getSmoothingRadius());
            double y2  = (height_step *BC_SPACING)+(0.5*BC_SPACING)+ (center_of_ring[1]-2*g_Param.getSmoothingRadius());
            double z2  = (radius_inner *sin(1.5*theta ))+0.5*ZGRID*g_Param.getSmoothingRadius() + (0*g_Param.getSmoothingRadius());

            double bound_position_outer[3] = {     x,     y,     z };
            double bound_velocity_outer[3] = {     0,     0,     0 };

            double bound_position_inner[3] = {     x2,    y2,    z2};
            double bound_velocity_inner[3] = {     0,     0,     0 };

            int grid_index_x_outer=((int)floor(bound_position_outer[0]/g_Param.getSmoothingRadius()));
            int grid_index_y_outer=((int)floor(bound_position_outer[1]/g_Param.getSmoothingRadius()));
            int grid_index_z_outer=((int)floor(bound_position_outer[2]/g_Param.getSmoothingRadius()));

            int grid_index_x_inner=((int)floor(bound_position_inner[0]/g_Param.getSmoothingRadius()));
            int grid_index_y_inner=((int)floor(bound_position_inner[1]/g_Param.getSmoothingRadius()));
            int grid_index_z_inner=((int)floor(bound_position_inner[2]/g_Param.getSmoothingRadius()));

            lance_boundaries<<bound_position_outer[0]<<","<<bound_position_outer[1]<<","<<bound_position_outer[2]<<","<<"["<<grid_index_x_outer<<"]"<<"["<<grid_index_y_outer<<"]"<<"["<<grid_index_z_outer<<"]"<<","<<endl; //print out to csv file
            lance_boundaries<<bound_position_inner[0]<<","<<bound_position_inner[1]<<","<<bound_position_inner[2]<<","<<"["<<grid_index_x_inner<<"]"<<"["<<grid_index_y_inner<<"]"<<"["<<grid_index_z_inner<<"]"<<","<<endl; //print out to csv file

            grid[grid_index_x_outer][grid_index_y_outer][grid_index_z_outer].boundary_particles.push_back( Particle(bound_position_outer, bound_velocity_outer) );
            grid[grid_index_x_inner][grid_index_y_inner][grid_index_z_inner].boundary_particles.push_back( Particle(bound_position_inner, bound_velocity_inner) );
        }
    }
    lance_boundaries.close();
}

void SPH::initializeStirGas()
{
    double H=g_Param.getSmoothingRadius();
    //--------------------------------------------------------------TYPE 1--------------------------------------------------------------------------------------
    if(g_Param.getStirringGasOn()==1)
    {

        if (g_Param.getStirType()==1)       //TYPE1 = Bubbles are seed randomly within a circular region
        {
            double center_of_bottom_ring[3]= {(0.5*XGRID*g_Param.getSmoothingRadius()),4*g_Param.getSmoothingRadius(),0.5*ZGRID*g_Param.getSmoothingRadius()};
            double radius2=0.006;

            for(int cycles =0; cycles<10; cycles++)
            {
                for(int n=0; n< 10; n++)
                {
                    double theta     = (double)n*(TwoPI/10);
                    double x  = (get_random(0.01,1)*radius2*cos(theta)) + (center_of_bottom_ring[0]);// get_random(0.01,1)* makes it so that that the particles can be seeded at any part of the circle
                    double y  = center_of_bottom_ring[1];
                    double z  = (get_random(0.01,1)*radius2 *sin( theta ))+ (center_of_bottom_ring[2]);

                    double init_position[3] = { x, y, z  };
                    double init_velocity[3] = { get_random(-0.02,0.02), get_random(8,9), get_random(-0.02,0.02) };

                    int grid_index_x = ((int)floor(x/H));
                    int grid_index_y = ((int)floor(y/H));
                    int grid_index_z = ((int)floor(z/H));

                    grid[grid_index_x][grid_index_y][grid_index_z].air_particles.push_back( Particle(init_position, init_velocity) );
                }
            }
        }
//-----------------------------------------------------------------------TYPE 2-------------------------------------------------------------------------------------------------

        if(g_Param.getStirType()==2)  //TYPE2= Bubbles are seeded from a set star shape.
        {

            double center_of_ring[3] = {g_Param.getStirPosX()*XGRID*H, g_Param.getStirPosY()*YGRID*H , g_Param.getStirPosZ()*ZGRID*H}; // where the air rings are created
            double radius = g_Param.getStirOpeningRadius(); //1//H
            //double circumferance = TwoPI*radius;
            //int n_points = (int)(floor(circumferance/(1.0*H))); //H/5 6 points
            double theta = 0;

            //double d_between_rings = H;//1/((double)number_of_rings);
            double Vx_min= g_Param.getStirOutputVelocityXMin(), Vx_max = g_Param.getStirOutputVelocityXMax();
            double Vy_min= g_Param.getStirOutputVelocityYMin(), Vy_max = g_Param.getStirOutputVelocityYMax();
            double Vz_min= g_Param.getStirOutputVelocityZMin(), Vz_max = g_Param.getStirOutputVelocityZMax();

            int  n_nozzles = 6;//6 //number of nozzles in a single ring
            int number_of_rings/*+1*/ = 3;
            double d_between_rings= 5*H;
            for (int rings=0; rings< number_of_rings; rings++)
            {
                for(int n=0; n< n_nozzles; n++)
                {
                    theta     = n*(TwoPI/n_nozzles);// 6 is the nuber of nozzles.
                    double x  = (radius*cos(theta))*(rings*d_between_rings) + (center_of_ring[0]);
                    double y  = center_of_ring[1];
                    double z  = (radius *sin( theta ))*(rings*d_between_rings)+ (center_of_ring[2]);

                    double nozzle_position[3] = { x, y, z  };
                    double throat_radius =.006;// from komagata report

                    //double nozzle_circumferance = TwoPI*throat_radius;
                    //double d_between_rings= thr9oat_radius/number_of_rings;
                    int n_seeds = 10;//10// (int)(floor(nozzle_circumferance/(0.8*H))); //H/5 6 points
                    double theta2 = 0;

                    int number_of_rings = 6;
                    for (int rings=0; rings< number_of_rings; rings++)
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

                            grid[grid_index_x][grid_index_y][grid_index_z].air_particles.push_back( Particle(init_position, init_velocity) );
                        }
                    }
                }
            }
        }
        if(g_Param.getStirType()==3)  //TYPE2= Bubbles are seeded from a set star shape.
        {

            double center_of_ring[3] = {g_Param.getStirPosX()*XGRID*H, g_Param.getStirPosY()*YGRID*H , g_Param.getStirPosZ()*ZGRID*H}; // where the air rings are created
            double radius = g_Param.getStirOpeningRadius(); //1//H
            //double circumferance = TwoPI*radius;
            //int n_points = (int)(floor(circumferance/(1.0*H))); //H/5 6 points
            double theta = 0;

            //double d_between_rings = H;//1/((double)number_of_rings);
            double Vx_min= g_Param.getStirOutputVelocityXMin(), Vx_max = g_Param.getStirOutputVelocityXMax();
            double Vy_min= g_Param.getStirOutputVelocityYMin(), Vy_max = g_Param.getStirOutputVelocityYMax();
            double Vz_min= g_Param.getStirOutputVelocityZMin(), Vz_max = g_Param.getStirOutputVelocityZMax();

            int  n_nozzles = 10;//number of nozzles in a single ring
            int number_of_rings/*+1*/ = 5;
            double d_between_rings= 5*H;
            for (int rings=0; rings< number_of_rings; rings++)
            {
                for(int n=0; n< n_nozzles; n++)
                {
                    theta     = n*(TwoPI/n_nozzles);// 6 is the nuber of nozzles.
                    double x  = (radius*cos(theta))*(rings*d_between_rings) + (center_of_ring[0]);
                    double y  = center_of_ring[1];
                    double z  = (radius *sin( theta ))*(rings*d_between_rings)+ (center_of_ring[2]);

                    double nozzle_position[3] = { x, y, z  };
                    double radius2 = 0.003;
                    for(int cycles = 0; cycles<4; cycles++)  // the other number of particles = (1+cycles)* n_final
                    {
                        int n_final = 2;
                        for(int n = 1 ; n < n_final; n++)
                        {
                            double theta     = get_random(0.0,100.0)*((double)n*(TwoPI/(double)n_final));
                            double x  = (get_random(0,1)*radius2*cos(theta)) + (nozzle_position[0]);// get_random(0.01,1)* makes it so that that the particles can be seeded at any part of the circle
                            double y  = nozzle_position[1];
                            double z  = (get_random(0,1)*radius2 *sin(theta))+ (nozzle_position[2]);

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
        }
    }
}

