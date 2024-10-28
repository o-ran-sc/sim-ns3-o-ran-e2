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

#include <ns3/kpm-function-description.h>
#include <ns3/asn1c-types.h>
#include <ns3/log.h>

extern "C" {
#include "RIC-EventTriggerStyle-Item.h"
#include "RIC-ReportStyle-Item.h"
}

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("KpmFunctionDescription");

KpmFunctionDescription::KpmFunctionDescription ()
{
  E2SM_KPM_RANfunction_Description_t *descriptor = new E2SM_KPM_RANfunction_Description_t ();
  FillAndEncodeKpmFunctionDescription (descriptor);
  ASN_STRUCT_FREE_CONTENTS_ONLY (asn_DEF_E2SM_KPM_RANfunction_Description, descriptor);
  delete descriptor;
}

KpmFunctionDescription::~KpmFunctionDescription ()
{
  free (m_buffer);
  m_size = 0;
}

void
KpmFunctionDescription::Encode (E2SM_KPM_RANfunction_Description_t *descriptor)
{
  if (false) {
    asn_codec_ctx_t *opt_cod = 0; // disable stack bounds checking
    // encode the structure into the e2smbuffer
    asn_encode_to_new_buffer_result_s encodedMsg = asn_encode_to_new_buffer (
        opt_cod, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2SM_KPM_RANfunction_Description, descriptor);

    if (encodedMsg.result.encoded < 0)
      {
        printf("I am Hereeeeeeeeeeeeeeeeeeeeeeee");
        
        NS_FATAL_ERROR ("Error during the encoding of the RIC Indication Header, errno: "
                        << strerror (errno) << ", failed_type " << encodedMsg.result.failed_type->name
                        << ", structure_ptr " << encodedMsg.result.structure_ptr);
      }

    m_buffer = encodedMsg.buffer;
    m_size = encodedMsg.result.encoded;
  } else {
    asn_codec_ctx_t *opt_cod = 0; // disable stack bounds checking
    // encode the structure into the e2smbuffer
    Bytee_array_t ba = {.buf = (uint8_t *) malloc (2048), .len = 2048};
    asn_enc_rval_t encodedMsg = asn_encode_to_buffer(
        opt_cod, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2SM_KPM_RANfunction_Description, descriptor,ba.buf,ba.len);

    if (encodedMsg.encoded < 0)
      {
        printf("I am Hereeeeeeeeeeeeeeeeeeeeeeee");
        
        NS_FATAL_ERROR ("Error during the encoding of the RIC Indication Header, errno: "
                        << strerror (errno) << ", failed_type " << encodedMsg.failed_type->name
                        << ", structure_ptr " << encodedMsg.structure_ptr);
      }

    m_buffer = ba.buf;
    m_size = encodedMsg.encoded;
  }
}


OCTET_STRING_t KpmFunctionDescription::cp_str_to_ba(const char* str)
{
    
    assert(str != NULL);
    const size_t sz = strlen(str);
    OCTET_STRING_t asn = {0};

    asn.buf = (uint8_t*)calloc(sz,sizeof(uint8_t));
      assert(asn.buf != NULL && "Memory exhausted");

      memcpy(asn.buf, str, sz);

    // asn.buf = (uint8_t*) calloc(sizeof(x) + 1, sizeof(char));
    // memcpy(asn.buf,&x,sizeof(x));
    // asn.size = sizeof(x);

    return asn;
}

void
KpmFunctionDescription::FillAndEncodeKpmFunctionDescription (
    E2SM_KPM_RANfunction_Description_t *ranfunc_desc)
{
  std::string shortNameBuffer = "ORAN-WG3-KPM";
  constexpr long STYLE_4_RIC_SERVICE_REPORT =4;
  constexpr long FORMAT_1_RIC_EVENT_TRIGGER =0;
  constexpr long STYLE_1_RIC_EVENT_TRIGGER =1;  //constexpr -> enum
  constexpr long FORMAT_1_INDICATION_HEADER =0;
  constexpr long FORMAT_3_INDICATION_MESSAGE =2;
  //KPM monitor
  uint8_t *descriptionBuffer = (uint8_t *) "KPM";
  uint8_t *oidBuffer = (uint8_t *) "OID12"; // this is optional, dummy value

  Ptr<OctetString> shortName = Create<OctetString> (shortNameBuffer, shortNameBuffer.size ());

  ranfunc_desc->ranFunction_Name.ranFunction_ShortName = shortName->GetValue ();

  long *inst = (long *) calloc (1, sizeof (long));

  //  ranfunc_desc->ranFunction_Name.ranFunction_Description = (OCTET_STRING_t*)calloc(1, sizeof(OCTET_STRING_t));
  ranfunc_desc->ranFunction_Name.ranFunction_Description.buf =
      (uint8_t *) calloc (1, strlen ((char *) descriptionBuffer));
  memcpy (ranfunc_desc->ranFunction_Name.ranFunction_Description.buf, descriptionBuffer,
          strlen ((char *) descriptionBuffer));
  ranfunc_desc->ranFunction_Name.ranFunction_Description.size = strlen ((char *) descriptionBuffer);
  ranfunc_desc->ranFunction_Name.ranFunction_Instance = inst;

  //  ranfunc_desc->ranFunction_Name.ranFunction_E2SM_OID = (OCTET_STRING_t*)calloc(1, sizeof(OCTET_STRING_t));
  ranfunc_desc->ranFunction_Name.ranFunction_E2SM_OID.buf =
      (uint8_t *) calloc (1, strlen ((char *) oidBuffer));
  memcpy (ranfunc_desc->ranFunction_Name.ranFunction_E2SM_OID.buf, oidBuffer,
          strlen ((char *) oidBuffer));
  ranfunc_desc->ranFunction_Name.ranFunction_E2SM_OID.size = strlen ((char *) oidBuffer);

  RIC_EventTriggerStyle_Item_t *trigger_style =
      (RIC_EventTriggerStyle_Item_t *) calloc (1, sizeof (RIC_EventTriggerStyle_Item_t));
  trigger_style->ric_EventTriggerStyle_Type = STYLE_1_RIC_EVENT_TRIGGER;
  //Periodic report
  uint8_t *eventTriggerStyleNameBuffer = (uint8_t *) "PR";
  //  trigger_style->ric_EventTriggerStyle_Name = (OCTET_STRING_t*)calloc(1, sizeof(OCTET_STRING_t));
  trigger_style->ric_EventTriggerStyle_Name.buf =
      (uint8_t *) calloc (1, strlen ((char *) eventTriggerStyleNameBuffer));
  memcpy (trigger_style->ric_EventTriggerStyle_Name.buf, eventTriggerStyleNameBuffer,
          strlen ((char *) eventTriggerStyleNameBuffer));
  trigger_style->ric_EventTriggerStyle_Name.size = strlen ((char *) eventTriggerStyleNameBuffer);
  trigger_style->ric_EventTriggerFormat_Type = FORMAT_1_RIC_EVENT_TRIGGER;

  ranfunc_desc->ric_EventTriggerStyle_List =
      (E2SM_KPM_RANfunction_Description::
           E2SM_KPM_RANfunction_Description__ric_EventTriggerStyle_List *)
          calloc (1, sizeof (E2SM_KPM_RANfunction_Description::
                                 E2SM_KPM_RANfunction_Description__ric_EventTriggerStyle_List));

  ASN_SEQUENCE_ADD (&ranfunc_desc->ric_EventTriggerStyle_List->list, trigger_style);

  RIC_ReportStyle_Item_t *report_style =
      (RIC_ReportStyle_Item_t *) calloc (1, sizeof (RIC_ReportStyle_Item_t));
  report_style->ric_ReportStyle_Type = STYLE_4_RIC_SERVICE_REPORT;
// O-CU-CP Measurement Container for the EPC connected deployment
  uint8_t *reportStyleNameBuffer =
      (uint8_t *) "O-CU-CP";

  //  report_style->ric_ReportStyle_Name = (OCTET_STRING_t*)calloc(1, sizeof(OCTET_STRING_t));
  report_style->ric_ReportStyle_Name.buf =
      (uint8_t *) calloc (1, strlen ((char *) reportStyleNameBuffer));
  memcpy (report_style->ric_ReportStyle_Name.buf, reportStyleNameBuffer,
          strlen ((char *) reportStyleNameBuffer));
  report_style->ric_ReportStyle_Name.size = strlen ((char *) reportStyleNameBuffer);
report_style->ric_IndicationHeaderFormat_Type = FORMAT_1_INDICATION_HEADER;  
report_style->ric_IndicationMessageFormat_Type = FORMAT_3_INDICATION_MESSAGE;
/*Added RICACtionFormat*/
report_style->ric_ActionFormat_Type = 3 ; 
MeasurementInfo_Action_Item * meas_item = (MeasurementInfo_Action_Item *)calloc(1, sizeof(MeasurementInfo_Action_Item));

  const char act[] = "DRB.RlcSduDelayDl";

meas_item->measName = cp_str_to_ba(act);

  ASN_SEQUENCE_ADD (&report_style->measInfo_Action_List.list, meas_item);

 ranfunc_desc->ric_ReportStyle_List =
       (E2SM_KPM_RANfunction_Description::E2SM_KPM_RANfunction_Description__ric_ReportStyle_List *)
           calloc (1, sizeof (E2SM_KPM_RANfunction_Description::
                                  E2SM_KPM_RANfunction_Description__ric_ReportStyle_List));

  ASN_SEQUENCE_ADD (&ranfunc_desc->ric_ReportStyle_List->list, report_style);

  Encode (ranfunc_desc);

  NS_LOG_INFO (xer_fprint (stderr, &asn_DEF_E2SM_KPM_RANfunction_Description, ranfunc_desc));
}

} // namespace ns3
