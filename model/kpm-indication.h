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

#ifndef KPM_INDICATION_H
#define KPM_INDICATION_H

#include "ns3/object.h"
#include <set>

extern "C" {
  #include "E2SM-KPM-RANfunction-Description.h"
  #include "E2SM-KPM-IndicationHeader.h"
  #include "E2SM-KPM-IndicationMessage.h"
  #include "RAN-Container.h"
  #include "PF-Container.h"
  #include "OCUUP-PF-Container.h"
  #include "OCUCP-PF-Container.h"
  #include "ODU-PF-Container.h"
  #include "PF-ContainerListItem.h"
  #include "asn1c-types.h"
}

namespace ns3 {

  class KpmIndicationHeader : public SimpleRefCount<KpmIndicationHeader>
  {
  public:
    enum GlobalE2nodeType { gNB = 0, eNB = 1, ng_eNB = 2, en_gNB = 3 };

    int TIMESTAMP_LIMIT_SIZE = 8;
    /**
    * Holds the values to be used to fill the RIC Indication header 
    */
    struct KpmRicIndicationHeaderValues
    {
      // E2SM-KPM Indication Header Format 1
      // KPM Node ID IE
      std::string m_gnbId; //!< gNB ID bit string 
      // TODO not supported
      // uint64_t m_cuUpId; //!< gNB-CU-UP ID, integer [0, 2^36-1], optional
      
      // Cell Global ID (NR CGI) IE
      uint16_t m_nrCellId; //!< NR, bit string
      
      // PLMN ID IE
      std::string m_plmId; //!< PLMN identity, octet string, 3 bytes
      
      // Slice ID (S-NSSAI) IE // TODO not supported
      // std::string m_sst; //!< SNSSAI sST, 1 byte
      // std::string m_sd; //!< SNSSAI sD, 3 bytes, optional
      
      // FiveQI IE // TODO not supported
      // uint8_t m_fiveqi; //!< fiveQI, integer [0, 255], optional
      
      // QCI IE // TODO not supported
      // long m_qci; //!< QCI, integer [0, 255], optional
      
      // TODO this value is placed in a fiels which seems not to be defined 
      // in the specs. See line 301 in encode_kpm.cpp
      // the field is called gNB_DU_ID
      // it should be part of KPM Node ID IE
      // m_duId
    
      // TODO this value is placed in a fiels which seems not to be defined 
      // in the specs. See line 290 in encode_kpm.cpp, the field is called
      // gNB_Name
      // m_cuUpName
      
      // CollectionTimeStamp
      uint64_t m_timestamp;
    };
    
    KpmIndicationHeader (GlobalE2nodeType nodeType,KpmRicIndicationHeaderValues values);
    ~KpmIndicationHeader ();
    void* m_buffer;
    size_t m_size;
    
  private: 
    /**
    * Fills the KPM INDICATION Header descriptor
    * This function fills the RIC Indication Header with the provided 
    * values
    *
    * \param descriptor object representing the KPM INDICATION Header
    * \param values struct holding the values to be used to fill the header 
    */
    void FillAndEncodeKpmRicIndicationHeader (E2SM_KPM_IndicationHeader_t* descriptor, 
                                              KpmRicIndicationHeaderValues values);
    
    void Encode (E2SM_KPM_IndicationHeader_t* descriptor);

    GlobalE2nodeType m_nodeType;
    };

  class MeasurementItemList : public SimpleRefCount<MeasurementItemList>
  {
  private:
    Ptr<OctetString> m_id; // ID, contains the UE IMSI if used to carry UE-specific measurement items
    std::vector<Ptr<MeasurementItem>> m_items; //!< list of Measurement Information Items
  public:
    MeasurementItemList ();
    MeasurementItemList (std::string ueId);
     ~MeasurementItemList ();

    // NOTE defined here to avoid undefined references
    template<class T> 
    void AddItem (std::string name, T value)
    {
      Ptr<MeasurementItem> item = Create<MeasurementItem> (name, value);
      m_items.push_back (item);
    }
    
    std::vector<Ptr<MeasurementItem>> GetItems();
    OCTET_STRING_t GetId ();
  };

  /**
  * Base class to carry PM Container values  
  */    
  class PmContainerValues : public SimpleRefCount<PmContainerValues> 
  {
  public:
    virtual ~PmContainerValues () = default;
  };

  /**
  * Contains the values to be inserted in the O-CU-CP Measurement Container  
  */
  class OCuCpContainerValues : public PmContainerValues
  {
  public:
    uint16_t m_numActiveUes; //!< mean number of RRC connections
  };
  
  /**
  * Contains the values to be inserted in the O-CU-UP Measurement Container  
  */
  class OCuUpContainerValues : public PmContainerValues
  {
  public:
    std::string m_plmId; //!< PLMN identity, octet string, 3 bytes
    long m_pDCPBytesUL; //!< total PDCP bytes transmitted UL
    long m_pDCPBytesDL; //!< total PDCP bytes transmitted DL
  };

  /**
  * Contains the values to be inserted in the O-DU EPC Measurement Container  
  */
  class EpcDuPmContainer : public SimpleRefCount<EpcDuPmContainer>
  {
  public:
    long m_qci; //!< QCI value
    long m_dlPrbUsage; //!< Used number of PRBs in an average of DL for the monitored slice during E2 reporting period
    long m_ulPrbUsage; //!< Used number of PRBs in an average of UL for the monitored slice during E2 reporting period
    virtual ~EpcDuPmContainer () = default;
  };

  /**
  * Contains the values to be inserted in the O-DU 5GC Measurement Container  
  */
  class FiveGcDuPmContainer : public SimpleRefCount<FiveGcDuPmContainer>
  {
  public:
    // Snssai m_sliceId; //!< S-NSSAI
    long m_fiveQi; //!< 5QI value
    long m_dlPrbUsage; //!< Used number of PRBs in an average of DL for the monitored slice during E2 reporting period
    long m_ulPrbUsage; //!< Used number of PRBs in an average of UL for the monitored slice during E2 reporting period
    virtual ~FiveGcDuPmContainer () = default;
  };

  class ServedPlmnPerCell : public SimpleRefCount<ServedPlmnPerCell>
  {
  public:
    std::string m_plmId; //!< PLMN identity, octet string, 3 bytes
    uint16_t m_nrCellId;
    std::set<Ptr<EpcDuPmContainer>> m_perQciReportItems;
  };

  class CellResourceReport : public SimpleRefCount<CellResourceReport>
  {
  public:
    std::string m_plmId; //!< PLMN identity, octet string, 3 bytes
    uint16_t m_nrCellId;
    long dlAvailablePrbs;
    long ulAvailablePrbs;
    std::set<Ptr<ServedPlmnPerCell>> m_servedPlmnPerCellItems;
  };

  /**
  * Contains the values to be inserted in the O-DU Measurement Container  
  */
  class ODuContainerValues : public PmContainerValues
  {
  public:
    std::set<Ptr<CellResourceReport>> m_cellResourceReportItems;
  };

  class KpmIndicationMessage : public SimpleRefCount<KpmIndicationMessage>
  {
  public:
    
    /**
    * Holds the values to be used to fill the RIC Indication Message 
    */
    struct KpmIndicationMessageValues
    {
      std::string m_cellObjectId; //!< Cell Object ID
      Ptr<PmContainerValues> m_pmContainerValues; //!< struct containing values to be inserted in the PM Container
      Ptr<MeasurementItemList> m_cellMeasurementItems; //!< list of cell-specific Measurement Information Items
      std::set<Ptr<MeasurementItemList>> m_ueIndications; //!< list of Measurement Information Items
    };

    KpmIndicationMessage (KpmIndicationMessageValues values);
    ~KpmIndicationMessage ();
    
    void* m_buffer;
    size_t m_size;
    
  private:
    static void CheckConstraints (KpmIndicationMessageValues values);
    void FillPmContainer (PF_Container_t *ranContainer, 
                          Ptr<PmContainerValues> values);
    void FillOCuUpContainer (PF_Container_t *ranContainer, 
                            Ptr<OCuUpContainerValues> values);
    void FillOCuCpContainer (PF_Container_t *ranContainer, 
                             Ptr<OCuCpContainerValues> values);
    void FillODuContainer (PF_Container_t *ranContainer, 
                           Ptr<ODuContainerValues> values);
    void FillAndEncodeKpmIndicationMessage (E2SM_KPM_IndicationMessage_t *descriptor,
                                            KpmIndicationMessageValues values);
    void Encode (E2SM_KPM_IndicationMessage_t *descriptor);
  };
}

#endif /* KPM_INDICATION_H */
