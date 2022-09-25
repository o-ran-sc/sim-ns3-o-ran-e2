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
* Create and encode RIC Indication messages.
* Prints the encoded messages in XML format. 
*/
int 
main (int argc, char *argv[])
{
  // LogComponentEnable ("Asn1Types", LOG_LEVEL_ALL);
  LogComponentEnable ("KpmIndication", LOG_LEVEL_INFO);
  LogComponentEnable ("KpmFunctionDescription", LOG_LEVEL_INFO);
  
  std::string plmId = "111";
  std::string gnbId = "1";
  uint16_t nrCellId = 5;

  uint64_t timestamp = 1630068655325;
  // "12:58:04.123456";
  uint64_t timestamp2 = 1630068679511;
  // "12:58:05.123457";
  uint64_t timestamp3 = 1630068680196;
  // "12:58:06.123458";
  uint64_t timestamp4 = 1630068681372;
  // "12:58:07.123459";

  NS_LOG_UNCOND ("----------- Begin of Kpm Indication header -----------");

  KpmIndicationHeader::KpmRicIndicationHeaderValues headerValues; 
  headerValues.m_plmId = plmId;
  headerValues.m_gnbId = gnbId;
  headerValues.m_nrCellId = nrCellId;
  headerValues.m_timestamp = timestamp;

KpmIndicationHeader::KpmRicIndicationHeaderValues headerValues2; 
  headerValues2.m_plmId = plmId;
  headerValues2.m_gnbId = gnbId;
  headerValues2.m_nrCellId = nrCellId;
  headerValues2.m_timestamp = timestamp2;

  KpmIndicationHeader::KpmRicIndicationHeaderValues headerValues3; 
  headerValues3.m_plmId = plmId;
  headerValues3.m_gnbId = gnbId;
  headerValues3.m_nrCellId = nrCellId;
  headerValues3.m_timestamp = timestamp3;

  KpmIndicationHeader::KpmRicIndicationHeaderValues headerValues4; 
  headerValues4.m_plmId = plmId;
  headerValues4.m_gnbId = gnbId;
  headerValues4.m_nrCellId = nrCellId;
  headerValues4.m_timestamp = timestamp4;

  Ptr<KpmIndicationHeader> header = Create<KpmIndicationHeader> (KpmIndicationHeader::GlobalE2nodeType::eNB, headerValues);
  Ptr<KpmIndicationHeader> header2 = Create<KpmIndicationHeader> (KpmIndicationHeader::GlobalE2nodeType::gNB, headerValues2);
  Ptr<KpmIndicationHeader> header3 = Create<KpmIndicationHeader> (KpmIndicationHeader::GlobalE2nodeType::ng_eNB, headerValues3);
  Ptr<KpmIndicationHeader> header4 = Create<KpmIndicationHeader> (KpmIndicationHeader::GlobalE2nodeType::en_gNB, headerValues4);

  NS_LOG_UNCOND ("----------- End of the Kpm Indication header -----------");
  return 0;

  KpmIndicationMessage::KpmIndicationMessageValues msgValues1;

  NS_LOG_UNCOND ("----------- Begin of the CU-UP message -----------");

  // Begin example CU-UP
  // uncomment this to test CU-UP

  Ptr<OCuUpContainerValues> cuUpValues = Create<OCuUpContainerValues> ();
  cuUpValues->m_plmId = plmId;
  cuUpValues->m_pDCPBytesUL = 100;
  cuUpValues->m_pDCPBytesDL = 100;

  Ptr<MeasurementItemList> ue0DummyValues = Create<MeasurementItemList> ("UE-0");
  ue0DummyValues->AddItem<long> ("DRB.PdcpSduVolumeDl_Filter.UEID", 6);
  ue0DummyValues->AddItem<long> ("QosFlow.PdcpPduVolumeDL_Filter.UEID", 7);
  ue0DummyValues->AddItem<long> ("Tot.PdcpSduNbrDl.UEID", 8);
  ue0DummyValues->AddItem<long> ("DRB.PdcpPduNbrDl.Qos.UEID", 9);
  ue0DummyValues->AddItem<double> ("DRB.IPThpDl.UEID", 10.0);
  ue0DummyValues->AddItem<double> ("DRB.IPLateDl.UEID", 11.0);
  msgValues1.m_ueIndications.insert (ue0DummyValues);
 
  Ptr<MeasurementItemList> ue1DummyValues = Create<MeasurementItemList> ("UE-1");
  ue1DummyValues->AddItem<long> ("DRB.PdcpSduVolumeDl_Filter.UEID", 6);
  ue1DummyValues->AddItem<long> ("QosFlow.PdcpPduVolumeDL_Filter.UEID", 7);
  ue1DummyValues->AddItem<long> ("Tot.PdcpSduNbrDl.UEID", 8);
  ue1DummyValues->AddItem<long> ("DRB.PdcpPduNbrDl.Qos.UEID", 9);
  ue1DummyValues->AddItem<double> ("DRB.IPThpDl.UEID", 10.0);
  ue1DummyValues->AddItem<double> ("DRB.IPLateDl.UEID", 11.0);
  msgValues1.m_ueIndications.insert(ue1DummyValues);
  msgValues1.m_pmContainerValues = cuUpValues; 

  Ptr<KpmIndicationMessage> msg = Create<KpmIndicationMessage> (msgValues1);

  NS_LOG_UNCOND ("----------- End of the CU-UP message -----------");

  NS_LOG_UNCOND ("----------- Begin of the CU-CP message -----------");
  KpmIndicationMessage::KpmIndicationMessageValues msgValues2;
  msgValues2.m_cellObjectId = "NRCellCU";
  Ptr<OCuCpContainerValues> cuCpValues = Create<OCuCpContainerValues> ();
  cuCpValues->m_numActiveUes = 100;

  Ptr<MeasurementItemList> ue2DummyValues =
      Create<MeasurementItemList> ("UE-2");
  ue2DummyValues->AddItem<long> ("DRB.EstabSucc.5QI.UEID", 6);
  ue2DummyValues->AddItem<long> ("DRB.RelActNbr.5QI.UEID", 7);
  msgValues2.m_ueIndications.insert (ue2DummyValues);

   Ptr<MeasurementItemList> ue3DummyValues =
      Create<MeasurementItemList> ("UE-3");
  ue3DummyValues->AddItem<long> ("DRB.EstabSucc.5QI.UEID", 6);
  ue3DummyValues->AddItem<long> ("DRB.RelActNbr.5QI.UEID", 7);
  msgValues2.m_ueIndications.insert (ue3DummyValues);

  Ptr<MeasurementItemList> ue4DummyValues =
      Create<MeasurementItemList> ("UE-4");

  Ptr<L3RrcMeasurements> l3RrcMeasurement = Create<L3RrcMeasurements> (RRCEvent_b1);
  Ptr<ServingCellMeasurementsWrap> servingCellMeasurements =
      Create<ServingCellMeasurementsWrap> (ServingCellMeasurements_PR_nr_measResultServingMOList);

  Ptr<MeasResultNr> measResultNr = Create<MeasResultNr> (39);
  Ptr<MeasQuantityResultsWrap> measQuantityResultWrap = Create<MeasQuantityResultsWrap> ();
  measQuantityResultWrap->AddSinr (20);
  measResultNr->AddCellResults (MeasResultNr::SSB, measQuantityResultWrap->GetPointer ());
  Ptr<MeasResultServMo> measResultServMo =
      Create<MeasResultServMo> (10, measResultNr->GetValue ());
  servingCellMeasurements->AddMeasResultServMo (measResultServMo->GetPointer ());
  l3RrcMeasurement->AddServingCellMeasurement (servingCellMeasurements->GetPointer ());

  ue4DummyValues->AddItem<Ptr<L3RrcMeasurements>> ("calla",l3RrcMeasurement);
  ue4DummyValues->AddItem<long> ("DRB.RelActNbr.5QI.UEID", 7);
  msgValues2.m_ueIndications.insert (ue4DummyValues);
  msgValues2.m_pmContainerValues = cuCpValues;

  Ptr<KpmIndicationMessage> msg2 = Create<KpmIndicationMessage> (msgValues2);
  NS_LOG_UNCOND ("----------- End of the CU-CP message -----------");
  
  NS_LOG_UNCOND ("----------- Begin test of the DU message -----------");
  KpmIndicationMessage::KpmIndicationMessageValues msgValues3;
  msgValues3.m_cellObjectId = "NRCellCU";

  Ptr<ODuContainerValues> oDuContainerVal = Create<ODuContainerValues> ();
  Ptr<CellResourceReport> cellResRep = Create<CellResourceReport> ();
  cellResRep->m_plmId = "111";
  // std::stringstream ss;
  // ss << std::hex << 1340012;
  cellResRep->m_nrCellId = 2;
  cellResRep->dlAvailablePrbs = 6;
  cellResRep->ulAvailablePrbs = 6;

    Ptr<CellResourceReport> cellResRep2 = Create<CellResourceReport> ();
  cellResRep2->m_plmId = "444";
  cellResRep2->m_nrCellId = 3;
  cellResRep2->dlAvailablePrbs = 5;
  cellResRep2->ulAvailablePrbs = 5;

  Ptr<ServedPlmnPerCell> servedPlmnPerCell = Create<ServedPlmnPerCell> ();
  servedPlmnPerCell->m_plmId = "121";
  servedPlmnPerCell->m_nrCellId = 3;

  Ptr<ServedPlmnPerCell> servedPlmnPerCell2 = Create<ServedPlmnPerCell> ();
  servedPlmnPerCell2->m_plmId = "121";
  servedPlmnPerCell2->m_nrCellId = 2;

  Ptr<EpcDuPmContainer> epcDuVal = Create<EpcDuPmContainer> ();
  epcDuVal->m_qci = 1;
  epcDuVal->m_dlPrbUsage = 1;
  epcDuVal->m_ulPrbUsage = 2;

  Ptr<EpcDuPmContainer> epcDuVal2 = Create<EpcDuPmContainer> ();
  epcDuVal2->m_qci = 1;
  epcDuVal2->m_dlPrbUsage = 3;
  epcDuVal2->m_ulPrbUsage = 4;

  servedPlmnPerCell->m_perQciReportItems.insert (epcDuVal);
  servedPlmnPerCell->m_perQciReportItems.insert (epcDuVal2);
  servedPlmnPerCell2->m_perQciReportItems.insert (epcDuVal);
  servedPlmnPerCell2->m_perQciReportItems.insert (epcDuVal2);
  cellResRep->m_servedPlmnPerCellItems.insert (servedPlmnPerCell2);
  cellResRep->m_servedPlmnPerCellItems.insert (servedPlmnPerCell);
  cellResRep2->m_servedPlmnPerCellItems.insert (servedPlmnPerCell2);
  cellResRep2->m_servedPlmnPerCellItems.insert (servedPlmnPerCell);
  
  oDuContainerVal->m_cellResourceReportItems.insert (cellResRep);
  oDuContainerVal->m_cellResourceReportItems.insert (cellResRep2);

  Ptr<MeasurementItemList> ue5DummyValues = Create<MeasurementItemList> ("UE-5");
  ue5DummyValues->AddItem<long> ("DRB.EstabSucc.5QI.UEID", 6);
  ue5DummyValues->AddItem<long> ("DRB.RelActNbr.5QI.UEID", 7);
  msgValues3.m_ueIndications.insert (ue5DummyValues);

  msgValues3.m_pmContainerValues = oDuContainerVal;
  Ptr<KpmIndicationMessage> msg3 = Create<KpmIndicationMessage> (msgValues3);
  
  NS_LOG_UNCOND ("----------- End test of the DU message -----------");

  NS_LOG_UNCOND ("----------- Begin test of the KpmFunctionDescription -----------");
  Ptr<KpmFunctionDescription> fd = Create<KpmFunctionDescription> ();
  NS_LOG_UNCOND ("----------- End test of the KpmFunctionDescription -----------");

  return 0;
}
