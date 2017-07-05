/** @file Coil.cpp
 *  @brief Implementation of JEMRIS Coil
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2014  Tony Stoecker
 *                        2007-2014  Kaveh Vahedipour
 *                        2009-2014  Daniel Pflugfelder
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

#include "Coil.h"
#include "Model.h"
#include "DynamicVariables.h"

/**********************************************************/
Coil::~Coil() {
	
	if (m_signal   != NULL) delete    m_signal;
	
}

/**********************************************************/
void Coil::Initialize (DOMNode* node) {
	
	m_node     = node;
	
	string s = StrX(((DOMElement*) node)->getAttribute (StrX("Name").XMLchar() )).std_str() ;
	
	if (s.empty()) {
		((DOMElement*) node)->setAttribute(StrX("Name").XMLchar(),node->getNodeName());
		SetName( StrX(node->getNodeName()).std_str() );
	}
	
	
}

/**********************************************************/
void Coil::InitSignal(long lADCs) {
	
    if (m_signal!=NULL)
        delete m_signal;
	
    m_signal = new Signal (lADCs, World::instance()->GetNoOfCompartments());
	
}

/**********************************************************/
void Coil::Receive (long lADC) {
	
    m_signal->Repo()->TP(lADC) = World::instance()->time;
	
    double sens  = GetSensitivity (m_signal->Repo()->TP(lADC));
    double phase = GetPhase       (m_signal->Repo()->TP(lADC));
	
	long   pos   = m_signal->Repo()->Position(lADC); 
	
	double tm    = 0.0;

	for (int i = 0; i < m_signal->Repo()->Compartments(); i++) {

		tm = - World::instance()->phase + phase + World::instance()->solution[PHASE+ i*3];

		m_signal->Repo()->at(pos +     i*3) +=  sens  * World::instance()->solution[i*3 + AMPL] * cos (tm);
		m_signal->Repo()->at(pos + 1 + i*3) +=  sens  * World::instance()->solution[i*3 + AMPL] * sin (tm);
		m_signal->Repo()->at(pos + 2 + i*3) +=  sens  * World::instance()->solution[i*3 + 2];


	}

}

/**********************************************************/
void Coil::GridMap () {

    double position[3]  = {0.0,0.0,0.0};
    double max = 0.0;

    for (int k=0; k< (m_dim==3?m_points:1); k++) {

        position [ZC] = (m_dim==3?k*m_extent/m_points-m_extent/2:0.0);

        for (int j=0; j<m_points; j++) {

            position [YC] = j*m_extent/m_points-m_extent/2;

            for (int i=0; i<m_points; i++) {

                position [XC] = i*m_extent/m_points-m_extent/2;
                double mag   = m_scale*GetSensitivity(position);
                m_sensmag(i,j,k) = mag;
                max = (max>mag?max:mag);
                m_senspha(i,j,k) = ( (m_conjugate?-1.0:1.0) * ( GetPhase(position) + m_phase) );
                if (m_senspha(i,j,k) != 0.0) m_complex = true;
            }
        }
    }
    m_norm = 1/max;

}

double* Coil::PhaseMap () {
	return m_senspha.Ptr();
}

double* Coil::MagnitudeMap () {
	return m_sensmag.Ptr();
}

int Coil::GetPoints () {
	return m_points;
}

unsigned Coil::GetNDim () {
	return m_dim;
}


/**********************************************************/
double  Coil::GetPhase (const double time) {

    if (!m_complex) return m_phase;

    double position[3];
    position[0]=World::instance()->Values[XC];
    position[1]=World::instance()->Values[YC];
    position[2]=World::instance()->Values[ZC];
    DynamicVariables* dv = DynamicVariables::instance();
    dv->m_Motion->GetValue(time,position);
//MODIF
    dv->m_Flow->GetValue(time,position);
//MODIF***

    if (m_interpolate)
		return ( m_phase + (m_conjugate?-1.0:1.0) * InterpolateSensitivity(position,false));
	else
		return ( m_phase + (m_conjugate?-1.0:1.0) * GetPhase(position));
}

/**********************************************************/
double  Coil::GetSensitivity (const double time) {

    double position[3];
    position[0]=World::instance()->Values[XC];
    position[1]=World::instance()->Values[YC];
    position[2]=World::instance()->Values[ZC];
    DynamicVariables* dv = DynamicVariables::instance();
    dv->m_Motion->GetValue(time,position);
//MODIF
    dv->m_Flow->GetValue(time,position);
//MODIF***

	if (m_interpolate) {
		return m_scale*InterpolateSensitivity(position);
	}
	else {
		return m_scale*GetSensitivity(position);
	}
}


/**********************************************************/
double Coil::InterpolateSensitivity (const double* position, bool magnitude){

	// expects  -m_extent/2 <= position[j] <= m_extent/2
    double x = (position[XC]+m_extent/2)*m_points/m_extent;
    double y = (position[YC]+m_extent/2)*m_points/m_extent;
    double z = (m_dim==3?(position[ZC]+m_extent/2)*m_points/m_extent:0.0);
	int    px   = int(x),  py   = int(y), pz   = int(z);
	double normx = x-px ,	normy = y-py ,	normz = z-pz;

	//check if point is on lattice
	if (px>m_points-1 || px<0 || py>m_points-1 || py<0 || pz>m_points-1 || pz<0 ) return 0.0;

    //bilinear interpolation (2D)
	int nx = (px+1<m_points?px+1:m_points-1);
	int ny = (py+1<m_points?py+1:m_points-1);
	double i11, i21;
	if (magnitude) {
		i11 = m_sensmag(px,py,pz) + (m_sensmag(px,ny,pz)-m_sensmag(px,py,pz))*normy;
		i21 = m_sensmag(nx,py,pz) + (m_sensmag(nx,ny,pz)-m_sensmag(nx,py,pz))*normy;
	} else {
		i11 = m_senspha(px,py,pz)+Unwrap(m_senspha(px,ny,pz)-m_senspha(px,py,pz),magnitude)*normy;
		i21 = m_senspha(nx,py,pz)+Unwrap(m_senspha(nx,ny,pz)-m_senspha(nx,py,pz),magnitude)*normy;
	}
	double iz1 = i11+Unwrap(i21-i11,magnitude)*normx;

	//check 2D
	if (m_dim<3) return iz1;

    //trilinear interpolation (3D)
	int nz = (pz+1<m_points?pz+1:m_points-1);
	double i12, i22;
	if (magnitude) {
		i12 = m_sensmag(px,py,nz)+Unwrap(m_sensmag(px,ny,nz)-m_sensmag(px,py,nz),magnitude)*normy;
		i22 = m_sensmag(nx,py,nz)+Unwrap(m_sensmag(nx,ny,nz)-m_sensmag(nx,py,nz),magnitude)*normy;
	} else {
		i12 = m_senspha(px,py,nz)+Unwrap(m_senspha(px,ny,nz)-m_senspha(px,py,nz),magnitude)*normy;
		i22 = m_senspha(nx,py,nz)+Unwrap(m_senspha(nx,ny,nz)-m_senspha(nx,py,nz),magnitude)*normy;
	}
	double iz2 = i12+Unwrap(i22-i12,magnitude)*normx;

	return (iz1+Unwrap(iz2-iz1,magnitude)*normz);

}

/**********************************************************/
double Coil::Unwrap(double checkwrap, bool magnitude){
	// only for phase interpolation:
	if (magnitude) return checkwrap;
	const double wrapfact = 1; // factor to determine when a phase wrap is likely to be detected.
	if (checkwrap>PI*wrapfact) checkwrap-=2*PI; if (checkwrap<-PI*wrapfact) checkwrap+=2*PI;
	return checkwrap;
}

/**********************************************************/

bool Coil::Prepare  (const PrepareMode mode) {

	bool success = false;
	m_azimuth = 0.0;
	m_polar   = 0.0;
	m_scale   = 1.0;
	m_norm    = 1.0;
	m_phase   = 0.0;
	m_dim     = 3;
	m_extent  = 0;
	m_points  = 0;
	m_complex = false;
	m_conjugate = false;

	ATTRIBUTE("XPos"   , m_position [XC]);
	ATTRIBUTE("YPos"   , m_position [YC]);
	ATTRIBUTE("ZPos"   , m_position [ZC]);
	ATTRIBUTE("Azimuth", m_azimuth      );
	ATTRIBUTE("Polar"  , m_polar        );
	ATTRIBUTE("Scale"  , m_scale        );
	ATTRIBUTE("Phase"  , m_phase        );
	ATTRIBUTE("Conj"   , m_conjugate    );
	ATTRIBUTE("Dim"    , m_dim          );
	ATTRIBUTE("Extent" , m_extent       );
	ATTRIBUTE("Points" , m_points       );

	m_mode		= mode;
	m_signal	= NULL;

    success = Prototype::Prepare(mode);

    m_phase   *= PI/180.0;
    m_polar   *= PI/180.0;
    m_azimuth *= PI/180.0;
    m_interpolate = (m_points>0 && m_extent>0.0);

    // dimensions with m_points==0 may lead to undefined memory access
    if (m_points==0) m_points=1;
    m_sensmag = NDData<double> (m_points, m_points, (m_dim==3?m_points:1));
    m_senspha = NDData<double> (m_points, m_points, (m_dim==3?m_points:1));

    return success;

}
