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
 *       Mina Yonan <mina.awadallah@orange.com>
 *       Mostafa Ashraf <mostafa.ashraf.ext@orange.com>
 *       Aya Kamal <aya.kamal.ext@orange.com>
 *       Abdelrhman Soliman <abdelrhman.soliman.ext@orange.com>
 */

#include <ns3/kpm-indication.h>
// #include "kpm-indication.h"

#include <ns3/asn1c-types.h>
#include <ns3/log.h>

extern "C" {
// #include "E2SM-KPM-IndicationHeader-Format1.h"
// #include "E2SM-KPM-IndicationMessage-Format1.h"
// #include "GlobalE2node-ID.h"
// #include "GlobalE2node-gNB-ID.h"
// #include "GlobalE2node-eNB-ID.h"
// #include "GlobalE2node-ng-eNB-ID.h"
// #include "GlobalE2node-en-gNB-ID.h"
// #include "NRCGI.h"
// #include "PM-Containers-Item.h"
// #include "RIC-EventTriggerStyle-Item.h"
// #include "RIC-ReportStyle-Item.h"
// #include "TimeStamp.h"
// #include "CUUPMeasurement-Container.h"
// #include "PlmnID-Item.h"
// #include "EPC-CUUP-PM-Format.h"
// #include "PerQCIReportListItemFormat.h"
// #include "PerUE-PM-Item.h"
// #include "PM-Info-Item.h"
// #include "MeasurementInfoList.h"
// #include "CellObjectID.h"
// #include "CellResourceReportListItem.h"
// #include "ServedPlmnPerCellListItem.h"
// #include "EPC-DU-PM-Container.h"
// #include "PerQCIReportListItem.h"
// //==============================================================
// #include "MeasurementRecordItem.h"
// #include "MeasurementDataItem.h"
// #include "MeasurementInfoItem.h"
// #include "LabelInfoItem.h"
// #include "MeasurementLabel.h"
// #include "E2SM-KPM-IndicationMessage-Format3.h"
// #include "UEMeasurementReportItem.h"
// #include "UEID-GNB.h"
#include <arpa/inet.h>

#include "conversions.h"
#include <assert.h>
#include <cassert>
#include <any>

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

uint64_t
KpmIndicationHeader::time_now_us_clck ()
{
  struct timespec tms;

  /* The C11 way */
  /* if (! timespec_get(&tms, TIME_UTC))  */

  /* POSIX.1-2008 way */
  if (clock_gettime (CLOCK_REALTIME, &tms))
    {
      return -1;
    }
  /* seconds, multiplied with 1 million */
  uint64_t micros = tms.tv_sec * 1000000;
  /* Add full microseconds */
  micros += tms.tv_nsec / 1000;
  /* round up if necessary */
  if (tms.tv_nsec % 1000 >= 500)
    {
      ++micros;
    }
  NS_LOG_INFO ("**micros Timing is " << micros);
  return micros;
}

// KPM ind_msg latency
OCTET_STRING_t
KpmIndicationHeader::get_time_now_us ()
{

  std::vector<uint8_t> byteArray (sizeof (uint64_t));

  auto val = time_now_us_clck ();

  // NS_LOG_DEBUG("**VAL Timing is " << val);

  memcpy (byteArray.data (), &val, sizeof (uint64_t));

  // NS_LOG_DEBUG("ByteArray Size is " << byteArray.size());

  OCTET_STRING_t dst = {0};

  dst.buf = (uint8_t *) calloc (byteArray.size (), sizeof (uint8_t));
  dst.size = byteArray.size ();

  memcpy (dst.buf, byteArray.data (), dst.size);

  return dst;
}

OCTET_STRING_t
KpmIndicationHeader::int_64_to_octet_string (uint64_t x)
{
  OCTET_STRING_t asn = {0};

  asn.buf = (uint8_t *) calloc (sizeof (x) + 1, sizeof (char));
  memcpy (asn.buf, &x, sizeof (x));
  asn.size = sizeof (x);

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

uint64_t
KpmIndicationHeader::octet_string_to_int_64 (OCTET_STRING_t asn)
{
  uint64_t x = {0};

  memcpy (&x, asn.buf, asn.size);

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

uint64_t
ntohll (uint64_t val)
{
  uint32_t high = ntohl ((uint32_t) (val >> 32)); // Convert high 32 bits
  uint32_t low = ntohl ((uint32_t) (val & 0xFFFFFFFF)); // Convert low 32 bits
  return ((uint64_t) high << 32) | low; // Combine the high and low 32 bits back together
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

  colstartTime = get_time_now_us ();
  // Convert from OCTET STRING to uint64
  uint64_t original_time;
  memcpy (&original_time, colstartTime.buf, sizeof (uint64_t));

  // Convert to network byte order
  uint64_t network_order_time = ntohll (original_time);

  // Assign network_order_time back to colletStartTime as an OCTET_STRING
  ind_header->colletStartTime.buf = (uint8_t *) calloc (8, sizeof (uint8_t));
  memcpy (ind_header->colletStartTime.buf, &network_order_time, sizeof (uint64_t));
  ind_header->colletStartTime.size = sizeof (uint64_t);

  NS_LOG_INFO (" LOOOOOK -> colletStartTime "
               << octet_string_to_int_64 (ind_header->colletStartTime));
  GlobalE2node_ID_t *globalE2nodeIdBuf = (GlobalE2node_ID *) calloc (1, sizeof (GlobalE2node_ID));

  switch (m_nodeType)
    {
      case gNB: {

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

      case eNB: {
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

      case ng_eNB: {
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

      case en_gNB: {
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

KpmIndicationMessage::KpmIndicationMessage (KpmIndicationMessageValues values,
                                            const std::map<std::string, std::any> &s_map)
    : SubscriptionR_map{s_map}
{
  E2SM_KPM_IndicationMessage_t *descriptor = new E2SM_KPM_IndicationMessage_t ();
  // printf("The map: %ld \n", s_map.size());
  // for (const auto& pair : SubscriptionR_map) {
  //   // Print the key
  //   printf("The map2:\n");
  //   printf("%s: ", pair.first.c_str());
  //       if (pair.second.type() == typeid(int)) {
  //           printf("%d\n", std::any_cast<int>(pair.second));
  //       } else if (pair.second.type() == typeid(std::string)) {
  //           printf("%s\n", std::any_cast<std::string>(pair.second).c_str());
  //       } else if (pair.second.type() == typeid(double)) {
  //           printf("%f\n", std::any_cast<double>(pair.second));
  //       } else if (pair.second.type() == typeid(uint64_t)) {
  //           printf("%lu\n", std::any_cast<uint64_t>(pair.second));
  //       } else {
  //           printf("Unknown type\n");
  //       }}
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
  NS_LOG_DEBUG ("m_cellObjectId=" << values.m_cellObjectId);
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

  const bool USE_PRIVATE_BUFFER = false;
  if (USE_PRIVATE_BUFFER)
    {

      darsh_byte_array_t ba_darsh = {.buf = (uint8_t *) malloc (2048), .len = 2048};
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

      NS_LOG_LOGIC ("encodedMsg.encoded=" << encodedMsg.encoded);
      NS_LOG_LOGIC ("ba_darsh.len=" << ba_darsh.len);

      assert (encodedMsg.encoded > -1);
      assert ((size_t) encodedMsg.encoded <= ba_darsh.len);

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
  *numActiveUes = long (values->m_numActiveUes);
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
              NS_LOG_LOGIC ("DL PRBs " << *dlUsedPrbs);

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

// int KpmIndicationMessage::count = 0;
// std::mutex KpmIndicationMessage::mtx;

std::pair<MeasurementInfoItem_t *, MeasurementDataItem_t *>
KpmIndicationMessage::getMesInfoItem (const Ptr<MeasurementItem> &mesItem,
                                      const OCTET_STRING_t &IMSI)
{

  // // 1. Adding a measurement values(item) to the packet.
  MeasurementDataItem_t *measureDataItem = nullptr;

  auto item = mesItem->GetValue ();

  MeasurementType_t *measurmentType = (MeasurementType_t *) calloc (1, sizeof (MeasurementType_t));
  *measurmentType = item.pmType;

  if (item.pmType.present == MeasurementType_PR_measName)
    {
      switch (item.pmVal.present)
        {
          case MeasurementValue_PR_NOTHING: {
            NS_LOG_INFO ("\n MeasurementValue_PR_NOTHING");
            break;
          }
          case MeasurementValue_PR_valueInt: {
            NS_LOG_INFO ("\n 	MeasurementValue_PR_valueInt" << item.pmVal.choice.valueInt);
            measureDataItem = getMesDataItem (static_cast<double> (item.pmVal.choice.valueInt));
            break;
          }
          case MeasurementValue_PR_valueReal: {
            NS_LOG_INFO ("\n MeasurementValue_PR_valueReal" << item.pmVal.choice.valueReal);
            measureDataItem = getMesDataItem (item.pmVal.choice.valueReal);
            break;
          }
          case MeasurementValue_PR_noValue: {
            NS_LOG_INFO ("\n MeasurementValue_PR_noValue" << item.pmVal.choice.noValue);
            break;
          }
          case MeasurementValue_PR_valueRRC: {
            // LabelInfoList_t *labelInfoolist = (LabelInfoList_t *) calloc (1, sizeof (LabelInfoList_t));

            auto measName =
                std::string (reinterpret_cast<const char *> (item.pmType.choice.measName.buf),
                             item.pmType.choice.measName.size);
            NS_LOG_INFO ("\n 	item.pmType.choice.measName" << measName);

            // Fill Neighbours cells of 5G, MeasResultNeighCells_PR_measResultListNR
            L3_RRC_Measurements *rrc = item.pmVal.choice.valueRRC;
            if (measName == "HO.TrgtCellQual.RS-SINR.UEID")
              {
                if (rrc->measResultNeighCells != nullptr &&
                    rrc->measResultNeighCells->present == MeasResultNeighCells_PR_measResultListNR)
                  {
                    assert (rrc->measResultNeighCells->choice.measResultListNR != nullptr);
                    try
                      {
                        if (rrc->measResultNeighCells->choice.measResultListNR->list.count > 0)
                          {
                            MeasResultListNR *_measResultListNR =
                                rrc->measResultNeighCells->choice.measResultListNR;
                            measureDataItem =
                                getMesDataItem (_measResultListNR, IMSI, measurmentType);
                          }

                    } catch (const std::exception &e)
                      {
                        std::cerr << "Error is" << e.what () << '\n';
                    }
                  }
              }
            // Fill Serving cells of 5G, ServingCellMeasurements_PR_nr_measResultServingMOList
            else if (measName == "HO.SrcCellQual.RS-SINR.UEID")
              {
                if (rrc->servingCellMeasurements != nullptr &&
                    rrc->servingCellMeasurements->present ==
                        ServingCellMeasurements_PR_nr_measResultServingMOList)
                  {
                    assert (rrc->servingCellMeasurements->choice.nr_measResultServingMOList !=
                            nullptr);
                    try
                      {
                        if (rrc->servingCellMeasurements->choice.nr_measResultServingMOList->list
                                .count > 0)
                          {
                            MeasResultServMOList *_MeasResultServMO =
                                rrc->servingCellMeasurements->choice.nr_measResultServingMOList;
                            measureDataItem =
                                getMesDataItem (_MeasResultServMO, IMSI, measurmentType);
                          }
                    } catch (const std::exception &e)
                      {
                        std::cerr << "Error is" << e.what () << '\n';
                    }
                  }
              }
            else
              {
                NS_LOG_INFO ("\n 	L3_RRC_Measurement but not handled, measName" << measName);
              }

            break;
          }
          default: {
            break;
          }
        }
    }
  else
    {
      NS_LOG_INFO ("\n 	item.pmType.choice.measID" << item.pmType.choice.measID);
    }

  MeasurementLabel_t *measure_label =
      (MeasurementLabel_t *) calloc (1, sizeof (MeasurementLabel_t));
  measure_label->noLabel = (long *) malloc (sizeof (long));
  assert (measure_label->noLabel != NULL && "Memory exhausted");
  *measure_label->noLabel = 0;
  LabelInfoItem_t *LabelInfoItem = (LabelInfoItem_t *) calloc (1, sizeof (LabelInfoItem_t));
  LabelInfoItem->measLabel = *measure_label;
  LabelInfoList_t *labelInfoolist = (LabelInfoList_t *) calloc (1, sizeof (LabelInfoList_t));
  ASN_SEQUENCE_ADD (&labelInfoolist->list, LabelInfoItem);

  MeasurementInfoItem_t *infoItem =
      (MeasurementInfoItem_t *) calloc (1, sizeof (MeasurementInfoItem_t));

  infoItem->labelInfoList = *labelInfoolist;
  infoItem->measType = *measurmentType;

  return std::make_pair (infoItem, measureDataItem);
}

MeasurementDataItem_t *
KpmIndicationMessage::getMesDataItem (const double &realVal)
{

  // 1. Adding a measurement values(item) to the packet.
  MeasurementRecord_t *measure_record =
      (MeasurementRecord_t *) calloc (1, sizeof (MeasurementRecord_t));

  MeasurementRecordItem_t *measure_record_item =
      (MeasurementRecordItem_t *) calloc (1, sizeof (MeasurementRecordItem_t));

  measure_record_item->present = MeasurementRecordItem_PR_real;
  measure_record_item->choice.real = realVal;

  // Stream measurement records to list.
  ASN_SEQUENCE_ADD (&measure_record->list, measure_record_item);

  MeasurementDataItem_t *measure_data_item =
      (MeasurementDataItem_t *) calloc (1, sizeof (MeasurementDataItem_t));

  measure_data_item->measRecord = *measure_record;

  return measure_data_item;
}

// Fill Serving cells
MeasurementDataItem_t *
KpmIndicationMessage::getMesDataItem (const MeasResultServMOList *_MeasResultServMOList,
                                      const OCTET_STRING_t &IMSI, MeasurementType_t *measurmentType)
{

  MeasurementRecord_t *measure_record =
      (MeasurementRecord_t *) calloc (1, sizeof (MeasurementRecord_t));

  MeasurementDataItem_t *measure_data_item =
      (MeasurementDataItem_t *) calloc (1, sizeof (MeasurementDataItem_t));

  for (int i = 0; i < _MeasResultServMOList->list.count; i++)
    {
      // MeasurementRecordItem_t *measure_record_item1 =
      //     (MeasurementRecordItem_t *) calloc (1, sizeof (MeasurementRecordItem_t));
      // measure_record_item1->present = MeasurementRecordItem_PR_integer;
      // measure_record_item1->choice.integer =
      //     static_cast<unsigned long> (_MeasResultServMOList->list.array[i]->servCellId);

      updateServingMsg (measurmentType, _MeasResultServMOList->list.array[i]->servCellId, IMSI);

      // MeasurementRecordItem_t *measure_record_item2 =
      //     (MeasurementRecordItem_t *) calloc (1, sizeof (MeasurementRecordItem_t));
      // measure_record_item2->present = MeasurementRecordItem_PR_integer;
      // measure_record_item2->choice.integer = static_cast<unsigned long> (
      //     *(_MeasResultServMOList->list.array[i]->measResultServingCell.physCellId));

      MeasurementRecordItem_t *measure_record_item3 =
          (MeasurementRecordItem_t *) calloc (1, sizeof (MeasurementRecordItem_t));
      measure_record_item3->present = MeasurementRecordItem_PR_real;
      measure_record_item3->choice.real =
          *(_MeasResultServMOList->list.array[i]
                ->measResultServingCell.measResult.cellResults.resultsSSB_Cell->sinr);

      // Stream measurement records to list.
      // ASN_SEQUENCE_ADD (&measure_record->list, measure_record_item1);
      // ASN_SEQUENCE_ADD (&measure_record->list, measure_record_item2);
      ASN_SEQUENCE_ADD (&measure_record->list, measure_record_item3);
    }

  measure_data_item->measRecord = *measure_record;

  return measure_data_item;
}

// Fill Neighbours cells
MeasurementDataItem_t *
KpmIndicationMessage::getMesDataItem (const MeasResultListNR *_measResultListNR,
                                      const OCTET_STRING_t &IMSI, MeasurementType_t *measurmentType)
{

  MeasurementRecord_t *measure_record =
      (MeasurementRecord_t *) calloc (1, sizeof (MeasurementRecord_t));

  MeasurementDataItem_t *measure_data_item =
      (MeasurementDataItem_t *) calloc (1, sizeof (MeasurementDataItem_t));

  for (int i = 0; i < _measResultListNR->list.count; i++)
    {
      if (*(_measResultListNR->list.array[i]->physCellId) < 0)
        {
          updateNeighMsg (measurmentType, IMSI, *(_measResultListNR->list.array[i]->physCellId));
          continue;
        }
      MeasurementRecordItem_t *measure_record_item2 =
          (MeasurementRecordItem_t *) calloc (1, sizeof (MeasurementRecordItem_t));
      measure_record_item2->present = MeasurementRecordItem_PR_integer;
      measure_record_item2->choice.integer = *(_measResultListNR->list.array[i]->physCellId);

      MeasurementRecordItem_t *measure_record_item =
          (MeasurementRecordItem_t *) calloc (1, sizeof (MeasurementRecordItem_t));

      measure_record_item->present = MeasurementRecordItem_PR_real;
      measure_record_item->choice.real =
          *(_measResultListNR->list.array[i]
                ->measResult.cellResults.resultsSSB_Cell->sinr); //(rand() % 256) + 0.1;

      // Stream measurement records to list.
      ASN_SEQUENCE_ADD (&measure_record->list, measure_record_item);
      ASN_SEQUENCE_ADD (&measure_record->list, measure_record_item2);
    }

  measure_data_item->measRecord = *measure_record;

  return measure_data_item;
}

// E-UTRA is the air interface of 3rd Generation Partnership Project (3GPP)
// Long Term Evolution (LTE) upgrade path for mobile networks.

// ueVal->AddItem<Ptr<L3RrcMeasurements>> ("HO.SrcCellQual.RS-SINR.UEID", l3RrcMeasurementServing);
// ueVal->AddItem<Ptr<L3RrcMeasurements>> ("HO.TrgtCellQual.RS-SINR.UEID", l3RrcMeasurementNeigh);
// RIC Style 4 cellID, UserID, SINR Map
// Event trigger - PrbUsage
// 0.371 enbdev 2 UE 3 L3 neigh 3 SINR -3.92736 sinr encoded 38 first insert
// 0.371 enbdev 2 UE 3 L3 neigh 4 SINR -11.9135 sinr encoded 22 first insert
// 0.371 enbdev 2 UE 4 L3 serving SINR -13.7703 L3 serving SINR 3gpp 19
// 0.371 enbdev 2 UE 4 L3 neigh 4 SINR -10.8886 sinr encoded 24 first insert
// 0.371 enbdev 2 UE 4 L3 neigh 3 SINR -34.2883 sinr encoded 0 first insert
// 0.371 enbdev 2 UE 1 L3 serving SINR -0.616781 L3 serving SINR 3gpp 45
// 0.371 enbdev 2 UE 1 L3 neigh 3 SINR -9.88701 sinr encoded 26 first insert

void
KpmIndicationMessage::updateServingMsg (MeasurementType_t *measurmentType, const int &cellID,
                                        const OCTET_STRING_t &IMSI)
{
  // 0.371 enbdev 2 UE 3 L3 serving SINR 3.28529 L3 serving SINR 3gpp 53
  std::ostringstream oss;
  // L3servingSINR3gpp_cell_XX
  // L3servingSINR3gpp_cell_XX_UEID_XX
  oss << "L3servingSINR3gpp_cell_" << cellID << "_UEID_" << IMSI.buf;
  memcpy (measurmentType->choice.measName.buf, oss.str ().data (), oss.str ().size ());
  measurmentType->choice.measName.size = oss.str ().size ();
}

void
KpmIndicationMessage::updateNeighMsg (MeasurementType_t *measurmentType, const OCTET_STRING_t &IMSI,
                                      const int &cellID)
{
  // 0.371 enbdev 2 UE 1 L3 neigh 4 SINR -19.4777 sinr encoded 7 first insert
  std::ostringstream oss;
  // L3neighSINR_cell_XX
  oss << "L3neighSINRListOf_UEID_" << IMSI.buf << "_of_Cell_" << (-cellID);
  memcpy (measurmentType->choice.measName.buf, oss.str ().data (), oss.str ().size ());
  measurmentType->choice.measName.size = oss.str ().size ();
}

void
KpmIndicationMessage::FillKpmIndicationMessageFormat1 (
    E2SM_KPM_IndicationMessage_Format1 *ind_msg_f_1)
{
  // 1. Adding a measurement values (item) to the packet.
  MeasurementData_t *measurement_data =
      (MeasurementData_t *) calloc (1, sizeof (MeasurementData_t));

  MeasurementRecord_t *measure_record =
      (MeasurementRecord_t *) calloc (1, sizeof (MeasurementRecord_t));

  MeasurementRecordItem_t *measure_record_item =
      (MeasurementRecordItem_t *) calloc (1, sizeof (MeasurementRecordItem_t));
  measure_record_item->present = MeasurementRecordItem_PR_real;
  measure_record_item->choice.real = 0; //(rand() % 256) + 0.1;
  // Stream measurement records to list.
  ASN_SEQUENCE_ADD (&measure_record->list, measure_record_item);
  MeasurementDataItem_t *measure_data_item =
      (MeasurementDataItem_t *) calloc (1, sizeof (MeasurementDataItem_t));
  measure_data_item->measRecord = *measure_record;

  ASN_SEQUENCE_ADD (&measurement_data->list, measure_data_item);

  // 2. TODO: Optional, but mandatory for flex ric "Fix warning commit".
  MeasurementType_t *measurmentType = (MeasurementType_t *) calloc (1, sizeof (MeasurementType_t));
  measurmentType->present = MeasurementType_PR_measName;
  // DRB.RlcSduDelayDl
  std::string name = "DRB.RlcSduDelayDl_Fake";
  MeasurementTypeName_t *m_measName =
      (MeasurementTypeName_t *) calloc (1, sizeof (MeasurementTypeName_t));
  m_measName->buf = (uint8_t *) calloc (1, sizeof (OCTET_STRING));
  m_measName->size = name.length ();
  memcpy (m_measName->buf, name.c_str (), m_measName->size);
  measurmentType->choice.measName = *m_measName;
  MeasurementLabel_t *measure_label =
      (MeasurementLabel_t *) calloc (1, sizeof (MeasurementLabel_t));
  // TODO: add real plmnid from m_values.
  // Ptr<OctetString> plmnid = Create<OctetString> ("3d9bf3", 3);
  // measure_label->plmnID = plmnid->GetPointer();
  measure_label->noLabel = (long *) malloc (sizeof (long));
  assert (measure_label->noLabel != NULL && "Memory exhausted");
  *measure_label->noLabel = 0;
  LabelInfoItem_t *LabelInfoItem = (LabelInfoItem_t *) calloc (1, sizeof (LabelInfoItem_t));
  LabelInfoItem->measLabel = *measure_label;
  LabelInfoList_t *labelInfoolist = (LabelInfoList_t *) calloc (1, sizeof (LabelInfoList_t));
  ASN_SEQUENCE_ADD (&labelInfoolist->list, LabelInfoItem);
  MeasurementInfoItem_t *infoItem =
      (MeasurementInfoItem_t *) calloc (1, sizeof (MeasurementInfoItem_t));
  infoItem->labelInfoList = *labelInfoolist;
  infoItem->measType = *measurmentType;

  MeasurementInfoList_t *infoList =
      (MeasurementInfoList_t *) calloc (1, sizeof (MeasurementInfoList_t));
  ASN_SEQUENCE_ADD (&infoList->list, infoItem);

  // 3. Adding Granularity Period
  GranularityPeriod_t *granPeriodMS =
      (GranularityPeriod_t *) calloc (1, sizeof (GranularityPeriod_t));
  *granPeriodMS = 100;

  ind_msg_f_1->measData = *measurement_data;
  ind_msg_f_1->measInfoList = infoList;
  ind_msg_f_1->granulPeriod = granPeriodMS;

  // Print ASN from Indication Message, format 1.
  NS_LOG_INFO (xer_fprint (stderr, &asn_DEF_E2SM_KPM_IndicationMessage_Format1, ind_msg_f_1));
}

void
KpmIndicationMessage::FillKpmIndicationMessageFormat1 (
    E2SM_KPM_IndicationMessage_Format1 *ind_msg_f_1, const Ptr<MeasurementItemList> ueIndication)
{

  // 1. Adding a measurement values (item) to the packet.MeasurementRecord_t *measure_record =
  // 2. Adding a Measurement Info Item(s), Optional, but mandatory for flex ric "Fix warning commit".
  // TODO: Iterate over whole mesItems
  MeasurementData_t *measurementDataList =
      (MeasurementData_t *) calloc (1, sizeof (MeasurementData_t));

  MeasurementInfoList_t *infoList =
      (MeasurementInfoList_t *) calloc (1, sizeof (MeasurementInfoList_t));
  auto UE_MeasurementItems = ueIndication->GetItems ();
  auto IMSI = ueIndication->GetId ();
  for (const auto &UE_MeasurementItem : UE_MeasurementItems)
    {
      auto _pairInfo = getMesInfoItem (UE_MeasurementItem, IMSI);

      MeasurementInfoItem_t *infoItem = _pairInfo.first;
      MeasurementDataItem_t *measureDataItem = _pairInfo.second;

      ASN_SEQUENCE_ADD (&measurementDataList->list, measureDataItem);
      ASN_SEQUENCE_ADD (&infoList->list, infoItem);
    }

  // 3. Adding Granularity Period
  GranularityPeriod_t *granPeriodMS =
      (GranularityPeriod_t *) calloc (1, sizeof (GranularityPeriod_t));
  if (SubscriptionR_map.find ("Granularity Period") != SubscriptionR_map.end ())
    {
      *granPeriodMS = std::any_cast<GranularityPeriod_t> (SubscriptionR_map["Granularity Period"]);
    }
  else
    {
      // TODO: replace by global system preodicity(GranularityPeriod).
      // TODO: Abort
      *granPeriodMS = 100;
    }

  ind_msg_f_1->measData = *measurementDataList;
  ind_msg_f_1->measInfoList = infoList;
  ind_msg_f_1->granulPeriod = granPeriodMS;

  // Print ASN from Indication Message, format 1.
  // NS_LOG_INFO (xer_fprint (stderr, &asn_DEF_E2SM_KPM_IndicationMessage_Format1, ind_msg_f_1));
}

void
KpmIndicationMessage::FillKpmIndicationMessageFormat3 (
    E2SM_KPM_IndicationMessage_Format3 *ind_msg_f_3,
    E2SM_KPM_IndicationMessage_Format1 *ind_msg_f_1, const KpmIndicationMessageValues &values)
{

  UEMeasurementReportItem_t *UE_data =
      (UEMeasurementReportItem_t *) calloc (1, sizeof (UEMeasurementReportItem_t));

  UEID_GNB_t *gnb_asn = (UEID_GNB_t *) calloc (1, sizeof (UEID_GNB_t));
  assert (gnb_asn != NULL && "Memory exhausted");

  gnb_asn->amf_UE_NGAP_ID.buf = (uint8_t *) calloc (5, sizeof (uint8_t));
  assert (gnb_asn->amf_UE_NGAP_ID.buf != NULL && "Memory exhausted");

  asn_ulong2INTEGER (&gnb_asn->amf_UE_NGAP_ID, rand () % 112358132134);
  // RRU_PrbUsedDl
  // dummy values
  gnb_asn->guami.aMFPointer = cp_amf_ptr_to_bit_string ((rand () % 2 ^ 6) + 0);
  gnb_asn->guami.aMFSetID = cp_amf_set_id_to_bit_string ((rand () % 2 ^ 10) + 0);
  gnb_asn->guami.aMFRegionID = cp_amf_region_id_to_bit_string ((rand () % 2 ^ 8) + 0);

  // MCC_MNC_TO_PLMNID((uint16_t)(208), (uint16_t)(01) , (uint8_t)2, &gnb_asn->guami.pLMNIdentity);
  gnb_asn->guami.pLMNIdentity = cp_plmn_identity_to_octant_string (rand () % 505, rand () % 99, 2);

  // TODO
  // gnb_asn->ran_UEID
  gnb_asn->ran_UEID = (RANUEID_t *) calloc (1, sizeof (*gnb_asn->ran_UEID));
  gnb_asn->ran_UEID->buf = (uint8_t *) calloc (8, sizeof (*gnb_asn->ran_UEID->buf));
  uint8_t ran_ue_id[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  memcpy (gnb_asn->ran_UEID->buf, ran_ue_id, 8);
  gnb_asn->ran_UEID->size = 8;

  UEID_t *ue_ID = (UEID_t *) calloc (1, sizeof (UEID_t));
  ue_ID->present = UEID_PR_gNB_UEID;
  ue_ID->choice.gNB_UEID = gnb_asn;

  UE_data->ueID = *ue_ID;

  // Adding format 1 part with format 3 part.
  UE_data->measReport = *ind_msg_f_1;

  // equivelent to code below -- old  code
  // ASN_SEQUENCE_ADD (&UE_data_list->list,UE_data);
  // kpm_ind_message_format3->ueMeasReportList = *UE_data_list ;
  ASN_SEQUENCE_ADD (&ind_msg_f_3->ueMeasReportList.list, UE_data);

  NS_LOG_INFO (xer_fprint (stderr, &asn_DEF_E2SM_KPM_IndicationMessage_Format3, ind_msg_f_3));
}

void
KpmIndicationMessage::FillUeID (UEID_t *ue_ID, const Ptr<MeasurementItemList> ueIndication)
{

  // NS_LOG_UNCOND("Hooo" << ueIndication->GetId());
  auto UEid = std::string (reinterpret_cast<const char *> (ueIndication->GetId ().buf),
                           ueIndication->GetId ().size);
  NS_LOG_INFO ("\n ID of current UE  " << UEid);

  UEID_GNB_t *gnb_asn = (UEID_GNB_t *) calloc (1, sizeof (UEID_GNB_t));
  assert (gnb_asn != NULL && "Memory exhausted");

  gnb_asn->amf_UE_NGAP_ID.buf = (uint8_t *) calloc (5, sizeof (uint8_t));
  assert (gnb_asn->amf_UE_NGAP_ID.buf != NULL && "Memory exhausted");

  // asn_ulong2INTEGER (&gnb_asn->amf_UE_NGAP_ID, rand () % 112358132134);
  asn_ulong2INTEGER (&gnb_asn->amf_UE_NGAP_ID,
                     static_cast<unsigned long> (
                         KpmIndicationHeader::octet_string_to_int_64 (ueIndication->GetId ())));

  // gnb_asn->amf_UE_NGAP_ID =  static_cast<INTEGER_t>(KpmIndicationHeader::octet_string_to_int_64(ueIndication->GetId()));

  gnb_asn->guami.aMFPointer = cp_amf_ptr_to_bit_string ((rand () % 2 ^ 6) + 0);
  gnb_asn->guami.aMFSetID = cp_amf_set_id_to_bit_string ((rand () % 2 ^ 10) + 0);
  gnb_asn->guami.aMFRegionID = cp_amf_region_id_to_bit_string ((rand () % 2 ^ 8) + 0);

  gnb_asn->guami.pLMNIdentity = cp_plmn_identity_to_octant_string (rand () % 505, rand () % 99, 2);
  // gnb_asn->guami.pLMNIdentity = ueIndication->GetId (); !!!!!!

  // TODO
  // gnb_asn->ran_UEID

  gnb_asn->ran_UEID = (RANUEID_t *) calloc (1, sizeof (*gnb_asn->ran_UEID));
  gnb_asn->ran_UEID->buf = (uint8_t *) calloc (8, sizeof (*gnb_asn->ran_UEID->buf));
  uint8_t ran_ue_id[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  memcpy (gnb_asn->ran_UEID->buf, ran_ue_id, 8);
  gnb_asn->ran_UEID->size = 8;

  gnb_asn->ran_UEID->buf = (uint8_t *) calloc (8, sizeof (*gnb_asn->ran_UEID->buf));

  // UEID_t *ue_ID = (UEID_t *) calloc (1, sizeof (UEID_t));
  ue_ID->present = UEID_PR_gNB_UEID;
  ue_ID->choice.gNB_UEID = gnb_asn;
}

void
KpmIndicationMessage::FillKpmIndicationMessageFormat3 (
    E2SM_KPM_IndicationMessage_Format3 *ind_msg_f_3, const KpmIndicationMessageValues &values)
{

  UEMeasurementReportList_t *m_ueMeasReportList =
      (UEMeasurementReportList_t *) calloc (1, sizeof (UEMeasurementReportList_t));

  // For each indication message fill UE fill values.
  for (const auto &ueIndication : values.m_ueIndications)
    {

      UEMeasurementReportItem_t *UE_data =
          (UEMeasurementReportItem_t *) calloc (1, sizeof (UEMeasurementReportItem_t));

      UEID_t *ue_ID = (UEID_t *) calloc (1, sizeof (UEID_t));

      E2SM_KPM_IndicationMessage_Format1 *ind_msg_f_1 =
          (E2SM_KPM_IndicationMessage_Format1 *) calloc (
              1, sizeof (E2SM_KPM_IndicationMessage_Format1));

      FillUeID (ue_ID, ueIndication);
      FillKpmIndicationMessageFormat1 (ind_msg_f_1, ueIndication);

      UE_data->ueID = *ue_ID;
      UE_data->measReport = *ind_msg_f_1;

      // TODO
      ASN_SEQUENCE_ADD (&m_ueMeasReportList->list, UE_data);
    }

  ind_msg_f_3->ueMeasReportList = *m_ueMeasReportList;
}

// NR = New Radio(5G)
// EUTRA = 4G
// CSI = Channel State Information
// SSB = synchronization signal block (SSB) in 5G New Radio (NR).
void
KpmIndicationMessage::FillAndEncodeKpmIndicationMessage (
    E2SM_KPM_IndicationMessage_t *descriptor, KpmIndicationMessageValues values,
    const E2SM_KPM_IndicationMessage_FormatType &format_type)
{

  // Fill the RAN container.
  // PF_Container_t *ranContainer = (PF_Container_t *) calloc (1, sizeof (PF_Container_t));
  // FillPmContainer (ranContainer, values.m_pmContainerValues);

  // Fill the message
  // PM_Containers_Item_t *containers_list =
  //     (PM_Containers_Item_t *) calloc (1, sizeof (PM_Containers_Item_t));
  // containers_list->performanceContainer = ranContainer;

  E2SM_KPM_IndicationMessage_t *ind_message =
      (E2SM_KPM_IndicationMessage_t *) calloc (1, sizeof (E2SM_KPM_IndicationMessage_t));

  switch (format_type)
    {
      case E2SM_KPM_INDICATION_MESSAGE_FORMART1: {
        // Format 1
        // E2SM_KPM_IndicationMessage_Format1_t *kpm_ind_message_format1 =
        //     (E2SM_KPM_IndicationMessage_Format1_t *) calloc (
        //         1, sizeof (E2SM_KPM_IndicationMessage_Format1_t));
        // FillKpmIndicationMessageFormat1 (kpm_ind_message_format1, values);
        // ind_message->indicationMessage_formats.present =
        //     E2SM_KPM_IndicationMessage__indicationMessage_formats_PR_indicationMessage_Format1;
        // // ASN_SEQUENCE_ADD(&ind_message->indicationMessage_formats.choice.indicationMessage_Format1,
        // // kpm_ind_message_format1) ;
        // ind_message->indicationMessage_formats.choice.indicationMessage_Format1 =
        //     kpm_ind_message_format1;
        break;
      }
      case E2SM_KPM_INDICATION_MESSAGE_FORMART2: {
        break;
      }
      case E2SM_KPM_INDICATION_MESSAGE_FORMART3: {

        // Format 3 section, consists of format 1 and some additional args
        // like (UE_ID, and Measurements report) and these are mandatory args.
        E2SM_KPM_IndicationMessage_Format3_t *kpm_ind_message_format3 =
            (E2SM_KPM_IndicationMessage_Format3_t *) calloc (
                1, sizeof (E2SM_KPM_IndicationMessage_Format3_t));

        assert (kpm_ind_message_format3 != nullptr && "Memory exhausted");
        NS_LOG_DEBUG ("2. NOWW values.m_ueIndications.size()=" << values.m_ueIndications.size ());

        // if (values.m_cellMeasurementItems) {

        // }
        if (values.m_ueIndications.size () > 0)
          {
            FillKpmIndicationMessageFormat3 (kpm_ind_message_format3, values);
          }
        else
          {
            // return;
            E2SM_KPM_IndicationMessage_Format1_t *kpm_ind_message_format1 =
                (E2SM_KPM_IndicationMessage_Format1_t *) calloc (
                    1, sizeof (E2SM_KPM_IndicationMessage_Format1_t));

            FillKpmIndicationMessageFormat1 (kpm_ind_message_format1);

            FillKpmIndicationMessageFormat3 (kpm_ind_message_format3, kpm_ind_message_format1,
                                             values);
          }

        ind_message->indicationMessage_formats.present =
            E2SM_KPM_IndicationMessage__indicationMessage_formats_PR_indicationMessage_Format3;
        ind_message->indicationMessage_formats.choice.indicationMessage_Format3 =
            kpm_ind_message_format3;

        break;
      }
    default:
      break;
    }

  NS_LOG_INFO (xer_fprint (stderr, &asn_DEF_E2SM_KPM_IndicationMessage, ind_message));
  Encode (ind_message);
  printf (" \n *** Done Encoding INDICATION Message ****** \n ");
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
