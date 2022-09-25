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

extern "C" {
  // #include "OCUCP-PF-Container.h"
  #include "OCTET_STRING.h"
  #include "asn_application.h"
  // #include "E2SM-KPM-IndicationMessage.h"
  // #include "FQIPERSlicesPerPlmnListItem.h"
  // #include "E2SM-KPM-RANfunction-Description.h"
  // #include "E2SM-KPM-IndicationHeader-Format1.h"
  // #include "E2SM-KPM-IndicationHeader.h"
  // #include "Timestamp.h"
  #include "E2AP-PDU.h"
  #include "RICsubscriptionRequest.h"
  #include "RICsubscriptionResponse.h"
  #include "RICactionType.h"
  #include "ProtocolIE-Field.h"
  #include "ProtocolIE-SingleContainer.h"
  #include "InitiatingMessage.h"
}

#include "e2sim.hpp"

using namespace ns3;

int 
main (int argc, char *argv[])
{
  E2Sim e2sim;
  e2sim.run_loop (argc, argv);
  
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
