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

#include <ns3/oran-interface.h>
#include <ns3/asn1c-types.h>
 
#include <ns3/log.h>
#include <thread>
#include "encode_e2apv1.hpp"
#include<unistd.h>
extern "C" {
  #include "RICsubscriptionRequest.h"
  #include "RICactionType.h"
  #include "ProtocolIE-Field.h"
  #include "InitiatingMessage.h"
}

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("E2Termination");

NS_OBJECT_ENSURE_REGISTERED (E2Termination);

TypeId E2Termination::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::E2Termination")
    .SetParent<Object>()
    .AddConstructor<E2Termination>();
  return tid;
}

E2Termination::E2Termination ()
{
  NS_FATAL_ERROR("Do not use the default constructor");
}

E2Termination::E2Termination(const std::string ricAddress, 
                  const uint16_t ricPort,
                  const uint16_t clientPort,
                  const std::string gnbId,
                  const std::string plmnId)
  : m_ricAddress (ricAddress),
    m_ricPort (ricPort),
    m_clientPort (clientPort),
    m_gnbId (gnbId),
    m_plmnId(plmnId)
{
  NS_LOG_FUNCTION (this);
  m_e2sim = new E2Sim;
  
  // create a new file which will be used to trace the encoded messages
  // TODO create an appropriate log class to handle these messages
  // FILE* f = fopen ("messages.txt", "w");
  // fclose (f);
}

void
E2Termination::RegisterFunctionDescToE2Sm (long ranFunctionId, Ptr<FunctionDescription> ranFunctionDescription)
{
  // create an octet string and copy the e2smbuffer
  OCTET_STRING_t *rfdBuf = (OCTET_STRING_t *) calloc (1, sizeof (OCTET_STRING_t));
  rfdBuf->buf = (uint8_t *) calloc (1, ranFunctionDescription->m_size);
  rfdBuf->size = ranFunctionDescription->m_size;
  memcpy (rfdBuf->buf, ranFunctionDescription->m_buffer, ranFunctionDescription->m_size);

  m_e2sim->register_e2sm (ranFunctionId, rfdBuf);
}

void
E2Termination::RegisterKpmCallbackToE2Sm (long ranFunctionId, Ptr<FunctionDescription> ranFunctionDescription,
                             SubscriptionCallback sbCb)
{
  RegisterFunctionDescToE2Sm (ranFunctionId,ranFunctionDescription);
  m_e2sim->register_subscription_callback (ranFunctionId, sbCb);
}

void
E2Termination::RegisterSmCallbackToE2Sm (long ranFunctionId, Ptr<FunctionDescription> ranFunctionDescription, SmCallback smCb)
{
  RegisterFunctionDescToE2Sm (ranFunctionId,ranFunctionDescription);
  m_e2sim->register_sm_callback (ranFunctionId, smCb);
}

void
E2Termination::RegisterCallbackFunctionToE2Sm (long functionId,CallbackFunction CbFun)
{
  m_e2sim->register_callback (functionId, CbFun);
}

void E2Termination::Start ()
{
  NS_LOG_FUNCTION (this);

  NS_ABORT_MSG_IF(m_ricAddress.empty(), "Set the RIC information first");
  
  // create a thread to host e2sim execution
  std::thread e2simThread (&E2Termination::DoStart, this);
  e2simThread.detach ();
}

void E2Termination::DoStart ()
{
  NS_LOG_FUNCTION (this);
  
  // start e2sim main loop
  // char second[14]; // RIC ADDRESS
  // std::strcpy (second, m_ricAddress.c_str ());
  // char third[6]; // RIC PORT
  // std::strcpy (third, std::to_string (m_ricPort).c_str ());
  // char fourth[5]; // GNB ID value
  // std::strncpy (fourth, m_gnbId.c_str (), 4);
  // char fifth[6]; // CLIENT PORT
  // std::strcpy (fifth, std::to_string (m_clientPort).c_str ());
  // char sixth[4]; //PLMN ID
  // std::strcpy (sixth, m_plmnId.c_str ());

  NS_LOG_INFO ("In ns3::E2Term:  GNB" << m_gnbId << ", clientPort " << m_clientPort << ", ricPort "
                                 << m_ricPort <<  ", PlmnID "
                                 << m_plmnId);

  // char* argv [] = {nullptr, &second [0], &third [0], &fourth[0], &fifth[0],&sixth[0]};
  m_e2sim->run_loop (m_ricAddress, m_ricPort, m_clientPort, m_gnbId, m_plmnId);
}

E2Termination::~E2Termination ()
{
  NS_LOG_FUNCTION (this);
  delete m_e2sim;
}

E2Termination::RicSubscriptionRequest_rval_s 
E2Termination::ProcessRicSubscriptionRequest (E2AP_PDU_t* sub_req_pdu)
{
  //Record RIC Request ID
  //Go through RIC action to be Setup List
  //Find first entry with REPORT action Type
  //Record ricActionID
  //Encode subscription response

  RICsubscriptionRequest_t orig_req = sub_req_pdu->choice.initiatingMessage->value.choice.RICsubscriptionRequest;

  // RICsubscriptionResponse_IEs_t *ricreqid = (RICsubscriptionResponse_IEs_t*)calloc(1, sizeof(RICsubscriptionResponse_IEs_t));
           
  int count = orig_req.protocolIEs.list.count;
  int size = orig_req.protocolIEs.list.size;

  RICsubscriptionRequest_IEs_t **ies = (RICsubscriptionRequest_IEs_t**)orig_req.protocolIEs.list.array;

  NS_LOG_DEBUG ("Number of IEs " << count);
  NS_LOG_DEBUG ("Size of IEs " << size);

  RICsubscriptionRequest_IEs__value_PR pres;
  
  uint16_t reqRequestorId {};
  uint16_t reqInstanceId {};
  uint16_t ranFuncionId {};
  uint8_t reqActionId {};
  
  std::vector<long> actionIdsAccept;
  std::vector<long> actionIdsReject;
  
  // iterate over the IEs
  for (int i = 0; i < count; i++) 
  {
    RICsubscriptionRequest_IEs_t *next_ie = ies[i];
    pres = next_ie->value.present; // value of the current IE
      
    switch(pres) 
    {
      // IE containing the RIC Request ID
      case RICsubscriptionRequest_IEs__value_PR_RICrequestID:
        {
          NS_LOG_DEBUG ("Processing RIC Request ID field");	
          RICrequestID_t reqId = next_ie->value.choice.RICrequestID;
          reqRequestorId = reqId.ricRequestorID;
          reqInstanceId = reqId.ricInstanceID;
          NS_LOG_DEBUG ( "RIC Requestor ID " << reqRequestorId);
          NS_LOG_DEBUG ( "RIC Instance ID " << reqInstanceId);
          break;
        }
      // IE containing the RAN Function ID
      case RICsubscriptionRequest_IEs__value_PR_RANfunctionID:
        {
          NS_LOG_DEBUG ("Processing RAN Function ID field");	
          ranFuncionId = next_ie->value.choice.RANfunctionID;
          NS_LOG_DEBUG ("RAN Function ID " << ranFuncionId);
          break;
        }
      case RICsubscriptionRequest_IEs__value_PR_RICsubscriptionDetails:
        {
          NS_LOG_DEBUG ("Processing RIC Subscription Details field");
          RICsubscriptionDetails_t subDetails = next_ie->value.choice.RICsubscriptionDetails;
          
          // RIC Event Trigger Definition
          RICeventTriggerDefinition_t triggerDef = subDetails.ricEventTriggerDefinition;

          // TODO How to decode this field?
          uint8_t size = 20;  
          uint8_t *buf = (uint8_t *)calloc(1,size);
          memcpy(buf, &triggerDef, size);
          NS_LOG_DEBUG ("RIC Event Trigger Definition " << std::to_string (*buf));
                    
          // Sequence of actions
          RICactions_ToBeSetup_List_t actionList = subDetails.ricAction_ToBeSetup_List;
          // TODO We are ignoring the trigger definition
  
          int actionCount = actionList.list.count;
          NS_LOG_DEBUG ("Number of actions " << actionCount);
  
          auto **item_array = actionList.list.array;
          bool foundAction = false;
  
          for (int i = 0; i < actionCount; i++) 
          {
            auto *next_item = item_array[i];
            RICactionID_t actionId = ((RICaction_ToBeSetup_ItemIEs*)next_item)->value.choice.RICaction_ToBeSetup_Item.ricActionID;
            RICactionType_t actionType = ((RICaction_ToBeSetup_ItemIEs*)next_item)->value.choice.RICaction_ToBeSetup_Item.ricActionType;
                        
            //We identify the first action whose type is REPORT
            //That is the only one accepted; all others are rejected
            if (!foundAction && (actionType == RICactionType_report || actionType == RICactionType_insert))
            {
              reqActionId = actionId;
              actionIdsAccept.push_back(reqActionId);
              NS_LOG_DEBUG ("Action ID " << actionId << " accepted");
              foundAction = true;
            } 
            else 
            {
              reqActionId = actionId;
              NS_LOG_DEBUG ("Action ID " << actionId << " rejected");
              // actionIdsReject.push_back(reqActionId);
            }
          }
          break;
        }
      default:
        {
          NS_LOG_DEBUG ("in case default");	
          break;
        }      
      }
  }
  
  NS_LOG_DEBUG ("Create RIC Subscription Response");
  
  E2AP_PDU *e2ap_pdu = (E2AP_PDU*)calloc(1,sizeof(E2AP_PDU));

  long *accept_array = &actionIdsAccept[0];
  long *reject_array = &actionIdsReject[0];
  int accept_size = actionIdsAccept.size();
  int reject_size = actionIdsReject.size();

  // RIC-SUB-RES
  encoding::generate_e2apv1_subscription_response_success(e2ap_pdu, accept_array, reject_array, accept_size, reject_size, reqRequestorId, reqInstanceId);

  NS_LOG_DEBUG ("Send RIC Subscription Response");
  m_e2sim->encode_and_send_sctp_data(e2ap_pdu);

  RicSubscriptionRequest_rval_s reqParams;
  reqParams.requestorId = reqRequestorId;
  reqParams.instanceId = reqInstanceId;
  reqParams.ranFuncionId = ranFuncionId;
  reqParams.actionId = reqActionId;
  return reqParams;
}

void
E2Termination::SendE2Message (E2AP_PDU* pdu)
{
  m_e2sim->encode_and_send_sctp_data (pdu);
  // sleep(1); 
}

}
