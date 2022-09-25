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

#ifndef FUNCTION_DESCRIPTION_H
#define FUNCTION_DESCRIPTION_H

#include "ns3/object.h"

// extern "C" {
//   #include "E2SM-KPM-RANfunction-Description.h"
//   #include "E2SM-KPM-IndicationHeader.h"
//   #include "E2SM-KPM-IndicationMessage.h"
//   #include "RAN-Container.h"
//   #include "PF-Container.h"
//   #include "OCUUP-PF-Container.h"
//   #include "PF-ContainerListItem.h"
//   #include "asn1c-types.h"
// }

namespace ns3 {

  class FunctionDescription : public SimpleRefCount<FunctionDescription>
  {
  public:
    FunctionDescription ();
    ~FunctionDescription ();

    void* m_buffer;
    size_t m_size;
    
    // TODO improve the abstraction
//   private:
//     virtual void Encode (E2SM_KPM_RANfunction_Description_t* descriptor);
  };
  
}

#endif /* FUNCTION_DESCRIPTION_H */
