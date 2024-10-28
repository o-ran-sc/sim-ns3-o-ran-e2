/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2022 Northeastern University
 * Copyright (c) 2022 Sapienza, University of Rome
 * Copyright (c) 2022 University of Padova
 * Copyright (c) 2024 Orange Innovation Egypt
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
 *		   Mostafa Ashraf <mostafa.ashraf.ext@orange.com>
 *       Aya Kamal <aya.kamal.ext@orange.com>
 *       Abdelrhman Soliman <abdelrhman.soliman.ext@orange.com>
 */

#include <ns3/asn1c-types.h>
#include <ns3/log.h>

NS_LOG_COMPONENT_DEFINE ("Asn1Types");

namespace ns3 {

OctetString::OctetString (std::string value, size_t size)
{
  NS_LOG_FUNCTION (this);
  CreateBaseOctetString (size);
  memcpy (m_octetString->buf, value.c_str (), size);
}

void OctetString::CreateBaseOctetString (size_t size)
{
  NS_LOG_FUNCTION (this);
  m_octetString = (OCTET_STRING_t *) calloc (1, sizeof (OCTET_STRING_t));
  m_octetString->buf = (uint8_t *) calloc (1, size);
  m_octetString->size = size;
}

OctetString::OctetString (void *value, size_t size)
{
  NS_LOG_FUNCTION (this);
  CreateBaseOctetString (size);
  memcpy (m_octetString->buf, value, size);
}

OctetString::~OctetString ()
{
  NS_LOG_FUNCTION (this);
  // if (m_octetString->buf != NULL)
    // free (m_octetString->buf);
  free (m_octetString);
}

OCTET_STRING_t *
OctetString::GetPointer ()
{
  return m_octetString;
}

OCTET_STRING_t
OctetString::GetValue ()
{
  return *m_octetString;
}

std::string OctetString::DecodeContent(){
  int size = this->GetValue ().size;
  char out[size + 1];
  std::memcpy (out, this->GetValue ().buf, size);
  out[size] = '\0';

  return std::string (out);
}

BitString::BitString (std::string value, size_t size)

{
  NS_LOG_FUNCTION (this);
  m_bitString = (BIT_STRING_t *) calloc (1, sizeof (BIT_STRING_t));
  m_bitString->buf = (uint8_t *) calloc (1, size);
  m_bitString->size = size;
  memcpy (m_bitString->buf, value.c_str(), size);
}

BitString::BitString (std::string value, size_t size, size_t bits_unused)
    : BitString::BitString (value, size)
{
  NS_LOG_FUNCTION (this);
  m_bitString->bits_unused = bits_unused;
}

BitString::~BitString ()
{
  NS_LOG_FUNCTION (this);
  free (m_bitString);
}

BIT_STRING_t *
BitString::GetPointer ()
{
  return m_bitString;
}

BIT_STRING_t
BitString::GetValue ()
{
  return *m_bitString;
}

NrCellId::NrCellId (uint16_t value)
{
  NS_LOG_FUNCTION (this);
  
  // TODO check why with more than 15 cells is not working
  // if (value > 15)
  // {
  //   NS_FATAL_ERROR ("TODO: update the encoding to support more than 15 cells");
  // }
  
  // convert value to a char array
  // char ar [5] {};
  // ar [4] = value * 16; // multiply by 16 to obtain a left shift of 4 bits
  uint16_t shifted = value * 16;
  std::string str_shift = std::to_string (shifted);
  m_bitString = Create<BitString> (str_shift, 5, 4);
}

NrCellId::~NrCellId ()
{  
}

BIT_STRING_t
NrCellId::GetValue ()
{
  return m_bitString->GetValue ();
}

BIT_STRING_t*
NrCellId::GetPointer ()
{
  return m_bitString->GetPointer ();
}

Snssai::Snssai (std::string sst)
{
  m_sNssai = (SNSSAI_t *) calloc (1, sizeof (SNSSAI_t));
  m_sst = (OCTET_STRING_t *) calloc (1, sizeof (OCTET_STRING_t));
  m_sst->buf = (uint8_t *) calloc (1, sst.size ());
  m_sst->size = sst.size ();
  memcpy (m_sst->buf, sst.c_str (), sst.size ());
  m_sNssai->sST = *m_sst;
}
Snssai::Snssai (std::string sst, std::string sd) : Snssai (sst)
{
  m_sd = (OCTET_STRING_t *) calloc (1, sizeof (OCTET_STRING_t));
  m_sd->buf = (uint8_t *) calloc (1, sst.size ());
  m_sd->size = sd.size ();
  memcpy (m_sd->buf, sd.c_str (), sd.size ());
  m_sNssai->sD = m_sd;
}

Snssai::~Snssai ()
{
  if (m_sNssai != NULL)
    ASN_STRUCT_FREE (asn_DEF_SNSSAI, m_sNssai);

  // if (m_sst != NULL)
  //   ASN_STRUCT_FREE (asn_DEF_OCTET_STRING, m_sst);
  // if (m_sd != NULL)
  //   ASN_STRUCT_FREE (asn_DEF_OCTET_STRING, m_sd);
}

SNSSAI_t *
Snssai::GetPointer ()
{
  return m_sNssai;
}

SNSSAI_t
Snssai::GetValue ()
{
  return *m_sNssai;
}

void
MeasQuantityResultsWrap::AddRsrp (long rsrp)
{

  m_measQuantityResults->rsrp = (RSRP_Range_t *) calloc (1, sizeof (RSRP_Range_t));
  *m_measQuantityResults->rsrp = rsrp;
}

void
MeasQuantityResultsWrap::AddRsrq (long rsrq)
{
  m_measQuantityResults->rsrq = (RSRQ_Range_t *) calloc (1, sizeof (RSRQ_Range_t));
  *m_measQuantityResults->rsrq = rsrq;
}

void
MeasQuantityResultsWrap::AddSinr (long sinr)
{
  m_measQuantityResults->sinr = (SINR_Range_t *) calloc (1, sizeof (SINR_Range_t));
  *m_measQuantityResults->sinr = sinr;
}

MeasQuantityResultsWrap::MeasQuantityResultsWrap ()
{
  m_measQuantityResults = (MeasQuantityResults_t *) calloc (1, sizeof (MeasQuantityResults_t));
}

MeasQuantityResultsWrap::~MeasQuantityResultsWrap ()
{
  // if (m_measQuantityResults->sinr != NULL)
  //   ASN_STRUCT_FREE (asn_DEF_SINR_Range, m_measQuantityResults->sinr);

  // if (m_measQuantityResults->rsrp != NULL)
  //   ASN_STRUCT_FREE (asn_DEF_RSRP_Range, m_measQuantityResults->rsrp);

  // if (m_measQuantityResults->rsrq != NULL)
  //   ASN_STRUCT_FREE (asn_DEF_RSRQ_Range, m_measQuantityResults->rsrq);

  // if (m_measQuantityResults != NULL){
  //     free (m_measQuantityResults);
  //   }
}

MeasQuantityResults_t *
MeasQuantityResultsWrap::GetPointer ()
{
  return m_measQuantityResults;
}

MeasQuantityResults_t
MeasQuantityResultsWrap::GetValue ()
{
  return *m_measQuantityResults;
}

ResultsPerCsiRsIndex::ResultsPerCsiRsIndex (long csiRsIndex, MeasQuantityResults_t *csiRsResults)
    : ResultsPerCsiRsIndex (csiRsIndex)
{
  m_resultsPerCsiRsIndex->csi_RS_Results = csiRsResults;
}

ResultsPerCsiRsIndex::ResultsPerCsiRsIndex (long csiRsIndex)
{
  m_resultsPerCsiRsIndex =
      (ResultsPerCSI_RS_Index_t *) calloc (1, sizeof (ResultsPerCSI_RS_Index_t));
  m_resultsPerCsiRsIndex->csi_RS_Index = csiRsIndex;
}

ResultsPerCSI_RS_Index_t *
ResultsPerCsiRsIndex::GetPointer ()
{
  return m_resultsPerCsiRsIndex;
}

ResultsPerCSI_RS_Index_t
ResultsPerCsiRsIndex::GetValue ()
{
  return *m_resultsPerCsiRsIndex;
}

ResultsPerSSBIndex::ResultsPerSSBIndex (long ssbIndex, MeasQuantityResults_t *ssbResults)
    : ResultsPerSSBIndex (ssbIndex)
{
  m_resultsPerSSBIndex->ssb_Results = ssbResults;
}

ResultsPerSSBIndex::ResultsPerSSBIndex (long ssbIndex)
{
  m_resultsPerSSBIndex = (ResultsPerSSB_Index_t *) calloc (1, sizeof (ResultsPerSSB_Index_t));
  m_resultsPerSSBIndex->ssb_Index = ssbIndex;
}

ResultsPerSSB_Index_t *
ResultsPerSSBIndex::GetPointer ()
{
  return m_resultsPerSSBIndex;
}

ResultsPerSSB_Index_t
ResultsPerSSBIndex::GetValue ()
{
  return *m_resultsPerSSBIndex;
}

void
MeasResultNr::AddCellResults (MeasResultNr::ResultCell cell, MeasQuantityResults_t *results)
{
  switch (cell)
    {
    case MeasResultNr::ResultCell::SSB:

      m_measResultNr->measResult.cellResults.resultsSSB_Cell = results;
      break;

    case MeasResultNr::ResultCell::CSI_RS:

      m_measResultNr->measResult.cellResults.resultsCSI_RS_Cell = results;
      break;

    default:
      NS_LOG_ERROR ("Unrecognized cell identifier.");
      break;
    }
}

void
MeasResultNr::AddPerSsbIndexResults (ResultsPerSSB_Index_t *resultsSSB_Index)
{
  ASN_SEQUENCE_ADD (m_measResultNr->measResult.rsIndexResults->resultsSSB_Indexes,
                    resultsSSB_Index);
}

void
MeasResultNr::AddPerCsiRsIndexResults (ResultsPerCSI_RS_Index_t *resultsCSI_RS_Index)
{
  ASN_SEQUENCE_ADD (m_measResultNr->measResult.rsIndexResults->resultsCSI_RS_Indexes,
                    resultsCSI_RS_Index);
}

void MeasResultNr::AddPhyCellId (long physCellId)
{
  PhysCellId_t *s_physCellId = (PhysCellId_t *) calloc (1, sizeof (PhysCellId_t));
  *s_physCellId = physCellId;
  m_measResultNr->physCellId = s_physCellId;
}

MeasResultNr::MeasResultNr (long physCellId) : MeasResultNr ()
{
  AddPhyCellId (physCellId);
}

MeasResultNr::MeasResultNr ()
{
  m_measResultNr = (MeasResultNR_t *) calloc (1, sizeof (MeasResultNR_t));
  m_shouldFree = false;
}

MeasResultNr::~MeasResultNr ()
{
  if (m_shouldFree)
    {
      free (m_measResultNr);
    }
}

MeasResultNR_t *
MeasResultNr::GetPointer ()
{
  // Fallback procedure, this should not happen if correctly used;
  m_shouldFree = false;
  return m_measResultNr;
}

MeasResultNR_t
MeasResultNr::GetValue ()
{
  m_shouldFree = true;
  return *m_measResultNr;
}

MeasResultEutra::MeasResultEutra (long eutraPhysCellId, long rsrp, long rsrq, long sinr)
    : MeasResultEutra (eutraPhysCellId)
{
  AddRsrp (rsrp);
  AddRsrq (rsrq);
  AddSinr (sinr);
}

MeasResultEutra::MeasResultEutra (long eutraPhysCellId)
{
  m_measResultEutra = (MeasResultEUTRA_t *) calloc (1, sizeof (MeasResultEUTRA_t));
  m_measResultEutra->eutra_PhysCellId = eutraPhysCellId;
}

void
MeasResultEutra::AddRsrp (long rsrp)
{
  m_measResultEutra->measResult.rsrp =
      (RSRP_RangeEUTRA_t *) calloc (1, sizeof (RSRP_RangeEUTRA_t));
  *m_measResultEutra->measResult.rsrp = rsrp;
}
void
MeasResultEutra::AddRsrq (long rsrq)
{
  m_measResultEutra->measResult.rsrq =
      (RSRQ_RangeEUTRA_t *) calloc (1, sizeof (RSRQ_RangeEUTRA_t));
  *m_measResultEutra->measResult.rsrq = rsrq;
}
void
MeasResultEutra::AddSinr (long sinr)
{
  m_measResultEutra->measResult.sinr =
      (SINR_RangeEUTRA_t *) calloc (1, sizeof (SINR_RangeEUTRA_t));
  *m_measResultEutra->measResult.sinr = sinr;
}

MeasResultEUTRA_t *
MeasResultEutra::GetPointer ()
{
  return m_measResultEutra;
}

MeasResultEUTRA_t
MeasResultEutra::GetValue ()
{
  return *m_measResultEutra;
}

MeasResultPCellWrap::MeasResultPCellWrap (long eutraPhysCellId, long rsrpResult, long rsrqResult)
    : MeasResultPCellWrap (eutraPhysCellId)
{
  AddRsrpResult (rsrpResult);
  AddRsrqResult (rsrqResult);
}

MeasResultPCellWrap::MeasResultPCellWrap (long eutraPhysCellId)
{
  m_measResultPCell = (MeasResultPCell_t *) calloc (1, sizeof (MeasResultPCell_t));
  m_measResultPCell->eutra_PhysCellId = eutraPhysCellId;
}

void
MeasResultPCellWrap::AddRsrpResult (long rsrpResult)
{
  m_measResultPCell->rsrpResult = rsrpResult;
}

void
MeasResultPCellWrap::AddRsrqResult (long rsrqResult)
{
  m_measResultPCell->rsrqResult = rsrqResult;
}

MeasResultPCell_t *
MeasResultPCellWrap::GetPointer ()
{
  return m_measResultPCell;
}

MeasResultPCell_t
MeasResultPCellWrap::GetValue ()
{
  return *m_measResultPCell;
}

MeasResultServMo::MeasResultServMo (long servCellId, MeasResultNR_t measResultServingCell,
                                    MeasResultNR_t *measResultBestNeighCell)
    : MeasResultServMo (servCellId, measResultServingCell)
{
  m_measResultServMo->measResultBestNeighCell = measResultBestNeighCell;
}

MeasResultServMo::MeasResultServMo (long servCellId, MeasResultNR_t measResultServingCell)
{
  m_measResultServMo = (MeasResultServMO_t *) calloc (1, sizeof (MeasResultServMO_t));
  m_measResultServMo->servCellId = servCellId;
  m_measResultServMo->measResultServingCell = measResultServingCell;
}

MeasResultServMO_t *
MeasResultServMo::GetPointer ()
{
  return m_measResultServMo;
}

MeasResultServMO_t
MeasResultServMo::GetValue ()
{
  return *m_measResultServMo;
}

void
ServingCellMeasurementsWrap::AddMeasResultPCell (MeasResultPCell_t *measResultPCell)
{
  if (m_servingCellMeasurements->present != ServingCellMeasurements_PR_eutra_measResultPCell)
    {
      NS_LOG_ERROR ("Wrong measurement item for this present, it will not be added.");
    }
  m_servingCellMeasurements->choice.eutra_measResultPCell = measResultPCell;
}

void
ServingCellMeasurementsWrap::AddMeasResultServMo (MeasResultServMO_t *measResultServMO)
{
  if (m_servingCellMeasurements->present != ServingCellMeasurements_PR_nr_measResultServingMOList)
    {
      NS_LOG_ERROR ("Wrong measurement item for this present, it will not be added.");
    }

  ASN_SEQUENCE_ADD (&m_nr_measResultServingMOList->list, measResultServMO);
}

ServingCellMeasurementsWrap::ServingCellMeasurementsWrap (ServingCellMeasurements_PR present)
{
  m_servingCellMeasurements =
      (ServingCellMeasurements_t *) calloc (1, sizeof (ServingCellMeasurements_t));
  m_servingCellMeasurements->present = present;

  if (m_servingCellMeasurements->present == ServingCellMeasurements_PR_nr_measResultServingMOList)
    {
      m_nr_measResultServingMOList =
          (MeasResultServMOList_t *) calloc (1, sizeof (MeasResultServMOList_t));
      m_servingCellMeasurements->choice.nr_measResultServingMOList = m_nr_measResultServingMOList;
    }
}

ServingCellMeasurements_t *
ServingCellMeasurementsWrap::GetPointer ()
{
  return m_servingCellMeasurements;
}

ServingCellMeasurements_t
ServingCellMeasurementsWrap::GetValue ()
{
  return *m_servingCellMeasurements;
}

Ptr<L3RrcMeasurements>
L3RrcMeasurements::CreateL3RrcUeSpecificSinrServing (long servingCellId, long physCellId, long sinr)
{
  Ptr<L3RrcMeasurements> l3RrcMeasurement = Create<L3RrcMeasurements> (RRCEvent_b1);
  Ptr<ServingCellMeasurementsWrap> servingCellMeasurements =
      Create<ServingCellMeasurementsWrap> (ServingCellMeasurements_PR_nr_measResultServingMOList);

  Ptr<MeasResultNr> measResultNr = Create<MeasResultNr> (physCellId);
  Ptr<MeasQuantityResultsWrap> measQuantityResultWrap = Create<MeasQuantityResultsWrap> ();
  measQuantityResultWrap->AddSinr (sinr);
  measResultNr->AddCellResults (MeasResultNr::SSB, measQuantityResultWrap->GetPointer ());
  Ptr<MeasResultServMo> measResultServMo =
      Create<MeasResultServMo> (servingCellId, measResultNr->GetValue ());
  servingCellMeasurements->AddMeasResultServMo (measResultServMo->GetPointer ());
  l3RrcMeasurement->AddServingCellMeasurement (servingCellMeasurements->GetPointer ());
  return l3RrcMeasurement;
}

Ptr<L3RrcMeasurements>
L3RrcMeasurements::CreateL3RrcUeSpecificSinrNeigh ()
{
  return Create<L3RrcMeasurements> (RRCEvent_b1);
}

void
L3RrcMeasurements::AddNeighbourCellMeasurement (long neighCellId, long sinr)
{
  Ptr<MeasResultNr> measResultNr = Create<MeasResultNr> (neighCellId);
  Ptr<MeasQuantityResultsWrap> measQuantityResultWrap = Create<MeasQuantityResultsWrap> ();
  measQuantityResultWrap->AddSinr (sinr);
  measResultNr->AddCellResults (MeasResultNr::SSB, measQuantityResultWrap->GetPointer ());

  this->AddMeasResultNRNeighCells (measResultNr->GetPointer ()); // MAX 8 UE per message (standard)
}

void
L3RrcMeasurements::AddServingCellMeasurement (ServingCellMeasurements_t *servingCellMeasurements)
{
  m_l3RrcMeasurements->servingCellMeasurements = servingCellMeasurements;
}

void
L3RrcMeasurements::AddMeasResultEUTRANeighCells (MeasResultEUTRA_t *measResultItemEUTRA)
{
  if (m_measItemsCounter == L3RrcMeasurements::MAX_MEAS_RESULTS_ITEMS)
    {
      NS_LOG_ERROR ("Maximum number of items ("
                    << L3RrcMeasurements::MAX_MEAS_RESULTS_ITEMS
                    << ")for the standard reached. This item will not be "
                       "inserted in the list");
      return;
    }

  if (m_l3RrcMeasurements->measResultNeighCells == NULL)
    {
      addMeasResultNeighCells (MeasResultNeighCells_PR_measResultListEUTRA);
    }

  if (m_l3RrcMeasurements->measResultNeighCells->present !=
      MeasResultNeighCells_PR_measResultListEUTRA)
    {
      NS_LOG_ERROR ("Wrong measurement item for this list, it will not be added.");
      return;
    }

  m_measItemsCounter++;
  ASN_SEQUENCE_ADD (&m_measResultListEUTRA->list, measResultItemEUTRA);
}

void
L3RrcMeasurements::AddMeasResultNRNeighCells (MeasResultNR_t *measResultItemNR)
{
  if (m_measItemsCounter == L3RrcMeasurements::MAX_MEAS_RESULTS_ITEMS)
    {
      NS_LOG_ERROR ("Maximum number of items ("
                    << L3RrcMeasurements::MAX_MEAS_RESULTS_ITEMS
                    << ")for the standard reached. This item will not be "
                       "inserted in the list");
      return;
    }

  if (m_l3RrcMeasurements->measResultNeighCells == NULL)
    {
      addMeasResultNeighCells (MeasResultNeighCells_PR_measResultListNR);
    }

  if (m_l3RrcMeasurements->measResultNeighCells->present !=
      MeasResultNeighCells_PR_measResultListNR)
    {
      NS_LOG_ERROR ("Wrong measurement item for this list, it will not be added.");
      return;
    }

  m_measItemsCounter++;
  ASN_SEQUENCE_ADD (&m_measResultListNR->list, measResultItemNR);
}

void
L3RrcMeasurements::addMeasResultNeighCells (MeasResultNeighCells_PR present)
{
  m_l3RrcMeasurements->measResultNeighCells =
      (MeasResultNeighCells_t *) calloc (1, sizeof (MeasResultNeighCells_t));
  m_l3RrcMeasurements->measResultNeighCells->present = present;

  switch (present)
    {
      case MeasResultNeighCells_PR_measResultListEUTRA: {
        m_measResultListEUTRA =
            (MeasResultListEUTRA_t *) calloc (1, sizeof (MeasResultListEUTRA_t));
        m_l3RrcMeasurements->measResultNeighCells->choice.measResultListEUTRA =
            m_measResultListEUTRA;
        break;
      }

      case MeasResultNeighCells_PR_measResultListNR: {
        m_measResultListNR = (MeasResultListNR_t *) calloc (1, sizeof (MeasResultListNR_t));
        m_l3RrcMeasurements->measResultNeighCells->choice.measResultListNR = m_measResultListNR;
        break;
      }

      default: {
        NS_LOG_ERROR ("Unrecognized present for Measurment result.");
        break;
      }
    }
}

L3RrcMeasurements::L3RrcMeasurements (RRCEvent_t rrcEvent)
{
  m_l3RrcMeasurements = (L3_RRC_Measurements_t *) calloc (1, sizeof (L3_RRC_Measurements_t));
  m_l3RrcMeasurements->rrcEvent = rrcEvent;
  m_measItemsCounter = 0;
}

L3RrcMeasurements::L3RrcMeasurements (L3_RRC_Measurements_t *l3RrcMeasurements)
{
  m_l3RrcMeasurements = l3RrcMeasurements;
}

L3RrcMeasurements::~L3RrcMeasurements ()
{
  // Memory deallocation is handled by RIC Indication Message 
  // if (m_l3RrcMeasurements != NULL)
  //   {
  //     ASN_STRUCT_FREE (asn_DEF_L3_RRC_Measurements, m_l3RrcMeasurements);
  //   }
}

L3_RRC_Measurements *
L3RrcMeasurements::GetPointer ()
{
  return m_l3RrcMeasurements;
}

L3_RRC_Measurements
L3RrcMeasurements::GetValue ()
{
  return *m_l3RrcMeasurements;
}

// TODO change definition and return the values
// this function shall be finished for decoding
void
L3RrcMeasurements::ExtractMeasurementsFromL3RrcMeas (L3_RRC_Measurements_t *l3RrcMeasurements)
{
  RRCEvent_t rrcEvent = l3RrcMeasurements->rrcEvent; // Mandatory
  switch (rrcEvent)
    {
      case RRCEvent_b1: {
        NS_LOG_DEBUG ("RRCEvent_b1");
      }
      break;

      case RRCEvent_a3: {
        NS_LOG_DEBUG ("RRCEvent_a3");
      }
      break;
      case RRCEvent_a5: {
        NS_LOG_DEBUG ("RRCEvent_a5");
      }
      break;
      case RRCEvent_periodic: {
        NS_LOG_DEBUG ("RRCEvent_periodic");
      }
      break;

      default: {
        NS_LOG_ERROR ("Rrc event unrecognised");
      }
      break;
    }

  if (l3RrcMeasurements->measResultNeighCells)
    {
      MeasResultNeighCells_t *measResultNeighCells = l3RrcMeasurements->measResultNeighCells;
      switch (measResultNeighCells->present)
        {
          case MeasResultNeighCells_PR_NOTHING: { /* No components present */
            NS_LOG_DEBUG ("No components present");
          }
          break;
          case MeasResultNeighCells_PR_measResultListNR: {
            NS_LOG_DEBUG ("MeasResultNeighCells_PR_measResultListNR");
            //  measResultNeighCells->choice.measResultListNR
          }
          break;
          case MeasResultNeighCells_PR_measResultListEUTRA: {
            NS_LOG_DEBUG ("MeasResultNeighCells_PR_measResultListEUTRA");
          }
          break;
        default:
          NS_LOG_ERROR ("measResultNeighCells present unrecognised");
          break;
        }
    }
  if (l3RrcMeasurements->servingCellMeasurements)
    {
      ServingCellMeasurements_t *servingCellMeasurements =
          l3RrcMeasurements->servingCellMeasurements;
      switch (servingCellMeasurements->present)
        {
          case ServingCellMeasurements_PR_NOTHING: { /* No components present */
            NS_LOG_DEBUG ("No components present");
          }
          break;
          case ServingCellMeasurements_PR_nr_measResultServingMOList: {
            NS_LOG_DEBUG ("ServingCellMeasurements_PR_nr_measResultServingMOList");
          }
          break;
          case ServingCellMeasurements_PR_eutra_measResultPCell: {
            NS_LOG_DEBUG ("ServingCellMeasurements_PR_eutra_measResultPCell");
          }
          break;
        default:
          NS_LOG_ERROR ("servingCellMeasurements present unrecognised");
          break;
        }
    }
}

double 
L3RrcMeasurements::ThreeGppMapSinr (double sinr)
{
  double inputEnd = 40;
  double inputStart = -23;
  double outputStart = 0;
  double outputEnd = 127;
  double outputSinr;
  double slope = (outputEnd - outputStart) / (inputEnd - inputStart);

  if (sinr < inputStart)
    {
      outputSinr = outputStart;
    }
  else if (sinr > inputEnd)
    {
      outputSinr = outputEnd;
    }
  else
    {
      outputSinr = outputStart + std::round (slope * (sinr - inputStart));
    }

  NS_LOG_DEBUG ("input sinr" << sinr << " output sinr" << outputSinr);

  return outputSinr;
}

MeasurementItem::MeasurementItem (std::string name)
{

  m_measurementItem = (PM_Info_Item_t *) calloc (1, sizeof (PM_Info_Item_t));
  m_pmType = (MeasurementType_t *) calloc (1, sizeof (MeasurementType_t));
  m_measurementItem->pmType = *m_pmType;

  m_measName =
      (MeasurementTypeName_t *) calloc (1, sizeof (MeasurementTypeName_t));
  m_measName->buf = (uint8_t *) calloc (1, sizeof (OCTET_STRING));
  m_measName->size = name.length ();
  memcpy (m_measName->buf, name.c_str (), m_measName->size);

  m_measurementItem->pmType.choice.measName = *m_measName;
  m_measurementItem->pmType.present = MeasurementType_PR_measName;
}

MeasurementItem::MeasurementItem (std::string name, long value) : MeasurementItem (name)
{
  NS_LOG_FUNCTION (this << name << "long" << value);
  this->CreateMeasurementValue (MeasurementValue_PR_valueInt);
  m_measurementItem->pmVal.choice.valueInt = value;
}

MeasurementItem::MeasurementItem (std::string name, double value) : MeasurementItem (name)
{
  NS_LOG_FUNCTION (this << name << "double" << value);
  this->CreateMeasurementValue (MeasurementValue_PR_valueReal);
  m_measurementItem->pmVal.choice.valueReal = value;
}

MeasurementItem::MeasurementItem (std::string name, Ptr<L3RrcMeasurements>value)
    : MeasurementItem (name)
{
  NS_LOG_FUNCTION (this << name << "L3 RRC" << value);
  this->CreateMeasurementValue (MeasurementValue_PR_valueRRC);
  m_measurementItem->pmVal.choice.valueRRC = value->GetPointer ();
}

void
MeasurementItem::CreateMeasurementValue (MeasurementValue_PR measurementValue_PR)
{
  m_pmVal = ((MeasurementValue_t *) calloc (1, sizeof (MeasurementValue_t)));
  m_measurementItem->pmVal = *m_pmVal;
  m_measurementItem->pmVal.present = measurementValue_PR;
}

MeasurementItem::~MeasurementItem ()
{
  NS_LOG_FUNCTION (this);
  if (m_pmVal != NULL)
    ASN_STRUCT_FREE (asn_DEF_MeasurementValue, m_pmVal);

  if (m_measName != NULL)
    {
      free (m_measName);
    }

  if (m_pmType != NULL)
    ASN_STRUCT_FREE (asn_DEF_MeasurementType, m_pmType);
}

PM_Info_Item_t *
MeasurementItem::GetPointer ()
{
  return m_measurementItem;
}

PM_Info_Item_t
MeasurementItem::GetValue ()
{
  return *m_measurementItem;
}

RANParameterItem::RANParameterItem (RANParameter_STRUCTURE_Item_t *ranParameterItem)
{
  m_ranParameterItem = ranParameterItem;
}

RANParameterItem::~RANParameterItem ()
{
  if (m_ranParameterItem != NULL)
    ASN_STRUCT_FREE (asn_DEF_RANParameter_Item, m_ranParameterItem);
}

std::vector<RANParameterItem>
RANParameterItem::ExtractRANParametersFromRANParameter (RANParameter_STRUCTURE_Item_t *ranParameterItem)
{
  std::vector<RANParameterItem> ranParameterList;

  // NS_LOG_DEBUG ("RAN Parameter examined:");
  // xer_fprint (stderr, &asn_DEF_RANParameter_Item, ranParameterItem);
  // NS_LOG_DEBUG ("----");
  // NS_LOG_DEBUG (" ID " << ranParameterItem->ranParameterItem_ID);

  switch (ranParameterItem->ranParameter_valueType->present)
    {
      case RANParameter_ValueType_PR_NOTHING: {
        NS_LOG_DEBUG ("[E2SM] RANParameter_ValueType_PR_NOTHING");
        break;
      }
      case RANParameter_ValueType_PR_ranP_Choice_ElementTrue: {
        RANParameterItem newItem =
            RANParameterItem (ranParameterItem);
        NS_LOG_DEBUG ("[E2SM] RANParameter_ValueType_PR_ranParameter_Element");
        // RANParameter_ELEMENT_t *ranParameterElement =
        // RANParameter_ValueType_Choice_ElementTrue_t* 
        // RANParameter_Item_t
        // RANParameter_ValueType_t
        
        // /* ????? */  ranParameterElement=
        //     ranParameterItem->ranParameterItem_valueType->choice.ranP_Choice_ElementTrue;

        RANParameter_ValueType_t* ranPar_valueType = ranParameterItem->ranParameter_valueType;
        RANParameter_ValueType_Choice_ElementTrue_t* elemTrue = ranPar_valueType->choice.ranP_Choice_ElementTrue;
        // newItem.m_keyFlag = &ranParameterElement->keyFlag;
        switch (elemTrue->ranParameter_value.present)
          {
            case RANParameter_Value_PR_NOTHING: {
              NS_LOG_DEBUG ("[E2SM] RANParameter_Value_PR_NOTHING");
              newItem.m_valueType = ValueType::Nothing;
              break;
            }
            case RANParameter_Value_PR_valueInt: {
              NS_LOG_DEBUG ("[E2SM] RANParameter_Value_PR_valueInt");
              // RANParameter_Value_t* ran_value = ranPar_valueType->choice.ranP_Choice_ElementTrue.ranParameter_value.choice.valueInt;
              RANParameter_ValueType_Choice_ElementTrue_t* elemTrue = ranPar_valueType->choice.ranP_Choice_ElementTrue;
              newItem.m_valueInt = elemTrue->ranParameter_value.choice.valueInt;
              newItem.m_valueType = ValueType::Int;
              NS_LOG_DEBUG ("[E2SM] Value: " << newItem.m_valueInt);
              break;
            }
            case RANParameter_Value_PR_valueBoolean : {
              break;
            }
            case RANParameter_Value_PR_valueOctS: {
              NS_LOG_DEBUG ("[E2SM] RANParameter_Value_PR_valueOctS");
              // RANParameter_ValueType_Choice_ElementTrue_t* elemTrue = ranPar_valueType->choice.ranP_Choice_ElementTrue;

              newItem.m_valueStr = Create<OctetString> (
                  (void *) elemTrue->ranParameter_value.choice.valueOctS.buf,
                  elemTrue->ranParameter_value.choice.valueOctS.size);
              newItem.m_valueType = ValueType::OctectString;
              NS_LOG_DEBUG ("[E2SM] Value: OctectString");
              break;
            }
            case RANParameter_Value_PR_valueReal : 
            case RANParameter_Value_PR_valueBitS : 
            case RANParameter_Value_PR_valuePrintableString : {
              break;
            }
          }
        ranParameterList.push_back (newItem);
        break;
      }
     break;
      case RANParameter_ValueType_PR_ranP_Choice_ElementFalse : {
        break;
      }
      case RANParameter_ValueType_PR_ranP_Choice_Structure: {
        NS_LOG_DEBUG ("[E2SM] RANParameter_ValueType_PR_ranParameter_Structure");
        // RANParameter_STRUCTURE_t *ranParameterStructure =
        // RANParameter_ValueType_Choice_Structure_t* _ranParameterStructure = ranParameterItem->ranParameterItem_valueType->choice.ranP_Choice_Structure;
        
        RANParameter_ValueType_Choice_Structure_t* _ranParameterStructure = ranParameterItem->ranParameter_valueType->choice.ranP_Choice_Structure;
       
        
        //RANParameter_ValueType_Choice_Structure* ranP_Choice_Structure=
      
        int count = _ranParameterStructure->ranParameter_Structure->sequence_of_ranParameters->list.count;
        for (int i = 0; i < count; i++)
          {
            // RANParameter_Item_t *childRanItem =
            RANParameter_STRUCTURE_Item_t *childRanItem = 
                _ranParameterStructure->ranParameter_Structure->sequence_of_ranParameters->list.array[i];

            for (RANParameterItem extractedParameter : ExtractRANParametersFromRANParameter (childRanItem))
              {
                ranParameterList.push_back (extractedParameter);
              }
          }
        break;
      }
      case RANParameter_ValueType_PR_ranP_Choice_List: {
        NS_LOG_DEBUG ("[E2SM] RANParameter_ValueType_PR_ranParameter_List");
        // No list passed for the moment from RIC, thus no parsed as case
        // ranParameterItem->ranParameterItem_valueType->choice.ranParameter_List;
        break;
      }
    }

  return ranParameterList;
}


}; // namespace ns3
