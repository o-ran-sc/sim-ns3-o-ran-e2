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

#ifndef KPM_FUNCTION_DESCRIPTION_H
#define KPM_FUNCTION_DESCRIPTION_H

#include "ns3/object.h"
#include <ns3/function-description.h>

extern "C" {
  #include "E2SM-KPM-RANfunction-Description.h"
  #include "E2SM-KPM-IndicationHeader.h"
  #include "E2SM-KPM-IndicationMessage.h"
  #include "RAN-Container.h"
  #include "PF-Container.h"
  #include "OCUUP-PF-Container.h"
  #include "PF-ContainerListItem.h"
  #include "asn1c-types.h"
}

namespace ns3 {

  class KpmFunctionDescription : public FunctionDescription
  {
  public:
    KpmFunctionDescription ();
    ~KpmFunctionDescription ();
    
  private:
    /**
    * Encodes the RAN Function Description item for the KPM Service Model.
    *
    * \param kpmFunctionDescription the RAN Function Description item
    */
    void FillAndEncodeKpmFunctionDescription (E2SM_KPM_RANfunction_Description_t* descriptor);
    void Encode (E2SM_KPM_RANfunction_Description_t* descriptor);
  };
  
}

#endif /* KPM_FUNCTION_DESCRIPTION_H */
