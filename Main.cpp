#include <iostream>
#include "Parameters.h"

using namespace std;

Param g_Param;

int main()
{

   //random seed to make "random functions" truly random
   srand((unsigned)time(0));
   //object
   SPH sph;

   //output files
   string note_outfile;
   note_outfile=g_Param.getRunName();
   ofstream run_log;

   //display homescreen and set initial parameters
   sph.homeScreen();

   //if booundary spinning was set 1 (on) in the home screen then begin spinning
    if(g_Param.getSpinSpeed()>0){
        g_Param.setSpinOn(1);
    }else g_Param.setSpinOn(0);


   cout<<"    NOTES: "<<g_Param.getNotes()<<endl;
   cout<<"    Number of Water Particles: "<<sph.getWaterCount()<<endl;

   //prints this iformation the output files : data/run.txt    &   data/csv/grid.csv
   run_log<<endl<<"    Number of Water Particles: "<<sph.getWaterCount()<<endl;
   sph.writeGrid();
   sph.printSettings();


   if(g_Param.getParticleBlockOn())
   {sph.initializeParticleBlock();
     cout<<"    Number of Water Particles After Particle Block: "<<sph.getWaterCount()<<endl;
     run_log<<"    Number of Water Particles After Particle Block: "<<sph.getWaterCount()<<endl;
   }

   // initialize the appropriate boundary particle geometry
   if (g_Param.getMillOn()==1)sph.initializeMillBoundary();
   if (g_Param.getConverterOn()==1)sph.initializeConverterBoundary();

   cout<<"    Number of Total Boundary Particles: "<<sph.getBoundaryCount()<<endl<<endl;
   run_log<<"    Number of Boundary Particles: "<<sph.getBoundaryCount()<<endl;

   cout<< "STEP COUNT:"<<endl;
   cout<< "----------- "<<endl;




   //main loop which calculates densities, forces, positions etc.
   for(int i = 0; i < g_Param.getTimesteps(); i++)    {
      g_Param.accumulateStepCount();
      sph.processStep();
      if(g_Param.getStepCount()%g_Param.getPrintEvery()==0||g_Param.getStepCount()==1){ //78 at 0.0005 delta t 26frames/sec
        sph.writeCSV();
        sph.writeVTK();
      }
    }
    run_log<<"Number of Steps:"<< g_Param.getStepCount()<<endl;
    cout<<"DONE!"<<endl;

   // close data/run.txt output file
   run_log.close();
    return 0;
}
