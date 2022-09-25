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

#include <ns3/kpm-indication.h>
#include <ns3/asn1c-types.h>
#include <ns3/log.h>

extern "C" {
#include "E2SM-KPM-IndicationHeader-Format1.h"
#include "E2SM-KPM-IndicationMessage-Format1.h"
#include "GlobalE2node-ID.h"
#include "GlobalE2node-gNB-ID.h"
#include "GlobalE2node-eNB-ID.h"
#include "GlobalE2node-ng-eNB-ID.h"
#include "GlobalE2node-en-gNB-ID.h"
#include "NRCGI.h"
#include "PM-Containers-Item.h"
#include "RIC-EventTriggerStyle-Item.h"
#include "RIC-ReportStyle-Item.h"
#include "TimeStamp.h"
#include "CUUPMeasurement-Container.h"
#include "PlmnID-Item.h"
#include "EPC-CUUP-PM-Format.h"
#include "PerQCIReportListItemFormat.h"
#include "PerUE-PM-Item.h"
#include "PM-Info-Item.h"
#include "MeasurementInfoList.h"
#include "CellObjectID.h"
#include "CellResourceReportListItem.h"
#include "ServedPlmnPerCellListItem.h"
#include "EPC-DU-PM-Container.h"
#include "PerQCIReportListItem.h"
}

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("KpmIndication");

KpmIndicationHeader::KpmIndicationHeader (GlobalE2nodeType nodeType,KpmRicIndicationHeaderValues values)
{
  m_nodeType = nodeType;
  E2SM_KPM_IndicationHeader_t *descriptor = new E2SM_KPM_IndicationHeader_t;
  FillAndEncodeKpmRicIndicationHeader (descriptor, values);
  delete descriptor;
}

KpmIndicationHeader::~KpmIndicationHeader ()
{
  NS_LOG_FUNCTION (this);
  free (m_buffer);
  m_size = 0;
}

void
KpmIndicationHeader::Encode (E2SM_KPM_IndicationHeader_t *descriptor)
{
  asn_codec_ctx_t *opt_cod = 0; // disable stack bounds checking
  asn_encode_to_new_buffer_result_s encodedHeader = asn_encode_to_new_buffer (
      opt_cod, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2SM_KPM_IndicationHeader, descriptor);

  if (encodedHeader.result.encoded < 0)
    {
      NS_FATAL_ERROR ("Error during the encoding of the RIC Indication Header, errno: "
                      << strerror (errno) << ", failed_type "
                      << encodedHeader.result.failed_type->name << ", structure_ptr "
                      << encodedHeader.result.structure_ptr);
    }

  m_buffer = encodedHeader.buffer;
  m_size = encodedHeader.result.encoded;
}

void
KpmIndicationHeader::FillAndEncodeKpmRicIndicationHeader (E2SM_KPM_IndicationHeader_t *descriptor,
                                                          KpmRicIndicationHeaderValues values)
{

  E2SM_KPM_IndicationHeader_Format1_t *ind_header = (E2SM_KPM_IndicationHeader_Format1_t *) calloc (
      1, sizeof (E2SM_KPM_IndicationHeader_Format1_t));

  Ptr<OctetString> plmnid = Create<OctetString> (values.m_plmId, 3);
  Ptr<BitString> cellId_bstring;

  GlobalE2node_ID *globalE2nodeIdBuf = (GlobalE2node_ID *) calloc (1, sizeof (GlobalE2node_ID));
  ind_header->id_GlobalE2node_ID = *globalE2nodeIdBuf;

  switch (m_nodeType)
    {
      case gNB: {
        static int sizeGnb = 4; // 3GPP Specs
        
        cellId_bstring = Create<BitString> (values.m_gnbId, sizeGnb);

        ind_header->id_GlobalE2node_ID.present = GlobalE2node_ID_PR_gNB;
        GlobalE2node_gNB_ID_t *globalE2node_gNB_ID =
            (GlobalE2node_gNB_ID_t *) calloc (1, sizeof (GlobalE2node_gNB_ID_t));
        globalE2node_gNB_ID->global_gNB_ID.plmn_id = plmnid->GetValue ();
        globalE2node_gNB_ID->global_gNB_ID.gnb_id.present = GNB_ID_Choice_PR_gnb_ID;
        globalE2node_gNB_ID->global_gNB_ID.gnb_id.choice.gnb_ID = cellId_bstring->GetValue ();
        ind_header->id_GlobalE2node_ID.choice.gNB = globalE2node_gNB_ID;
      }
      break;

      case eNB: {
        static int sizeEnb =
            3; // 3GPP TS 36.413 version 14.8.0 Release 14, Section 9.2.1.37 Global eNB ID
        static int unsedSizeEnb = 4;
        
        cellId_bstring = Create<BitString> (values.m_gnbId, sizeEnb, unsedSizeEnb);
        
        ind_header->id_GlobalE2node_ID.present = GlobalE2node_ID_PR_eNB;
        GlobalE2node_eNB_ID_t *globalE2node_eNB_ID =
            (GlobalE2node_eNB_ID_t *) calloc (1, sizeof (GlobalE2node_eNB_ID_t));
        globalE2node_eNB_ID->global_eNB_ID.pLMN_Identity = plmnid->GetValue ();
        globalE2node_eNB_ID->global_eNB_ID.eNB_ID.present = ENB_ID_PR_macro_eNB_ID;
        globalE2node_eNB_ID->global_eNB_ID.eNB_ID.choice.macro_eNB_ID = cellId_bstring->GetValue ();
        ind_header->id_GlobalE2node_ID.choice.eNB = globalE2node_eNB_ID;
      }
      break;

      case ng_eNB: {
        static int sizeEnb =
            3; // 3GPP TS 36.413 version 14.8.0 Release 14, Section 9.2.1.37 Global eNB ID
        static int unsedSizeEnb = 4;

        cellId_bstring = Create<BitString> (values.m_gnbId, sizeEnb, unsedSizeEnb);

        ind_header->id_GlobalE2node_ID.present = GlobalE2node_ID_PR_ng_eNB;
        GlobalE2node_ng_eNB_ID_t *globalE2node_ng_eNB_ID =
            (GlobalE2node_ng_eNB_ID_t *) calloc (1, sizeof (GlobalE2node_ng_eNB_ID_t));

        globalE2node_ng_eNB_ID->global_ng_eNB_ID.plmn_id = plmnid->GetValue ();
        globalE2node_ng_eNB_ID->global_ng_eNB_ID.enb_id.present = ENB_ID_Choice_PR_enb_ID_macro;
        globalE2node_ng_eNB_ID->global_ng_eNB_ID.enb_id.choice.enb_ID_macro =
            cellId_bstring->GetValue ();
        ind_header->id_GlobalE2node_ID.choice.ng_eNB = globalE2node_ng_eNB_ID;
      }
      break;

      case en_gNB: {
        static int sizeGnb = 4; // 3GPP Specs
        cellId_bstring = Create<BitString> (values.m_gnbId, sizeGnb);

        ind_header->id_GlobalE2node_ID.present = GlobalE2node_ID_PR_en_gNB;
        GlobalE2node_en_gNB_ID_t *globalE2node_en_gNB_ID =
            (GlobalE2node_en_gNB_ID_t *) calloc (1, sizeof (GlobalE2node_en_gNB_ID_t));
        globalE2node_en_gNB_ID->global_gNB_ID.pLMN_Identity = plmnid->GetValue ();
        globalE2node_en_gNB_ID->global_gNB_ID.gNB_ID.present = ENGNB_ID_PR_gNB_ID;
        globalE2node_en_gNB_ID->global_gNB_ID.gNB_ID.choice.gNB_ID = cellId_bstring->GetValue ();
        ind_header->id_GlobalE2node_ID.choice.en_gNB = globalE2node_en_gNB_ID;
      }
      break;

    default:
      NS_FATAL_ERROR (
          "Unrecognized node type for KpmRicIndicationHeader, value passed: " << m_nodeType);
      break;
    }

    NS_LOG_DEBUG ("Timestamp received: " << values.m_timestamp);
    long bigEndianTimestamp = htobe64 (values.m_timestamp);
    NS_LOG_DEBUG ("Timestamp inverted: " << bigEndianTimestamp);
    
    Ptr<OctetString> ts = Create<OctetString> ((void *) &bigEndianTimestamp, TIMESTAMP_LIMIT_SIZE);
    //NS_LOG_INFO (xer_fprint (stderr, &asn_DEF_OCTET_STRING, ts->GetPointer() ));
    
    // Ptr<OctetString> ts2 = Create<OctetString> ((void *) &values.m_timestamp, TIMESTAMP_LIMIT_SIZE);
    // NS_LOG_INFO (xer_fprint (stderr, &asn_DEF_OCTET_STRING, ts2->GetPointer()));

    ind_header->collectionStartTime = ts->GetValue ();


    NS_LOG_INFO (xer_fprint (stderr, &asn_DEF_E2SM_KPM_IndicationHeader_Format1, ind_header));

    descriptor->present = E2SM_KPM_IndicationHeader_PR_indicationHeader_Format1;
    descriptor->choice.indicationHeader_Format1 = ind_header;

    Encode (descriptor);
    ASN_STRUCT_FREE (asn_DEF_E2SM_KPM_IndicationHeader_Format1, ind_header);
    free (globalE2nodeIdBuf);
    // TraceMessage (&asn_DEF_E2SM_KPM_IndicationHeader, header, "RIC Indication Header");
}

KpmIndicationMessage::KpmIndicationMessage (KpmIndicationMessageValues values)
{
  E2SM_KPM_IndicationMessage_t *descriptor = new E2SM_KPM_IndicationMessage_t ();
  CheckConstraints (values);
  FillAndEncodeKpmIndicationMessage (descriptor, values);
  delete descriptor;
}

KpmIndicationMessage::~KpmIndicationMessage ()
{
  free (m_buffer);
  m_size = 0;
}

void
KpmIndicationMessage::CheckConstraints (KpmIndicationMessageValues values)
{
  // TODO remove?
  // if (values.m_crnti.length () != 2)
  //   {
  //     NS_FATAL_ERROR ("C-RNTI should have length 2");
  //   }
  // if (values.m_plmId.length () != 3)
  //   {
  //     NS_FATAL_ERROR ("PLMID should have length 3");
  //   }
  // if (values.m_nrCellId.length () != 5)
  //   {
  //     NS_FATAL_ERROR ("NR Cell ID should have length 5");
  //   }
  // TODO add other constraints
}

void
KpmIndicationMessage::Encode (E2SM_KPM_IndicationMessage_t *descriptor)
{
  asn_codec_ctx_t *opt_cod = 0; // disable stack bounds checking
  asn_encode_to_new_buffer_result_s encodedMsg = asn_encode_to_new_buffer (
      opt_cod, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2SM_KPM_IndicationMessage, descriptor);

  if (encodedMsg.result.encoded < 0)
    {
      NS_FATAL_ERROR ("Error during the encoding of the RIC Indication Message, errno: "
                      << strerror (errno) << ", failed_type " << encodedMsg.result.failed_type->name
                      << ", structure_ptr " << encodedMsg.result.structure_ptr);
    }

  m_buffer = encodedMsg.buffer;
  m_size = encodedMsg.result.encoded;
}

void
KpmIndicationMessage::FillPmContainer (PF_Container_t *ranContainer, Ptr<PmContainerValues> values)
{
  Ptr<OCuUpContainerValues> cuUpVal = DynamicCast<OCuUpContainerValues> (values);
  Ptr<OCuCpContainerValues> cuCpVal = DynamicCast<OCuCpContainerValues> (values);
  Ptr<ODuContainerValues> duVal = DynamicCast<ODuContainerValues> (values);

  if (cuUpVal)
    {
      FillOCuUpContainer (ranContainer, cuUpVal);
    }
  else if (cuCpVal)
    {
      FillOCuCpContainer (ranContainer, cuCpVal);
    }
  else if (duVal)
   {
     FillODuContainer (ranContainer, duVal);
   }
  else
    {
      NS_FATAL_ERROR ("Unknown PM Container type");
    }
}

void
KpmIndicationMessage::FillOCuUpContainer (PF_Container_t *ranContainer,
                                          Ptr<OCuUpContainerValues> values)
{
  OCUUP_PF_Container_t* ocuup = (OCUUP_PF_Container_t*) calloc (1, sizeof (OCUUP_PF_Container_t));
  PF_ContainerListItem_t* pcli = (PF_ContainerListItem_t*) calloc (1, sizeof (PF_ContainerListItem_t));
  pcli->interface_type = NI_Type_x2_u;
  
  CUUPMeasurement_Container_t* cuuppmc = (CUUPMeasurement_Container_t*) calloc (1, sizeof (CUUPMeasurement_Container_t)); 
  PlmnID_Item_t* plmnItem = (PlmnID_Item_t*) calloc (1, sizeof (PlmnID_Item_t)); 
  Ptr<OctetString> plmnidstr = Create<OctetString> (values->m_plmId, 3);
  plmnItem->pLMN_Identity = plmnidstr->GetValue ();
  
  EPC_CUUP_PM_Format_t* cuuppmf = (EPC_CUUP_PM_Format_t*) calloc (1, sizeof (EPC_CUUP_PM_Format_t));
  plmnItem->cu_UP_PM_EPC = cuuppmf;
  PerQCIReportListItemFormat_t* pqrli = (PerQCIReportListItemFormat_t*) calloc (1, sizeof (PerQCIReportListItemFormat_t));
  pqrli->drbqci = 0;

  INTEGER_t *pDCPBytesDL = (INTEGER_t *) calloc (1, sizeof (INTEGER_t));
  INTEGER_t *pDCPBytesUL = (INTEGER_t *) calloc (1, sizeof (INTEGER_t));

  asn_long2INTEGER (pDCPBytesDL, values->m_pDCPBytesDL);
  asn_long2INTEGER (pDCPBytesUL, values->m_pDCPBytesUL);

  pqrli->pDCPBytesDL = pDCPBytesDL;
  pqrli->pDCPBytesUL = pDCPBytesUL;

  ASN_SEQUENCE_ADD (&cuuppmf->perQCIReportList_cuup.list, pqrli);

  ASN_SEQUENCE_ADD (&cuuppmc->plmnList.list, plmnItem);

  pcli->o_CU_UP_PM_Container = *cuuppmc;
  ASN_SEQUENCE_ADD (&ocuup->pf_ContainerList, pcli);
  ranContainer->choice.oCU_UP = ocuup;
  ranContainer->present = PF_Container_PR_oCU_UP;

  free (cuuppmc);
}

void
KpmIndicationMessage::FillOCuCpContainer (PF_Container_t *ranContainer,
                                          Ptr<OCuCpContainerValues> values)
{
  OCUCP_PF_Container_t *ocucp = (OCUCP_PF_Container_t *) calloc (1, sizeof(OCUCP_PF_Container_t));
  long *numActiveUes = (long *) calloc (1, sizeof (long));
  *numActiveUes = long(values->m_numActiveUes);
  ocucp->cu_CP_Resource_Status.numberOfActive_UEs = numActiveUes;
  ranContainer->choice.oCU_CP = ocucp;
  ranContainer->present = PF_Container_PR_oCU_CP;
}

void
KpmIndicationMessage::FillODuContainer (PF_Container_t *ranContainer,
                                        Ptr<ODuContainerValues> values)
{
  ODU_PF_Container_t *odu = (ODU_PF_Container_t *) calloc (1, sizeof (ODU_PF_Container_t));
  
  for (auto cellReport : values->m_cellResourceReportItems)
    {
      NS_LOG_LOGIC ("O-DU: Add Cell Resource Report Item");
      CellResourceReportListItem_t *crrli =
          (CellResourceReportListItem_t *) calloc (1, sizeof (CellResourceReportListItem_t));

      Ptr<OctetString> plmnid = Create<OctetString> (cellReport->m_plmId, 3);
      Ptr<NrCellId> nrcellid = Create<NrCellId> (cellReport->m_nrCellId);
      crrli->nRCGI.pLMN_Identity = plmnid->GetValue ();
      crrli->nRCGI.nRCellIdentity = nrcellid->GetValue ();

      long *dlAvailablePrbs = (long *) calloc (1, sizeof (long));
      *dlAvailablePrbs = cellReport->dlAvailablePrbs;
      crrli->dl_TotalofAvailablePRBs = dlAvailablePrbs;
      
      long *ulAvailablePrbs = (long *) calloc (1, sizeof (long));
      *ulAvailablePrbs = cellReport->ulAvailablePrbs;
      crrli->ul_TotalofAvailablePRBs = ulAvailablePrbs;
      ASN_SEQUENCE_ADD (&odu->cellResourceReportList.list, crrli);
      
      for (auto servedPlmnCell : cellReport->m_servedPlmnPerCellItems)
        {
          NS_LOG_LOGIC ("O-DU: Add Served Plmn Per Cell Item");
          ServedPlmnPerCellListItem_t *sppcl =
              (ServedPlmnPerCellListItem_t *) calloc (1, sizeof (ServedPlmnPerCellListItem_t));
          Ptr<OctetString> servedPlmnId = Create<OctetString> (servedPlmnCell->m_plmId, 3);
          sppcl->pLMN_Identity = servedPlmnId->GetValue ();
          
          EPC_DU_PM_Container_t *edpc =
              (EPC_DU_PM_Container_t *) calloc (1, sizeof (EPC_DU_PM_Container_t));

          for (auto perQciReportItem : servedPlmnCell->m_perQciReportItems)
            {
              NS_LOG_LOGIC ("O-DU: Add Per QCI Report Item");
              PerQCIReportListItem_t *pqrl =
                  (PerQCIReportListItem_t *) calloc (1, sizeof (PerQCIReportListItem_t));
              pqrl->qci = perQciReportItem->m_qci;
              
              NS_ABORT_MSG_IF ((perQciReportItem->m_dlPrbUsage < 0) | (perQciReportItem->m_dlPrbUsage > 100), 
                              "As per ASN definition, dl_PRBUsage should be between 0 and 100");
              long *dlUsedPrbs = (long *) calloc (1, sizeof (long));
              *dlUsedPrbs = perQciReportItem->m_dlPrbUsage;
              pqrl->dl_PRBUsage = dlUsedPrbs;
              NS_LOG_LOGIC ("DL PRBs " << dlUsedPrbs);
              
              NS_ABORT_MSG_IF ((perQciReportItem->m_ulPrbUsage < 0) | (perQciReportItem->m_ulPrbUsage > 100), 
                              "As per ASN definition, ul_PRBUsage should be between 0 and 100");
              long *ulUsedPrbs = (long *) calloc (1, sizeof (long));
              *ulUsedPrbs = perQciReportItem->m_ulPrbUsage;
              pqrl->ul_PRBUsage = ulUsedPrbs;
              ASN_SEQUENCE_ADD (&edpc->perQCIReportList_du.list, pqrl);
            }

          sppcl->du_PM_EPC = edpc;
          ASN_SEQUENCE_ADD (&crrli->servedPlmnPerCellList.list, sppcl);
        }
    }
  ranContainer->choice.oDU = odu;
  ranContainer->present = PF_Container_PR_oDU;
}

void
KpmIndicationMessage::FillAndEncodeKpmIndicationMessage (E2SM_KPM_IndicationMessage_t *descriptor,
                                                         KpmIndicationMessageValues values)
{
  // Create and fill the RAN Container
  PF_Container_t *ranContainer = (PF_Container_t *) calloc (1, sizeof (PF_Container_t));
  FillPmContainer (ranContainer, values.m_pmContainerValues);

  //------- now fill the message
  PM_Containers_Item_t *containers_list =
      (PM_Containers_Item_t *) calloc (1, sizeof (PM_Containers_Item_t));
  containers_list->performanceContainer = ranContainer;

  E2SM_KPM_IndicationMessage_Format1_t *format = (E2SM_KPM_IndicationMessage_Format1_t *) calloc (
      1, sizeof (E2SM_KPM_IndicationMessage_Format1_t));

  ASN_SEQUENCE_ADD (&format->pm_Containers.list, containers_list);

  // Cell Object ID
  CellObjectID_t *cellObjectID = (CellObjectID_t *) calloc (1, sizeof (CellObjectID_t));
  cellObjectID->size = values.m_cellObjectId.length ();
  cellObjectID->buf = (uint8_t *) calloc (1, cellObjectID->size);
  memcpy (cellObjectID->buf, values.m_cellObjectId.c_str (), values.m_cellObjectId.length ());
  format->cellObjectID = *cellObjectID;
  
  // Measurement Information List
  if (values.m_cellMeasurementItems)
  {
      format->list_of_PM_Information = (E2SM_KPM_IndicationMessage_Format1::
                                        E2SM_KPM_IndicationMessage_Format1__list_of_PM_Information *) 
                                        calloc (1, sizeof (E2SM_KPM_IndicationMessage_Format1::
                                        E2SM_KPM_IndicationMessage_Format1__list_of_PM_Information));
    for (auto item : values.m_cellMeasurementItems->GetItems ())
    {
      ASN_SEQUENCE_ADD (&format->list_of_PM_Information->list, item->GetPointer ());
    }
  }
  
  // List of matched UEs
  if (values.m_ueIndications.size () > 0)
  {
    format->list_of_matched_UEs = (E2SM_KPM_IndicationMessage_Format1_t::E2SM_KPM_IndicationMessage_Format1__list_of_matched_UEs*) 
                                   calloc (1, sizeof (E2SM_KPM_IndicationMessage_Format1_t::E2SM_KPM_IndicationMessage_Format1__list_of_matched_UEs));


    for (auto ueIndication : values.m_ueIndications)
      {
        PerUE_PM_Item_t *perUEItem = (PerUE_PM_Item_t *) calloc (1, sizeof (PerUE_PM_Item_t));

        // UE Identity
        perUEItem->ueId = ueIndication->GetId ();
        // xer_fprint (stderr, &asn_DEF_UE_Identity, &perUEItem->ueId);
        // NS_LOG_UNCOND ("Values " << ueIndication->m_drbIPLateDlUEID);

        // List of Measurements PM information
        perUEItem->list_of_PM_Information =
            (PerUE_PM_Item::PerUE_PM_Item__list_of_PM_Information *) calloc (
                1, sizeof (PerUE_PM_Item::PerUE_PM_Item__list_of_PM_Information));

        for (auto measurementItem : ueIndication->GetItems ())
        {
          ASN_SEQUENCE_ADD (&perUEItem->list_of_PM_Information->list,
            measurementItem->GetPointer ());
        }
        ASN_SEQUENCE_ADD (&format->list_of_matched_UEs->list, perUEItem);
      }
  }

  descriptor->present = E2SM_KPM_IndicationMessage_PR_indicationMessage_Format1;
  descriptor->choice.indicationMessage_Format1 = format;
  
  
  NS_LOG_INFO (xer_fprint (stderr, &asn_DEF_E2SM_KPM_IndicationMessage_Format1, format));

  // xer_fprint (stderr, &asn_DEF_PF_Container, ranContainer);
  Encode (descriptor);

  free (cellObjectID);
  // free (ranContainer);
  ASN_STRUCT_FREE (asn_DEF_E2SM_KPM_IndicationMessage_Format1, format);
}

MeasurementItemList::MeasurementItemList ()
{
  m_id = NULL;
}

MeasurementItemList::MeasurementItemList (std::string id)
{
  m_id = Create<OctetString> (id, id.length ());
}

MeasurementItemList::~MeasurementItemList (){};

std::vector<Ptr<MeasurementItem>>
MeasurementItemList::GetItems ()
{
  return m_items;
}

OCTET_STRING_t
MeasurementItemList::GetId ()
{
  NS_ABORT_IF (m_id == NULL);
  return m_id->GetValue ();
}

} // namespace ns3
