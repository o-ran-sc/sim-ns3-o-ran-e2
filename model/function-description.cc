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

#include <ns3/function-description.h>
#include <ns3/asn1c-types.h>
#include <ns3/log.h>


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("FunctionDescription");

FunctionDescription::FunctionDescription ()
{
//   E2SM_KPM_RANfunction_Description_t *descriptor = new E2SM_KPM_RANfunction_Description_t ();
//   FillAndEncodeKpmFunctionDescription (descriptor);
//   ASN_STRUCT_FREE_CONTENTS_ONLY (asn_DEF_E2SM_KPM_RANfunction_Description, descriptor);
//   delete descriptor;
    m_size = 0;
}

FunctionDescription::~FunctionDescription ()
{
  free (m_buffer);
  m_size = 0;
}

// TODO improve
// void
// FunctionDescription::Encode (E2SM_KPM_RANfunction_Description_t *descriptor)
// {
//   asn_codec_ctx_t *opt_cod = 0; // disable stack bounds checking
//   // encode the structure into the e2smbuffer
//   asn_encode_to_new_buffer_result_s encodedMsg = asn_encode_to_new_buffer (
//       opt_cod, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2SM_KPM_RANfunction_Description, descriptor);

//   if (encodedMsg.result.encoded < 0)
//     {
//       NS_FATAL_ERROR ("Error during the encoding of the RIC Indication Header, errno: "
//                       << strerror (errno) << ", failed_type " << encodedMsg.result.failed_type->name
//                       << ", structure_ptr " << encodedMsg.result.structure_ptr);
//     }

//   m_buffer = encodedMsg.buffer;
//   m_size = encodedMsg.result.encoded;
// }

} // namespace ns3
