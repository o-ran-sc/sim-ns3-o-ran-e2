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
 
#include <ns3/ric-control-message.h>
#include <ns3/asn1c-types.h>
#include <ns3/log.h>
#include <bitset>
namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("RicControlMessage");


RicControlMessage::RicControlMessage (E2AP_PDU_t* pdu)
{
  NS_LOG_INFO("Start of RicControlMessage::RicControlMessage()");
  DecodeRicControlMessage (pdu);
  NS_LOG_INFO("End of RicControlMessage::RicControlMessage()");
}

RicControlMessage::~RicControlMessage ()
{

}

void  
RicControlMessage::DecodeRicControlMessage(E2AP_PDU_t* pdu)
{
    InitiatingMessage_t* mess = pdu->choice.initiatingMessage;
    auto *request = (RICcontrolRequest_t *) &mess->value.choice.RICcontrolRequest;
    NS_LOG_INFO (xer_fprint(stderr, &asn_DEF_RICcontrolRequest, request));

    size_t count = request->protocolIEs.list.count; 
    if (count <= 0) {
        NS_LOG_ERROR("[E2SM] received empty list");
        return;
    }

    for (size_t i = 0; i < count; i++) 
    {
        RICcontrolRequest_IEs_t *ie = request->protocolIEs.list.array [i];
        switch (ie->value.present) {
            case RICcontrolRequest_IEs__value_PR_RICrequestID: {
                NS_LOG_DEBUG("[E2SM] RICcontrolRequest_IEs__value_PR_RICrequestID");
                m_ricRequestId = ie->value.choice.RICrequestID;
                switch (m_ricRequestId.ricRequestorID) {
                    case 1001: {
                        NS_LOG_DEBUG("TS xApp message");
                        m_requestType = ControlMessageRequestIdType::TS;
                        break;
                    }
                    case 1002: {
                        NS_LOG_DEBUG("QoS xApp message");
                        m_requestType = ControlMessageRequestIdType::QoS;
                        break;
                    }
                    case 1024: {
                        NS_LOG_DEBUG("RC xApp message");
                        m_requestType = ControlMessageRequestIdType::RC;
                        break;
                    }
                    default:
                        NS_LOG_DEBUG("Unhandled ricRequestorID\n");
                        m_requestType = static_cast<ControlMessageRequestIdType>(m_ricRequestId.ricRequestorID);
                        break;
                }
                break;
            }
            case RICcontrolRequest_IEs__value_PR_RANfunctionID: {
                m_ranFunctionId = ie->value.choice.RANfunctionID;

                NS_LOG_DEBUG("[E2SM] RICcontrolRequest_IEs__value_PR_RANfunctionID");
                break;
            }
            case RICcontrolRequest_IEs__value_PR_RICcallProcessID: {
                m_ricCallProcessId = ie->value.choice.RICcallProcessID;
                NS_LOG_DEBUG("[E2SM] RICcontrolRequest_IEs__value_PR_RICcallProcessID");
                break;
            }
            case RICcontrolRequest_IEs__value_PR_RICcontrolHeader: {
                NS_LOG_DEBUG("[E2SM] RICcontrolRequest_IEs__value_PR_RICcontrolHeader");
                // xer_fprint(stderr, &asn_DEF_RICcontrolHeader, &ie->value.choice.RICcontrolHeader);

                auto *e2smControlHeader = (E2SM_RC_ControlHeader_t *) calloc(1,
                                                                             sizeof(E2SM_RC_ControlHeader_t));
                ASN_STRUCT_RESET(asn_DEF_E2SM_RC_ControlHeader, e2smControlHeader);
                asn_dec_rval_t rval = asn_decode(nullptr, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2SM_RC_ControlHeader,
                                     (void **) &e2smControlHeader,
                                     ie->value.choice.RICcontrolHeader.buf,
                                     ie->value.choice.RICcontrolHeader.size);
                // Check if decoding was successful
                if (rval.code != RC_OK) {
                    NS_LOG_ERROR("[E2SM] Error decoding RICcontrolHeader");
                    break;
                }


                NS_LOG_INFO (xer_fprint (stderr, &asn_DEF_E2SM_RC_ControlHeader, e2smControlHeader));
                if (e2smControlHeader->ric_controlHeader_formats.present == E2SM_RC_ControlHeader__ric_controlHeader_formats_PR_controlHeader_Format1) {
                    m_e2SmRcControlHeaderFormat1 = e2smControlHeader->ric_controlHeader_formats.choice.controlHeader_Format1;
                    //m_e2SmRcControlHeaderFormat1->ric_ControlAction_ID;
                    //m_e2SmRcControlHeaderFormat1->ric_ControlStyle_Type;
                    //m_e2SmRcControlHeaderFormat1->ueId;
                } else {
                    NS_LOG_DEBUG("[E2SM] Error in checking format of E2SM Control Header");
                }
                break;
                 // Free the allocated memory for e2smControlHeader if needed
                 ASN_STRUCT_FREE(asn_DEF_E2SM_RC_ControlHeader, e2smControlHeader);

            }
            case RICcontrolRequest_IEs__value_PR_RICcontrolMessage: {
                NS_LOG_DEBUG("[E2SM] RICcontrolRequest_IEs__value_PR_RICcontrolMessage");
                // xer_fprint(stderr, &asn_DEF_RICcontrolMessage, &ie->value.choice.RICcontrolMessage);

                auto *e2SmControlMessage = (E2SM_RC_ControlMessage_t *) calloc(1,
                                                                               sizeof(E2SM_RC_ControlMessage_t));
                ASN_STRUCT_RESET(asn_DEF_E2SM_RC_ControlMessage, e2SmControlMessage);
                // Decode message then assign to e2SmControlMessage with sutable format.
                asn_decode (nullptr, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2SM_RC_ControlMessage,
                            (void **) &e2SmControlMessage, ie->value.choice.RICcontrolMessage.buf,
                            ie->value.choice.RICcontrolMessage.size);

                NS_LOG_DEBUG("********** START Print Message and Size");
                NS_LOG_DEBUG(ie->value.present);

                NS_LOG_DEBUG(ie->value.choice.RICcontrolMessage.size);
                int buf_lennnn = static_cast<int>(ie->value.choice.RICcontrolMessage.size);
                for(int i =0 ;i <  buf_lennnn; ++i) {

                NS_LOG_DEBUG(static_cast<int>(ie->value.choice.RICcontrolMessage.buf[i]));

                }
                NS_LOG_DEBUG("********** END Print Message and Size");

                NS_LOG_INFO (xer_fprint(stderr, &asn_DEF_E2SM_RC_ControlMessage, e2SmControlMessage));

                NS_LOG_DEBUG("****  e2SmControlMessage->present **** ");
                NS_LOG_DEBUG(e2SmControlMessage->ric_controlMessage_formats.present);

                const bool DISABLE_FOR_OCTANT_STRING = false;
                if (e2SmControlMessage->ric_controlMessage_formats.present == E2SM_RC_ControlMessage__ric_controlMessage_formats_PR_controlMessage_Format1)
                  {
                    m_e2SmRcControlMessageFormat1 = e2SmControlMessage->ric_controlMessage_formats.choice.controlMessage_Format1;



                    if(DISABLE_FOR_OCTANT_STRING) {
                        NS_LOG_DEBUG ("[E2SM] E2SM_RC_ControlMessage_PR_controlMessage_Format1");
                        E2SM_RC_ControlMessage_Format1_t *e2SmRcControlMessageFormat1 = (E2SM_RC_ControlMessage_Format1_t*) calloc(0, sizeof(E2SM_RC_ControlMessage_Format1_t));

                        e2SmRcControlMessageFormat1 = e2SmControlMessage->ric_controlMessage_formats.choice.controlMessage_Format1;
                        NS_LOG_INFO (xer_fprint(stderr, &asn_DEF_E2SM_RC_ControlMessage_Format1, e2SmRcControlMessageFormat1));
                        NS_LOG_DEBUG("*** DONE e2SmControlMessage->choice.controlMessage_Format1 **");
                        
                        assert(e2SmRcControlMessageFormat1 != nullptr && " e2SmRcControlMessageFormat1 is Null");
                        
                        NS_LOG_DEBUG("*** DONE ASSERT ExtractRANParametersFromControlMessage **");
                        m_valuesExtracted =
                            RicControlMessage::ExtractRANParametersFromControlMessage (e2SmRcControlMessageFormat1);
                        
                        NS_LOG_DEBUG("*** DONE ExtractRANParametersFromControlMessage **");
                        if (m_requestType == ControlMessageRequestIdType::TS)
                        {
                            // Get and parse the secondaty cell id according to 3GPP TS 38.473, Section 9.2.2.1
                            for (RANParameterItem item : m_valuesExtracted)
                            {
                                if (item.m_valueType == RANParameterItem::ValueType::OctectString)
                                {
                                    // First 3 digits are the PLMNID (always 111), last digit is CellId
                                    std::string cgi = item.m_valueStr->DecodeContent ();
                                    NS_LOG_INFO ("Decoded CGI value is: " << cgi);
                                    m_secondaryCellId = cgi.back();
                                }
                            }         
                        }
                    }
              
                  }
                else
                  {
                    NS_LOG_DEBUG("[E2SM] Error in checking format of E2SM Control Message");
                  }
                break;
            }
            case RICcontrolRequest_IEs__value_PR_RICcontrolAckRequest: {
                NS_LOG_DEBUG("[E2SM] RICcontrolRequest_IEs__value_PR_RICcontrolAckRequest");

                switch (ie->value.choice.RICcontrolAckRequest) {
                    case RICcontrolAckRequest_noAck: {
                        NS_LOG_DEBUG("[E2SM] RIC Control ack value: NO ACK");
                        break;
                    }
                    case RICcontrolAckRequest_ack: {
                        NS_LOG_DEBUG("[E2SM] RIC Control ack value: ACK");
                        break;
                    }
                    case RICcontrolAckRequest_nAck: {
                        NS_LOG_DEBUG("[E2SM] RIC Control ack value: NACK");
                        break;
                    }
                    default: {
                        NS_LOG_DEBUG("[E2SM] RIC Control ack value unknown");
                        break;
                    }
                }
                break;
            }
            case RICcontrolRequest_IEs__value_PR_NOTHING: {
                NS_LOG_DEBUG("[E2SM] RICcontrolRequest_IEs__value_PR_NOTHING");
                NS_LOG_DEBUG("[E2SM] Nothing");
                break;
            }
            default: {
                NS_LOG_DEBUG("[E2SM] RIC Control value unknown");
                break;
            }
        }
    }

    NS_LOG_INFO ("End of DecodeRicControlMessage");
}

std::string
RicControlMessage::GetSecondaryCellIdHO ()
{
  return m_secondaryCellId;
}

/*std::vector<RANParameterItem> RicControlMessage::ExtractRANParametersFromControlMessage (
      E2SM_RC_ControlMessage_Format1_t *e2SmRcControlMessageFormat1) 
{
  NS_LOG_DEBUG("***** ExtractRANParametersFromControlMessage 0 ****");
  std::vector<RANParameterItem> ranParameterList;
  NS_LOG_DEBUG("***** ExtractRANParametersFromControlMessage 1 *****");

  assert(e2SmRcControlMessageFormat1->ranP_List != nullptr && " ranParametersList is null");
  // Not implemeted in the FlexRIC side 
  int count = e2SmRcControlMessageFormat1->ranP_List.list.count;

  NS_LOG_DEBUG("***** ExtractRANParametersFromControlMessage 2 *****");
  for (int i = 0; i < count; i++)
    {
      RANParameter_Item_t *ranParameterItem =
          e2SmRcControlMessageFormat1->ranP_List.list.array[i];
      for (RANParameterItem extractedParameter :
           RANParameterItem::ExtractRANParametersFromRANParameter (ranParameterItem))
        {
          ranParameterList.push_back (extractedParameter);
        }
    }

  return ranParameterList;
}*/

} // namespace ns3
