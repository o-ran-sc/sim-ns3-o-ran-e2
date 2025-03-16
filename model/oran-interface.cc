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
 *         Tommaso Zugno <tommasozugno@gmail.com>
 *         Michele Polese <michele.polese@gmail.com>
 */

#include <ns3/oran-interface.h>
#include <ns3/asn1c-types.h>
#include <ns3/log.h>
#include <thread>
#include "encode_e2apv1.hpp"
#include <unistd.h>
#include <any>
//#include <boost/any.hpp>
#include <string>
#include <map>
#include <typeinfo>

extern "C" {
  #include "RICsubscriptionRequest.h"
  #include "RICactionType.h"
  #include "ProtocolIE-Field.h"
  #include "InitiatingMessage.h"
  #include "RICactionDefinition.h"
  #include "RICsubsequentAction.h"
  #include "E2SM-KPM-EventTriggerDefinition.h"
  #include "E2SM-KPM-EventTriggerDefinition-Format1.h"
  #include "E2SM-KPM-ActionDefinition.h"
  #include "E2SM-KPM-ActionDefinition-Format4.h"
  #include "MatchingUeCondPerSubList.h"
  #include "MatchingUeCondPerSubItem.h"
  #include "TestCondInfo.h"
  #include "TestCond-Type.h"
  #include "TestCond-Expression.h"
  #include "TestCond-Value.h"
  #include "E2SM-KPM-ActionDefinition-Format1.h"
  #include "MeasurementInfoList.h"
  #include "GranularityPeriod.h"
  #include "CGI.h"
  #include "MeasurementInfoItem.h"
  #include "MeasurementType.h"
  #include "LabelInfoList.h"
  #include "LabelInfoItem.h"
  #include "MeasurementLabel.h"
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
 // NS_LOG_INFO("E2Termination Constructor Called");
    // subs_details.clear();
}

void E2Termination::StoreSubscriptionDetail(const std::string& key, const std::any& value)
{
    subs_details[key] = value;
    NS_LOG_INFO("Stored Subscription Detail: " << key);
}

template<typename T>
T E2Termination::GetSubscriptionDetail(const std::string& key, const T& defaultValue) const
{
    auto it = subs_details.find(key);
    if (it != subs_details.end())
    {
        try
        {
            return std::any_cast<T>(it->second);
        }
        catch (const std::bad_any_cast&)
        {
            NS_LOG_WARN("Type mismatch when retrieving key: " << key);
        }
    }
    return defaultValue;
}

void E2Termination::PrintSubscriptionDetails() const
{
    for (const auto& pair : subs_details) {
    // Print the key
    printf("%s: ", pair.first.c_str());

        if (pair.second.type() == typeid(int)) {
            printf("%d\n", std::any_cast<int>(pair.second));
        } else if (pair.second.type() == typeid(std::string)) {
            printf("%s\n", std::any_cast<std::string>(pair.second).c_str());
        } else if (pair.second.type() == typeid(double)) {
            printf("%f\n", std::any_cast<double>(pair.second));
        } else if (pair.second.type() == typeid(uint64_t)) {
            printf("%lu\n", std::any_cast<uint64_t>(pair.second));
        } else {
            printf("Unknown type\n");
        }
  
}
}

void E2Termination::DecodeLabelInfo(MeasurementLabel_t* label) {
      if (label->noLabel) {
        StoreSubscriptionDetail("Measurement Label",std::string("No Label"));
        //std::any_cast<std::string>
    }
    if (label->plmnID) {
        StoreSubscriptionDetail("PLMN ID",std::string(reinterpret_cast<char*>(label->plmnID->buf)));
        //std::any_cast<std::string>
    }
    if (label->fiveQI) {
        StoreSubscriptionDetail("FiveQI",std::any_cast<int>(*label->fiveQI));
    }
    if (label->qFI) {
        StoreSubscriptionDetail("QoS Flow Identifier",std::any_cast<int>(*label->qFI));
    }
    if (label->qCI) {
        StoreSubscriptionDetail("QCI",std::any_cast<int>(*label->qCI));
    }
    if (label->startEndInd) {
        StoreSubscriptionDetail("startEndInd",std::any_cast<int>(*label->startEndInd));
    }
    if (label->min) {
        StoreSubscriptionDetail("min",std::any_cast<int>(*label->min));
    }
    if (label->max) {
        StoreSubscriptionDetail("max",std::any_cast<int>(*label->max));
    }
    if (label->avg) {
        StoreSubscriptionDetail("avg",std::any_cast<int>(*label->avg));
    }
}

void E2Termination::DecodeRICEventTriggerDefinition(const uint8_t* buffer, size_t size) {
    E2SM_KPM_EventTriggerDefinition_t* eventTriggerDef = NULL;

    asn_dec_rval_t rval = asn_decode(
        NULL, ATS_ALIGNED_BASIC_PER,
        &asn_DEF_E2SM_KPM_EventTriggerDefinition,
        (void**)&eventTriggerDef, buffer, size
    );
    assert(rval.code == RC_OK && "Event Trigger Definition decoding failed!");

    if (eventTriggerDef->eventDefinition_formats.present == E2SM_KPM_EventTriggerDefinition__eventDefinition_formats_PR_eventDefinition_Format1) {
        uint64_t reportingPeriod = eventTriggerDef->eventDefinition_formats.choice.eventDefinition_Format1->reportingPeriod;
        StoreSubscriptionDetail("Event Trigger Definition Format",static_cast<int>(E2SM_KPM_EventTriggerDefinition__eventDefinition_formats_PR_eventDefinition_Format1));
        StoreSubscriptionDetail("Reporting Period",std::any_cast<uint64_t>(reportingPeriod));
    } else {
        NS_LOG_DEBUG("Unknown or unsupported Event Trigger Definition Format");
    }

    ASN_STRUCT_FREE(asn_DEF_E2SM_KPM_EventTriggerDefinition, eventTriggerDef);
}

void E2Termination::DecodeRICActionDefinition(const uint8_t* buffer, size_t size) {
    E2SM_KPM_ActionDefinition_t* actionDef = NULL;

    asn_dec_rval_t rval = asn_decode(NULL, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2SM_KPM_ActionDefinition, (void**)&actionDef, buffer, size);

    assert(rval.code == RC_OK && "Action Definition decoding failed!");

    StoreSubscriptionDetail("RIC Style Type", static_cast<int>(actionDef->ric_Style_Type));
    switch (actionDef->actionDefinition_formats.present) {
    case E2SM_KPM_ActionDefinition__actionDefinition_formats_PR_actionDefinition_Format1:
        StoreSubscriptionDetail("Action Definition Format",static_cast<int>(E2SM_KPM_ActionDefinition__actionDefinition_formats_PR_actionDefinition_Format1));
        break;
    case E2SM_KPM_ActionDefinition__actionDefinition_formats_PR_actionDefinition_Format2:
        StoreSubscriptionDetail("Action Definition Format",static_cast<int>(E2SM_KPM_ActionDefinition__actionDefinition_formats_PR_actionDefinition_Format2));
        break;
    case E2SM_KPM_ActionDefinition__actionDefinition_formats_PR_actionDefinition_Format3:
        StoreSubscriptionDetail("Action Definition Format",static_cast<int>(E2SM_KPM_ActionDefinition__actionDefinition_formats_PR_actionDefinition_Format3));
        break;
    case E2SM_KPM_ActionDefinition__actionDefinition_formats_PR_actionDefinition_Format4: {
        StoreSubscriptionDetail("Action Definition Format",static_cast<int>(E2SM_KPM_ActionDefinition__actionDefinition_formats_PR_actionDefinition_Format4));

        E2SM_KPM_ActionDefinition_Format4_t* format4 = actionDef->actionDefinition_formats.choice.actionDefinition_Format4;
        MatchingUeCondPerSubList_t* matchingList = &format4->matchingUeCondList;

        for (int i = 0; i < matchingList->list.count; i++) {
            MatchingUeCondPerSubItem_t* subItem = matchingList->list.array[i];
            TestCondInfo_t* testCondInfo = &subItem->testCondInfo;
            // printf("Condition %d:\n", i + 1);
            switch (testCondInfo->testType.present) {
                case TestCond_Type_PR_gBR:
                    StoreSubscriptionDetail("Test Condition Type",static_cast<int>(TestCond_Type_PR_gBR));
                    break;
                case TestCond_Type_PR_aMBR:
                    StoreSubscriptionDetail("Test Condition Type",static_cast<int>(TestCond_Type_PR_aMBR));
                    break;
                case TestCond_Type_PR_isStat:
                   StoreSubscriptionDetail("Test Condition Type",static_cast<int>(TestCond_Type_PR_isStat));
                    break;
                case TestCond_Type_PR_isCatM:
                    StoreSubscriptionDetail("Test Condition Type",static_cast<int>(TestCond_Type_PR_isCatM));
                    break;
                case TestCond_Type_PR_rSRP:
                    StoreSubscriptionDetail("Test Condition Type",static_cast<int>(TestCond_Type_PR_rSRP));
                    break;
                case TestCond_Type_PR_rSRQ:
                    StoreSubscriptionDetail("Test Condition Type",static_cast<int>(TestCond_Type_PR_rSRQ));
                    break;
                case TestCond_Type_PR_ul_rSRP:
                    StoreSubscriptionDetail("Test Condition Type",static_cast<int>(TestCond_Type_PR_ul_rSRP));
                    break;
                case TestCond_Type_PR_cQI:
                    StoreSubscriptionDetail("Test Condition Type",static_cast<int>(TestCond_Type_PR_cQI));
                    break;
                case TestCond_Type_PR_fiveQI:
                    StoreSubscriptionDetail("Test Condition Type",static_cast<int>(TestCond_Type_PR_fiveQI));
                    break;
                case TestCond_Type_PR_qCI:
                    StoreSubscriptionDetail("Test Condition Type",static_cast<int>(TestCond_Type_PR_qCI));
                    break;
                case TestCond_Type_PR_sNSSAI:
                    StoreSubscriptionDetail("Test Condition Type",static_cast<int>(TestCond_Type_PR_sNSSAI));
                    break;
                default:
                    StoreSubscriptionDetail("Test Condition Type",static_cast<int>(TestCond_Type_PR_NOTHING));
                    break;
            }

            if (*testCondInfo->testExpr == TestCond_Expression_equal) {
                StoreSubscriptionDetail("Test Condition Expression",static_cast<int>(TestCond_Expression_equal));
            }
            else if (*testCondInfo->testExpr == TestCond_Expression_greaterthan) {
                StoreSubscriptionDetail("Test Condition Expression",static_cast<int>(TestCond_Expression_greaterthan));
            } else if (*testCondInfo->testExpr == TestCond_Expression_lessthan) {
                StoreSubscriptionDetail("Test Condition Expression",static_cast<int>(TestCond_Expression_lessthan));
            } else if (*testCondInfo->testExpr == TestCond_Expression_contains) {
                StoreSubscriptionDetail("Test Condition Expression",static_cast<int>(TestCond_Expression_contains));
            } else if (*testCondInfo->testExpr == TestCond_Expression_present) {
                StoreSubscriptionDetail("Test Condition Expression",static_cast<int>(TestCond_Expression_present));
            } else {
            }

            TestCond_Value_t* testValue = testCondInfo->testValue;
            if (testValue) {
                switch (testValue->present) {
                    case TestCond_Value_PR_valueInt:
                        StoreSubscriptionDetail("Test Condition Value",static_cast<int>(testValue->choice.valueInt));
                        break;
                    case TestCond_Value_PR_valueEnum:
                        StoreSubscriptionDetail("Test Condition Value",static_cast<int>(testValue->choice.valueEnum));
                        break;
                    case TestCond_Value_PR_valueBool:
                        StoreSubscriptionDetail("Test Condition Value",testValue->choice.valueBool);
                        break;
                    case TestCond_Value_PR_valueBitS:
                        StoreSubscriptionDetail("Test Condition Value", testValue->choice.valueBitS.buf);
                        break;
                    case TestCond_Value_PR_valueOctS:
                        StoreSubscriptionDetail("Test Condition Value", testValue->choice.valueOctS.buf);
                        break;
                    case TestCond_Value_PR_valuePrtS:
                        StoreSubscriptionDetail("Test Condition Value", testValue->choice.valuePrtS.buf);
                        break;
                    case TestCond_Value_PR_valueReal:
                        StoreSubscriptionDetail("Test Condition Value", testValue->choice.valueReal);
                        break;
                    default:
                         break;
                    }
            } else {
                   StoreSubscriptionDetail("Test Condition Value", NULL);
            }

            }
        //////////////////////////////////////////
        // Decode Subscription Info (Format1)
        E2SM_KPM_ActionDefinition_Format1_t* subscriptionInfo = &format4->subscriptionInfo;
         //printf("Decoding Subscription Info:\n");
        uint64_t granularityPeriod = subscriptionInfo->granulPeriod;
        StoreSubscriptionDetail("Granularity Period",std::any_cast<uint64_t>(granularityPeriod));
        MeasurementInfoList_t* measInfoList = &subscriptionInfo->measInfoList;
        if (measInfoList->list.count == 0) {
            // printf("Measurement Info List is empty\n");
        } else {
            //printf("Measurement Info List:\n");
            for (int i = 0; i < measInfoList->list.count; i++) {
            MeasurementInfoItem_t* measItem = measInfoList->list.array[i];
            //printf("Measurement Item %d:\n", i + 1);
            
            if (measItem->measType.present == MeasurementType_PR_measName) {
              StoreSubscriptionDetail("Measurement Name",std::string(reinterpret_cast<char*>(measItem->measType.choice.measName.buf)));
              //std::any_cast<std::string>
            } else {
              //printf("Measurement Type: Unknown or unsupported\n");
            }
            
            // Decode label info list
            LabelInfoList_t* labelInfoList = &measItem->labelInfoList;
            if (labelInfoList->list.count == 0) {
                //printf("  Label Info List is empty\n");
            } else {
                // printf("  Label Info List:\n");
                for (int j = 0; j < labelInfoList->list.count; j++) {
                    LabelInfoItem_t* labelItem = labelInfoList->list.array[j];
                   // printf("    Label Item %d:\n", j + 1);
                    DecodeLabelInfo(&labelItem->measLabel);
                }
            }
        }}
      ////////////////////////////////
        break;
    }

    case E2SM_KPM_ActionDefinition__actionDefinition_formats_PR_actionDefinition_Format5:
        StoreSubscriptionDetail("Action Definition Format", static_cast<int>(E2SM_KPM_ActionDefinition__actionDefinition_formats_PR_actionDefinition_Format5));
        break;

    default:
        printf("Unknown or unsupported Action Definition Format\n");
        break;
}
    ASN_STRUCT_FREE(asn_DEF_E2SM_KPM_ActionDefinition, actionDef);
}

const Subscription_map& E2Termination::SubscriptionMapRef() const{
  return subs_details; 
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

  RICsubscriptionRequest_t *orig_req =(RICsubscriptionRequest_t*)calloc(1, sizeof(RICsubscriptionRequest_t));

  orig_req=&sub_req_pdu->choice.initiatingMessage->value.choice.RICsubscriptionRequest;

  // RICsubscriptionResponse_IEs_t *ricreqid = (RICsubscriptionResponse_IEs_t*)calloc(1, sizeof(RICsubscriptionResponse_IEs_t));
           
  int count = orig_req->protocolIEs.list.count;
  int size = orig_req->protocolIEs.list.size;

  RICsubscriptionRequest_IEs_t **ies = (RICsubscriptionRequest_IEs_t**)orig_req->protocolIEs.list.array;
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
   // std::cout << "IE " << i << ": pres = " << static_cast<int>(pres) 
     //         << ", IE type = " << next_ie->id << std::endl;
      
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
          ////////////////////////
          // TODO How to decode this field?
          // uint8_t size = 20;  
           //uint8_t *buf = (uint8_t *)calloc(1,size);
           //memcpy(buf, &triggerDef.buf, size);
           //DecodeRICEventTriggerDefinition(buf, size);

           const uint8_t* buf = triggerDef.buf; 
           size_t size_event_trigger = triggerDef.size;  
          DecodeRICEventTriggerDefinition(buf, size_event_trigger);
          NS_LOG_DEBUG ("RIC Event Trigger Definition " << std::to_string (*buf)); 
          //NS_LOG_DEBUG("Size of Event Trigger Definition: " <<size);

   
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
         RICactionDefinition_t* actiondef = ((RICaction_ToBeSetup_ItemIEs*)next_item)->value.choice.RICaction_ToBeSetup_Item.ricActionDefinition;
         RICsubsequentAction_t* subsequentact=((RICaction_ToBeSetup_ItemIEs*)next_item)->value.choice.RICaction_ToBeSetup_Item.ricSubsequentAction;         
            
            //We identify the first action whose type is REPORT
            //That is the only one accepted; all others are rejected
            if (!foundAction && (actionType == RICactionType_report || actionType == RICactionType_insert))
            {
              reqActionId = actionId;
              actionIdsAccept.push_back(reqActionId);
              NS_LOG_DEBUG ("Action ID " << actionId << " accepted");
              foundAction = true;
              NS_LOG_DEBUG ("Action Type " << actionType << "\n");
              ////////////////////
              // RIC Action Definition. Optional 
              if (actiondef!=NULL) {
               uint8_t* buf_ad = (uint8_t*)calloc(1, actiondef->size);
              if (buf_ad) {
               memcpy(buf_ad, actiondef->buf, actiondef->size);
               NS_LOG_DEBUG("RIC Action Definition: " << std::to_string(*buf_ad));
               DecodeRICActionDefinition(buf_ad, actiondef->size);
               }}

        if (subsequentact!=NULL){
          NS_LOG_DEBUG("Subsequent Action Type: " << subsequentact->ricSubsequentActionType);
        if (subsequentact->ricTimeToWait) {
            NS_LOG_DEBUG("Time to Wait: " << subsequentact->ricTimeToWait);
        }
    }
    ////////////////////////////////////////
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
      // subsDetails_r
          // printf("The map:\n");
           //NS_LOG_INFO("Printing the map of Subscription Request details");
            PrintSubscriptionDetails();
            printf("Size of subsDetails_r: %ld\n", SubscriptionMapRef().size());
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
