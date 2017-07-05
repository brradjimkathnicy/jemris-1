/** @file TrajectoryInterface.h
 * @brief Implementation of JEMRIS TrajectoryInterface
 *
 *  Created on: Dec 4, 2009
 *      Author: dpflug
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

#ifndef TRAJECTORYINTERFACE_H_
#define TRAJECTORYINTERFACE_H_

#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cmath>

#include "World.h"

using namespace std;

/**
 * @brief pure virtual base class for all Trajectories
 */

class TrajectoryInterface {
public:
	TrajectoryInterface();
	virtual ~TrajectoryInterface();

	/*
	 * checks if data available; then call GetValueDerived:
	 */
	void GetValue(double time, double *value) ;

	/**
	 * @brief retrieve value at given time-point
	 */
	virtual void GetValueDerived(double time, double *value) =0;

	/**
	 * @brief load trajectory from file
	 */
	virtual void LoadFile(string filename) =0;

	virtual void UpdateTrajectory(bool init=false){};

protected:
	/**
	 * @brief find data index for interpolation
	 * get lower index for given timepoint using hunt search algorithm.
	 */
	int GetLowerIndex(double time);

	int m_LastHuntIndex;

	vector<double> m_time;

};

#endif /* TRAJECTORYINTERFACE_H_ */
