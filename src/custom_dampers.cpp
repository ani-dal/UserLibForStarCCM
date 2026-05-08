#include "uclib.h"
#include <math.h>


static inline double clamp(double x, double lo, double hi)
{
    if (x < lo ) return lo;
    if (x > hi ) return hi;
    return x;
}

static inline double compute_f2(double x0, double x1) // x0: y+ & x1: y*
{
    return (0.000242 + x1 * (-9.61e-8)) * (x1 + (x0 + x1 * x1 * (-0.00107) + (x0 * 3.87e-6 - 0.192) * (x0 * x0 * (x1 * (-1.49e-7) - 0.000179) + x0)) * (-0.0563)) + 0.937;
}

static inline double compute_fmu(double x0, double x1) // x0: y* & x1: Rt
{
    return x0 * 0.000254 + (x1 * x1 * 3.36e-5 - 5.27e-7 * x1 * x0 * (-0.000331 * x0 * x0 + x0) + x1 + x0 * x0 * 0.00884) * (-1.12e-5) + 0.936;
}


void USERFUNCTION_EXPORT f2_custom(CoordReal *result, int size, Real *eps, Real *k, CoordReal *y, Real *mu, Real *rho)
{
    const double Cmu = 0.09;

    for (int i = 0; i < size; i++)
    {
        const double eps_i  = (eps[i]  > 1e-30) ? eps[i]  : 1e-30;
        const double k_i    = (k[i]    > 1e-30) ? k[i]    : 1e-30;
        const double y_i    = (y[i]    > 1e-30) ? y[i]    : 1e-30;
        const double mu_i   = (mu[i]   > 1e-30) ? mu[i]   : 1e-30;
        const double rho_i  = (rho[i]  > 1e-30) ? rho[i]  : 1e-30;

        const double nu      = mu_i / rho_i;
        const double u_tau   = pow(Cmu, 0.25) * sqrt(k_i);   // standard approximation
        const double yplus_i = u_tau * y_i / nu;              // now defined

        const double u_eps  = pow(eps_i * nu, 0.25);
        const double ystar  = u_eps * y_i / nu;

        const double f2 = compute_f2(yplus_i,ystar);
        result[i] = (CoordReal)clamp(f2, 0.0, 1.0);
    }
}


void USERFUNCTION_EXPORT fmu_custom(CoordReal *result, int size, Real *eps, Real *k, CoordReal *y, Real *mu, Real *rho)
{
    for (int i = 0; i < size; i++)
    {
        //inputs per cell
        const double eps_i = (eps[i] > 1e-30) ? eps[i] : 1e-30; //turbulent dissipation rate [m^2/s^3]
        const double k_i = (k[i] > 1e-30) ? k[i] : 1e-30; //turbulent kinetic energy [m^2/s^2]
        const double y_i = (y[i] > 1e-30) ? y[i] : 1e-30; //wall distance [m]
        const double mu_i = (mu[i] > 1e-30) ? mu[i] : 1e-30; //dynamic viscosity [kg/{m.s}]
        const double rho_i= (rho[i] > 1e-30) ? rho[i] : 1e-30; // density [kg/m^3]
        //to avoid null points switch to 1e-30
        
        const double nu = mu_i / rho_i;
        const double u_eps = pow(eps_i * nu, 0.25);
        const double ystar = u_eps * y_i / nu;
        const double Rt = (k_i * k_i) / (nu * eps_i);
        /*
        *TODO: currently in form for AKN, find a to import SR form into this function using .bin or .txt format 
        */
        const double fmu = compute_fmu(ystar, Rt);

        result[i] = (CoordReal)clamp(fmu, 0.0, 1.0);

    }
}

void USERFUNCTION_EXPORT nuT_custom(CoordReal *result, int size, Real *eps, Real *k, CoordReal *y, Real *mu, Real *rho)
{
    const double Cmu = 0.09;

    for(int i = 0; i < size; i++)
    {
        //inputs per cell
        double eps_i = (eps[i] > 1e-30) ? eps[i] : 1e-30; //turbulent dissipation rate [m^2/s^3]
        double k_i = (k[i] > 1e-30) ? k[i] : 1e-30; //turbulent kinetic energy [m^2/s^2]
        double y_i = (y[i] > 1e-30) ? y[i] : 1e-30; //wall distance [m]
        double mu_i = (mu[i] > 1e-30) ? mu[i] : 1e-30; //dynamic viscosity [kg/{m.s}]
        double rho_i= (rho[i] > 1e-30) ? rho[i] : 1e-30; // density [kg/m^3]
        //to avoid null points switch to 1e-30
        const double nu    = mu_i / rho_i;
        const double u_eps = pow(eps_i * nu, 0.25);
        const double ystar = u_eps * y_i / nu;
        const double Rt    = (k_i * k_i) / (nu * eps_i);

        const double fmu   = compute_fmu(ystar, Rt);
        const double nu_t  = Cmu * fmu * (k_i * k_i) / eps_i;

        result[i] = (CoordReal)nu_t;

    }
}

void USERFUNCTION_EXPORT muT_custom(CoordReal *result, int size, Real *eps, Real *k, CoordReal *y, Real *mu, Real *rho)
{
    const double Cmu = 0.09;

    for(int i = 0; i < size; i++)
    {
        //inputs per cell
        double eps_i = (eps[i] > 1e-30) ? eps[i] : 1e-30; //turbulent dissipation rate [m^2/s^3]
        double k_i = (k[i] > 1e-30) ? k[i] : 1e-30; //turbulent kinetic energy [m^2/s^2]
        double y_i = (y[i] > 1e-30) ? y[i] : 1e-30; //wall distance [m]
        double mu_i = (mu[i] > 1e-30) ? mu[i] : 1e-30; //dynamic viscosity [kg/{m.s}]
        double rho_i= (rho[i] > 1e-30) ? rho[i] : 1e-30; // density [kg/m^3]
        //to avoid null points switch to 1e-30
        const double nu    = mu_i / rho_i;
        const double u_eps = pow(eps_i * nu, 0.25);
        const double ystar = u_eps * y_i / nu;
        const double Rt    = (k_i * k_i) / (nu * eps_i);

        const double fmu   = compute_fmu(ystar, Rt);
        const double mu_t  = rho_i * Cmu * fmu * (k_i * k_i) / eps_i;

        result[i] = (CoordReal)mu_t;

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
    //register field functions
    
    //f2
    ucfunc(reinterpret_cast<void*>(f2_custom), "ScalarFieldFunction", "f2_custom");
    ucarg (reinterpret_cast<void*>(f2_custom), "Cell", "TurbulentDissipationRate", sizeof(Real));
    ucarg (reinterpret_cast<void*>(f2_custom), "Cell", "TurbulentKineticEnergy",   sizeof(Real));
    ucarg (reinterpret_cast<void*>(f2_custom), "Cell", "WallDistance",             sizeof(CoordReal));
    ucarg (reinterpret_cast<void*>(f2_custom), "Cell", "DynamicViscosity",         sizeof(Real));
    ucarg (reinterpret_cast<void*>(f2_custom), "Cell", "Density",                  sizeof(Real));

    //fmu
    ucfunc(reinterpret_cast<void*>(fmu_custom), "ScalarFieldFunction", "fmu_custom");
    ucarg (reinterpret_cast<void*>(fmu_custom), "Cell", "TurbulentDissipationRate", sizeof(Real));
    ucarg (reinterpret_cast<void*>(fmu_custom), "Cell", "TurbulentKineticEnergy",   sizeof(Real));
    ucarg (reinterpret_cast<void*>(fmu_custom), "Cell", "WallDistance",             sizeof(CoordReal));
    ucarg (reinterpret_cast<void*>(fmu_custom), "Cell", "DynamicViscosity",         sizeof(Real));
    ucarg (reinterpret_cast<void*>(fmu_custom), "Cell", "Density",                  sizeof(Real));

    //nut
    ucfunc(reinterpret_cast<void*>(nuT_custom), "ScalarFieldFunction", "nut_custom");
    ucarg (reinterpret_cast<void*>(nuT_custom), "Cell", "TurbulentDissipationRate", sizeof(Real));
    ucarg (reinterpret_cast<void*>(nuT_custom), "Cell", "TurbulentKineticEnergy",   sizeof(Real));
    ucarg (reinterpret_cast<void*>(nuT_custom), "Cell", "WallDistance",             sizeof(CoordReal));
    ucarg (reinterpret_cast<void*>(nuT_custom), "Cell", "DynamicViscosity",         sizeof(Real));
    ucarg (reinterpret_cast<void*>(nuT_custom), "Cell", "Density",                  sizeof(Real));

    //mut
    ucfunc(reinterpret_cast<void*>(muT_custom), "ScalarFieldFunction", "mut_custom");
    ucarg (reinterpret_cast<void*>(muT_custom), "Cell", "TurbulentDissipationRate", sizeof(Real));
    ucarg (reinterpret_cast<void*>(muT_custom), "Cell", "TurbulentKineticEnergy",   sizeof(Real));
    ucarg (reinterpret_cast<void*>(muT_custom), "Cell", "WallDistance",             sizeof(CoordReal));
    ucarg (reinterpret_cast<void*>(muT_custom), "Cell", "DynamicViscosity",         sizeof(Real));
    ucarg (reinterpret_cast<void*>(muT_custom), "Cell", "Density",                  sizeof(Real));
}
