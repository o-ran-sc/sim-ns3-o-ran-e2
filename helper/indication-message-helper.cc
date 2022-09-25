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

#include <ns3/indication-message-helper.h>

namespace ns3 {

IndicationMessageHelper::IndicationMessageHelper (IndicationMessageType type, bool isOffline,
                                                  bool reducedPmValues)
    : m_type (type), m_offline (isOffline), m_reducedPmValues (reducedPmValues)
{

  if (!m_offline)
    {
      switch (type)
        {
        case IndicationMessageType::CuUp:
          m_cuUpValues = Create<OCuUpContainerValues> ();
          break;

        case IndicationMessageType::CuCp:
          m_cuCpValues = Create<OCuCpContainerValues> ();
          m_msgValues.m_cellObjectId = "NRCellCU";
          break;

        case IndicationMessageType::Du:
          m_duValues = Create<ODuContainerValues> ();
          break;

        default:

          break;
        }
    }
}

void
IndicationMessageHelper::FillBaseCuUpValues (std::string plmId)
{
  NS_ABORT_MSG_IF (m_type != IndicationMessageType::CuUp, "Wrong function for this object");
  m_cuUpValues->m_plmId = plmId;
  m_msgValues.m_pmContainerValues = m_cuUpValues;
}

void
IndicationMessageHelper::FillBaseCuCpValues (uint16_t numActiveUes)
{
  NS_ABORT_MSG_IF (m_type != IndicationMessageType::CuCp, "Wrong function for this object");
  m_cuCpValues->m_numActiveUes = numActiveUes;
  m_msgValues.m_pmContainerValues = m_cuCpValues;
}

IndicationMessageHelper::~IndicationMessageHelper ()
{
}

Ptr<KpmIndicationMessage>
IndicationMessageHelper::CreateIndicationMessage ()
{
  return Create<KpmIndicationMessage> (m_msgValues);
}

} // namespace ns3