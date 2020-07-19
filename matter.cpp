#include <vector>
#include <array>
#include <iostream>
#include <cmath>

#define MOLE (6.02214076*pow(10,23))

class Matter;

struct GenericForce
{
};

template <class T1, class T2>
struct Force : GenericForce
{
public:
  Eigen::Vector3d components;
  T1* source;
  T2* target;
  Force();
  Force(T1* src, T2* end, Eigen::Vector3d parts);
};

template<class T1, class T2>
Force<T1,T2>::Force()
  :components{0,0,0}, target{nullptr}, source{nullptr}
{
}

template<class T1, class T2>
Force<T1,T2>::Force(T1* src, T2* end, Eigen::Vector3d parts)
  :components{parts}, target{end}, source{src}
{
}

class Matter
{
public:
  double mass;
  double radius;
  
  Eigen::Vector3d position;
  Eigen::Vector3d velocity;
  Eigen::Vector3d acceleration;
  Force<Matter, Matter> net_force;
  Matter(double m, double r, Eigen::Vector3d x, Eigen::Vector3d v, Eigen::Vector3d a);
};

Matter::Matter(double m, double r, Eigen::Vector3d x, Eigen::Vector3d v, Eigen::Vector3d a)
  :radius{r}, mass{m}, net_force{this, NULL, {0,0,0}}, position{x}, velocity{v}, acceleration{a}
{
  assert (radius > 0);
  assert (mass >= 0);
}

class Photon
{
public:
  Eigen::Vector3d position;
  Eigen::Vector3d direction; // Unit vector in correct direction
  Photon(Eigen::Vector3d x, Eigen::Vector3d d);
};

Photon::Photon(Eigen::Vector3d x, Eigen::Vector3d d)
  :position{x}, direction{d}
{
}

class Star : public Matter
{
public:
  double luminosity;
  double core[5];
  double core_radius;
  double core_temp;
  double shell[5];

  std::vector<Photon> photons; // TODO: Rethink star managing its photons
  
  Star(double m, double r, Eigen::Vector3d x, Eigen::Vector3d v, Eigen::Vector3d a, double L);
  void fusion();
  void emit_light();
  void kill_light();
};

Star::Star(double m, double r, Eigen::Vector3d x, Eigen::Vector3d v, Eigen::Vector3d a, double L)
  : Matter(m, r, x, v, a), luminosity{L}, core{(0.1*mass/1.007825)*MOLE,0,0,0,0}, shell{(0.9*mass/1.007825)*MOLE,0,0,0,0}, core_radius{0.25*r}
{
  assert (luminosity >= 0);
  assert (mass > 0);
}

void Star::fusion()
{
  if (core[0] > 0) {
    core[0] -= ((luminosity/(pow(LIGHTSPEED,2)))/0.028698) * 4;
    core[1] += ((luminosity/(pow(LIGHTSPEED,2)))/0.028698);
    core_temp = 
    mass = (((core[0]/MOLE) * 1.007825)+((core[1]/MOLE) * 4.002602)) + ((shell[0]/MOLE) * 1.007825);
  }
  else if (shell[0] > 0) {
    shell[0] -= ((luminosity/(pow(LIGHTSPEED,2)))/0.028698) * 4;
    shell[1] += ((luminosity/(pow(LIGHTSPEED,2)))/0.028698);
    core_radius -= core_radius * 0.01; // Simulate core contraction.
    
    mass = ((core[1]/MOLE) * 4.002602) + ((shell[0]/MOLE) * 1.007825)+((shell[1]/MOLE) * 4.002602);
  }
}

void Star::emit_light()
{
  double num_photons = round(LIGHT_FRAC * (luminosity / PLANCK_CONST));
  // Very useful: https://stackoverflow.com/questions/9600801/evenly-distributing-n-points-on-a-sphere
  for (int i = 0; (double) i < num_photons; i++) {
    Eigen::Vector3d point;
    point[1] = 1 - (i / (float)(num_photons - 1)) * 2;
    radius = sqrt(1 - point[1] * point[1]);
    double theta = PHI * i;
    point[0] = cos(theta) * radius;
    point[2] = sin(theta) * radius;
    photons.emplace_back(position, point);
  }
}

void Star::kill_light()
{
  // Should not be static... as a result changing luminosity not supported.
  double num_photons = round(LIGHT_FRAC * (luminosity / PLANCK_CONST));
  std::vector<decltype(photons)::value_type>(photons.begin()+num_photons, photons.end()).swap(photons);
}
