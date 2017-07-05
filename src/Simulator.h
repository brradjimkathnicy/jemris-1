/** @file Simulator.h
 *  @brief Implementation of JEMRIS Simulator
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

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include "ConcatSequence.h"
#include "XMLIO.h"
#include "World.h"
#include "Sample.h"
#include "CoilArray.h"
#include "Bloch_CV_Model.h"
#include "KSpace.h"

/**
 * @brief The simulator
 */

class Simulator {


 public:

	/**
	 * @brief Construct a new simulator with given XML descriptor
	 *
	 * @param fname     File name of XML configuration
	 * @param fsample   File name of sample
	 * @param ftxarray  File name of RX array
	 * @param frxarray  File name of TX array
	 * @param fsequence File name of sequence
	 * @param fmodel    Model name
	 */
	Simulator        (
			const std::string& fname,
			const std::string& fsample   = "",
			const std::string& frxarray  = "",
			const std::string& ftxarray  = "",
			const std::string& fsequence = "",
			const std::string& fmodel    = "");


	/**
	 * @brief Default destructor
	 */
	virtual ~Simulator       ();

	/**
	 * @brief Get my status
	 */
	bool GetStatus           () const { return m_state; }

	/**
	 * @brief Get my evolution counter
	 */
	int         GetEvolution () const { return m_evol;  }

	/**
	 * @brief Get a particular attribute value by a given key string
	 *
	 * @param element The element for which the attribute is requested
	 * @param key     The key of the attribute
	 *
	 * @return        The requested attribute string
	 */
	string      GetAttr      (DOMElement* element, const string& key);

	/**
	 * @brief Get a array of attributes given by the array of key strings
	 *
	 * @param element The element for which the attribute is requested
	 * @param keys    The array of key strings of the attributes
	 *
	 * @return        The requested array of attribute strings
	 */
	string      GetAtts      (DOMElement* element, string* keys);

	/**
	 * @brief Get a particular element from my DOM document
	 *
	 * @param name    The name of the element
	 */
	DOMElement* GetElem      (string name);

	/**
	 * @brief Start the simulation after every necessary credentials have been set
	 */
	void        Simulate     (bool bDumpSignal = true);

	/**
	 * @brief Set my sample from XML
	 */
	void      SetSample      (std::string fsample);

	/**
	 * @brief Set my sample to a new sample
	 */
	void      SetSample      (Sample* sample);

	/**
	 * @brief Get the sample
	 *
	 * @return The sample
	 */
	Sample*          GetSample      () { return    m_sample;      };

	/**
	 * @brief Get the model
	 *
	 * @return The model.
	 */
	Model*           GetModel       () { return    m_model;       };

	/**
	 * @brief Get the sequence
	 *
	 * @return The sequence
	 */
	ConcatSequence*  GetSequence    () { return    m_sequence;    };

	/**
	 * @brief Get the recieving coil array.
	 *
	 * @return The recieving coil array
	 */
	CoilArray*       GetTxCoilArray () { return    m_tx_coil_array; };

	/**
	 * @brief Get the transmitting coil array.
	 *
	 * @return
	 */
	CoilArray*       GetRxCoilArray () { return    m_rx_coil_array; };

	/**
	 * @brief Set my sequence according to XML configuration
	 */
	void      SetSequence    (string seq="");

	/**
	 * @brief Checks if restart files present; sets jemris state
	 */
	void	 CheckRestart();

	/**
	 * delete restart files (after successful simulation)
	 */
	void	 DeleteTmpFiles();

 //protected:

	/**
	 * @brief Default constructor
	 */
	Simulator ();

	/**
	 * @brief Initialize my world singleton
	 */
	void      SetWorld       ();

	/**
	 * @brief Set up my recieve coils according to XML configuration
	 */
	void      SetRxCoilArray (string frxarray="");

	/**
	 * @brief Set up my Transmit coils according to XML configuration
	 */
	void      SetTxCoilArray (string ftxarray="");

	/**
	 * @brief Set my solver model according to XML configuration
	 */
	void      SetModel       (string fmodel="");

	/**
	 * @brief Set miscellaneous parameters according to XML configuration
	 */
	void      SetParameter   ();

	/**
	 * @brief Set output directory
	 */
	void      SetOutputDir(string output_dir);

	/**
	 * @brief Set signal prefix
	 */
	void      SetSignalPrefix(string prefix);

 private:

	/**
	 * moves restart files to *.bak if the do not fit to current simulation
	 */
	void  	 MoveTmpFiles();

	bool                     m_state;             /**< @brief My status                   */
	int                      m_evol;              /**< @brief Evolution steps             */

	DOMDocument*             m_dom_doc;           /**< @brief Simulation file             */
	Sample*                  m_sample;            /**< @brief Sample                      */
	World*                   m_world;             /**< @brief World to be simulated       */
	Model*                   m_model;			  /**< @brief Model to be simulated       */
	ConcatSequence*          m_sequence;          /**< @brief Sequence to be simulated    */
	SequenceTree*            m_seqtree;           /**< @brief SequenceTree to be simulated*/
	XMLIO*                   m_xio;               /**< @brief XML file handler            */
	CoilArray*               m_tx_coil_array;     /**< @brief Transmit coil array         */
	CoilArray*               m_rx_coil_array;     /**< @brief Receive coil array          */
	DOMTreeErrorReporter*    m_domtree_error_rep; /**< @brief DOM tree error reporter     */
	KSpace<double,4>*        m_kspace;            /**< @brief K-Space                     */
	string                   m_output_dir;        /**< @brief Output directory            */
	string                   m_signal_prefix;     /**< @brief Signal prefix               */

};

#endif /*SIMULATOR_H_*/
