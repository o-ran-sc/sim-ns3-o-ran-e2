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
 
#ifndef ORAN_INTERFACE_H
#define ORAN_INTERFACE_H

#include "ns3/object.h"
#include <ns3/kpm-indication.h>
#include <ns3/kpm-function-description.h>
#include <ns3/ric-control-function-description.h>
// #include <ns3/ric-delete-function-description.h>
#include <ns3/ric-control-message.h>
#include "e2sim.hpp"

namespace ns3 {
  
  class E2Termination : public Object 
  {
    public:

      E2Termination();

      /**
      *
      * \param ricAddress RIC IP address
      * \param ricPort RIC port
      * \param clientPort the local port to which the client will bind 
      * \param gnbId the GNB ID
      * \param plmnId the PLMN ID
      */
      E2Termination(const std::string ricAddress, 
                  const uint16_t ricPort,
                  const uint16_t clientPort,
                  const std::string gnbId,
                  const std::string plmnId);
      
      virtual ~E2Termination ();
      
      /**
      *  inherited from Object
      * @return
      */
      static TypeId GetTypeId();
      
      /**
      * Start the E2 termination.
      * Create a separate thread to host the execution of e2sim. The thread will 
      * execute the method DoStart.  
      */
      void Start ();
      
      /**
      * Register an E2 Service Model.
      * Create a RAN Function Description item containing the configurations 
      * for the SM, add it to the list of supported RAN functions, and 
      * register a callback.
      * Whenever a RIC Subscription Request to this RAN Function is received, 
      * the callback is triggered.  
      *
      * \param ranFunctionId ID used to identify the KPM RAN Function
      * \param ranFunctionDescription 
      * \param cb callback that will be triggered if the RIC subscribes to 
      *        this function
      */
      void RegisterKpmCallbackToE2Sm (long ranFunctionId, 
                         Ptr<FunctionDescription> ranFunctionDescription, 
                         SubscriptionCallback sbCb);

      /**
      * Register an E2 Service Model.
      * Create a RAN Function Description item containing the configurations 
      * for the SM, add it to the list of supported RAN functions, and 
      * register a callback.
      * Whenever a Sm message to this RAN Function is received, 
      * the callback is triggered.  
      *
      * \param ranFunctionId ID used to identify the KPM RAN Function
      * \param ranFunctionDescription 
      * \param cb callback that will be triggered if the RIC subscribes to 
      *        this function
      */
      void RegisterSmCallbackToE2Sm (long ranFunctionId,
                                     Ptr<FunctionDescription> ranFunctionDescription,
                                     SmCallback smCb);

      /**
      * Reguster a callback function that handle events.
      *
      * \param functionId ID used to identify function wants to call.
      * \param CbFun callback function that will be triggered if its called.
      */
      void RegisterCallbackFunctionToE2Sm (long functionId,CallbackFunction CbFun);

      /**
      * Struct holding the values returned by ProcessRicSubscriptionRequest
      */
      struct RicSubscriptionRequest_rval_s
      {
        uint16_t requestorId; //!< RIC Requestor ID
        uint16_t instanceId; //!< RIC Instance ID
        uint16_t ranFuncionId; //!< RAN Function ID
        uint8_t actionId; //!< RIC Action ID
      }; 

      /**
      * Process RIC Subscription Request.
      * This function processes the RIC Subscription Request and sends the 
      * RIC Subscription Response.
      *
      * \param sub_req_pdu request message
      * \return RIC subscription request parameters
      */
      RicSubscriptionRequest_rval_s ProcessRicSubscriptionRequest (E2AP_PDU_t* sub_req_pdu);

      /**
      * Sends an E2 message to the RIC
      * This function encodes and sends an E2 message to the RIC
      *
      * \param pdu the PDU of the message
      */
      void SendE2Message (E2AP_PDU* pdu);   

    private:
      /**
      * Run the e2sim main loop.
      * Starts the e2sim main loop, it will open a socket towards the RIC and 
      * start the reception routine.
      */
      void DoStart ();

      /**
       * \brief Accessory function to populate to the registration of the ran function description to e2sim
       * 
       * \param ranFunctionId 
       * \param ranFunctionDescription 
       */
      void RegisterFunctionDescToE2Sm (long ranFunctionId,
                                Ptr<FunctionDescription> ranFunctionDescription);

      E2Sim* m_e2sim; //!< pointer to an instance of the O-RAN E2 simulator
      std::string m_ricAddress; //!< IP address of the RIC
      uint16_t m_ricPort; //!< port of the RIC
      uint16_t m_clientPort; //!< local bind port
      std::string m_gnbId; //!< GNB id
      std::string m_plmnId; //!< PLMN Id
  };
}

#endif /* ORAN_INTERFACE_H */
