/** @file TPOI.h
 *  @brief Implementation of JEMRIS TPOI
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2015  Tony Stoecker
 *                        2007-2015  Kaveh Vahedipour
 *                        2009-2015  Daniel Pflugfelder
 *                                  
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef TPOI_H_
#define TPOI_H_

#include <cstdlib>
#include <vector>
#include <ostream>
#include <iostream>
#include <bitset>
#include <cmath>

using namespace std;

#include "Declarations.h"

//const double TIME_ERR_TOL = 1e-6; // Avoid CVODE warnings. Doesn't affect physics.

//! Time points of interest of all modules

template<class T> inline T
BIT (const T& x) { return T(1) << x; }

static const size_t JTPOI_T (0);
static const size_t ADC_T (1);
static const size_t EXCITE_T (2);
static const size_t REFOCUS_T (3);

template<class T, class S> inline bool
check_bit (const T& x, const S& y) { return 0 != (x & BIT(y)); }

template<class T, class S> inline void
set_bit (T& x, const S& y) { x |= T(1) << y; }


class TPOI {

 public:

    //! The set of data of each time point of interest
    struct set {


        double dtime;    /**< particular time point of this set.*/
        double dphase;   /**< particular corresponding reciever phase.*/
        size_t bmask; /**< Qualifier bit mask */

        /**
         * @brief Constructor
         *
         * @param time    The time from start of the pulse 0.
         * @param phase   Phase lock, if this TPOI is an ADC.
         */
        inline set (const double time, const double phase = -1.,
        		const size_t mask = 0) : dtime(time), dphase (phase), bmask(mask) {}

    };

    /**
     * Constructor
     *
     * The Cunstructor will Initialize a new data acquisition repository with
     * initial size of 1000.
     */
    TPOI() {
        // Reset my data repository
        Reset();
    };


    /**
     * Reset my data repository
     */
    void Reset () {
        // Set position back to first slot and values to zero
        m_time.clear();
        m_phase.clear();
        m_mask.clear();
    }

    /**
     * Destructor
     *
     * Free memory of the data;
     */
    ~TPOI() {}


    /**
     * Add two sets of time points.
     *
     * Merge two sets of time points. Elimintaing overlapping times and sorting
     * on the time axis.
     * @param tpoi Time points of interest to be added
     */
    void operator += (const TPOI& tpoi);

    /**
     * Add two sets of time points.
     *
     * Merge two sets of time points. Elimintaing overlapping times and sorting
     * on the time axis.
     * @param tpoi Time points of interest to be added
     * @return Sum
     */
    TPOI& operator + (const TPOI& tpoi);

    /**
     * Append new single time point.
     *
     * Add a new time point of interest to the set.
     * @param data New set
     */
    void operator + (const set& data);

    /**
     * Get the size of this repository
     *
     * @return Size of this repository
     */
     int GetSize () const ;


     void Print (std::ostream& os) const {
    	 for (size_t i = 0; i < m_time.size(); ++i)
    		 os << m_time[i] << "\t" << m_phase[i] << "\t" << m_mask[i] << std::endl;
    	 os << std::endl;
     }

    /**
     * Get the time of a pos-th point
     *
     * @param  pos The particular position in the list
     * @return The pos-th time
     */
     inline double GetTime  (const size_t pos) const {return m_time[pos]; }

     /**
      * Get the phase of a pos-th point
      *
      * @param  pos The particular position in the list
      * @return The pos-th phase
      */
     inline double GetPhase (const size_t pos) const {return m_phase[pos]; }

     bool IsADC (const size_t pos) {return check_bit (m_mask[pos], 0); }
     bool IsExcitation (const size_t pos) {return check_bit (m_mask[pos], 1); }
     bool IsRefocussing (const size_t pos) {return check_bit (m_mask[pos], 2); }

     inline size_t GetMask (const size_t pos) const {return m_mask[pos];}

    /**
     * Sort my own data
     */
    void Sort ();

    /**
     * Purge my own data
     */
    void Purge ();

 private:

    vector<double> m_time;    /**< vector of time points.*/
    vector<double> m_phase;   /**< vector of corresponding receiver phase.*/
    vector<size_t> m_mask;    /**< vector of bitmasks */

};



inline std::ostream& operator<< (std::ostream& os, const TPOI& tpoi) {
	tpoi.Print(os);
	return os;
}

#endif
