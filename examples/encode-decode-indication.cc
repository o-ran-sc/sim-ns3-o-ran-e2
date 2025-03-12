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
#include <E2SM-KPM-IndicationMessage-Format1.h>
#include <PM-Containers-Item.h>
#include "ProtocolIE-Field.h"
#include "CellResourceReportListItem.h"
#include "ServedPlmnPerCellListItem.h"
#include "EPC-DU-PM-Container.h"
#include "FGC-DU-PM-Container.h"
#include "PerQCIReportListItem.h"

using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("EncodeDecodeIndication");

std::string DecodeOctectString(OCTET_STRING_t* octetString){
  int size = octetString->size;
  char out[size + 1];
  std::memcpy (out, octetString->buf, size);
  out[size] = '\0';

  return std::string (out);
}

void
readPfDuContainer (ODU_PF_Container_t *oDU)
{
  int count = oDU->cellResourceReportList.list.count;
  if (count <= 0) {
    NS_LOG_ERROR("[E2SM] received empty ODU list");
    return;
  }

  for (int i = 0; i < count; i++)
  {
    CellResourceReportListItem_t *report = oDU->cellResourceReportList.list.array[i];
    // NS_LOG_UNCOND ("CellResourceReportListItem " << i);
    // NS_LOG_UNCOND (xer_fprint (stderr, &asn_DEF_CellResourceReportListItem, report));
    NS_LOG_UNCOND ("NRCGI");
    NS_LOG_UNCOND ("Buf PlmNID: " << report->nRCGI.pLMN_Identity.buf);
    NS_LOG_UNCOND ("String PlmNID: " << std::string (DecodeOctectString(&report->nRCGI.pLMN_Identity)));
    
    // TODO Decode the report->nRCGI.nRCellIdentity to a std::string
    // It's a bitstring I don't know how to do it

    long ulTotalOfAvailablePRBs = *(report->ul_TotalofAvailablePRBs);
    long dlTotalOfAvailablePRBs = *(report->dl_TotalofAvailablePRBs);
    NS_LOG_UNCOND ("UL Total of Available PRBs: " << ulTotalOfAvailablePRBs);
    NS_LOG_UNCOND("DL Total of Available PRBs: " << dlTotalOfAvailablePRBs);

    for (int j = 0; j < report->servedPlmnPerCellList.list.count; j++)
    {
      ServedPlmnPerCellListItem_t *servedPlmnPerCell = report->servedPlmnPerCellList.list.array[j];
      NS_LOG_UNCOND ("ServedPLMNPerCell " << j << " with PlmNID: " << DecodeOctectString(&servedPlmnPerCell->pLMN_Identity));
      // NS_LOG_UNCOND (xer_fprint (stderr, &asn_DEF_ServedPlmnPerCellListItem, servedPlmnPerCell));
      
      FGC_DU_PM_Container_t* fgcDuPmContainer = servedPlmnPerCell->du_PM_5GC;
      if (fgcDuPmContainer != NULL)
      {
        NS_LOG_UNCOND ("5GC DU PM Container");
        NS_LOG_UNCOND (xer_fprint (stderr, &asn_DEF_FGC_DU_PM_Container, fgcDuPmContainer));
      }

      EPC_DU_PM_Container_t *epcDuPmContainer = servedPlmnPerCell->du_PM_EPC;
      if(epcDuPmContainer != NULL)
      {
        NS_LOG_UNCOND ("EPC DU PM Container");
        // NS_LOG_UNCOND (xer_fprint (stderr, &asn_DEF_EPC_DU_PM_Container, epcDuPmContainer));

        for (int z = 0; z < epcDuPmContainer->perQCIReportList_du.list.count; z++)
          {
            PerQCIReportListItem_t * perQCIReportItem = epcDuPmContainer->perQCIReportList_du.list.array[z];
            long dlPrbUsage = *perQCIReportItem->dl_PRBUsage;
            long ulPrbUsage = *perQCIReportItem->ul_PRBUsage;
            long qci =  perQCIReportItem->qci;

            NS_LOG_UNCOND ("QCI " << qci << ", dlPrbUsage: " << dlPrbUsage
                                  << ", ulPrbUsage: " << ulPrbUsage);
          }
      }
 
    }

  }
}

void
readPfCuContainer (OCUCP_PF_Container_t *oCU_CP)
{
  // Decode the values in container and create a variable for each decoded values and then print on screen with UNCOND
  NS_LOG_UNCOND (xer_fprint (stderr, &asn_DEF_OCUCP_PF_Container, oCU_CP));
  NS_LOG_UNCOND ("OCUCP_PF_Container_t");
  long numActiveUes = *oCU_CP->cu_CP_Resource_Status.numberOfActive_UEs;
  NS_LOG_UNCOND (numActiveUes);
}

void
readPfCuContainer (OCUUP_PF_Container_t *oCU_UP)
{
}

void
ProcessIndicationMessage (E2SM_KPM_IndicationMessage_t *indMsg)
{
  if (indMsg->present == E2SM_KPM_IndicationMessage_PR_indicationMessage_Format1)
  {
    NS_LOG_UNCOND ("Format 1 present\n");

    E2SM_KPM_IndicationMessage_Format1_t *e2SmIndicationMessageFormat1 = indMsg->choice.indicationMessage_Format1;

    // extract RAN container, which is where we put our payload
    std::vector<uint8_t *> serving_cell_payload_vec;
    std::vector<uint8_t *> neighbor_cell_payload_vec;
    for (int i = 0; i < e2SmIndicationMessageFormat1->pm_Containers.list.count; i++){
      PM_Containers_Item_t *pmContainer = e2SmIndicationMessageFormat1->pm_Containers.list.array[i];
      // NS_LOG_UNCOND ("PM Container");
      // NS_LOG_UNCOND (xer_fprint (stderr, &asn_DEF_PM_Containers_Item, pmContainer));
      PF_Container_t *pfContainer = pmContainer->performanceContainer;
      switch (pfContainer->present)
        {
        case PF_Container_PR_NOTHING:
          NS_LOG_ERROR ("PF Container is empty");
          break;
        case PF_Container_PR_oDU:
          // NS_LOG_UNCOND ("oDU PF Container");
          // NS_LOG_UNCOND (xer_fprint (stderr, &asn_DEF_PF_Container, pfContainer));
          readPfDuContainer (pfContainer->choice.oDU);
          break;
        case PF_Container_PR_oCU_CP:
          // NS_LOG_UNCOND ("oCU CP PF Container");
          // NS_LOG_UNCOND (xer_fprint (stderr, &asn_DEF_PF_Container, pfContainer));
          readPfCuContainer (pfContainer->choice.oCU_CP);
          break;

        case PF_Container_PR_oCU_UP:
          // NS_LOG_UNCOND ("oCU UP PF Container");
          // NS_LOG_UNCOND (xer_fprint (stderr, &asn_DEF_PF_Container, pfContainer));
          readPfCuContainer (pfContainer->choice.oCU_UP);
          break;

        default:
          NS_LOG_ERROR ("PF Container not supported");
          break;
        }
    }

    //                        // combine content of vectors, there should be a single entry in the vector anyway
    //                        std::ostringstream serving_cell_payload_oss;
    //                        std::copy(serving_cell_payload_vec.begin(), serving_cell_payload_vec.end() - 1, std::ostream_iterator<uint8_t*>(serving_cell_payload_oss, ", "));
    //                        serving_cell_payload_oss << serving_cell_payload_vec.back();
    //                        std::string serving_cell_payload = serving_cell_payload_oss.str();
    //
    //                        std::ostringstream neighbor_cell_payload_oss;
    //                        std::copy(neighbor_cell_payload_vec.begin(), neighbor_cell_payload_vec.end() - 1, std::ostream_iterator<uint8_t*>(neighbor_cell_payload_oss, ", "));
    //                        neighbor_cell_payload_oss << neighbor_cell_payload_vec.back();
    //                        std::string neighbor_cell_payload = neighbor_cell_payload_oss.str();
    //
    //                        NS_LOG_UNCOND( "String conversion: serving_Cell_RF_Type %s, neighbor_Cell_RF: %s\n", serving_cell_payload.c_str(), neighbor_cell_payload.c_str());
    //
    //                        // assemble final payload
    //                        if (serving_cell_payload.length() > 0 && neighbor_cell_payload.length() > 0) {
    //                            payload = serving_cell_payload + ", " + neighbor_cell_payload;
    //                        }
    //                        else if (serving_cell_payload.length() > 0 && neighbor_cell_payload.length() == 0) {
    //                            payload = serving_cell_payload;
    //                        }
    //                        else if (serving_cell_payload.length() == 0 && neighbor_cell_payload.length() > 0) {
    //                            payload = neighbor_cell_payload;
    //                        }
    //                        else {
    //                            payload = "";
    //                        }
    //
    //                        NS_LOG_UNCOND( "Payload from RIC Indication message: %s\n", payload.c_str());
  }
  else
  {
    NS_LOG_UNCOND ("No payload received in RIC Indication message (or was unable to decode "
                   "received payload\n");
  }
  //                    add_gnb_to_vector_unique(gnb_id);
  //
  //                    if (payload.length() > 0) {
  //                        // add gnb id to payload
  //                        payload += "\n{\"gnb_id\": \"" + std::string(reinterpret_cast<char const*>(gnb_id)) + "\"}";
  //
  //                        NS_LOG_UNCOND( "Sending RIC Indication message to agent\n");
  //                        send_socket(payload.c_str());
  //                    }
  //                    else if (payload.length() <= 0) {
  //                        NS_LOG_UNCOND( "Received empty payload\n");
  //                    }
  //                    else {
  //                        NS_LOG_UNCOND( "Returned empty agent IP\n");
  //                    }
}

void
DecodeIndicationMessage (Ptr<KpmIndicationMessage> msg)
{
  asn_dec_rval_t decode_result;
  E2SM_KPM_IndicationMessage_t *indMsg = 0;

  decode_result = aper_decode_complete (NULL, &asn_DEF_E2SM_KPM_IndicationMessage,
                                        (void **) &indMsg, msg->m_buffer, msg->m_size);

  if (decode_result.code == RC_OK)
    {
      NS_LOG_UNCOND ("Decode OKAY");
      // NS_LOG_UNCOND (xer_fprint (stderr, &asn_DEF_E2SM_KPM_IndicationMessage, indMsg));
      ProcessIndicationMessage (indMsg);
    }
  else
    {
      ASN_STRUCT_FREE (asn_DEF_E2SM_KPM_IndicationMessage, indMsg);
      NS_LOG_UNCOND ("DECODE NOT OKAY");
    }
}

/**
* Create and encode RIC Indication messages.
* Prints the encoded messages in XML format. 
*/

int 
main (int argc, char *argv[])
{
  // LogComponentEnable ("Asn1Types", LOG_LEVEL_ALL);
  LogComponentEnable ("KpmIndication", LOG_LEVEL_INFO);
  
  std::string plmId = "111";
  std::string gnbId = "1";
  uint16_t nrCellId = 5;

  uint64_t timestamp = 1630068655325;

  NS_LOG_UNCOND ("----------- Begin of Kpm Indication header -----------");

  KpmIndicationHeader::KpmRicIndicationHeaderValues headerValues; 
  headerValues.m_plmId = plmId;
  headerValues.m_gnbId = gnbId;
  headerValues.m_nrCellId = nrCellId;
  headerValues.m_timestamp = timestamp;

  Ptr<KpmIndicationHeader> header = Create<KpmIndicationHeader> (KpmIndicationHeader::GlobalE2nodeType::eNB, headerValues);
  
  NS_LOG_UNCOND ("----------- End of the Kpm Indication header -----------");

  NS_LOG_UNCOND ("----------- Start decode of Header -----------");
  asn_dec_rval_t decode_header_result;
  E2SM_KPM_IndicationHeader_t *indHdr = 0;
  decode_header_result = aper_decode_complete(NULL, &asn_DEF_E2SM_KPM_IndicationHeader, (void **)&indHdr, header->m_buffer,
  header->m_size);
   if(decode_header_result.code == RC_OK) {
       NS_LOG_UNCOND ("Decode OKAY");
       NS_LOG_UNCOND (xer_fprint (stderr, &asn_DEF_E2SM_KPM_IndicationHeader, indHdr));
    }
    else {
        ASN_STRUCT_FREE(asn_DEF_E2SM_KPM_IndicationHeader, indHdr);
        NS_LOG_UNCOND ("DECODE NOT OKAY");
    }
  NS_LOG_UNCOND ("----------- End test of decode header -----------");


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
  Ptr<KpmIndicationMessage> msg = Create<KpmIndicationMessage> (msgValues3);
  
  NS_LOG_UNCOND ("----------- End test of the DU message -----------");

  NS_LOG_UNCOND ("----------- Start decode of DU message -----------");
  DecodeIndicationMessage (msg);
  NS_LOG_UNCOND ("----------- End test of decode DU message -----------");

  return 0;
}
