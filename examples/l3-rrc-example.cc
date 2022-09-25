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

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("L3RrcExample");

Ptr<L3RrcMeasurements>
CreateL3RrcUeSpecificSinrServing (long servingCellId, long physCellId, long sinr)
{
  Ptr<L3RrcMeasurements> l3RrcMeasurement = Create<L3RrcMeasurements> (RRCEvent_b1);
  Ptr<ServingCellMeasurementsWrap> servingCellMeasurements =
      Create<ServingCellMeasurementsWrap> (ServingCellMeasurements_PR_nr_measResultServingMOList);

  Ptr<MeasResultNr> measResultNr = Create<MeasResultNr> (physCellId);
  Ptr<MeasQuantityResultsWrap> measQuantityResultWrap = Create<MeasQuantityResultsWrap> ();
  measQuantityResultWrap->AddSinr (sinr);
  measResultNr->AddCellResults (MeasResultNr::SSB, measQuantityResultWrap->GetPointer ());
  Ptr<MeasResultServMo> measResultServMo =
      Create<MeasResultServMo> (servingCellId, measResultNr->GetValue ());
  servingCellMeasurements->AddMeasResultServMo (measResultServMo->GetPointer ());
  l3RrcMeasurement->AddServingCellMeasurement (servingCellMeasurements->GetPointer ());
  return l3RrcMeasurement;
}

Ptr<L3RrcMeasurements>
CreateL3RrcUeSpecificSinrNeigh (long neighCellId, long sinr)
{
  Ptr<L3RrcMeasurements> l3RrcMeasurement = Create<L3RrcMeasurements> (RRCEvent_b1);
  Ptr<MeasResultNr> measResultNr = Create<MeasResultNr> (neighCellId);
  Ptr<MeasQuantityResultsWrap> measQuantityResultWrap = Create<MeasQuantityResultsWrap> ();
  measQuantityResultWrap->AddSinr (sinr);
  measResultNr->AddCellResults (MeasResultNr::SSB, measQuantityResultWrap->GetPointer ());

  l3RrcMeasurement->AddMeasResultNRNeighCells (
      measResultNr->GetPointer ()); // MAX 8 UE per message (standard)

  return l3RrcMeasurement;
}


// The memory leaks occurring in this file are wanted because L3-RRC is usually freed after use in the Ric Indication Message

int
main (int argc, char *argv[])
{
  LogComponentEnableAll (LOG_PREFIX_ALL);
  LogComponentEnable ("L3RrcExample", LOG_LEVEL_ALL);
  LogComponentEnable ("Asn1Types", LOG_LEVEL_ALL);

  //  1 UE-specific (L3) SINR from NR serving cells
  NS_LOG_INFO ("1 UE-specific (L3) SINR from NR serving cells");
  Ptr<L3RrcMeasurements> l3RrcMeasurement1 = CreateL3RrcUeSpecificSinrServing (1, 1, 10);
  xer_fprint (stderr, &asn_DEF_L3_RRC_Measurements, l3RrcMeasurement1->GetPointer ());

  //  2 UE-specific (L3) SINR from NR neighboring cells
  NS_LOG_INFO ("2 UE-specific (L3) SINR from NR neighboring cells");
  Ptr<L3RrcMeasurements> l3RrcMeasurement2 = CreateL3RrcUeSpecificSinrNeigh (2, 20);

  //  3 UE-specific (L3) SINR report from NR neighboring cells
  NS_LOG_INFO ("3 UE-specific (L3) SINR report from NR neighboring cells");
  long neighCellId3 = 3;
  long sinr3 = 30;

  Ptr<MeasResultNr> measResultNr3 = Create<MeasResultNr> (neighCellId3);
  Ptr<MeasQuantityResultsWrap> measQuantityResultWrap3 = Create<MeasQuantityResultsWrap> ();
  measQuantityResultWrap3->AddSinr (sinr3);
  measResultNr3->AddCellResults (MeasResultNr::SSB, measQuantityResultWrap3->GetPointer ());

  l3RrcMeasurement2->AddMeasResultNRNeighCells (measResultNr3->GetPointer ());

  xer_fprint (stderr, &asn_DEF_L3_RRC_Measurements, l3RrcMeasurement2->GetPointer ());

  //  4 UE-specific (L3) SINR from LTE serving cells
  NS_LOG_INFO ("4 UE-specific (L3) SINR from LTE serving cells");
  long eutraPhysCellId4 = 4;
  long servCellId4 = 4;
  long sinr4 = 40;
  Ptr<L3RrcMeasurements> l3RrcMeasurement4 =
      CreateL3RrcUeSpecificSinrServing (servCellId4, eutraPhysCellId4, sinr4);
  xer_fprint (stderr, &asn_DEF_L3_RRC_Measurements, l3RrcMeasurement4->GetPointer ());


  //  5 UE-specific (L3) SINR from LTE neighboring cells
  NS_LOG_INFO ("5 UE-specific (L3) SINR from LTE neighboring cells");
  long neighCellId5 = 5;
  long sinr5 = 50;
  Ptr<L3RrcMeasurements> l3RrcMeasurement3 = Create<L3RrcMeasurements> (RRCEvent_a5);
  Ptr<MeasResultEutra> measResultEutra5 = Create<MeasResultEutra> (neighCellId5);
  measResultEutra5->AddSinr (sinr5);
  l3RrcMeasurement3->AddMeasResultEUTRANeighCells (measResultEutra5->GetPointer ());
  xer_fprint (stderr, &asn_DEF_L3_RRC_Measurements, l3RrcMeasurement3->GetPointer ());


  return 0;
}
