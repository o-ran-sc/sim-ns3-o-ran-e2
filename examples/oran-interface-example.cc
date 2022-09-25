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

#include "ns3/core-module.h"
#include "ns3/oran-interface.h"
#include "encode_e2apv1.hpp"
#include <errno.h>


using namespace ns3;

Ptr<E2Termination> e2Term;
// TODO create getters for these parameters in e2Term
std::string plmId = "111";
uint16_t cellId = 1;
const std::string gnb = std::to_string (cellId);

/**
* Creates an empty RIC Report message and send it to the RIC
*
* \param params the RIC Subscription Request parameters
*/
static void BuildAndSendReportMessage (E2Termination::RicSubscriptionRequest_rval_s params)
{
  KpmIndicationHeader::KpmRicIndicationHeaderValues headerValues; 
  headerValues.m_plmId = plmId;
  headerValues.m_gnbId = cellId;
  headerValues.m_nrCellId = cellId;

  Ptr<KpmIndicationHeader> header = Create<KpmIndicationHeader> (KpmIndicationHeader::GlobalE2nodeType::gNB, headerValues);
  
  KpmIndicationMessage::KpmIndicationMessageValues msgValues;
  
  Ptr<OCuUpContainerValues> cuUpValues = Create<OCuUpContainerValues> ();
  cuUpValues->m_plmId = plmId;
  cuUpValues->m_pDCPBytesUL = 100;
  cuUpValues->m_pDCPBytesDL = 100;
  msgValues.m_pmContainerValues = cuUpValues;
  
  Ptr<MeasurementItemList> ue0DummyValues = Create<MeasurementItemList> ("UE-0");
  ue0DummyValues->AddItem<long> ("DRB.PdcpSduVolumeDl_Filter.UEID", 6);
  ue0DummyValues->AddItem<long> ("QosFlow.PdcpPduVolumeDL_Filter.UEID", 7);
  ue0DummyValues->AddItem<long> ("Tot.PdcpSduNbrDl.UEID", 8);
  ue0DummyValues->AddItem<long> ("DRB.PdcpPduNbrDl.Qos.UEID", 9);
  ue0DummyValues->AddItem<double> ("DRB.IPThpDl.UEID", 10.0);
  ue0DummyValues->AddItem<double> ("DRB.IPLateDl.UEID", 11.0);
  msgValues.m_ueIndications.insert (ue0DummyValues);
 
  Ptr<MeasurementItemList> ue1DummyValues = Create<MeasurementItemList> ("UE-1");;
  ue1DummyValues->AddItem<long> ("DRB.PdcpSduVolumeDl_Filter.UEID", 6);
  ue1DummyValues->AddItem<long> ("QosFlow.PdcpPduVolumeDL_Filter.UEID", 7);
  ue1DummyValues->AddItem<long> ("Tot.PdcpSduNbrDl.UEID", 8);
  ue1DummyValues->AddItem<long> ("DRB.PdcpPduNbrDl.Qos.UEID", 9);
  ue1DummyValues->AddItem<double> ("DRB.IPThpDl.UEID", 10.0);
  ue1DummyValues->AddItem<double> ("DRB.IPLateDl.UEID", 11.0);
  msgValues.m_ueIndications.insert (ue1DummyValues);
  
  Ptr<KpmIndicationMessage> msg = Create<KpmIndicationMessage> (msgValues);
  
  E2AP_PDU *pdu_cuup_ue = new E2AP_PDU;	
  encoding::generate_e2apv1_indication_request_parameterized(pdu_cuup_ue, 
                                                             params.requestorId,
                                                             params.instanceId,
                                                             params.ranFuncionId,
                                                             params.actionId,
                                                             1, // TODO sequence number  
                                                             (uint8_t*) header->m_buffer, // buffer containing the encoded header
                                                             header->m_size, // size of the encoded header
                                                             (uint8_t*) msg->m_buffer, // buffer containing the encoded message
                                                             msg->m_size); // size of the encoded message  
  e2Term->SendE2Message (pdu_cuup_ue);
  delete pdu_cuup_ue;
  
}

/**
* KPM Subscription Request callback.
* This function is triggered whenever a RIC Subscription Request for 
* the KPM RAN Function is received.
*
* \param pdu request message
*/
static void KpmSubscriptionCallback (E2AP_PDU_t* sub_req_pdu)
{
  NS_LOG_UNCOND ("\n\nReceived RIC Subscription Request");
  
  E2Termination::RicSubscriptionRequest_rval_s params = e2Term->ProcessRicSubscriptionRequest (sub_req_pdu);
  NS_LOG_UNCOND ("requestorId " << +params.requestorId << 
                 ", instanceId " << +params.instanceId << 
                 ", ranFuncionId " << +params.ranFuncionId << 
                 ", actionId " << +params.actionId);  
  
  BuildAndSendReportMessage (params);
}

/**
* RIC Control Message callback.
* This function is triggered whenever a RIC Control Message is received.
*
* \param pdu request message
*/
static void
RicControlMessageCallback (E2AP_PDU_t *ric_ctrl_pdu)
{
  NS_LOG_UNCOND ("\n\nReceived RIC Control Message");

  RicControlMessage msg = RicControlMessage (ric_ctrl_pdu);
  // TODO log something
}


int 
main (int argc, char *argv[])
{
  LogComponentEnable ("Asn1Types", LOG_LEVEL_ALL);
  LogComponentEnable ("RicControlMessage", LOG_LEVEL_ALL);
  e2Term = CreateObject<E2Termination> ("10.244.0.191", 36422, 38472, gnb, plmId);
  Ptr<KpmFunctionDescription> kpmFd = Create<KpmFunctionDescription> ();
  e2Term->RegisterKpmCallbackToE2Sm (200, kpmFd, &KpmSubscriptionCallback);    
  Ptr<RicControlFunctionDescription> rcFd = Create<RicControlFunctionDescription> ();
  e2Term->RegisterSmCallbackToE2Sm (300, rcFd, &RicControlMessageCallback);

  return 0;
}
