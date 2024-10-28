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
  constexpr long FORMAT_1_E2SM_RC_CTRL_HDR =0;
  constexpr long FORMAT_1_E2SM_RC_CTRL_MSG =0;
  std::string shortNameBuffer = "ORAN-WG3-RC";

  Ptr<OctetString> shortName = Create<OctetString> (shortNameBuffer, shortNameBuffer.size ());

  ranfunc_desc->ranFunction_Name.ranFunction_ShortName = shortName->GetValue ();

  // This part is not in the specs, maybe it can be removed?
  // RIC Control Definitions
  uint8_t *descriptionBuffer = (uint8_t *) "RICC";
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

  ranfunc_desc->ranFunctionDefinition_Control =
      (RANFunctionDefinition_Control_t*)
          calloc (1, sizeof (RANFunctionDefinition_Control_t));

 RANFunctionDefinition_Control_Item_t *control_style0 = (RANFunctionDefinition_Control_Item_t *)
          calloc (1, sizeof (RANFunctionDefinition_Control_Item_t));

  control_style0->ric_ControlStyle_Type = 1;  //Radio Bearer control
  uint8_t *controlStyleName0Buffer = (uint8_t *) "Radio Bearer Control";
  size_t controlStyleName0BufferSize = strlen ((char *) controlStyleName0Buffer);
  control_style0->ric_ControlStyle_Name.buf = (uint8_t *) calloc (1, controlStyleName0BufferSize);
  memcpy (control_style0->ric_ControlStyle_Name.buf, controlStyleName0Buffer,
          controlStyleName0BufferSize);
  control_style0->ric_ControlStyle_Name.size = controlStyleName0BufferSize;

  // these two were not present in the spec in this control style but just in the second, remove if they lead to crash
  control_style0->ric_ControlMessageFormat_Type = FORMAT_1_E2SM_RC_CTRL_MSG;
  control_style0->ric_ControlHeaderFormat_Type = FORMAT_1_E2SM_RC_CTRL_HDR;

  control_style0->ric_ControlAction_List = (RANFunctionDefinition_Control_Item::RANFunctionDefinition_Control_Item__ric_ControlAction_List *)
          calloc (1, sizeof (RANFunctionDefinition_Control_Item::RANFunctionDefinition_Control_Item__ric_ControlAction_List ));

RANFunctionDefinition_Control_Action_Item_t *control_action0 =
      (RANFunctionDefinition_Control_Action_Item_t *) calloc (1, sizeof (RANFunctionDefinition_Control_Action_Item_t));

  control_action0->ric_ControlAction_ID = 2;
  // DRB split ratio control
  uint8_t *controlActionName0Buffer = (uint8_t *) "QoS flow mapping configuration";
  size_t controlActionName0BufferSize = strlen ((char *) controlActionName0Buffer);
  control_action0->ric_ControlAction_Name.buf =
      (uint8_t *) calloc (1, controlActionName0BufferSize);
  memcpy (control_action0->ric_ControlAction_Name.buf, controlActionName0Buffer,
          controlActionName0BufferSize);
  control_action0->ric_ControlAction_Name.size = controlActionName0BufferSize;
  control_action0->ran_ControlActionParameters_List =
      (RANFunctionDefinition_Control_Action_Item::RANFunctionDefinition_Control_Action_Item__ran_ControlActionParameters_List *) calloc (
          2, sizeof (RANFunctionDefinition_Control_Action_Item::RANFunctionDefinition_Control_Action_Item__ran_ControlActionParameters_List ));

 // Downlink PDCP Data Split
  ControlAction_RANParameter_Item_t *ranParameter0 =
      (ControlAction_RANParameter_Item_t *) calloc (1, sizeof (ControlAction_RANParameter_Item_t));
  uint8_t *ranParameter0NameBuffer = (uint8_t *) "DRB";
  size_t ranParameter0NameBufferSize = strlen ((char *) ranParameter0NameBuffer);
  ranParameter0->ranParameter_name.buf = (uint8_t *) calloc (1, ranParameter0NameBufferSize);
  memcpy (ranParameter0->ranParameter_name.buf, ranParameter0NameBuffer,
          ranParameter0NameBufferSize);
  ranParameter0->ranParameter_name.size = ranParameter0NameBufferSize;
  ranParameter0->ranParameter_ID = 1;

ControlAction_RANParameter_Item_t *ranParameter1 =
      (ControlAction_RANParameter_Item_t *) calloc (1, sizeof (ControlAction_RANParameter_Item_t));
  uint8_t *ranParameter1NameBuffer = (uint8_t *) "QOS_FLOWS";
  size_t ranParameter1NameBufferSize = strlen ((char *) ranParameter1NameBuffer);
  ranParameter1->ranParameter_name.buf = (uint8_t *) calloc (1, ranParameter1NameBufferSize);
  memcpy (ranParameter1->ranParameter_name.buf, ranParameter1NameBuffer,
          ranParameter1NameBufferSize);
  ranParameter1->ranParameter_name.size = ranParameter1NameBufferSize;
  ranParameter1->ranParameter_ID = 2;

  ASN_SEQUENCE_ADD (&control_action0->ran_ControlActionParameters_List->list,ranParameter0);
  ASN_SEQUENCE_ADD (&control_action0->ran_ControlActionParameters_List->list,ranParameter1);

  ASN_SEQUENCE_ADD (&control_style0->ric_ControlAction_List->list,control_action0);

  ASN_SEQUENCE_ADD (&ranfunc_desc->ranFunctionDefinition_Control->ric_ControlStyle_List.list, control_style0);

  Encode (ranfunc_desc);

  NS_LOG_INFO (xer_fprint (stderr, &asn_DEF_E2SM_RC_RANFunctionDefinition, ranfunc_desc));
}

} // namespace ns3