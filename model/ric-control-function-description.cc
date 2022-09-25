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

#include <ns3/ric-control-function-description.h>
#include <ns3/asn1c-types.h>
#include <ns3/log.h>

extern "C" {  
  #include "RIC-ControlStyle-Item.h"
  #include "RIC-ControlAction-Item.h"
  #include "RAN-ControlParameter-Item.h"
}

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("RicControlFunctionDescription");

RicControlFunctionDescription::RicControlFunctionDescription ()
{
  E2SM_RC_RANFunctionDefinition_t *descriptor = new E2SM_RC_RANFunctionDefinition_t ();
  FillAndEncodeRCFunctionDescription (descriptor);
  ASN_STRUCT_FREE_CONTENTS_ONLY (asn_DEF_E2SM_RC_RANFunctionDefinition, descriptor);
  delete descriptor;
}

RicControlFunctionDescription::~RicControlFunctionDescription ()
{

}

void
RicControlFunctionDescription::Encode (E2SM_RC_RANFunctionDefinition_t *descriptor)
{
  asn_codec_ctx_t *opt_cod = 0; // disable stack bounds checking
  // encode the structure into the e2smbuffer
  asn_encode_to_new_buffer_result_s encodedMsg = asn_encode_to_new_buffer (
      opt_cod, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2SM_RC_RANFunctionDefinition, descriptor);

  if (encodedMsg.result.encoded < 0)
    {
      NS_FATAL_ERROR ("Error during the encoding of the RIC Indication Header, errno: "
                      << strerror (errno) << ", failed_type " << encodedMsg.result.failed_type->name
                      << ", structure_ptr " << encodedMsg.result.structure_ptr);
    }

  m_buffer = encodedMsg.buffer;
  m_size = encodedMsg.result.encoded;
}

void
RicControlFunctionDescription::FillAndEncodeRCFunctionDescription (
    E2SM_RC_RANFunctionDefinition_t *ranfunc_desc)
{

  std::string shortNameBuffer = "ORAN-WG3-RC";

  Ptr<OctetString> shortName = Create<OctetString> (shortNameBuffer, shortNameBuffer.size ());

  ranfunc_desc->ranFunction_Name.ranFunction_ShortName = shortName->GetValue ();

  // This part is not in the specs, maybe it can be removed?

  uint8_t *descriptionBuffer = (uint8_t *) "RIC Control Definitions";
  uint8_t *oidBuffer = (uint8_t *) "OID123"; // this is optional, dummy value

  long *inst = (long *) calloc (1, sizeof (long));

  ranfunc_desc->ranFunction_Name.ranFunction_Description.buf =
      (uint8_t *) calloc (1, strlen ((char *) descriptionBuffer));
  memcpy (ranfunc_desc->ranFunction_Name.ranFunction_Description.buf, descriptionBuffer,
          strlen ((char *) descriptionBuffer));
  ranfunc_desc->ranFunction_Name.ranFunction_Description.size = strlen ((char *) descriptionBuffer);
  ranfunc_desc->ranFunction_Name.ranFunction_Instance = inst;
  ranfunc_desc->ranFunction_Name.ranFunction_E2SM_OID.buf =
      (uint8_t *) calloc (1, strlen ((char *) oidBuffer));
  memcpy (ranfunc_desc->ranFunction_Name.ranFunction_E2SM_OID.buf, oidBuffer,
          strlen ((char *) oidBuffer));
  ranfunc_desc->ranFunction_Name.ranFunction_E2SM_OID.size = strlen ((char *) oidBuffer);

  // End part of of specs

  RIC_ControlStyle_Item_t *control_style0 =
      (RIC_ControlStyle_Item_t *) calloc (1, sizeof (RIC_ControlStyle_Item_t));
  control_style0->ric_ControlStyle_Type = 1;

  uint8_t *controlStyleName0Buffer = (uint8_t *) "Radio Bearer Control";
  size_t controlStyleName0BufferSize = strlen ((char *) controlStyleName0Buffer);
  control_style0->ric_ControlStyle_Name.buf = (uint8_t *) calloc (1, controlStyleName0BufferSize);
  memcpy (control_style0->ric_ControlStyle_Name.buf, controlStyleName0Buffer,
          controlStyleName0BufferSize);
  control_style0->ric_ControlStyle_Name.size = controlStyleName0BufferSize;

  // these two were not present in the spec in this control style but just in the second, remove if they lead to crash
  control_style0->ric_ControlMessageFormat_Type = 1;
  control_style0->ric_ControlHeaderFormat_Type = 1;

  control_style0->ric_ControlAction_List =
      (RIC_ControlStyle_Item::RIC_ControlStyle_Item__ric_ControlAction_List *) calloc (
          1, sizeof (RIC_ControlStyle_Item::RIC_ControlStyle_Item__ric_ControlAction_List));

  RIC_ControlAction_Item_t *control_action0 =
      (RIC_ControlAction_Item_t *) calloc (1, sizeof (RIC_ControlAction_Item_t));

  control_action0->ric_ControlAction_ID = 6;
  uint8_t *controlActionName0Buffer = (uint8_t *) "DRB split ratio control";
  size_t controlActionName0BufferSize = strlen ((char *) controlActionName0Buffer);
  control_action0->ric_ControlAction_Name.buf =
      (uint8_t *) calloc (1, controlActionName0BufferSize);
  memcpy (control_action0->ric_ControlAction_Name.buf, controlActionName0Buffer,
          controlActionName0BufferSize);
  control_action0->ric_ControlAction_Name.size = controlActionName0BufferSize;
  control_action0->ran_ControlParameters_List =
      (RIC_ControlAction_Item::RIC_ControlAction_Item__ran_ControlParameters_List *) calloc (
          1, sizeof (RIC_ControlAction_Item::RIC_ControlAction_Item__ran_ControlParameters_List));

  RAN_ControlParameter_Item_t *ranParameter0 =
      (RAN_ControlParameter_Item_t *) calloc (1, sizeof (RAN_ControlParameter_Item_t));
  uint8_t *ranParameter0NameBuffer = (uint8_t *) "Downlink PDCP Data Split";
  size_t ranParameter0NameBufferSize = strlen ((char *) ranParameter0NameBuffer);
  ranParameter0->ranParameter_Name.buf = (uint8_t *) calloc (1, ranParameter0NameBufferSize);
  memcpy (ranParameter0->ranParameter_Name.buf, ranParameter0NameBuffer,
          ranParameter0NameBufferSize);
  ranParameter0->ranParameter_Name.size = ranParameter0NameBufferSize;
  ranParameter0->ranParameter_ID = 3;

  RAN_ControlParameter_Item_t *ranParameter1 =
      (RAN_ControlParameter_Item_t *) calloc (1, sizeof (RAN_ControlParameter_Item_t));
  uint8_t *ranParameter1NameBuffer = (uint8_t *) "Uplink PDCP Data Split Threshold";
  size_t ranParameter1NameBufferSize = strlen ((char *) ranParameter1NameBuffer);
  ranParameter1->ranParameter_Name.buf = (uint8_t *) calloc (1, ranParameter1NameBufferSize);
  memcpy (ranParameter1->ranParameter_Name.buf, ranParameter1NameBuffer,
          ranParameter1NameBufferSize);
  ranParameter1->ranParameter_ID = 2;

  ASN_SEQUENCE_ADD (&control_action0->ran_ControlParameters_List->list, ranParameter0);
  ASN_SEQUENCE_ADD (&control_action0->ran_ControlParameters_List->list, ranParameter1);

  ASN_SEQUENCE_ADD (&control_style0->ric_ControlAction_List->list, control_action0);

  RIC_ControlStyle_Item_t *control_style1 =
      (RIC_ControlStyle_Item_t *) calloc (1, sizeof (RIC_ControlStyle_Item_t));
  control_style1->ric_ControlStyle_Type = 3;

  uint8_t *controlStyleName1Buffer = (uint8_t *) "Radio Bearer Control";
  size_t controlStyleName1BufferSize = strlen ((char *) controlStyleName1Buffer);
  control_style1->ric_ControlStyle_Name.buf = (uint8_t *) calloc (1, controlStyleName1BufferSize);
  memcpy (control_style1->ric_ControlStyle_Name.buf, controlStyleName1Buffer,
          controlStyleName1BufferSize);
  control_style1->ric_ControlStyle_Name.size = controlStyleName1BufferSize;

  control_style1->ric_ControlMessageFormat_Type = 1;
  control_style1->ric_ControlHeaderFormat_Type = 1;

  control_style1->ric_ControlAction_List =
      (RIC_ControlStyle_Item::RIC_ControlStyle_Item__ric_ControlAction_List *) calloc (
          1, sizeof (RIC_ControlStyle_Item::RIC_ControlStyle_Item__ric_ControlAction_List));

  RIC_ControlAction_Item_t *control_action1 =
      (RIC_ControlAction_Item_t *) calloc (1, sizeof (RIC_ControlAction_Item_t));

  control_action1->ric_ControlAction_ID = 1;
  uint8_t *controlActionName1Buffer = (uint8_t *) "Handover control";
  size_t controlActionName1BufferSize = strlen ((char *) controlActionName1Buffer);
  control_action1->ric_ControlAction_Name.buf =
      (uint8_t *) calloc (1, controlActionName1BufferSize);
  memcpy (control_action1->ric_ControlAction_Name.buf, controlActionName1Buffer,
          controlActionName1BufferSize);
  control_action1->ric_ControlAction_Name.size = controlActionName1BufferSize;
  control_action1->ran_ControlParameters_List =
      (RIC_ControlAction_Item::RIC_ControlAction_Item__ran_ControlParameters_List *) calloc (
          1, sizeof (RIC_ControlAction_Item::RIC_ControlAction_Item__ran_ControlParameters_List));

  RAN_ControlParameter_Item_t *ranParameter2 =
      (RAN_ControlParameter_Item_t *) calloc (1, sizeof (RAN_ControlParameter_Item_t));
  uint8_t *ranParameter2NameBuffer = (uint8_t *) "NR CGI";
  size_t ranParameter2NameBufferSize = strlen ((char *) ranParameter2NameBuffer);
  ranParameter2->ranParameter_Name.buf = (uint8_t *) calloc (1, ranParameter2NameBufferSize);
  memcpy (ranParameter2->ranParameter_Name.buf, ranParameter2NameBuffer,
          ranParameter2NameBufferSize);
  ranParameter2->ranParameter_Name.size = ranParameter2NameBufferSize;
  ranParameter2->ranParameter_ID = 4;

  RAN_ControlParameter_Item_t *ranParameter3 =
      (RAN_ControlParameter_Item_t *) calloc (1, sizeof (RAN_ControlParameter_Item_t));
  uint8_t *ranParameter3NameBuffer = (uint8_t *) "E-UTRA CGI";
  size_t ranParameter3NameBufferSize = strlen ((char *) ranParameter3NameBuffer);
  ranParameter3->ranParameter_Name.buf = (uint8_t *) calloc (1, ranParameter3NameBufferSize);
  memcpy (ranParameter3->ranParameter_Name.buf, ranParameter3NameBuffer,
          ranParameter3NameBufferSize);
  ranParameter3->ranParameter_Name.size = ranParameter3NameBufferSize;
  ranParameter3->ranParameter_ID = 6;

  ASN_SEQUENCE_ADD (&control_action1->ran_ControlParameters_List->list, ranParameter2);
  ASN_SEQUENCE_ADD (&control_action1->ran_ControlParameters_List->list, ranParameter3);

  ASN_SEQUENCE_ADD (&control_style1->ric_ControlAction_List->list, control_action1);

  ranfunc_desc->ric_ControlStyle_List =
      (E2SM_RC_RANFunctionDefinition::E2SM_RC_RANFunctionDefinition__ric_ControlStyle_List *)
          calloc (1, sizeof (E2SM_RC_RANFunctionDefinition::
                                 E2SM_RC_RANFunctionDefinition__ric_ControlStyle_List));

  ASN_SEQUENCE_ADD (&ranfunc_desc->ric_ControlStyle_List->list, control_style0);
  ASN_SEQUENCE_ADD (&ranfunc_desc->ric_ControlStyle_List->list, control_style1);

  Encode (ranfunc_desc);

  NS_LOG_INFO (xer_fprint (stderr, &asn_DEF_E2SM_RC_RANFunctionDefinition, ranfunc_desc));
}

} // namespace ns3
