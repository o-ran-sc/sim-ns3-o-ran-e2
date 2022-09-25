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

#ifndef INDICATION_MESSAGE_HELPER_H
#define INDICATION_MESSAGE_HELPER_H

#include <ns3/kpm-indication.h>

namespace ns3 {

class IndicationMessageHelper : public Object
{
public:
  enum class IndicationMessageType { CuCp = 0, CuUp = 1, Du = 2 };
  IndicationMessageHelper (IndicationMessageType type, bool isOffline, bool reducedPmValues);

  ~IndicationMessageHelper ();

  Ptr<KpmIndicationMessage> CreateIndicationMessage ();

  bool const &
  IsOffline () const
  {
    return m_offline;
  }

protected:
  void FillBaseCuUpValues (std::string plmId);

  void FillBaseCuCpValues (uint16_t numActiveUes);

  IndicationMessageType m_type;
  bool m_offline;
  bool m_reducedPmValues;
  KpmIndicationMessage::KpmIndicationMessageValues m_msgValues;
  Ptr<OCuUpContainerValues> m_cuUpValues;
  Ptr<OCuCpContainerValues> m_cuCpValues;
  Ptr<ODuContainerValues> m_duValues;
};

} // namespace ns3

#endif /* INDICATION_MESSAGE_HELPER_H */