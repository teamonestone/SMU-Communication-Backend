/**
 * @file SMU-Communication-Backend.h
 * @brief The header file for SMU-Communication-Backend related stuff.
 * @author Jonas Merkle [JJM] <a href="mailto:jonas.merkle@tam-onestone.net">jonas.merkle@tam-onestone.net</a>
 * @author Dominik Authaler <a href="mailto:dominik.authaler@team-onestone.net">dominik.authaler@team-onestone.net</a>
 * @author
 * This library is maintained by <a href="https://team-onestone.net">Team Onestone</a>.
 * E-Mail: <a href="mailto:info@team-onestone.net">info@team-onestone.net</a>
 * @version 1.0.0
 * @date 09 January 2020
 * @copyright This project is released under the GNU General Public License v3.0
 */

#ifndef SMU_COM_BACKEND_H
#define SMU_COM_BACKEND_H

//////////////
// Includes //
//////////////

// basic Includes
#include <inttypes.h>
#include <Arduino.h>

class SMU_Com_Backend
{
// Begin PUBLIC ------------------------------------------------------------------
	public:
		uint8_t getChecksum(uint8_t* dataArray, uint8_t lenght);

// End PUBLIC --------------------------------------------------------------------

// Begin PRIVATE -----------------------------------------------------------------
	private:

// End PRIVATE -------------------------------------------------------------------
};

#endif