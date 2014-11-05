/* *************************************************************
 *  
 *   Active Particles on Curved Spaces (APCS)
 *   
 *   Author: Rastko Sknepnek
 *  
 *   Division of Physics
 *   School of Engineering, Physics and Mathematics
 *   University of Dundee
 *   
 *   (c) 2013, 2014
 *   
 *   This program cannot be used, copied, or modified without
 *   explicit permission of the author.
 * 
 * ************************************************************* */

/*!
 * \file log_angle_eng.hpp
 * \author Rastko Sknepnek, sknepnek@gmail.com
 * \date 05-Nov-2014
 * \brief Declaration of LogAngleEng class
 */ 

#ifndef __LOG_ANGLE_ENG_H__
#define __LOG_ANGLE_ENG_H__


#include "log.hpp"

//! LogAngleEng class
/*! Logs different types of angle energies
 */
class LogAngleEng : public Log
{
public:
  
  //! Construct Log object
  //! \param sys pointer to a system object
  //! \param compute pointer to a compute object
  //! \param pot Interaction handler object
  //! \param align Alignment handler object
  //! \param type Type of the pair energy to log (should match the pair_potential type)
  LogAngleEng(SystemPtr sys, MessengerPtr msg, PotentialPtr pot, AlignerPtr align, const string& type) : Log(sys, msg, pot, align), m_type(type)  { }
  
  virtual ~LogAngleEng() { }
  
   //! \return current time step
  string operator()()
  {
    return str(format("%12.6e ") % m_potential->compute_angle_potential_energy_of_type(m_type));
  }
  
private:

  string m_type;    //!< Angle energy type to log
  
};

typedef shared_ptr<LogAngleEng> LogAngleEngPtr;

#endif