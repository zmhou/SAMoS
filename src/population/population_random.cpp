/* *************************************************************
 *  
 *   Soft Active Mater on Surfaces (SAMoS)
 *   
 *   Author: Rastko Sknepnek
 *  
 *   Division of Physics
 *   School of Engineering, Physics and Mathematics
 *   University of Dundee
 *   
 *   (c) 2013, 2014
 * 
 *   School of Science and Engineering
 *   School of Life Sciences 
 *   University of Dundee
 * 
 *   (c) 2015
 * 
 *   Author: Silke Henkes
 * 
 *   Department of Physics 
 *   Institute for Complex Systems and Mathematical Biology
 *   University of Aberdeen  
 * 
 *   (c) 2014, 2015
 *  
 *   This program cannot be used, copied, or modified without
 *   explicit written permission of the authors.
 * 
 * ************************************************************* */

/*!
 * \file population_random.cpp
 * \author Rastko Sknepnek, sknepnek@gmail.com
 * \date 28-Sept-2014
 * \brief Implementation of PopulationRandom class.
 */ 

#include "population_random.hpp"


/*! Divide particles according to their age.
 *  We draw a uniform random number between 0 and 1
 *  if this number is less than particle age divided by
 *  the division rate, split particle. 
 * 
 *  Particle is split along the direction vector n
 *  New particles are placed one radius apart. Original 
 *  particle is pushed back by 1/2r and the new one is
 *  placed at 1/2r.
 * 
 *  \param t current time step
 *  
*/
void PopulationRandom::divide(int t)
{
  if (m_freq > 0 && t % m_freq == 0)  // Attempt division only at certain time steps
  { 
    int new_type;   // type of new particle
    double new_r;   // radius of newly formed particle
    int N = m_system->get_group(m_group_name)->get_size();
    vector<int> particles = m_system->get_group(m_group_name)->get_particles();
    bool periodic = m_system->get_periodic();
    BoxPtr box = m_system->get_box();
    double prob_div = m_div_rate*m_freq*m_system->get_integrator_step(); // actual probability of dividing now: rate * (attempt_freq * dt)
    if (prob_div > 1.0)
    {
	    m_msg->msg(Messenger::ERROR,"Division rate "+lexical_cast<string>(prob_div)+" is too large for current time step and attempt rate.");
	    throw runtime_error("Too high division.");
    }
    for (int i = 0; i < N; i++)
    {
      int pi = particles[i];
      Particle& p = m_system->get_particle(pi);
      if (m_rng->drnd() < p.age*prob_div)
      {
        Particle p_new(m_system->size(), p.get_type(), p.get_radius());
        p_new.x = p.x + m_alpha*p.get_radius()*p.nx;
        p_new.y = p.y + m_alpha*p.get_radius()*p.ny;
        p_new.z = p.z + m_alpha*p.get_radius()*p.nz;
        if (periodic)
        {
          if (p_new.x > box->xhi) p_new.x -= box->Lx;
          else if (p_new.x < box->xlo) p_new.x += box->Lx;
          if (p_new.y > box->yhi) p_new.y -= box->Ly;
          else if (p_new.y < box->ylo) p_new.y += box->Ly;
          if (p_new.z > box->zhi) p_new.z -= box->Lz;
          else if (p_new.z < box->zlo) p_new.z += box->Lz;
        }
        p.x -= (1.0-m_alpha)*p.get_radius()*p.nx;
        p.y -= (1.0-m_alpha)*p.get_radius()*p.ny;
        p.z -= (1.0-m_alpha)*p.get_radius()*p.nz;
        if (periodic)
        {
          if (p.x > box->xhi) p.x -= box->Lx;
          else if (p.x < box->xlo) p.x += box->Lx;
          if (p.y > box->yhi) p.y -= box->Ly;
          else if (p.y < box->ylo) p.y += box->Ly;
          if (p.z > box->zhi) p.z -= box->Lz;
          else if (p.z < box->zlo) p.z += box->Lz;
        }
        p_new.nx = p.nx; p_new.ny = p.ny; p_new.nz = p.nz;
        p_new.vx = p.vx; p_new.vy = p.vy; p_new.vz = p.vz;
        p_new.Nx = p.Nx; p_new.Ny = p.Ny; p_new.Nz = p.Nz;
        p.age = 0.0;
        p_new.age = 0.0;
        for(list<string>::iterator it_g = p.groups.begin(); it_g != p.groups.end(); it_g++)
          p_new.groups.push_back(*it_g);
        if (m_rng->drnd() < m_type_change_prob_1)  // Attempt to change type, radius and group for first child
        {
          if (m_new_type == 0)
            new_type = p.get_type();
          else
            new_type = m_new_type;
          p.set_type(new_type);
          if (m_new_radius == 0.0)
            new_r = p.get_radius();
          else
            new_r = m_new_radius;
          p.set_radius(new_r);
          m_system->change_group(p,m_old_group,m_new_group);
        }
        m_system->add_particle(p_new);
        Particle& pr = m_system->get_particle(p_new.get_id());
        if (m_rng->drnd() < m_type_change_prob_2)  // Attempt to change type, radius and group for second child
        {
          if (m_new_type == 0)
            new_type = pr.get_type();
          else
            new_type = m_new_type;
          pr.set_type(new_type);
          if (m_new_radius == 0.0)
            new_r = pr.get_radius();
          else
            new_r = m_new_radius;
          pr.set_radius(new_r);
          m_system->change_group(pr,m_old_group,m_new_group);
        }
      }
    }
    m_system->set_force_nlist_rebuild(true);
  }
}

/*! Remove particle. 
 * 
 *  In biological systems all cells die. Here we assume that the death is random, but 
 *  proportional to the particles age.
 * 
 *  \param t current time step
 * 
*/
void PopulationRandom::remove(int t)
{
  if (m_freq > 0 && t % m_freq == 0)  // Attempt removal only at certain time steps
  { 
    int N = m_system->get_group(m_group_name)->get_size();
    vector<int> particles = m_system->get_group(m_group_name)->get_particles();
    vector<int> to_remove;
    double prob_death = m_death_rate*m_freq*m_system->get_integrator_step(); // actual probability of dividing now: rate * (attempt_freq * dt)
    if (prob_death>1.0)
      {
	cout << "Error: death probability " << prob_death << " is too large for current time step and attempt rate!" << endl;
	throw runtime_error("Too high death.");
      }
    for (int i = 0; i < N; i++)
    {
      int pi = particles[i];
      Particle& p = m_system->get_particle(pi);
      if (m_rng->drnd() < p.age*prob_death)
        to_remove.push_back(p.get_id());
    }
    int offset = 0;
    for (vector<int>::iterator it = to_remove.begin(); it != to_remove.end(); it++)
    {
      m_system->remove_particle((*it)-offset);
      offset++;      
    }
    if (m_system->size() == 0)
    {
      m_msg->msg(Messenger::ERROR,"Random population control. No particles left in the system. Please reduce that death rate.");
      throw runtime_error("No particles left in the system.");
    }
    if (!m_system->group_ok(m_group_name))
    {
      cout << "Remove P: Group info mismatch for group : " << m_group_name << endl;
      throw runtime_error("Group mismatch.");
    }
    m_system->set_force_nlist_rebuild(true);
  }
}