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
 * \file regster_external_potentials.cpp
 * \author Rastko Sknepnek, sknepnek@gmail.com
 * \date 03-Mar-2015
 * \brief Register all external potentials with the class factory.
*/

#include "register.hpp"

void register_external_potentials(ExternalPotentialMap& external_potentials)
{
  // Register gravity to the external potential class factory
  external_potentials["gravity"] = boost::factory<ExternalGravityPotentialPtr>();
  // Register harmonic potential to the external potential class factory
  external_potentials["harmonic"] = boost::factory<ExternalHarmonicPotentialPtr>();
  // Register self propulsion to the external potential class factory
  external_potentials["self_propulsion"] = boost::factory<ExternalSelfPropulsionPtr>();
}