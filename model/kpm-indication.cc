/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2022 Northeastern University
 * Copyright (c) 2022 Sapienza, University of Rome
 * Copyright (c) 2022 University of Padova
 * Copyright (c) 2024 Orange Innovation Egypt
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
 *       Mostafa Ashraf <mostafa.ashraf.ext@orange.com>
 *       Aya Kamal <aya.kamal.ext@orange.com>
 *       Abdelrhman Soliman <abdelrhman.soliman.ext@orange.com>
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
//==============================================================
#include "MeasurementRecordItem.h"
#include "MeasurementDataItem.h"
#include "MeasurementInfoItem.h"
#include "LabelInfoItem.h"
#include "MeasurementLabel.h"
#include "E2SM-KPM-IndicationMessage-Format3.h"
#include "UEMeasurementReportItem.h"
#include "UEID-GNB.h"
#include <arpa/inet.h>

#include "conversions.h"

// #include "timing_ms.h"

BIT_STRING_t cp_amf_region_id_to_bit_string (uint8_t src);

uint8_t cp_amf_region_id_to_u8 (BIT_STRING_t src);

/* AMF Set ID */
BIT_STRING_t cp_amf_set_id_to_bit_string (uint16_t val);

uint16_t cp_amf_set_id_to_u16 (BIT_STRING_t src);

/* AMF Pointer */
BIT_STRING_t cp_amf_ptr_to_bit_string (uint8_t src);

uint8_t cp_amf_ptr_to_u8 (BIT_STRING_t src);

OCTET_STRING_t cp_plmn_identity_to_octant_string (uint8_t src);
}

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("KpmIndication");

KpmIndicationHeader::KpmIndicationHeader (GlobalE2nodeType nodeType,
                                          KpmRicIndicationHeaderValues values)
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

uint64_t KpmIndicationHeader::time_now_us_clck() {
  struct timespec tms;

  /* The C11 way */
  /* if (! timespec_get(&tms, TIME_UTC))  */

  /* POSIX.1-2008 way */
  if (clock_gettime(CLOCK_REALTIME,&tms)) {
    return -1;
  }
  /* seconds, multiplied with 1 million */
  uint64_t micros = tms.tv_sec * 1000000;
  /* Add full microseconds */
  micros += tms.tv_nsec/1000;
  /* round up if necessary */
  if (tms.tv_nsec % 1000 >= 500) {
    ++micros;
  }
  NS_LOG_INFO("**micros Timing is " << micros);
  return micros;
}

// KPM ind_msg latency 
OCTET_STRING_t KpmIndicationHeader::get_time_now_us() {

  std::vector<uint8_t> byteArray(sizeof(uint64_t));

  auto val = time_now_us_clck();

  // NS_LOG_DEBUG("**VAL Timing is " << val);

  memcpy(byteArray.data(), &val, sizeof(uint64_t));

  // NS_LOG_DEBUG("ByteArray Size is " << byteArray.size());
  
  OCTET_STRING_t dst = {0};
  
  dst.buf = (uint8_t*)calloc(byteArray.size(), sizeof(uint8_t)); 
  dst.size = byteArray.size();

  memcpy(dst.buf, byteArray.data(), dst.size);

  return dst;
}

OCTET_STRING_t KpmIndicationHeader::int_64_to_octet_string(uint64_t x)
{
    OCTET_STRING_t asn = {0};

    asn.buf = (uint8_t*) calloc(sizeof(x) + 1, sizeof(char));
    memcpy(asn.buf,&x,sizeof(x));
    asn.size = sizeof(x);

    return asn;
}

// OCTET_STRING_t KpmIndicationHeader::int_64_to_octet_string(uint64_t value)
// {
//   std::vector<uint8_t> byteArray(sizeof(uint64_t));
//   memcpy(byteArray.data(), &value, sizeof(uint64_t));

//   OCTET_STRING_t dst = {0};

//   dst.buf = (uint8_t*)calloc(byteArray.size(), sizeof(uint8_t)); 
//   dst.size = byteArray.size();

//   memcpy(dst.buf, byteArray.data(), dst.size);

//   return dst;
// }

uint64_t KpmIndicationHeader::octet_string_to_int_64(OCTET_STRING_t asn)
{
    uint64_t x = {0};

    memcpy(&x, asn.buf, asn.size);

    return x;
}

void
KpmIndicationHeader::Encode (E2SM_KPM_IndicationHeader_t *descriptor)
{
  asn_codec_ctx_t *opt_cod = 0; // disable stack bounds checking
  asn_encode_to_new_buffer_result_s encodedHeader = asn_encode_to_new_buffer (
      opt_cod, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2SM_KPM_IndicationHeader, descriptor);

  if (encodedHeader.result.encoded < 0)
    {
      NS_FATAL_ERROR ("*Error during the encoding of the RIC Indication Header, errno: "
                      << strerror (errno) << ", failed_type "
                      << encodedHeader.result.failed_type->name << ", structure_ptr "
                      << encodedHeader.result.structure_ptr);
    }

  m_buffer = encodedHeader.buffer;
  m_size = encodedHeader.result.encoded;
}

uint64_t ntohll(uint64_t val) {
    uint32_t high = ntohl((uint32_t)(val >> 32));  // Convert high 32 bits
    uint32_t low = ntohl((uint32_t)(val & 0xFFFFFFFF));  // Convert low 32 bits

    return ((uint64_t)high << 32) | low;  // Combine the high and low 32 bits back together
}
void
KpmIndicationHeader::FillAndEncodeKpmRicIndicationHeader (E2SM_KPM_IndicationHeader_t *descriptor,
                                                          KpmRicIndicationHeaderValues values)
{

  NS_LOG_INFO ("FillAndEncodeKpmRicIndicationHeader");
  E2SM_KPM_IndicationHeader_Format1_t *ind_header = (E2SM_KPM_IndicationHeader_Format1_t *) calloc (
      1, sizeof (E2SM_KPM_IndicationHeader_Format1_t));

  Ptr<OctetString> plmnid = Create<OctetString> (values.m_plmId, 3);
  Ptr<BitString> cellId_bstring;
  TimeStamp_t colstartTime;

  colstartTime = get_time_now_us();
// Convert from OCTET STRING to uint64
uint64_t original_time;
memcpy(&original_time, colstartTime.buf, sizeof(uint64_t));

// Convert to network byte order
uint64_t network_order_time = ntohll(original_time);

// Assign network_order_time back to colletStartTime as an OCTET_STRING
ind_header->colletStartTime.buf = (uint8_t *)calloc(8, sizeof(uint8_t));
memcpy(ind_header->colletStartTime.buf, &network_order_time, sizeof(uint64_t));
ind_header->colletStartTime.size = sizeof(uint64_t);

  
   NS_LOG_INFO(" LOOOOOK -> colletStartTime " << octet_string_to_int_64(ind_header->colletStartTime));
  GlobalE2node_ID_t *globalE2nodeIdBuf = (GlobalE2node_ID *) calloc (1, sizeof (GlobalE2node_ID));


  switch (m_nodeType)
    {
    case gNB:
      {

        NS_LOG_INFO ("gNB Header");
        static int sizeGnb = 4; // 3GPP Specs

        cellId_bstring = Create<BitString> (values.m_gnbId, sizeGnb);

        globalE2nodeIdBuf->present = GlobalE2node_ID_PR_gNB;
        GlobalE2node_gNB_ID_t *globalE2node_gNB_ID =
            (GlobalE2node_gNB_ID_t *) calloc (1, sizeof (GlobalE2node_gNB_ID_t));
        globalE2node_gNB_ID->global_gNB_ID.plmn_id = plmnid->GetValue ();
        globalE2node_gNB_ID->global_gNB_ID.gnb_id.present = GNB_ID_Choice_PR_gnb_ID;
        globalE2node_gNB_ID->global_gNB_ID.gnb_id.choice.gnb_ID = cellId_bstring->GetValue ();
        globalE2nodeIdBuf->choice.gNB = globalE2node_gNB_ID;
      }
      break;

    case eNB:
      {
        // Use this.
        NS_LOG_INFO ("eNB Header");
        static int sizeEnb =
            3; // 3GPP TS 36.413 version 14.8.0 Release 14, Section 9.2.1.37 Global eNB ID
        static int unsedSizeEnb = 4;

        cellId_bstring = Create<BitString> (values.m_gnbId, sizeEnb, unsedSizeEnb);

        globalE2nodeIdBuf->present = GlobalE2node_ID_PR_eNB;
        GlobalE2node_eNB_ID_t *globalE2node_eNB_ID =
            (GlobalE2node_eNB_ID_t *) calloc (1, sizeof (GlobalE2node_eNB_ID_t));
        globalE2node_eNB_ID->global_eNB_ID.pLMNIdentity = plmnid->GetValue ();
        globalE2node_eNB_ID->global_eNB_ID.eNB_ID.present = ENB_ID_PR_macro_eNB_ID;
        globalE2node_eNB_ID->global_eNB_ID.eNB_ID.choice.macro_eNB_ID = cellId_bstring->GetValue ();
        globalE2nodeIdBuf->choice.eNB = globalE2node_eNB_ID;
      }
      break;

    case ng_eNB:
      {
        NS_LOG_INFO ("ng_eNB Header");

        static int sizeEnb =
            3; // 3GPP TS 36.413 version 14.8.0 Release 14, Section 9.2.1.37 Global eNB ID
        static int unsedSizeEnb = 4;

        cellId_bstring = Create<BitString> (values.m_gnbId, sizeEnb, unsedSizeEnb);
        globalE2nodeIdBuf->present = GlobalE2node_ID_PR_ng_eNB;

        GlobalE2node_ng_eNB_ID_t *globalE2node_ng_eNB_ID =
            (GlobalE2node_ng_eNB_ID_t *) calloc (1, sizeof (GlobalE2node_ng_eNB_ID_t));

        globalE2node_ng_eNB_ID->global_ng_eNB_ID.plmn_id = plmnid->GetValue ();
        globalE2node_ng_eNB_ID->global_ng_eNB_ID.enb_id.present = ENB_ID_Choice_PR_enb_ID_macro;
        globalE2node_ng_eNB_ID->global_ng_eNB_ID.enb_id.choice.enb_ID_macro =
            cellId_bstring->GetValue ();
        globalE2nodeIdBuf->choice.ng_eNB = globalE2node_ng_eNB_ID;
      }
      break;

    case en_gNB:
      {
        NS_LOG_INFO ("en_gNB Header");

        static int sizeGnb = 4; // 3GPP Specs
        cellId_bstring = Create<BitString> (values.m_gnbId, sizeGnb);

        globalE2nodeIdBuf->present = GlobalE2node_ID_PR_en_gNB;
        GlobalE2node_en_gNB_ID_t *globalE2node_en_gNB_ID =
            (GlobalE2node_en_gNB_ID_t *) calloc (1, sizeof (GlobalE2node_en_gNB_ID_t));
        globalE2node_en_gNB_ID->global_gNB_ID.pLMN_Identity = plmnid->GetValue ();
        globalE2node_en_gNB_ID->global_gNB_ID.en_gNB_ID.present = EN_GNB_ID_PR_en_gNB_ID;
        globalE2node_en_gNB_ID->global_gNB_ID.en_gNB_ID.choice.en_gNB_ID =
            cellId_bstring->GetValue ();
        globalE2nodeIdBuf->choice.en_gNB = globalE2node_en_gNB_ID;
      }
      break;

    default:
      NS_FATAL_ERROR (
          "Unrecognized node type for KpmRicIndicationHeader, value passed: " << m_nodeType);
      break;
    }

  // ind_header->id_GlobalE2node_ID = *globalE2nodeIdBuf;

  NS_LOG_INFO (xer_fprint (stderr, &asn_DEF_E2SM_KPM_IndicationHeader_Format1, ind_header));

  descriptor->indicationHeader_formats.present =
      E2SM_KPM_IndicationHeader__indicationHeader_formats_PR_indicationHeader_Format1;
  descriptor->indicationHeader_formats.choice.indicationHeader_Format1 = ind_header;

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

typedef struct
{
  uint8_t *buf;
  size_t len;
} darsh_byte_array_t;

void
KpmIndicationMessage::Encode (E2SM_KPM_IndicationMessage_t *descriptor)
{

  const bool USER_PRIVATE_BUFFER = true;
  if (USER_PRIVATE_BUFFER)
    {


      darsh_byte_array_t ba_darsh = {.buf = (uint8_t *) malloc (2048), .len = 2048};
      // byte_array_t ba_darsh = {.len = 2048, .buf = (uint8_t *) malloc (2048)};

      asn_enc_rval_t encodedMsg =
          asn_encode_to_buffer (NULL, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2SM_KPM_IndicationMessage,
                                descriptor, ba_darsh.buf, ba_darsh.len);

      if (encodedMsg.encoded < 0)
        {
          assert (encodedMsg.structure_ptr != nullptr);

          NS_FATAL_ERROR ("Error during the encoding of the RIC Indication Message, errno: "
                          << strerror (errno) << ", failed_type " << encodedMsg.failed_type->name
                          << ", structure_ptr " << encodedMsg.structure_ptr);

          NS_LOG_INFO ("Error during encoding ");
        }

      assert(encodedMsg.encoded > -1 && (size_t)encodedMsg.encoded <= ba_darsh.len);
      m_buffer = ba_darsh.buf;
      m_size = encodedMsg.encoded;
    }
  else
    {

      // asn_codec_ctx_t *opt_cod = 0; // disable stack bounds checking
      asn_encode_to_new_buffer_result_s encodedMsg = asn_encode_to_new_buffer (
          NULL, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2SM_KPM_IndicationMessage, descriptor);

      if (encodedMsg.result.encoded < 0)
        {
          assert (encodedMsg.result.structure_ptr != nullptr);

          NS_FATAL_ERROR ("Error during the encoding of the RIC Indication Message, errno: "
                          << strerror (errno) << ", failed_type "
                          << encodedMsg.result.failed_type->name << ", structure_ptr "
                          << encodedMsg.result.structure_ptr);
        }

      m_buffer = encodedMsg.buffer;
      m_size = encodedMsg.result.encoded;
    }
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
  OCUUP_PF_Container_t *ocuup = (OCUUP_PF_Container_t *) calloc (1, sizeof (OCUUP_PF_Container_t));
  PF_ContainerListItem_t *pcli =
      (PF_ContainerListItem_t *) calloc (1, sizeof (PF_ContainerListItem_t));
  pcli->interface_type = NI_Type_x2_u;

  CUUPMeasurement_Container_t *cuuppmc =
      (CUUPMeasurement_Container_t *) calloc (1, sizeof (CUUPMeasurement_Container_t));
  PlmnID_Item_t *plmnItem = (PlmnID_Item_t *) calloc (1, sizeof (PlmnID_Item_t));
  Ptr<OctetString> plmnidstr = Create<OctetString> (values->m_plmId, 3);
  plmnItem->pLMN_Identity = plmnidstr->GetValue ();

  EPC_CUUP_PM_Format_t *cuuppmf =
      (EPC_CUUP_PM_Format_t *) calloc (1, sizeof (EPC_CUUP_PM_Format_t));
  plmnItem->cu_UP_PM_EPC = cuuppmf;
  PerQCIReportListItemFormat_t *pqrli =
      (PerQCIReportListItemFormat_t *) calloc (1, sizeof (PerQCIReportListItemFormat_t));
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
  OCUCP_PF_Container_t *ocucp = (OCUCP_PF_Container_t *) calloc (1, sizeof (OCUCP_PF_Container_t));
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

              NS_ABORT_MSG_IF ((perQciReportItem->m_dlPrbUsage < 0) |
                                   (perQciReportItem->m_dlPrbUsage > 100),
                               "As per ASN definition, dl_PRBUsage should be between 0 and 100");
              long *dlUsedPrbs = (long *) calloc (1, sizeof (long));
              *dlUsedPrbs = perQciReportItem->m_dlPrbUsage;
              pqrl->dl_PRBUsage = dlUsedPrbs;
              NS_LOG_LOGIC ("DL PRBs " << dlUsedPrbs);

              NS_ABORT_MSG_IF ((perQciReportItem->m_ulPrbUsage < 0) |
                                   (perQciReportItem->m_ulPrbUsage > 100),
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
// void KpmIndicationMessage::AddMeasurementType() {

// }
void
KpmIndicationMessage::FillAndEncodeKpmIndicationMessage (E2SM_KPM_IndicationMessage_t *descriptor,
                                                         KpmIndicationMessageValues values)
{

  const bool ENABLE_FORMAT_ONE = false;

  if (ENABLE_FORMAT_ONE)
    {
      // Format 1
      // MOCK for buiding KPM Indication messages

      E2SM_KPM_IndicationMessage_t *ind_message =
          (E2SM_KPM_IndicationMessage_t *) calloc (1, sizeof (E2SM_KPM_IndicationMessage_t));

      E2SM_KPM_IndicationMessage_Format1_t *test_kpm_ind_message =
          (E2SM_KPM_IndicationMessage_Format1_t *) calloc (
              1, sizeof (E2SM_KPM_IndicationMessage_Format1_t));

      MeasurementRecord_t *measure_record =
          (MeasurementRecord_t *) calloc (1, sizeof (MeasurementRecord_t));

      MeasurementRecordItem_t *measure_record_item =
          (MeasurementRecordItem_t *) calloc (1, sizeof (MeasurementRecordItem_t));
      measure_record_item->present = MeasurementRecordItem_PR_integer;
      measure_record_item->choice.integer = 1;

      ASN_SEQUENCE_ADD (&measure_record->list, measure_record_item);

      MeasurementDataItem_t *measure_data_item =
          (MeasurementDataItem_t *) calloc (1, sizeof (MeasurementDataItem_t));

      measure_data_item->measRecord = *measure_record;

      MeasurementData_t *measurement_data =
          (MeasurementData_t *) calloc (1, sizeof (MeasurementData_t));

      ASN_SEQUENCE_ADD (&measurement_data->list, measure_data_item);

      test_kpm_ind_message->measData = *measurement_data;

      ind_message->indicationMessage_formats.present =
          E2SM_KPM_IndicationMessage__indicationMessage_formats_PR_indicationMessage_Format1;

      ind_message->indicationMessage_formats.choice.indicationMessage_Format1 =
          test_kpm_ind_message;

      // ASN_SEQUENCE_ADD(&ind_message->indicationMessage_formats.choice.indicationMessage_Format1, test_kpm_ind_message) ;

      Encode (ind_message);
    }
  else
    {

      // Format 3 Section

      // Format 3 sections,
      // consists of format 1 and some additional args like (UE_ID, and Measurements report) and these are mandatory args.

      // Format 1 Part
      E2SM_KPM_IndicationMessage_t *ind_message =
          (E2SM_KPM_IndicationMessage_t *) calloc (1, sizeof (E2SM_KPM_IndicationMessage_t));

      // 1. Adding a measurement values(item) to the packet.
      MeasurementRecord_t *measure_record =
          (MeasurementRecord_t *) calloc (1, sizeof (MeasurementRecord_t));

      MeasurementRecordItem_t *measure_record_item =
          (MeasurementRecordItem_t *) calloc (1, sizeof (MeasurementRecordItem_t));
      // measure_record_item->present = MeasurementRecordItem_PR_integer;
      // measure_record_item->choice.integer = 1;

      measure_record_item->present = MeasurementRecordItem_PR_real;
      measure_record_item->choice.real = 0;//(rand() % 256) + 0.1;

      // Stream measurement records to list.
      ASN_SEQUENCE_ADD (&measure_record->list, measure_record_item);

      MeasurementDataItem_t *measure_data_item =
          (MeasurementDataItem_t *) calloc (1, sizeof (MeasurementDataItem_t));

      measure_data_item->measRecord = *measure_record;

      MeasurementData_t *measurement_data =
          (MeasurementData_t *) calloc (1, sizeof (MeasurementData_t));

      ASN_SEQUENCE_ADD (&measurement_data->list, measure_data_item);


      // 2. TODO: Optional, but mandatory for flex ric "Fix warning commit".
      /*
      
        m_measName =
      (MeasurementTypeName_t *) calloc (1, sizeof (MeasurementTypeName_t));
  m_measName->buf = (uint8_t *) calloc (1, sizeof (OCTET_STRING));
  m_measName->size = name.length ();
  memcpy (m_measName->buf, name.c_str (), m_measName->size);

  m_measurementItem->pmType.choice.measName = *m_measName;
  m_measurementItem->pmType.present = MeasurementType_PR_measName;

      */
      MeasurementType_t * measurmentType = (MeasurementType_t *) calloc (1, sizeof (MeasurementType_t));
      measurmentType->present = MeasurementType_PR_measName;
      // DRB.RlcSduDelayDl
      std::string name = "DRB.RlcSduDelayDl";
      MeasurementTypeName_t * m_measName = (MeasurementTypeName_t *) calloc (1, sizeof (MeasurementTypeName_t));
      m_measName->buf = (uint8_t *) calloc (1, sizeof (OCTET_STRING));
      m_measName->size = name.length ();
      memcpy (m_measName->buf, name.c_str (), m_measName->size);

      measurmentType->choice.measName = *m_measName;

      MeasurementLabel_t * measure_label = (MeasurementLabel_t *) calloc(1, sizeof(MeasurementLabel_t));

      // TODO: add real plmnid from m_values.
      // Ptr<OctetString> plmnid = Create<OctetString> ("3d9bf3", 3);
      // measure_label->plmnID = plmnid->GetPointer();
      measure_label->noLabel = (long *) malloc (sizeof(long));
      assert (measure_label->noLabel != NULL && "Memory exhausted");
      *measure_label->noLabel = 0;


      LabelInfoItem_t * LabelInfoItem = (LabelInfoItem_t *) calloc (1, sizeof (LabelInfoItem_t));
      LabelInfoItem->measLabel = *measure_label;

      LabelInfoList_t * labelInfoolist = (LabelInfoList_t *) calloc (1, sizeof (LabelInfoList_t));
      ASN_SEQUENCE_ADD (&labelInfoolist->list, LabelInfoItem);

      MeasurementInfoItem_t * infoItem = (MeasurementInfoItem_t *) calloc (1, sizeof (MeasurementInfoItem_t));

      infoItem->labelInfoList = *labelInfoolist;
      infoItem->measType = *measurmentType;

      MeasurementInfoList_t * infoList = (MeasurementInfoList_t *) calloc (1, sizeof (MeasurementInfoList_t));
      ASN_SEQUENCE_ADD (&infoList->list, infoItem);


      // GranularityPeriod_t CollectiveTime = time_now_us();
/*
  msg_frm_1.gran_period_ms = calloc(1, sizeof(*msg_frm_1.gran_period_ms));
  assert(msg_frm_1.gran_period_ms != NULL && "Memory exhausted");
  *msg_frm_1.gran_period_ms = (rand() % 4294967295) + 1;
*/

      // 4. create Indication Message Format 1
      E2SM_KPM_IndicationMessage_Format1_t *test_kpm_ind_message =
          (E2SM_KPM_IndicationMessage_Format1_t *) calloc (
              1, sizeof (E2SM_KPM_IndicationMessage_Format1_t));

      test_kpm_ind_message->measData = *measurement_data;

      test_kpm_ind_message->measInfoList = infoList;
      // Print ASN from Indication Message, format 1.
      NS_LOG_INFO (xer_fprint (stderr, &asn_DEF_E2SM_KPM_IndicationMessage_Format1, test_kpm_ind_message));

      // Format 3 Part

      E2SM_KPM_IndicationMessage_Format3_t *test_kpm_ind_message_format3 =
          (E2SM_KPM_IndicationMessage_Format3_t *) calloc (
              1, sizeof (E2SM_KPM_IndicationMessage_Format3_t));
      assert (test_kpm_ind_message_format3 != nullptr && "Memory exhausted");

      // TODOL Multi UEs
      if(false && values.m_ueIndications.size() > 0) {
          // arrayof<UEID_GNB_t> ues gnb_asn
          NS_LOG_DEBUG("Mina NOWW values.m_ueIndications.size()= " << values.m_ueIndications.size());
          // NS_LOG_INFO(values.m_ueIndications.size());

          if(values.m_ueIndications.size() > 0) {

          NS_LOG_DEBUG("2. NOWW values.m_ueIndications.size()= " << values.m_ueIndications.size());

            UEMeasurementReportList_t* m_ueMeasReportList = (UEMeasurementReportList_t *) calloc (
              values.m_ueIndications.size(), sizeof (UEMeasurementReportList_t));

            // TODO: Empty m_ueIndications always!!!!!
            int _index = 0;
            for(auto ueIndication : values.m_ueIndications) {

              UEMeasurementReportItem_t *UE_data =
                  (UEMeasurementReportItem_t *) calloc (1, sizeof (UEMeasurementReportItem_t));

              UEID_GNB_t *gnb_asn = (UEID_GNB_t *) calloc (1, sizeof (UEID_GNB_t));
              assert (gnb_asn != NULL && "Memory exhausted");

              gnb_asn->amf_UE_NGAP_ID.buf = (uint8_t *) calloc (5, sizeof (uint8_t));
              assert (gnb_asn->amf_UE_NGAP_ID.buf != NULL && "Memory exhausted");

              asn_ulong2INTEGER (&gnb_asn->amf_UE_NGAP_ID,
              static_cast<unsigned long>(KpmIndicationHeader::octet_string_to_int_64(ueIndication->GetId())));

              // gnb_asn->amf_UE_NGAP_ID =  static_cast<INTEGER_t>(KpmIndicationHeader::octet_string_to_int_64(ueIndication->GetId()));

              gnb_asn->guami.aMFPointer = cp_amf_ptr_to_bit_string ((rand () % 2 ^ 6) + 0);
              gnb_asn->guami.aMFSetID = cp_amf_set_id_to_bit_string ((rand () % 2 ^ 10) + 0);
              gnb_asn->guami.aMFRegionID = cp_amf_region_id_to_bit_string ((rand () % 2 ^ 8) + 0);

              gnb_asn->guami.pLMNIdentity = cp_plmn_identity_to_octant_string (rand() % 505, rand() % 99, 2);

              // TODO
              // gnb_asn->ran_UEID 

              gnb_asn->ran_UEID = (RANUEID_t *) calloc(1, sizeof(*gnb_asn->ran_UEID));
              gnb_asn->ran_UEID->buf = (uint8_t *) calloc(8, sizeof(*gnb_asn->ran_UEID->buf));
              uint8_t ran_ue_id[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
              memcpy(gnb_asn->ran_UEID->buf, ran_ue_id, 8);
              gnb_asn->ran_UEID->size = 8;
              /* 
                gnb_asn->ran_UEID->buf = calloc(8, sizeof(*gnb_asn->ran_UEID->buf));
              */

              UEID_t *ue_ID = (UEID_t *) calloc (1, sizeof (UEID_t));
              ue_ID->present = UEID_PR_gNB_UEID;
              ue_ID->choice.gNB_UEID = gnb_asn;

              UE_data->ueID = *ue_ID;
              UE_data->measReport = *test_kpm_ind_message;

              // TODO
              ASN_SEQUENCE_ADD(&m_ueMeasReportList[_index++].list, UE_data);


              // test_kpm_ind_message_format3->ueMeasReportList.list[index] = UE_data;

            }
            test_kpm_ind_message_format3->ueMeasReportList = *m_ueMeasReportList;
            // ASN_SEQUENCE_ADD (&test_kpm_ind_message_format3->ueMeasReportList.list, m_ueMeasReportList);
          }
      } else {
          UEMeasurementReportItem_t *UE_data =
                  (UEMeasurementReportItem_t *) calloc (1, sizeof (UEMeasurementReportItem_t));

              UEID_GNB_t *gnb_asn = (UEID_GNB_t *) calloc (1, sizeof (UEID_GNB_t));
              assert (gnb_asn != NULL && "Memory exhausted");

              gnb_asn->amf_UE_NGAP_ID.buf = (uint8_t *) calloc (5, sizeof (uint8_t));
              assert (gnb_asn->amf_UE_NGAP_ID.buf != NULL && "Memory exhausted");

              // asn_ulong2INTEGER (&gnb_asn->amf_UE_NGAP_ID, rand() % 112358132134);
              asn_ulong2INTEGER (&gnb_asn->amf_UE_NGAP_ID, 1);
              // asn_ulong2INTEGER (&gnb_asn->amf_UE_NGAP_ID, 112358132134);


              // dummy values
              // gnb_asn->guami.aMFPointer = cp_amf_ptr_to_bit_string ((rand () % 2 ^ 6) + 0);
              // gnb_asn->guami.aMFSetID = cp_amf_set_id_to_bit_string ((rand () % 2 ^ 10) + 0);
              // gnb_asn->guami.aMFRegionID = cp_amf_region_id_to_bit_string ((rand () % 2 ^ 8) + 0);
              gnb_asn->guami.aMFPointer = cp_amf_ptr_to_bit_string (0);
              gnb_asn->guami.aMFSetID = cp_amf_set_id_to_bit_string (1);
              gnb_asn->guami.aMFRegionID = cp_amf_region_id_to_bit_string (2 + 0);

              // MCC_MNC_TO_PLMNID((uint16_t)(208), (uint16_t)(01) , (uint8_t)2, &gnb_asn->guami.pLMNIdentity);
              // gnb_asn->guami.pLMNIdentity = cp_plmn_identity_to_octant_string (505, 01, 2);
              gnb_asn->guami.pLMNIdentity = cp_plmn_identity_to_octant_string (rand() % 505, rand() % 99, 2);

              // TODO
              // gnb_asn->ran_UEID 
              gnb_asn->ran_UEID = (RANUEID_t *) calloc(1, sizeof(*gnb_asn->ran_UEID));
              gnb_asn->ran_UEID->buf = (uint8_t *) calloc(8, sizeof(*gnb_asn->ran_UEID->buf));
              uint8_t ran_ue_id[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
              memcpy(gnb_asn->ran_UEID->buf, ran_ue_id, 8);
              gnb_asn->ran_UEID->size = 8;

              UEID_t *ue_ID = (UEID_t *) calloc (1, sizeof (UEID_t));
              ue_ID->present = UEID_PR_gNB_UEID;
              ue_ID->choice.gNB_UEID = gnb_asn;

              UE_data->ueID = *ue_ID;

              // Adding format 1 part with format 3 part.
              UE_data->measReport = *test_kpm_ind_message;

              // equivelent to code below -- old  code
              // ASN_SEQUENCE_ADD (&UE_data_list->list,UE_data);
              // test_kpm_ind_message_format3->ueMeasReportList = *UE_data_list ;
              ASN_SEQUENCE_ADD (&test_kpm_ind_message_format3->ueMeasReportList.list, UE_data);
      }

      NS_LOG_INFO (xer_fprint (stderr, &asn_DEF_E2SM_KPM_IndicationMessage_Format3, test_kpm_ind_message_format3));

      // NS_LOG_INFO (xer_fprint (stderr, &asn_DEF_E2SM_KPM_IndicationMessage, test_kpm_ind_message_format3));

// asn_DEF_E2SM_KPM_IndicationMessage

      ind_message->indicationMessage_formats.present =
          E2SM_KPM_IndicationMessage__indicationMessage_formats_PR_indicationMessage_Format3;
      ind_message->indicationMessage_formats.choice.indicationMessage_Format3 =
          test_kpm_ind_message_format3;

      NS_LOG_INFO (xer_fprint (stderr, &asn_DEF_E2SM_KPM_IndicationMessage, ind_message));

      Encode (ind_message);

      printf (" \n *** Done Encoding INDICATION Message ****** \n ");
    }
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
