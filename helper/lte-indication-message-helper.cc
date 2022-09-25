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


#include <ns3/lte-indication-message-helper.h>

namespace ns3 {

LteIndicationMessageHelper::LteIndicationMessageHelper (IndicationMessageType type, bool isOffline,
                                                        bool reducedPmValues)
    : IndicationMessageHelper (type, isOffline, reducedPmValues)
{
  NS_ABORT_MSG_IF (type == IndicationMessageType::Du,
                   "Wrong type for LTE Indication Message, expected CuUp or CuCp");
}

void
LteIndicationMessageHelper::AddCuUpUePmItem (std::string ueImsiComplete, long txBytes,
                                             long txDlPackets, double pdcpThroughput,
                                             double pdcpLatency)
{
  Ptr<MeasurementItemList> ueVal = Create<MeasurementItemList> (ueImsiComplete);

  if (!m_reducedPmValues)
    {
      // UE-specific PDCP SDU volume from LTE eNB. Unit is Mbits
      ueVal->AddItem<long> ("DRB.PdcpSduVolumeDl_Filter.UEID", txBytes);

      // UE-specific number of PDCP SDUs from LTE eNB
      ueVal->AddItem<long> ("Tot.PdcpSduNbrDl.UEID", txDlPackets);

      // UE-specific Downlink IP combined EN-DC throughput from LTE eNB. Unit is kbps
      ueVal->AddItem<double> ("DRB.PdcpSduBitRateDl.UEID", pdcpThroughput);

      //UE-specific Downlink IP combined EN-DC throughput from LTE eNB
      ueVal->AddItem<double> ("DRB.PdcpSduDelayDl.UEID", pdcpLatency);
    }

  m_msgValues.m_ueIndications.insert (ueVal);
}

void
LteIndicationMessageHelper::AddCuUpCellPmItem (double cellAverageLatency)
{
  if (!m_reducedPmValues)
    {
      Ptr<MeasurementItemList> cellVal = Create<MeasurementItemList> ();
      cellVal->AddItem<double> ("DRB.PdcpSduDelayDl", cellAverageLatency);
      m_msgValues.m_cellMeasurementItems = cellVal;
    }
}

void
LteIndicationMessageHelper::FillCuUpValues (std::string plmId, long pdcpBytesUl, long pdcpBytesDl)
{
  m_cuUpValues->m_pDCPBytesUL = pdcpBytesUl;
  m_cuUpValues->m_pDCPBytesDL = pdcpBytesDl;
  FillBaseCuUpValues (plmId);
}

void
LteIndicationMessageHelper::FillCuCpValues (uint16_t numActiveUes)
{
  FillBaseCuCpValues (numActiveUes);
}

void
LteIndicationMessageHelper::AddCuCpUePmItem (std::string ueImsiComplete, long numDrb,
                                             long drbRelAct)
{

  Ptr<MeasurementItemList> ueVal = Create<MeasurementItemList> (ueImsiComplete);
  if (!m_reducedPmValues)
    {
      ueVal->AddItem<long> ("DRB.EstabSucc.5QI.UEID", numDrb);
      ueVal->AddItem<long> ("DRB.RelActNbr.5QI.UEID", drbRelAct); // not modeled in the simulator
    }
  m_msgValues.m_ueIndications.insert (ueVal);
}

LteIndicationMessageHelper::~LteIndicationMessageHelper ()
{
}

} // namespace ns3