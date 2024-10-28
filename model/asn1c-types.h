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

#ifndef ASN1C_TYPES_H
#define ASN1C_TYPES_H

#include "ns3/object.h"
#include <ns3/math.h>

extern "C" {
  #include "OCTET_STRING.h"
  #include "BIT_STRING.h"
  #include "PM-Info-Item.h"
  #include "SNSSAI.h"
  #include "RRCEvent.h"
  #include "L3-RRC-Measurements.h"
  #include "ServingCellMeasurements.h"
  #include "MeasResultNeighCells.h"
  #include "MeasResultNR.h"
  #include "MeasResultEUTRA.h"
  #include "MeasResultPCell.h"
  #include "MeasResultListEUTRA.h"
  #include "MeasResultListNR.h"
  #include "MeasResultServMO.h"
  #include "MeasResultServMOList.h"
  #include "MeasQuantityResults.h"
  #include "ResultsPerSSB-Index.h"
  #include "ResultsPerCSI-RS-Index.h"
  #include "E2SM-RC-ControlMessage-Format1.h"
  #include "E2SM-RC-ControlMessage-Format2.h"
  #include "RANParameter-Item.h"
  #include "RANParameter-ValueType.h"
  #include "RANParameter-ELEMENT.h"
  #include "RANParameter-STRUCTURE.h"
  #include "RANParameter-ValueType-Choice-ElementTrue.h"
  #include "RANParameter-ValueType-Choice-Structure.h"
  #include "RANParameter-STRUCTURE-Item.h"
}

namespace ns3 {

/**
* Wrapper for class for OCTET STRING  
*/
class OctetString : public SimpleRefCount<OctetString>
{
public:
  OctetString (std::string value, size_t size);
  OctetString (void *value, size_t size);
  ~OctetString ();
  OCTET_STRING_t *GetPointer ();
  OCTET_STRING_t GetValue ();
  std::string DecodeContent ();

private:
  void CreateBaseOctetString (size_t size);
  OCTET_STRING_t *m_octetString;
};

/**
* Wrapper for class for BIT STRING  
*/
class BitString : public SimpleRefCount<BitString>
{
public:
  BitString (std::string value, size_t size);
  BitString (std::string value, size_t size, size_t bits_unused);
  ~BitString ();
  BIT_STRING_t *GetPointer ();
  BIT_STRING_t GetValue ();
  // TODO maybe a to string or a decode method should be created

private:
  BIT_STRING_t *m_bitString;
};

class NrCellId : public SimpleRefCount<NrCellId>
{
public: 
  NrCellId (uint16_t value);
  virtual ~NrCellId ();
  BIT_STRING_t *GetPointer ();
  BIT_STRING_t GetValue ();
  
private: 
  Ptr<BitString> m_bitString;
};

/**
* Wrapper for class for S-NSSAI  
*/
class Snssai : public SimpleRefCount<Snssai>
{
public:
  Snssai (std::string sst);
  Snssai (std::string sst, std::string sd);
  ~Snssai ();
  SNSSAI_t *GetPointer ();
  SNSSAI_t GetValue ();

private:
  OCTET_STRING_t *m_sst;
  OCTET_STRING_t *m_sd;
  SNSSAI_t *m_sNssai;
};

/**
* Wrapper for class for MeasQuantityResults_t
*/
class MeasQuantityResultsWrap : public SimpleRefCount<MeasQuantityResultsWrap>
{
public:
  MeasQuantityResultsWrap ();
  ~MeasQuantityResultsWrap ();
  MeasQuantityResults_t *GetPointer ();
  MeasQuantityResults_t GetValue ();
  void AddRsrp (long rsrp);
  void AddRsrq (long rsrq);
  void AddSinr (long sinr);

private:
  MeasQuantityResults_t *m_measQuantityResults;
};

/**
* Wrapper for class for ResultsPerCSI_RS_Index_t
*/
class ResultsPerCsiRsIndex : public SimpleRefCount<ResultsPerCsiRsIndex>
{
public:
  ResultsPerCsiRsIndex (long csiRsIndex, MeasQuantityResults_t *csiRsResults);
  ResultsPerCsiRsIndex (long csiRsIndex);
  ResultsPerCSI_RS_Index_t *GetPointer ();
  ResultsPerCSI_RS_Index_t GetValue ();

private:
  ResultsPerCSI_RS_Index_t *m_resultsPerCsiRsIndex;
};

/**
* Wrapper for class for ResultsPerSSB_Index_t
*/
class ResultsPerSSBIndex : public SimpleRefCount<ResultsPerSSBIndex>
{
public:
  ResultsPerSSBIndex (long ssbIndex, MeasQuantityResults_t *ssbResults);
  ResultsPerSSBIndex (long ssbIndex);
  ResultsPerSSB_Index_t *GetPointer ();
  ResultsPerSSB_Index_t GetValue ();

private:
  ResultsPerSSB_Index_t *m_resultsPerSSBIndex;
};

/**
* Wrapper for class for MeasResultNR_t
*/
class MeasResultNr : public SimpleRefCount<MeasResultNr>
{
public:
  enum ResultCell { SSB = 0, CSI_RS = 1 };
  MeasResultNr (long physCellId);
  MeasResultNr ();
  ~MeasResultNr ();
  MeasResultNR_t *GetPointer ();
  MeasResultNR_t GetValue ();
  void AddCellResults (ResultCell cell, MeasQuantityResults_t *results);
  void AddPerSsbIndexResults (ResultsPerSSB_Index_t *resultsSsbIndex);
  void AddPerCsiRsIndexResults (ResultsPerCSI_RS_Index_t *resultsCsiRsIndex);
  void AddPhyCellId (long physCellId);

private:
  MeasResultNR_t *m_measResultNr;
  bool m_shouldFree;
};

/**
* Wrapper for class for MeasResultEUTRA_t
*/
class MeasResultEutra : public SimpleRefCount<MeasResultEutra>
{
public:
  MeasResultEutra (long eutraPhysCellId, long rsrp, long rsrq, long sinr);
  MeasResultEutra (long eutraPhysCellId);
  MeasResultEUTRA_t *GetPointer ();
  MeasResultEUTRA_t GetValue ();
  void AddRsrp (long rsrp);
  void AddRsrq (long rsrq);
  void AddSinr (long sinr);

private:
  MeasResultEUTRA_t *m_measResultEutra;
};

/**
* Wrapper for class for MeasResultPCell_t
*/
class MeasResultPCellWrap : public SimpleRefCount<MeasResultPCellWrap>
{
public:
  MeasResultPCellWrap (long eutraPhysCellId, long rsrpResult, long rsrqResult);
  MeasResultPCellWrap (long eutraPhysCellId);
  MeasResultPCell_t *GetPointer ();
  MeasResultPCell_t GetValue ();
  void AddRsrpResult (long rsrpResult);
  void AddRsrqResult (long rsrqResult);

private:
  MeasResultPCell_t *m_measResultPCell;
};

/**
* Wrapper for class for MeasResultServMO_t
*/
class MeasResultServMo : public SimpleRefCount<MeasResultServMo>
{
public:
  MeasResultServMo (long servCellId, MeasResultNR_t measResultServingCell,
                    MeasResultNR_t *measResultBestNeighCell);
  MeasResultServMo (long servCellId, MeasResultNR_t measResultServingCell);
  MeasResultServMO_t *GetPointer ();
  MeasResultServMO_t GetValue ();

private:
  MeasResultServMO_t *m_measResultServMo;
};

/**
* Wrapper for class for ServingCellMeasurements_t
*/
class ServingCellMeasurementsWrap : public SimpleRefCount<ServingCellMeasurementsWrap>
{
public:
  ServingCellMeasurementsWrap (ServingCellMeasurements_PR present);
  ServingCellMeasurements_t *GetPointer ();
  ServingCellMeasurements_t GetValue ();
  void AddMeasResultPCell (MeasResultPCell_t *measResultPCell);
  void AddMeasResultServMo (MeasResultServMO_t *measResultServMO);

private:
  ServingCellMeasurements_t *m_servingCellMeasurements;
  MeasResultServMOList_t *m_nr_measResultServingMOList;
};

/**
* Wrapper for class for L3 RRC Measurements
*/
class L3RrcMeasurements : public SimpleRefCount<L3RrcMeasurements>
{
public:
  int MAX_MEAS_RESULTS_ITEMS = 8; // Maximum 8 per UE (standard)
  L3RrcMeasurements (RRCEvent_t rrcEvent);
  L3RrcMeasurements (L3_RRC_Measurements_t *l3RrcMeasurements);
  ~L3RrcMeasurements ();
  L3_RRC_Measurements_t *GetPointer ();
  L3_RRC_Measurements_t GetValue ();

  void AddMeasResultEUTRANeighCells (MeasResultEUTRA_t *measResultItemEUTRA);
  void AddMeasResultNRNeighCells (MeasResultNR_t *measResultItemNR);
  void AddServingCellMeasurement (ServingCellMeasurements_t *servingCellMeasurements);
  void AddNeighbourCellMeasurement (long neighCellId, long sinr);

  static Ptr<L3RrcMeasurements> CreateL3RrcUeSpecificSinrServing (long servingCellId,
                                                                  long physCellId, long sinr);

  static Ptr<L3RrcMeasurements> CreateL3RrcUeSpecificSinrNeigh ();

  // TODO change definition and return the values (to be used for decoding)
  static void ExtractMeasurementsFromL3RrcMeas (L3_RRC_Measurements_t *l3RrcMeasurements);
  
  /**
   * Returns the input SINR on a 0-127 scale
   * 
   * Refer to 3GPP TS 38.133 V17.2.0(2021-06), Table 10.1.16.1-1: SS-SINR and CSI-SINR measurement report mapping
   * 
   * @param sinr 
   * @return double 
   */
  static double ThreeGppMapSinr (double sinr);

private:
  void addMeasResultNeighCells (MeasResultNeighCells_PR present);
  L3_RRC_Measurements_t *m_l3RrcMeasurements;
  MeasResultListEUTRA_t *m_measResultListEUTRA;
  MeasResultListNR_t *m_measResultListNR;
  int m_measItemsCounter;
};

/**
* Wrapper for class for PM_Info_Item_t
*/
class MeasurementItem : public SimpleRefCount<MeasurementItem>
{
public:
  MeasurementItem (std::string name, long value);
  MeasurementItem (std::string name, double value);
  MeasurementItem (std::string name, Ptr<L3RrcMeasurements> value);
  ~MeasurementItem ();
  PM_Info_Item_t *GetPointer ();
  PM_Info_Item_t GetValue ();

private:
  MeasurementItem (std::string name);
  void CreateMeasurementValue (MeasurementValue_PR measurementValue_PR);
  // Main struct to be compiled
  PM_Info_Item_t *m_measurementItem;

  // Accessory structs that we must track to release memory after use
  MeasurementTypeName_t *m_measName;
  MeasurementValue_t *m_pmVal;
  MeasurementType_t *m_pmType;
};

/**
* Wrapper for class for RANParameter_STRUCTURE_Item_t 
*/
class RANParameterItem : public SimpleRefCount<RANParameterItem>
{
public:
  enum ValueType{ Nothing = 0, Int = 1, OctectString = 2 };
  RANParameterItem (RANParameter_STRUCTURE_Item_t *ranParameterItem);
  ~RANParameterItem ();
  RANParameter_STRUCTURE_Item_t *GetPointer ();
  RANParameter_STRUCTURE_Item_t GetValue ();

  ValueType m_valueType;
  long m_valueInt;
  Ptr<OctetString> m_valueStr;

  static std::vector<RANParameterItem>
  ExtractRANParametersFromRANParameter (RANParameter_STRUCTURE_Item_t *ranParameterItem);

private:
  // Main struct
  RANParameter_STRUCTURE_Item_t *m_ranParameterItem;
  BOOLEAN_t *m_keyFlag;
};

} // namespace ns3
#endif /* ASN1C_TYPES_H */
