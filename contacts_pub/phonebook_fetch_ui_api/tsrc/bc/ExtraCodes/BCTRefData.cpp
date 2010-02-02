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








// INCLUDE FILES
#include "BCTRefData.h"
#include "BCTFileText.h"
#include "BCTBCardData.h"

// LOCAL CONSTANTS AND MACROS
// These are waiting for an suitable time:
_LIT(KTagLastName, "LASTNAME");
_LIT(KTagFirstName, "FIRSTNAME");
_LIT(KTagPhoneNumberStandard, "PHONE NUMBER STANDARD");
_LIT(KTagPhoneNumberHome, "PHONE NUMBER HOME");
_LIT(KTagPhoneNumberWork, "PHONE NUMBER WORK");

_LIT(KTagPhoneNumberMobile, "PHONE NUMBER MOBILE");
_LIT(KTagPhoneNumberMobileWork, "PHONE NUMBER MOBILE WORK");
_LIT(KTagPhoneNumberMobileHome, "PHONE NUMBER MOBILE HOME");

_LIT(KTagFaxNumber, "FAX NUMBER");
_LIT(KTagFaxNumberWork, "FAX NUMBER WORK");
_LIT(KTagFaxNumberHome, "FAX NUMBER HOME");

_LIT(KTagPagerNumber, "PAGER NUMBER");

_LIT(KTagEmailAddress, "EMAIL ADDRESS");
_LIT(KTagEmailAddressWork, "EMAIL ADDRESS WORK");
_LIT(KTagEmailAddressHome, "EMAIL ADDRESS HOME");

_LIT(KTagPostalAddress, "POSTAL ADDRESS");
_LIT(KTagPostalAddressHome, "POSTAL ADDRESS HOME");

_LIT(KTagURL, "URL");
_LIT(KTagURLHome, "URL HOME");
_LIT(KTagURLWork, "URL WORK");

_LIT(KTagJobTitle, "JOB TITLE");
_LIT(KTagCompanyName, "COMPANY NAME");
_LIT(KTagCompanyAddress, "COMPANY ADDRESS");
_LIT(KTagDate, "DATE");
_LIT(KTagNote, "NOTE");
_LIT(KTagPicture, "PICTURE");
_LIT(KTagThumbnailImage, "THUMBNAIL");

// ADR general fields:
_LIT(KTagPOBox, "POBOX");
_LIT(KTagExtensionAddress, "EXTADR");
_LIT(KTagStreet, "STREET");
_LIT(KTagPostalCode, "POSTALCODE");
_LIT(KTagCity, "CITY");
_LIT(KTagState, "STATE");
_LIT(KTagCountry, "COUNTRY");

// ADR home fields:
_LIT(KTagPOBoxHome, "POBOXHOME");
_LIT(KTagExtensionAddressHome, "EXTADRHOME");
_LIT(KTagStreetHome, "STREETHOME");
_LIT(KTagPostalCodeHome, "POSTALCODEHOME");
_LIT(KTagCityHome, "CITYHOME");
_LIT(KTagStateHome, "STATEHOME");
_LIT(KTagCountryHome, "COUNTRYHOME");

// ADR work fields:
_LIT(KTagPOBoxWork, "POBOXWORK");
_LIT(KTagExtensionAddressWork, "EXTADRWORK");
_LIT(KTagStreetWork, "STREETWORK");
_LIT(KTagPostalCodeWork, "POSTALCODEWORK");
_LIT(KTagCityWork, "CITYWORK");
_LIT(KTagStateWork, "STATEWORK");
_LIT(KTagCountryWork, "COUNTRYWORK");

_LIT(KDataDelimiter, "=");
_LIT(KDataNull, "<NULL>");
_LIT(KLineFeedTag, "<LINEFEED>");
_LIT(KLineFeedValue, "\r\n");


// MODULE DATA STRUCTURES

// C++ default constructor can NOT contain any code, that
// might leave.
//
CBCTRefData::CBCTRefData(const TFileName& aFileName)
        :iFileName(aFileName) 
    {
    }

// Symbian OS default constructor can leave.
void CBCTRefData::ConstructL()
    {    
    // Create linemapping array    
    iLineMappings.Append(TLineMapping(KTagLastName, EIndexLastName));
    iLineMappings.Append(TLineMapping(KTagFirstName, EIndexFirstName));
    iLineMappings.Append(TLineMapping(KTagPhoneNumberStandard, EIndexPhoneNumberStandard));
    iLineMappings.Append(TLineMapping(KTagPhoneNumberHome, EIndexPhoneNumberHome));
    iLineMappings.Append(TLineMapping(KTagPhoneNumberWork, EIndexPhoneNumberWork));
    iLineMappings.Append(TLineMapping(KTagPagerNumber, EIndexPagerNumber));

    iLineMappings.Append(TLineMapping(KTagPhoneNumberMobile, EIndexPhoneNumberMobile));
    iLineMappings.Append(TLineMapping(KTagPhoneNumberMobileHome, EIndexPhoneNumberMobileHome));
    iLineMappings.Append(TLineMapping(KTagPhoneNumberMobileWork, EIndexPhoneNumberMobileWork));
        
    iLineMappings.Append(TLineMapping(KTagFaxNumber, EIndexFaxNumber));
    iLineMappings.Append(TLineMapping(KTagFaxNumberHome, EIndexFaxNumberHome));
    iLineMappings.Append(TLineMapping(KTagFaxNumberWork, EIndexFaxNumberWork));
    
    iLineMappings.Append(TLineMapping(KTagEmailAddress, EIndexEmailAddress));
    iLineMappings.Append(TLineMapping(KTagEmailAddressHome, EIndexEmailAddressHome));
    iLineMappings.Append(TLineMapping(KTagEmailAddressWork, EIndexEmailAddressWork));
       
    iLineMappings.Append(TLineMapping(KTagPostalAddress, EIndexPostalAddress));
    iLineMappings.Append(TLineMapping(KTagPostalAddressHome, EIndexPostalAddressHome));
    iLineMappings.Append(TLineMapping(KTagCompanyAddress, EIndexCompanyAddress));

    iLineMappings.Append(TLineMapping(KTagURL, EIndexURL));
    iLineMappings.Append(TLineMapping(KTagURLWork, EIndexURLWork));
    iLineMappings.Append(TLineMapping(KTagURLHome, EIndexURLHome));

    iLineMappings.Append(TLineMapping(KTagJobTitle, EIndexJobTitle));
    iLineMappings.Append(TLineMapping(KTagCompanyName, EIndexCompanyName));
    iLineMappings.Append(TLineMapping(KTagDate, EIndexDate));
    iLineMappings.Append(TLineMapping(KTagNote, EIndexNote));
    iLineMappings.Append(TLineMapping(KTagPicture, EIndexPicture));
    iLineMappings.Append(TLineMapping(KTagThumbnailImage, EIndexThumbnailImage));    
    // ADR general fields
    iLineMappings.Append(TLineMapping(KTagPOBox, EIndexPOBox));
    iLineMappings.Append(TLineMapping(KTagExtensionAddress, EIndexExtensionAddress));
    iLineMappings.Append(TLineMapping(KTagStreet, EIndexStreet));
    iLineMappings.Append(TLineMapping(KTagPostalCode, EIndexPostalCode));
    iLineMappings.Append(TLineMapping(KTagCity, EIndexCity));
    iLineMappings.Append(TLineMapping(KTagState, EIndexState));
    iLineMappings.Append(TLineMapping(KTagCountry, EIndexCountry));
    // ADR home fields
    iLineMappings.Append(TLineMapping(KTagPOBoxHome, EIndexPOBoxHome));    
    iLineMappings.Append(TLineMapping(KTagExtensionAddressHome, EIndexExtensionAddressHome));    
    iLineMappings.Append(TLineMapping(KTagStreetHome, EIndexStreetHome));    
    iLineMappings.Append(TLineMapping(KTagPostalCodeHome, EIndexPostalCodeHome));    
    iLineMappings.Append(TLineMapping(KTagCityHome, EIndexCityHome));    
    iLineMappings.Append(TLineMapping(KTagStateHome, EIndexStateHome));    
    iLineMappings.Append(TLineMapping(KTagCountryHome, EIndexCountryHome));    

    iLineMappings.Append(TLineMapping(KTagPOBoxWork, EIndexPOBoxWork));    
    iLineMappings.Append(TLineMapping(KTagExtensionAddressWork, EIndexExtensionAddressWork));    
    iLineMappings.Append(TLineMapping(KTagStreetWork, EIndexStreetWork));    
    iLineMappings.Append(TLineMapping(KTagPostalCodeWork, EIndexPostalCodeWork));    
    iLineMappings.Append(TLineMapping(KTagCityWork, EIndexCityWork));    
    iLineMappings.Append(TLineMapping(KTagStateWork, EIndexStateWork));    
    iLineMappings.Append(TLineMapping(KTagCountryWork, EIndexCountryWork));    


    }

// Two-phased constructor.
CBCTRefData* CBCTRefData::NewL(const TFileName& aFileName)
    {
    CBCTRefData* self = new (ELeave) CBCTRefData (aFileName);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    
// Destructor
CBCTRefData::~CBCTRefData()
    {
    iLineMappings.Close();
    }

/**
* Read datafile to structure.
*/ 
void CBCTRefData::ReadFileL(CBCTBCardData& aData)
    { 
    const TInt KMaxLine = 500;
    
    aData.ResetData();

    CBCTFileText* file = CBCTFileText::NewL(iFileName);
    CleanupStack::PushL(file); 

    HBufC16* lineBufHeap = HBufC16::NewL(KMaxLine); 
    CleanupStack::PushL(lineBufHeap); 
    TPtr16 lineBuf = lineBufHeap->Des();

    TInt ret;
    TInt count = 0;
    TLineMappingIndexes index;
    do    
        {
        ret =  file->ReadLineL(lineBuf);
        if (ret == KErrNone)
            {
            HBufC* lineDataHeap = HBufC::NewLC(lineBuf.Length());
            TPtr lineData = lineDataHeap->Des();

            if (GetDataFromLineL(lineBuf,lineData,index))
                {
                if (lineData == KDataNull)
                    {
                    CleanupStack::PopAndDestroy(lineDataHeap); // lineDataHeap 
                    StoreLineData(aData,index,NULL);
                    }
                else
                    {
                    StoreLineData(aData,index,lineDataHeap);
                    CleanupStack::Pop(lineDataHeap); // lineDataHeap 
                    }
                count++;
                }
            else
                {
                CleanupStack::PopAndDestroy(lineDataHeap); // lineDataHeap 
                }
            }
        }
    while (ret == KErrNone);

    CleanupStack::PopAndDestroy(2); // file, lineBufHeap 
    }

/**
* Get datavalue from line.
*/ 
TBool CBCTRefData::GetDataFromLineL(
        const TDes& aLine, TDes& aData, TLineMappingIndexes& aIndex)
    {
    aData.Zero();
    TInt pos = aLine.Find(KDataDelimiter);
    if (pos > 0)
        {
        TPtrC ptr = aLine.Left(pos);
        aIndex = FindIndex(ptr);
        aData = aLine.Mid(pos + 1);
        // after this convert possible linefeed
        ConvertLineFeedsL(aData);
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

/**
* Convert linefeed tag to chars.
*/ 
void CBCTRefData::ConvertLineFeedsL(TDes& aData)
    {
    HBufC* tmpHeap = HBufC::NewLC(aData.Length());
    TPtr tmp = tmpHeap->Des();
    tmp = aData;
        
    aData.Zero();
    TInt pos;
    TBool process = EFalse;
    do
        {
        pos = tmp.Find(KLineFeedTag);
        
        if (pos > 0)
            {
            process = ETrue;
            }
        else if (tmp.Length() >= KLineFeedTag().Length())
            {
            if (tmp.Left(KLineFeedTag().Length()) == KLineFeedTag)
                {
                process = ETrue;
                }
            else
                {
                process = EFalse;
                }
            }
        else
            {
            process = EFalse;
            }

        if (process)
            {
            aData.Append(tmp.Left(pos));
            aData.Append(KLineFeedValue);
            tmp = tmp.Mid(pos + KLineFeedTag().Length());
            }
        else
            {
            aData.Append(tmp);
            }
        }
    while (process);
    CleanupStack::PopAndDestroy(tmpHeap);
    }

CBCTRefData::TLineMappingIndexes CBCTRefData::FindIndex(TDesC& aTag)
    {
    for (TInt i = 0; i < iLineMappings.Count(); i++)
        {
        if (iLineMappings[i].iTag == aTag)
            {
            return iLineMappings[i].iIndex;
            }

        }
    return STATIC_CAST(CBCTRefData::TLineMappingIndexes, KErrNotFound);
    }

/**
* Store the dataitem.
*/ 
void CBCTRefData::StoreLineData(CBCTBCardData& aData, TLineMappingIndexes aIndex, HBufC* aLineData)
    {
    switch (aIndex)
        {        
        case EIndexLastName:
            {
            aData.iLastName = aLineData;
            break;
            }
        case EIndexFirstName:
            {
            aData.iFirstName = aLineData;
            break;
            }
        case EIndexPhoneNumberStandard:
            {
            aData.iPhoneNumberStandard = aLineData;
            break;
            }
        case EIndexPhoneNumberHome:
            {
            aData.iPhoneNumberHome = aLineData;
            break;
            }
        case EIndexPhoneNumberWork:
            {
            aData.iPhoneNumberWork = aLineData;
            break;
            }
        case EIndexPhoneNumberMobile:
            {
            aData.iPhoneNumberMobile = aLineData;
            break;
            }
        case EIndexPhoneNumberMobileWork:
            {
            aData.iPhoneNumberMobileWork = aLineData;
            break;
            }
        case EIndexPhoneNumberMobileHome:
            {
            aData.iPhoneNumberMobileHome = aLineData;
            break;
            }
        case EIndexFaxNumber:
            {
            aData.iFaxNumber = aLineData;
            break;
            }
        case EIndexFaxNumberWork:
            {
            aData.iFaxNumberWork = aLineData;
            break;
            }
        case EIndexFaxNumberHome:
            {
            aData.iFaxNumberHome = aLineData;
            break;
            }
        case EIndexPagerNumber:
            {
            aData.iPagerNumber = aLineData;
            break;
            }

        case EIndexEmailAddress:
            {
            aData.iEmailAddress = aLineData;
            break;
            }
        case EIndexEmailAddressWork:
            {
            aData.iEmailAddressWork = aLineData;
            break;
            }
        case EIndexEmailAddressHome:
            {
            aData.iEmailAddressHome = aLineData;
            break;
            }
        case EIndexPostalAddress:
            {
            aData.iPostalAddress = aLineData;
            break;
            }
        case EIndexPostalAddressHome:
            {
            aData.iPostalAddressHome = aLineData;
            break;
            }
        case EIndexURL:
            {
            aData.iURL = aLineData;
            break;
            }
        case EIndexURLWork:
            {
            aData.iURLWork = aLineData;
            break;
            }
        case EIndexURLHome:
            {
            aData.iURLHome = aLineData;
            break;
            }
        case EIndexJobTitle:
            {
            aData.iJobTitle = aLineData;
            break;
            }
        case EIndexCompanyName:
            {
            aData.iCompanyName = aLineData;
            break;
            }
        case EIndexCompanyAddress:
            {
            aData.iCompanyAddress = aLineData;
            break;
            }
        case EIndexDate:
            {
            aData.iDate = aLineData;
            break;
            }
        case EIndexNote:
            {
            aData.iNote = aLineData;
            break;
            }
        case EIndexPicture:
            {
            aData.iPicture = aLineData;
            break;
            }
        case EIndexThumbnailImage:
            {
            aData.iThumbnailImage = aLineData;
            break;
            }
        // ADR general fields
        case EIndexPOBox:
            {
            aData.iPOBox = aLineData;
            break;
            }
        case EIndexExtensionAddress:
            {
            aData.iExtensonAdr = aLineData;
            break;
            }
        case EIndexStreet:
            {
            aData.iStreet = aLineData;
            break;
            }
        case EIndexPostalCode:
            {
            aData.iPostalCode = aLineData;
            break;
            }
        case EIndexCity:
            {
            aData.iCity= aLineData;
            break;
            }
        case EIndexState:
            {
            aData.iState = aLineData;
            break;
            }
        case EIndexCountry:
            {
            aData.iCountry = aLineData;
            break;
            }
        // ADR home fields
        case EIndexPOBoxHome:
            {
            aData.iPOBoxHome = aLineData;
            break;
            }
        case EIndexExtensionAddressHome:
            {
            aData.iExtensonAdrHome = aLineData;
            break;
            }
        case EIndexStreetHome:
            {
            aData.iStreetHome = aLineData;
            break;
            }
        case EIndexPostalCodeHome:
            {
            aData.iPostalCodeHome = aLineData;
            break;
            }
        case EIndexCityHome:
            {
            aData.iCityHome = aLineData;
            break;
            }
        case EIndexStateHome:
            {
            aData.iStateHome = aLineData;
            break;
            }
        case EIndexCountryHome:
            {
            aData.iCountryHome = aLineData;
            break;
            }
        // ADR work fields
        case EIndexPOBoxWork:
            {
            aData.iPOBoxWork = aLineData;
            break;
            }
        case EIndexExtensionAddressWork:
            {
            aData.iExtensonAdrWork = aLineData;
            break;
            }
        case EIndexStreetWork:
            {
            aData.iStreetWork = aLineData;
            break;
            }
        case EIndexPostalCodeWork:
            {
            aData.iPostalCodeWork = aLineData;
            break;
            }
        case EIndexCityWork:
            {
            aData.iCityWork = aLineData;
            break;
            }
        case EIndexStateWork:
            {
            aData.iStateWork = aLineData;
            break;
            }
        case EIndexCountryWork:
            {
            aData.iCountryWork = aLineData;
            break;
            }
        default:
            {
                //ignore
            }
        } 
    }

//  End of File  
