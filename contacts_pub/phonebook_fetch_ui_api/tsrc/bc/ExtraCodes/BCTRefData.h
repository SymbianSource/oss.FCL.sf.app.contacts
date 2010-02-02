/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:       
*       Provides methods for the test dll.
*
*/








#ifndef __BCTREFDATA_H__
#define __BCTREFDATA_H__

//  INCLUDES
#include <E32BASE.H>

// CLASS DECLARATION
class CCnvCharacterSetConverter;
class CBCTBCardData;

class CBCTRefData :public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CBCTRefData(const TFileName& aFileName);

        /**
        * Two-phased constructor.
        */
        static CBCTRefData* NewL(const TFileName& aFileName);
        
        /**
        * Destructor.
        */
        virtual ~CBCTRefData();

    public: // New functions
        void ReadFileL(CBCTBCardData& aData);

    private:    // Types
        enum TLineMappingIndexes
	        {
            EIndexLastName = 0,
            EIndexFirstName,
            EIndexPhoneNumberStandard,
            EIndexPhoneNumberHome,
            EIndexPhoneNumberWork,
            EIndexPhoneNumberMobile,
            EIndexPhoneNumberMobileWork,
            EIndexPhoneNumberMobileHome,
            EIndexFaxNumber,
            EIndexFaxNumberHome,
            EIndexFaxNumberWork,
            EIndexPagerNumber,
            EIndexEmailAddress,
            EIndexEmailAddressWork,
            EIndexEmailAddressHome,
            EIndexPostalAddress,
            EIndexPostalAddressHome,            
            EIndexURL,
            EIndexURLWork,
            EIndexURLHome,
            EIndexJobTitle,
            EIndexCompanyName,
            EIndexCompanyAddress,
            EIndexDate,
            EIndexNote,
            EIndexPicture,
            EIndexThumbnailImage,
            // ADR general fields:
            EIndexPOBox,
            EIndexExtensionAddress,
            EIndexStreet,
            EIndexPostalCode, 
            EIndexCity,
            EIndexState,
            EIndexCountry,
            // ADR home fields:
            EIndexPOBoxHome,
            EIndexExtensionAddressHome,
            EIndexStreetHome,
            EIndexPostalCodeHome, 
            EIndexCityHome, 
            EIndexStateHome,
            EIndexCountryHome,
            // ADR work fields:
            EIndexPOBoxWork,
            EIndexExtensionAddressWork,
            EIndexStreetWork,
            EIndexPostalCodeWork,
            EIndexCityWork,
            EIndexStateWork,
            EIndexCountryWork
	        };

        struct TLineMapping
            {
            TPtrC iTag;
            TLineMappingIndexes iIndex;
            // default constructor
            TLineMapping();
            // constructor
            TLineMapping(const TDesC& aTag, TLineMappingIndexes aIndex);
            };

    private:    // Implemetation
        void ConstructL();
        TBool GetDataFromLineL(const TDes& aLine, TDes& aData,TLineMappingIndexes& aIndex);
        void StoreLineData(CBCTBCardData& aData, TLineMappingIndexes aIndex, HBufC* aLineData);
        void ConvertLineFeedsL(TDes& aData);
        TLineMappingIndexes FindIndex(TDesC& aTag);

    private:    // Data
        const TFileName& iFileName;
        RArray<TLineMapping> iLineMappings;
    };

inline CBCTRefData::TLineMapping::TLineMapping()
    {
    }

inline CBCTRefData::TLineMapping::TLineMapping
        (const TDesC& aTag, TLineMappingIndexes aIndex) :
    iTag(aTag), iIndex(aIndex)
    {
    }

#endif // __BCTREFDATA_H__
            
// End of File
