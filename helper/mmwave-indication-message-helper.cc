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
 *       Mina Yonan <mina.awadallah@orange.com>
 *       Mostafa Ashraf <mostafa.ashraf.ext@orange.com>
 */

#include <ns3/mmwave-indication-message-helper.h>

namespace ns3 {

MmWaveIndicationMessageHelper::MmWaveIndicationMessageHelper (IndicationMessageType type,
                                                              bool isOffline, bool reducedPmValues)
    : IndicationMessageHelper (type, isOffline, reducedPmValues)
{
}

void
MmWaveIndicationMessageHelper::AddCuUpUePmItem (std::string ueImsiComplete,
                                                long txPdcpPduBytesNrRlc, long txPdcpPduNrRlc)
{
  Ptr<MeasurementItemList> ueVal = Create<MeasurementItemList> (ueImsiComplete);
  if (!m_reducedPmValues)
    {
      // UE-specific PDCP PDU volume transmitted to NR gNB (Unit is Kbits)
      ueVal->AddItem<long> ("QosFlow.PdcpPduVolumeDL_Filter.UEID", txPdcpPduBytesNrRlc);

      // UE-specific number of PDCP PDUs split with NR gNB
      ueVal->AddItem<long> ("DRB.PdcpPduNbrDl.Qos.UEID", txPdcpPduNrRlc);
    }

  m_msgValues.m_ueIndications.insert (ueVal);
}

void
MmWaveIndicationMessageHelper::FillCuUpValues (std::string plmId)
{
  FillBaseCuUpValues (plmId);
}

void
MmWaveIndicationMessageHelper::FillCuCpValues (uint16_t numActiveUes)
{
  FillBaseCuCpValues (numActiveUes);
}

void
MmWaveIndicationMessageHelper::FillDuValues (std::string cellObjectId)
{
  m_msgValues.m_cellObjectId = cellObjectId;
  m_msgValues.m_pmContainerValues = m_duValues;
}

void
MmWaveIndicationMessageHelper::AddDuUePmItem (
    std::string ueImsiComplete, long macPduUe, long macPduInitialUe, long macQpsk, long mac16Qam,
    long mac64Qam, long macRetx, long macVolume, long macPrb, long macMac04, long macMac59,
    long macMac1014, long macMac1519, long macMac2024, long macMac2529, long macSinrBin1,
    long macSinrBin2, long macSinrBin3, long macSinrBin4, long macSinrBin5, long macSinrBin6,
    long macSinrBin7, long rlcBufferOccup, double drbThrDlUeid)
{

  Ptr<MeasurementItemList> ueVal = Create<MeasurementItemList> (ueImsiComplete);
  if (!m_reducedPmValues)
    {
      ueVal->AddItem<long> ("TB.TotNbrDl.1.UEID", macPduUe);
      ueVal->AddItem<long> ("TB.TotNbrDlInitial.UEID", macPduInitialUe);
      ueVal->AddItem<long> ("TB.TotNbrDlInitial.Qpsk.UEID", macQpsk);
      ueVal->AddItem<long> ("TB.TotNbrDlInitial.16Qam.UEID", mac16Qam);
      ueVal->AddItem<long> ("TB.TotNbrDlInitial.64Qam.UEID", mac64Qam);
      ueVal->AddItem<long> ("TB.ErrTotalNbrDl.1.UEID", macRetx);
      ueVal->AddItem<long> ("QosFlow.PdcpPduVolumeDL_Filter.UEID", macVolume);
      ueVal->AddItem<long> ("RRU.PrbUsedDl.UEID", (long) std::ceil (macPrb));
      ueVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin1.UEID", macMac04);
      ueVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin2.UEID", macMac59);
      ueVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin3.UEID", macMac1014);
      ueVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin4.UEID", macMac1519);
      ueVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin5.UEID", macMac2024);
      ueVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin6.UEID", macMac2529);
      ueVal->AddItem<long> ("L1M.RS-SINR.Bin34.UEID", macSinrBin1);
      ueVal->AddItem<long> ("L1M.RS-SINR.Bin46.UEID", macSinrBin2);
      ueVal->AddItem<long> ("L1M.RS-SINR.Bin58.UEID", macSinrBin3);
      ueVal->AddItem<long> ("L1M.RS-SINR.Bin70.UEID", macSinrBin4);
      ueVal->AddItem<long> ("L1M.RS-SINR.Bin82.UEID", macSinrBin5);
      ueVal->AddItem<long> ("L1M.RS-SINR.Bin94.UEID", macSinrBin6);
      ueVal->AddItem<long> ("L1M.RS-SINR.Bin127.UEID", macSinrBin7);
      ueVal->AddItem<long> ("DRB.BufferSize.Qos.UEID", rlcBufferOccup);
    }

  // This value is not requested anymore, so it has been removed from the delivery, but it will be still logged;
  // ueVal->AddItem<double> ("DRB.UEThpDlPdcpBased.UEID", drbThrDlPdcpBasedUeid);
  
  ueVal->AddItem<double> ("DRB.UEThpDl.UEID", drbThrDlUeid);

  m_msgValues.m_ueIndications.insert (ueVal);
}

void
MmWaveIndicationMessageHelper::AddDuCellPmItem (
    long macPduCellSpecific, long macPduInitialCellSpecific, long macQpskCellSpecific,
    long mac16QamCellSpecific, long mac64QamCellSpecific, double prbUtilizationDl,
    long macRetxCellSpecific, long macVolumeCellSpecific, long macMac04CellSpecific,
    long macMac59CellSpecific, long macMac1014CellSpecific, long macMac1519CellSpecific,
    long macMac2024CellSpecific, long macMac2529CellSpecific, long macSinrBin1CellSpecific,
    long macSinrBin2CellSpecific, long macSinrBin3CellSpecific, long macSinrBin4CellSpecific,
    long macSinrBin5CellSpecific, long macSinrBin6CellSpecific, long macSinrBin7CellSpecific,
    long rlcBufferOccupCellSpecific, long activeUeDl)
{
  Ptr<MeasurementItemList> cellVal = Create<MeasurementItemList> ();

  if (!m_reducedPmValues)
    {
      cellVal->AddItem<long> ("TB.TotNbrDl.1", macPduCellSpecific);
      cellVal->AddItem<long> ("TB.TotNbrDlInitial", macPduInitialCellSpecific);
    }

  cellVal->AddItem<long> ("TB.TotNbrDlInitial.Qpsk", macQpskCellSpecific);
  cellVal->AddItem<long> ("TB.TotNbrDlInitial.16Qam", mac16QamCellSpecific);
  cellVal->AddItem<long> ("TB.TotNbrDlInitial.64Qam", mac64QamCellSpecific);
  cellVal->AddItem<long> ("RRU.PrbUsedDl", (long) std::ceil (prbUtilizationDl));

  if (!m_reducedPmValues)
    {
      cellVal->AddItem<long> ("TB.ErrTotalNbrDl.1", macRetxCellSpecific);
      cellVal->AddItem<long> ("QosFlow.PdcpPduVolumeDL_Filter", macVolumeCellSpecific);
      cellVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin1", macMac04CellSpecific);
      cellVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin2", macMac59CellSpecific);
      cellVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin3", macMac1014CellSpecific);
      cellVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin4", macMac1519CellSpecific);
      cellVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin5", macMac2024CellSpecific);
      cellVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin6", macMac2529CellSpecific);
      cellVal->AddItem<long> ("L1M.RS-SINR.Bin34", macSinrBin1CellSpecific);
      cellVal->AddItem<long> ("L1M.RS-SINR.Bin46", macSinrBin2CellSpecific);
      cellVal->AddItem<long> ("L1M.RS-SINR.Bin58", macSinrBin3CellSpecific);
      cellVal->AddItem<long> ("L1M.RS-SINR.Bin70", macSinrBin4CellSpecific);
      cellVal->AddItem<long> ("L1M.RS-SINR.Bin82", macSinrBin5CellSpecific);
      cellVal->AddItem<long> ("L1M.RS-SINR.Bin94", macSinrBin6CellSpecific);
      cellVal->AddItem<long> ("L1M.RS-SINR.Bin127", macSinrBin7CellSpecific);
      cellVal->AddItem<long> ("DRB.BufferSize.Qos", rlcBufferOccupCellSpecific);
    }

  cellVal->AddItem<long> ("DRB.MeanActiveUeDl",activeUeDl);

  m_msgValues.m_cellMeasurementItems = cellVal;
}

void
MmWaveIndicationMessageHelper::AddDuCellResRepPmItem (Ptr<CellResourceReport> cellResRep)
{
  m_duValues->m_cellResourceReportItems.insert (cellResRep);
}

void
MmWaveIndicationMessageHelper::AddCuCpUePmItem (std::string ueImsiComplete, long numDrb,
                                                long drbRelAct,
                                                Ptr<L3RrcMeasurements> l3RrcMeasurementServing,
                                                Ptr<L3RrcMeasurements> l3RrcMeasurementNeigh)
{

  Ptr<MeasurementItemList> ueVal = Create<MeasurementItemList> (ueImsiComplete);
  if (!m_reducedPmValues)
    {
      ueVal->AddItem<long> ("DRB.EstabSucc.5QI.UEID", numDrb);
      ueVal->AddItem<long> ("DRB.RelActNbr.5QI.UEID", drbRelAct); // not modeled in the simulator
    }

  // L3servingSINR3gpp_cell_XX
  ueVal->AddItem<Ptr<L3RrcMeasurements>> ("HO.SrcCellQual.RS-SINR.UEID", l3RrcMeasurementServing);
  // L3servingSINR3gpp_cell_XX_UEID_XX
  // L3neighSINR_cell_XX
  ueVal->AddItem<Ptr<L3RrcMeasurements>> ("HO.TrgtCellQual.RS-SINR.UEID", l3RrcMeasurementNeigh);

  m_msgValues.m_ueIndications.insert (ueVal);
}

MmWaveIndicationMessageHelper::~MmWaveIndicationMessageHelper ()
{
}

} // namespace ns3