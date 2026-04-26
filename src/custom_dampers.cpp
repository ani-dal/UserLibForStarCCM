#include "uclib.h"
#include <math.h>


static inline double clamp(double x, double lo, double hi)
{
    if (x < lo ) return lo;
    if (x > hi ) return hi;
    return x;
}

void USERFUNCTION_EXPORT f2_akn(CoordReal *result, int size, CoordReal *eps, CoordReal *k, CoordReal *y, CoordReal *mu, CoordReal *rho)
{
    for (int i = 0; i < size; i++)
    {
        //inputs per cell
        double eps_i = (eps[i] > 1e-30) ? eps[i] : 1e-30; //turbulent dissipation rate [m^2/s^3]
        double k_i = (k[i] > 1e-30) ? k[i] : 1e-30; //turbulent kinetic energy [m^2/s^2]
        double y_i = (y[i] > 1e-30) ? y[i] : 1e-30; //wall distance [m]
        double mu_i = (mu[i] > 1e-30) ? mu[i] : 1e-30; //dynamic viscosity [kg/{m.s}]
        double rho_i= (rho[i] > 1e-30) ? rho[i] : 1e-30; // density [kg/m^3]
        //to avoid null points switch to 1e-30
        
        double nu = mu_i / rho_i;
        double u_eps = pow(eps_i * nu, 0.25);
        double ystar = u_eps * y_i / nu;
        double Rt = (k_i * k_i) / (nu * eps_i);
        /*
        *TODO: currently in form for AKN, find a to import SR form into this function using .bin or .txt format 
        */
        double f2 = pow((1.0 - exp(-ystar / 3.1)),2) * (1.0 - 0.3 * exp(-pow(Rt / 6.5, 2.0)));

        result[i] = clamp(f2, 0.0, 1.0);

    }
}

void USERFUNCTION_EXPORT fmu_akn(CoordReal *result, int size, CoordReal *eps, CoordReal *k, CoordReal *y, CoordReal *mu, CoordReal *rho)
{
    for (int i = 0; i < size; i++)
    {
        //inputs per cell
        double eps_i = (eps[i] > 1e-30) ? eps[i] : 1e-30; //turbulent dissipation rate [m^2/s^3]
        double k_i = (k[i] > 1e-30) ? k[i] : 1e-30; //turbulent kinetic energy [m^2/s^2]
        double y_i = (y[i] > 1e-30) ? y[i] : 1e-30; //wall distance [m]
        double mu_i = (mu[i] > 1e-30) ? mu[i] : 1e-30; //dynamic viscosity [kg/{m.s}]
        double rho_i= (rho[i] > 1e-30) ? rho[i] : 1e-30; // density [kg/m^3]
        //to avoid null points switch to 1e-30
        
        double nu = mu_i / rho_i;
        double u_eps = pow(eps_i * nu, 0.25);
        double ystar = u_eps * y_i / nu;
        double Rt = (k_i * k_i) / (nu * eps_i);
        /*
        *TODO: currently in form for AKN, find a to import SR form into this function using .bin or .txt format 
        */
        double fmu = pow((1.0 - exp(-ystar / 14.0)),2) * (1.0 + (5.0 / pow(Rt, 0.75)) * exp(-pow(Rt / 200.0, 2.0)));

        result[i] = clamp(fmu, 0.0, 1.0);

    }
}


/*
 * uclib  —  Registration function
 * STAR-CCM+ calls this automatically when the .so is loaded.
 * ucfunc() registers the function as a custom scalar field function.
 * ucarg()  registers each STAR-CCM+ field that feeds into the function. 
 * The order of ucarg() calls MUST match the argument order in the C++ function signature above (after result and size).
*/
void USERFUNCTION_EXPORT
uclib()
{
    //register f2
    ucfunc(reinterpret_cast<void*>(f2_akn), "ScalarFieldFunction", "f2_AKN_custom");

    ucarg(reinterpret_cast<void*>(f2_akn), "Cell", "TurbulentDissipationRate", sizeof(CoordReal));

    ucarg(reinterpret_cast<void*>(f2_akn), "Cell", "TurbulentKineticEnergy", sizeof(CoordReal));

    ucarg(reinterpret_cast<void*>(f2_akn), "Cell", "WallDistance", sizeof(CoordReal));

    ucarg(reinterpret_cast<void*>(f2_akn), "Cell", "DynamicViscosity", sizeof(CoordReal));

    ucarg(reinterpret_cast<void*>(f2_akn), "Cell", "Density", sizeof(CoordReal));

    //register fmu
    ucfunc(reinterpret_cast<void*>(fmu_akn), "ScalarFieldFunction","fmu_AKN_custom");

    ucarg(reinterpret_cast<void*>(fmu_akn), "Cell", "TurbulentDissipationRate", sizeof(CoordReal));

    ucarg(reinterpret_cast<void*>(fmu_akn), "Cell", "TurbulentKineticEnergy", sizeof(CoordReal));

    ucarg(reinterpret_cast<void*>(fmu_akn), "Cell", "WallDistance", sizeof(CoordReal));

    ucarg(reinterpret_cast<void*>(fmu_akn), "Cell", "DynamicViscosity", sizeof(CoordReal));

    ucarg(reinterpret_cast<void*>(fmu_akn), "Cell", "Density", sizeof(CoordReal));
}
