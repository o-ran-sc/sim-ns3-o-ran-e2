/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2022 Northeastern University
 * Copyright (c) 2022 Sapienza, University of Rome
 * Copyright (c) 2022 University of Padova
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Andrea Lacava <thecave003@gmail.com>
 *		   Tommaso Zugno <tommasozugno@gmail.com>
 *		   Michele Polese <michele.polese@gmail.com>
 */

#include "ns3/core-module.h"
#include "ns3/oran-interface.h"
#include <errno.h>

using namespace ns3;

/**
* Test field for the wrappers and their functions
*/
int 
main (int argc, char *argv[])
{
  // LogComponentEnable ("Asn1Types", LOG_LEVEL_ALL);
  std::string test = "test";
//   Ptr<OctetString> one = Create<OctetString> (test, test.size ());
  // Ptr<OctetString> due = Create<OctetString> (test, test.size ());
  // Ptr<Snssai> snssai = Create<Snssai> ("test");

  // std::cout << due->DecodeContent() << std::endl;

  std::vector<Ptr<NrCellId>> nrCellIds;
  for (uint16_t i = 0; i < 20; i++)
    {
      nrCellIds.push_back(Create<NrCellId> (i));
      NS_LOG_UNCOND ("Count: " << i << " , value: ");
      xer_fprint (stdout, &asn_DEF_BIT_STRING, nrCellIds[i]->GetPointer ());
    }

  return 0;
}
