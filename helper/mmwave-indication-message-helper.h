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

#ifndef MMWAVE_INDICATION_MESSAGE_HELPER_H
#define MMWAVE_INDICATION_MESSAGE_HELPER_H

#include <ns3/indication-message-helper.h>

namespace ns3 {

class MmWaveIndicationMessageHelper : public IndicationMessageHelper
{
public:
  MmWaveIndicationMessageHelper (IndicationMessageType type, bool isOffline, bool reducedPmValues);

  ~MmWaveIndicationMessageHelper ();

  void FillCuUpValues (std::string plmId);

  void AddCuUpUePmItem (std::string ueImsiComplete, long txPdcpPduBytesNrRlc, long txPdcpPduNrRlc);

  void FillCuCpValues (uint16_t numActiveUes);
  
  void FillDuValues (std::string cellObjectId);

  void AddDuUePmItem (std::string ueImsiComplete, long macPduUe, long macPduInitialUe, long macQpsk,
                      long mac16Qam, long mac64Qam, long macRetx, long macVolume, long macPrb,
                      long macMac04, long macMac59, long macMac1014, long macMac1519,
                      long macMac2024, long macMac2529, long macSinrBin1, long macSinrBin2,
                      long macSinrBin3, long macSinrBin4, long macSinrBin5, long macSinrBin6,
                      long macSinrBin7, long rlcBufferOccup, double drbThrDlUeid);

  void AddDuCellPmItem (
      long macPduCellSpecific, long macPduInitialCellSpecific, long macQpskCellSpecific,
      long mac16QamCellSpecific, long mac64QamCellSpecific, double prbUtilizationDl,
      long macRetxCellSpecific, long macVolumeCellSpecific, long macMac04CellSpecific,
      long macMac59CellSpecific, long macMac1014CellSpecific, long macMac1519CellSpecific,
      long macMac2024CellSpecific, long macMac2529CellSpecific, long macSinrBin1CellSpecific,
      long macSinrBin2CellSpecific, long macSinrBin3CellSpecific, long macSinrBin4CellSpecific,
      long macSinrBin5CellSpecific, long macSinrBin6CellSpecific, long macSinrBin7CellSpecific,
      long rlcBufferOccupCellSpecific, long activeUeDl);
  void AddDuCellResRepPmItem (Ptr<CellResourceReport> cellResRep);
  void AddCuCpUePmItem (std::string ueImsiComplete, long numDrb, long drbRelAct,
                        Ptr<L3RrcMeasurements> l3RrcMeasurementServing,
                        Ptr<L3RrcMeasurements> l3RrcMeasurementNeigh);
  // void AddCuCpUePmItem (std::string ueImsiComplete, long numDrb, long drbRelAct,
  //                       Ptr<L3RrcMeasurements> l3RrcMeasurementServing,
  //                       Ptr<L3RrcMeasurements> l3RrcMeasurementNeigh, long m_cellId, long imsi,
  //                         long sinrThisCell,  long convertedSinr);

private:
};

} // namespace ns3

#endif /* MMWAVE_INDICATION_MESSAGE_HELPER_H */