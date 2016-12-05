#include "Parameters.h"
#include <utility>



/*
                        Particle *neighbour_pt = &(grid[id_x+x][id_y+y][id_z+z].air_particles[i]);
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
                            double shear_stiff = (2/5)*hertzian_stiff;

                            double d1[3]={ pt->velocity[0]*deltime,pt->velocity[1]*deltime,pt->velocity[2]*deltime };
                            double d2[3]={ neighbour_pt->velocity[0]*deltime,neighbour_pt->velocity[1]*deltime,neighbour_pt->velocity[2]*deltime };
                            double dFF[3]={shear_stiff*(d1[0]-d2[0]),shear_stiff*(d1[1]-d2[1]),shear_stiff*(d1[2]-d2[2])};
                            //compute tangental part by subtracting normal component

                            dFF[0] -= ((dFF[0]*distance)/unit_vector[0])*distance;
                            dFF[1] -= ((dFF[1]*distance)/unit_vector[1])*distance;
                            dFF[2] -= ((dFF[2]*distance)/unit_vector[2])*distance;

                            double mu_fric = 0.5;
                            double Fric_mag = sqrt( dFF[0]*dFF[0] + dFF[1]*dFF[1] + dFF[2]*dFF[2]);

                          //  if (Fric_mag>f_contact_magnitude* mu_fric ){
                                f_friction[0] += dFF[0] * (f_contact_magnitude* mu_fric) /(Fric_mag);
                                f_friction[1] += dFF[1] * (f_contact_magnitude* mu_fric) /(Fric_mag);
                                f_friction[2] += dFF[2] * (f_contact_magnitude* mu_fric) /(Fric_mag);
                            //}else{
                                f_friction[0] += 0;
                                f_friction[1] += 0;
                                f_friction[2] += 0;
                           // }

                            cout<<"f_contact = "<<f_contact[0]<<","<<f_contact[1]<<","<<f_contact[2]<<endl;
                            cout<<"f_damping = "<<f_damping[0]<<","<<f_damping[1]<<","<<f_damping[2]<<endl;
                            cout<<"f_friction= "<<f_friction[0]<<","<<f_friction[1]<<","<<f_friction[2]<<endl;
                        }
*/

