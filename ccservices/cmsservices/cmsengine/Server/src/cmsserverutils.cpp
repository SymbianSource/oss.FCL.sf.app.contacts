/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       
*
*/


// INCLUDE FILES
#include <vpbkeng.rsg>
#include <spproperty.h>
#include <spsettings.h>
#include <spentry.h>
#include "cmsserver.h"
#include "cmsserverutils.h"
#include "cmsdebug.h"

// ----------------------------------------------------------
// CCmsSession::ReadMsgParamLC
// 
// ----------------------------------------------------------
//
HBufC* CmsServerUtils::ReadMsgParam16LC( TInt aMsgIndex, const RMessage2& aMessage )
    {
    
    PRINT( _L( "Start CmsServerUtils::ReadMsgParam16LC()" ) );
    
    TInt length = aMessage.GetDesLength( aMsgIndex );
    HBufC* buffer = HBufC::NewLC( length );
    TPtr descriptor( buffer->Des() );
    aMessage.ReadL( aMsgIndex, descriptor );
    PRINT( _L( "End CmsServerUtils::ReadMsgParam16LC()" ) );
    return buffer;
    }

// ----------------------------------------------------------
// CmsServerUtils::ReadMsgParam8LC
// 
// ----------------------------------------------------------
//
HBufC8* CmsServerUtils::ReadMsgParam8LC( TInt aMsgIndex, const RMessage2& aMessage )
    {
    PRINT( _L( "'Start CmsServerUtils::ReadMsgParam8LC()" ) );
    TInt length = aMessage.GetDesLength( aMsgIndex );
    HBufC8* buffer = HBufC8::NewLC( length );
    TPtr8 descriptor( buffer->Des() );
    aMessage.ReadL( aMsgIndex, descriptor );
    return buffer;
    }

// ----------------------------------------------------------
// CmsServerUtils::ConvertDesc
// 
// ----------------------------------------------------------
//
TInt CmsServerUtils::ConvertDesc( const TDesC8& aNumberDesc )
    {
    TInt ret = 0;
    TLex8 lex( aNumberDesc );
    TInt error = lex.Val( ret );
    return error == KErrNone ? ret : error;
    }

// ---------------------------------------------------------
// CmsServerUtils::DescriptorCast
//
// ---------------------------------------------------------
//
TPtr8 CmsServerUtils::DescriptorCast( const TDesC8& aConstData )
	{
    TInt length = aConstData.Length();
    TPtr8 constCast( CONST_CAST( TUint8*, aConstData.Ptr() ), length, length );
    constCast.TrimAll();
    return constCast;
	}

// ---------------------------------------------------------
// CmsServerUtils::ConvertDescL
//
// ---------------------------------------------------------
//
HBufC8* CmsServerUtils::ConvertDescLC( const TDesC& a16bitDescriptor )
    {
    HBufC8* eightBit = HBufC8::NewLC( a16bitDescriptor.Length() );
    eightBit->Des().Copy( a16bitDescriptor );
    return eightBit;
    }

// ---------------------------------------------------------
// CmsServerUtils::ConvertDescL
//
// ---------------------------------------------------------
//
HBufC* CmsServerUtils::ConvertDescLC( const TDesC8& a8bitDescriptor )
    {
    HBufC* sixteen = HBufC::NewLC( a8bitDescriptor.Length() );
    sixteen->Des().Copy( a8bitDescriptor );
    return sixteen;
    }

// ----------------------------------------------------
// CmsServerUtils::IsVoIPSupported
// 
// ----------------------------------------------------
// 
TBool CmsServerUtils::IsVoIPSupportedL()
    {
    TBool supported( EFalse );
    RIdArray idArray;
    CleanupClosePushL(idArray);
    CSPSettings* settings = CSPSettings::NewLC();
    
    User::LeaveIfError( settings->FindServiceIdsL(idArray) );
    for (TInt i = 0; !supported && i < idArray.Count(); ++i)
        {
        CSPEntry* entry = CSPEntry::NewLC();
        TServiceId id = idArray[i];
        User::LeaveIfError( settings->FindEntryL(id, *entry) );
        const CSPProperty* property = NULL;
        if (entry->GetProperty(property, ESubPropertyVoIPSettingsId) == KErrNone)
            {
            supported = ETrue;
            }

        if (entry->GetProperty(property, EPropertyServiceAttributeMask) == KErrNone)
            {
            TInt value = 0;
            property->GetValue(value);
            supported = value & ESupportsInternetCall; 
            }

        CleanupStack::PopAndDestroy(); // entry
        }
    CleanupStack::PopAndDestroy(2); // settings, idArray
    return supported;
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::IsImCapableL
// 
// ----------------------------------------------------
//
TBool CmsServerUtils::IsFeatureSupportedL( TInt aFeature, const TDesC& aScheme )
    {
    RIdArray ids;
    CleanupClosePushL( ids );
    TBool isImCapable = EFalse;
    CSPSettings* settings = CSPSettings::NewL();
    CleanupStack::PushL( settings );
    CDesCArrayFlat* nameArray = new ( ELeave ) CDesCArrayFlat( 2 );
    CleanupStack::PushL( nameArray );    
    TInt error = settings->FindServiceIdsL( ids );  
    User::LeaveIfError( error );     
    error = settings->FindServiceNamesL( ids, *nameArray );  
    User::LeaveIfError( error );
    TBool ready = EFalse;   
    const TInt count = nameArray->MdcaCount();
    for( TInt i = 0; !ready && i < count; i++)
        {
        TPtrC desc = nameArray->MdcaPoint( i );
        if( desc.CompareF( aScheme ) == 0 )
            {
            CSPProperty* property = CSPProperty::NewLC();
            TServicePropertyName feature = ( TServicePropertyName )aFeature; 
            error = settings->FindPropertyL( ids[i], feature, *property );
            if( KErrNone == error )
                {
                isImCapable = ETrue;
                ready = ETrue;
                }
            CleanupStack::PopAndDestroy();  //property
            }
        }
    CleanupStack::PopAndDestroy( 3 );  //nameArray, settings, ids
    return isImCapable;
    }

// ----------------------------------------------------------
// CmsServerUtils::MatchFieldType
// 
// ----------------------------------------------------------
//
TInt CmsServerUtils::MatchFieldType( CCmsContactFieldItem::TCmsContactField aCmsFieldType )
    {
    PRINT1( _L( "CmsServerUtils::MatchFieldType( enum ) - Field: %d" ), aCmsFieldType );
   
    switch( aCmsFieldType )
        {
        case CCmsContactFieldItem::ECmsLastName:
            return R_VPBK_FIELD_TYPE_LASTNAME;
        case CCmsContactFieldItem::ECmsMiddleName:
            return R_VPBK_FIELD_TYPE_MIDDLENAME;
        case CCmsContactFieldItem::ECmsFirstName:
            return R_VPBK_FIELD_TYPE_FIRSTNAME;
        case CCmsContactFieldItem::ECmsLandPhoneGeneric:
            return R_VPBK_FIELD_TYPE_LANDPHONEGEN;
        case CCmsContactFieldItem::ECmsLandPhoneHome:
            return R_VPBK_FIELD_TYPE_LANDPHONEHOME;
        case CCmsContactFieldItem::ECmsMobilePhoneHome:
            return R_VPBK_FIELD_TYPE_MOBILEPHONEHOME;
        case CCmsContactFieldItem::ECmsFaxNumberHome:
            return R_VPBK_FIELD_TYPE_FAXNUMBERHOME;
        case CCmsContactFieldItem::ECmsVideoNumberHome:
            return R_VPBK_FIELD_TYPE_VIDEONUMBERHOME;
        case CCmsContactFieldItem::ECmsAddrLabelGeneric:
            return R_VPBK_FIELD_TYPE_ADDRLABELGEN;
        case CCmsContactFieldItem::ECmsAddrPOGeneric:
            return R_VPBK_FIELD_TYPE_ADDRPOGEN;
        case CCmsContactFieldItem::ECmsAddrExtGeneric:
            return R_VPBK_FIELD_TYPE_ADDREXTGEN;
        case CCmsContactFieldItem::ECmsAddrStreetGeneric:
            return R_VPBK_FIELD_TYPE_ADDRSTREETGEN;
        case CCmsContactFieldItem::ECmsAddrLocalGeneric:
            return R_VPBK_FIELD_TYPE_ADDRLOCALGEN;
        case CCmsContactFieldItem::ECmsAddrRegionGeneric:
            return R_VPBK_FIELD_TYPE_ADDRREGIONGEN;
        case CCmsContactFieldItem::ECmsAddrPostcodeGeneric:
            return R_VPBK_FIELD_TYPE_ADDRPOSTCODEGEN;
        case CCmsContactFieldItem::ECmsAddrCountryGeneric:
            return R_VPBK_FIELD_TYPE_ADDRCOUNTRYGEN;        
        case CCmsContactFieldItem::ECmsAddrLabelHome:
            return R_VPBK_FIELD_TYPE_ADDRLABELHOME;
        case CCmsContactFieldItem::ECmsAddrPOHome:
            return R_VPBK_FIELD_TYPE_ADDRPOHOME;
        case CCmsContactFieldItem::ECmsAddrExtHome:
            return R_VPBK_FIELD_TYPE_ADDREXTHOME;
        case CCmsContactFieldItem::ECmsAddrStreetHome:
            return R_VPBK_FIELD_TYPE_ADDRSTREETHOME;
        case CCmsContactFieldItem::ECmsAddrLocalHome:
           return R_VPBK_FIELD_TYPE_ADDRLOCALHOME;
        case CCmsContactFieldItem::ECmsAddrRegionHome:
            return R_VPBK_FIELD_TYPE_ADDRREGIONHOME;
        case CCmsContactFieldItem::ECmsAddrPostcodeHome:
            return R_VPBK_FIELD_TYPE_ADDRPOSTCODEHOME;
        case CCmsContactFieldItem::ECmsAddrCountryHome:
            return R_VPBK_FIELD_TYPE_ADDRCOUNTRYHOME;
            
        case CCmsContactFieldItem::ECmsAddrLabelWork:
            return R_VPBK_FIELD_TYPE_ADDRLABELWORK;
        case CCmsContactFieldItem::ECmsAddrPOWork:
            return R_VPBK_FIELD_TYPE_ADDRPOWORK;
        case CCmsContactFieldItem::ECmsAddrExtWork:
            return R_VPBK_FIELD_TYPE_ADDREXTWORK;
        case CCmsContactFieldItem::ECmsAddrStreetWork:
            return R_VPBK_FIELD_TYPE_ADDRSTREETWORK;
        case CCmsContactFieldItem::ECmsAddrLocalWork:
           return R_VPBK_FIELD_TYPE_ADDRLOCALWORK;
        case CCmsContactFieldItem::ECmsAddrRegionWork:
            return R_VPBK_FIELD_TYPE_ADDRREGIONWORK;
        case CCmsContactFieldItem::ECmsAddrPostcodeWork:
            return R_VPBK_FIELD_TYPE_ADDRPOSTCODEWORK;
        case CCmsContactFieldItem::ECmsAddrCountryWork:
            return R_VPBK_FIELD_TYPE_ADDRCOUNTRYWORK;
            
        case CCmsContactFieldItem::ECmsVoipNumberHome:
            return R_VPBK_FIELD_TYPE_VOIPHOME;
        case CCmsContactFieldItem::ECmsEmailHome:
            return R_VPBK_FIELD_TYPE_EMAILHOME;
        case CCmsContactFieldItem::ECmsLandPhoneWork:
            return R_VPBK_FIELD_TYPE_LANDPHONEWORK;
        case CCmsContactFieldItem::ECmsEmailGeneric:
            return R_VPBK_FIELD_TYPE_EMAILGEN;
        case CCmsContactFieldItem::ECmsMobilePhoneGeneric:
            return R_VPBK_FIELD_TYPE_MOBILEPHONEGEN;
        case CCmsContactFieldItem::ECmsMobilePhoneWork:
            return R_VPBK_FIELD_TYPE_MOBILEPHONEWORK;
        case CCmsContactFieldItem::ECmsVideoNumberWork:
            return R_VPBK_FIELD_TYPE_VIDEONUMBERWORK;
        case CCmsContactFieldItem::ECmsVideoNumberGeneric:
            return R_VPBK_FIELD_TYPE_VIDEONUMBERGEN;  
        case CCmsContactFieldItem::ECmsFaxNumberWork:
            return R_VPBK_FIELD_TYPE_FAXNUMBERWORK;
        case CCmsContactFieldItem::ECmsVoipNumberWork:
            return R_VPBK_FIELD_TYPE_VOIPWORK;
        case CCmsContactFieldItem::ECmsVoipNumberGeneric:
            return R_VPBK_FIELD_TYPE_VOIPGEN;
        case CCmsContactFieldItem::ECmsEmailWork:
            return R_VPBK_FIELD_TYPE_EMAILWORK;
        case CCmsContactFieldItem::ECmsSipAddress:
            return R_VPBK_FIELD_TYPE_SIP;
        case CCmsContactFieldItem::ECmsThumbnailPic:
            return R_VPBK_FIELD_TYPE_THUMBNAILPIC;
        case CCmsContactFieldItem::ECmsImpp:       
            return R_VPBK_FIELD_TYPE_IMPP;    
        case CCmsContactFieldItem::ECmsFullName:
            return aCmsFieldType; //do not change anything
        case CCmsContactFieldItem::ECmsUrlWork:
            return R_VPBK_FIELD_TYPE_URLWORK;
        case CCmsContactFieldItem::ECmsUrlHome:
            return R_VPBK_FIELD_TYPE_URLHOME;
        case CCmsContactFieldItem::ECmsUrlGeneric:
            return R_VPBK_FIELD_TYPE_URLGEN;
        case CCmsContactFieldItem::ECmsNote:
            return R_VPBK_FIELD_TYPE_NOTE;
        case CCmsContactFieldItem::ECmsNickname:
            return R_VPBK_FIELD_TYPE_SECONDNAME;    //TODO!!
        case CCmsContactFieldItem::ECmsJobTitle:
            return R_VPBK_FIELD_TYPE_JOBTITLE;
        case CCmsContactFieldItem::ECmsCompanyName:
            return R_VPBK_FIELD_TYPE_COMPANYNAME;
        case CCmsContactFieldItem::ECmsDepartment:
            return R_VPBK_FIELD_TYPE_DEPARTMENT;
        case CCmsContactFieldItem::ECmsBirthday:
            return KErrNotFound;    //TODO!!
        case CCmsContactFieldItem::ECmsAnniversary:
            return R_VPBK_FIELD_TYPE_ANNIVERSARY;
        case CCmsContactFieldItem::ECmsAssistantNumber:
            return R_VPBK_FIELD_TYPE_ASSTPHONE;
        case CCmsContactFieldItem::ECmsPagerNumber:
            return R_VPBK_FIELD_TYPE_PAGERNUMBER;
        case CCmsContactFieldItem::ECmsCarPhone:
            return R_VPBK_FIELD_TYPE_CARPHONE;
        case CCmsContactFieldItem::ECmsPushToTalk:
            return R_VPBK_FIELD_TYPE_POC;
        case CCmsContactFieldItem::ECmsShareView:
            return R_VPBK_FIELD_TYPE_SWIS;
        case CCmsContactFieldItem::ECmsAddrGeoGeneric:
            return R_VPBK_FIELD_TYPE_GEOGEN;
        case CCmsContactFieldItem::ECmsAddrGeoHome:
            return R_VPBK_FIELD_TYPE_GEOHOME;
        case CCmsContactFieldItem::ECmsAddrGeoWork:
            return R_VPBK_FIELD_TYPE_GEOWORK;
        case CCmsContactFieldItem::ECmsImageName:
            return R_VPBK_FIELD_TYPE_CALLEROBJIMG;
        default:
            return KErrNotFound; 
        }
    }

// ----------------------------------------------------------
// CmsServerUtils::MatchFieldType
// 
// ----------------------------------------------------------
//
CCmsContactFieldItem::TCmsContactField CmsServerUtils::MatchFieldType( TInt aPhonebookResId )
    {
    PRINT1( _L( "CmsServerUtils::MatchFieldType( ResID ) - Field: %d" ), aPhonebookResId );
    
    switch( aPhonebookResId )
        {
        case R_VPBK_FIELD_TYPE_LASTNAME:
            return CCmsContactFieldItem::ECmsLastName;
        case R_VPBK_FIELD_TYPE_MIDDLENAME:
            return CCmsContactFieldItem::ECmsMiddleName;
        case R_VPBK_FIELD_TYPE_FIRSTNAME:
            return CCmsContactFieldItem::ECmsFirstName;
        case R_VPBK_FIELD_TYPE_LANDPHONEGEN:
            return CCmsContactFieldItem::ECmsLandPhoneGeneric;
        case R_VPBK_FIELD_TYPE_LANDPHONEHOME:
            return CCmsContactFieldItem::ECmsLandPhoneHome;
        case R_VPBK_FIELD_TYPE_MOBILEPHONEHOME:
            return CCmsContactFieldItem::ECmsMobilePhoneHome;
        case R_VPBK_FIELD_TYPE_ADDRLABELGEN:
            return CCmsContactFieldItem::ECmsAddrLabelGeneric;
        case R_VPBK_FIELD_TYPE_ADDRPOGEN:
            return CCmsContactFieldItem::ECmsAddrPOGeneric;
        case R_VPBK_FIELD_TYPE_ADDREXTGEN:
            return CCmsContactFieldItem::ECmsAddrExtGeneric;
        case R_VPBK_FIELD_TYPE_ADDRSTREETGEN:
            return CCmsContactFieldItem::ECmsAddrStreetGeneric;
        case R_VPBK_FIELD_TYPE_ADDRLOCALGEN:
            return CCmsContactFieldItem::ECmsAddrLocalGeneric;
        case R_VPBK_FIELD_TYPE_ADDRREGIONGEN:
            return CCmsContactFieldItem::ECmsAddrRegionGeneric;
        case R_VPBK_FIELD_TYPE_ADDRPOSTCODEGEN:
            return CCmsContactFieldItem::ECmsAddrPostcodeGeneric;
        case R_VPBK_FIELD_TYPE_ADDRCOUNTRYGEN:
            return CCmsContactFieldItem::ECmsAddrCountryGeneric;
        case R_VPBK_FIELD_TYPE_ADDRLABELHOME:
            return CCmsContactFieldItem::ECmsAddrLabelHome;
        case R_VPBK_FIELD_TYPE_ADDRPOHOME:
            return CCmsContactFieldItem::ECmsAddrPOHome;
        case R_VPBK_FIELD_TYPE_ADDREXTHOME:
            return CCmsContactFieldItem::ECmsAddrExtHome;
        case R_VPBK_FIELD_TYPE_ADDRSTREETHOME:
            return CCmsContactFieldItem::ECmsAddrStreetHome; 
        case R_VPBK_FIELD_TYPE_ADDRLOCALHOME:
           return CCmsContactFieldItem::ECmsAddrLocalHome; 
        case R_VPBK_FIELD_TYPE_ADDRREGIONHOME:
            return CCmsContactFieldItem::ECmsAddrRegionHome;
        case R_VPBK_FIELD_TYPE_ADDRPOSTCODEHOME:
            return CCmsContactFieldItem::ECmsAddrPostcodeHome;
        case R_VPBK_FIELD_TYPE_ADDRCOUNTRYHOME:
            return CCmsContactFieldItem::ECmsAddrCountryHome;
            
        case R_VPBK_FIELD_TYPE_ADDRLABELWORK:
            return CCmsContactFieldItem::ECmsAddrLabelWork;
        case R_VPBK_FIELD_TYPE_ADDRPOWORK:
            return CCmsContactFieldItem::ECmsAddrPOWork;
        case R_VPBK_FIELD_TYPE_ADDREXTWORK:
            return CCmsContactFieldItem::ECmsAddrExtWork;
        case R_VPBK_FIELD_TYPE_ADDRSTREETWORK:
            return CCmsContactFieldItem::ECmsAddrStreetWork; 
        case R_VPBK_FIELD_TYPE_ADDRLOCALWORK:
           return CCmsContactFieldItem::ECmsAddrLocalWork; 
        case R_VPBK_FIELD_TYPE_ADDRREGIONWORK:
            return CCmsContactFieldItem::ECmsAddrRegionWork;
        case R_VPBK_FIELD_TYPE_ADDRPOSTCODEWORK:
            return CCmsContactFieldItem::ECmsAddrPostcodeWork;
        case R_VPBK_FIELD_TYPE_ADDRCOUNTRYWORK:
            return CCmsContactFieldItem::ECmsAddrCountryWork;
            
        case R_VPBK_FIELD_TYPE_FAXNUMBERHOME:
            return CCmsContactFieldItem::ECmsFaxNumberHome;
        case R_VPBK_FIELD_TYPE_VIDEONUMBERHOME:
            return CCmsContactFieldItem::ECmsVideoNumberHome;
        case R_VPBK_FIELD_TYPE_VOIPHOME:
            return CCmsContactFieldItem::ECmsVoipNumberHome;
        case R_VPBK_FIELD_TYPE_EMAILHOME:
            return CCmsContactFieldItem::ECmsEmailHome;
        case R_VPBK_FIELD_TYPE_LANDPHONEWORK:
            return CCmsContactFieldItem::ECmsLandPhoneWork;
        case R_VPBK_FIELD_TYPE_MOBILEPHONEWORK:
            return CCmsContactFieldItem::ECmsMobilePhoneWork;
        case R_VPBK_FIELD_TYPE_VIDEONUMBERWORK:
            return CCmsContactFieldItem::ECmsVideoNumberWork;
        case R_VPBK_FIELD_TYPE_FAXNUMBERWORK:
            return CCmsContactFieldItem::ECmsFaxNumberWork;
        case R_VPBK_FIELD_TYPE_VOIPWORK:
            return CCmsContactFieldItem::ECmsVoipNumberWork;
        case R_VPBK_FIELD_TYPE_EMAILWORK:
            return CCmsContactFieldItem::ECmsEmailWork;
        case R_VPBK_FIELD_TYPE_MOBILEPHONEGEN:
            return CCmsContactFieldItem::ECmsMobilePhoneGeneric;
        case R_VPBK_FIELD_TYPE_VOIPGEN:
            return CCmsContactFieldItem::ECmsVoipNumberGeneric;
        case R_VPBK_FIELD_TYPE_VIDEONUMBERGEN:
            return CCmsContactFieldItem::ECmsVideoNumberGeneric;
        case R_VPBK_FIELD_TYPE_EMAILGEN:
            return CCmsContactFieldItem::ECmsEmailGeneric;
        case R_VPBK_FIELD_TYPE_SIP:
            return CCmsContactFieldItem::ECmsSipAddress;
        case R_VPBK_FIELD_TYPE_THUMBNAILPIC:
            return CCmsContactFieldItem::ECmsThumbnailPic;
        case R_VPBK_FIELD_TYPE_URLGEN:
            return CCmsContactFieldItem::ECmsUrlGeneric;
        case R_VPBK_FIELD_TYPE_URLHOME:
            return CCmsContactFieldItem::ECmsUrlHome;
        case R_VPBK_FIELD_TYPE_URLWORK:
            return CCmsContactFieldItem::ECmsUrlWork;
        case R_VPBK_FIELD_TYPE_IMPP:        
            return CCmsContactFieldItem::ECmsImpp;
        case R_VPBK_FIELD_TYPE_NOTE:
            return CCmsContactFieldItem::ECmsNote;
        case R_VPBK_FIELD_TYPE_SECONDNAME:
            return CCmsContactFieldItem::ECmsNickname;    //TODO!!
        case R_VPBK_FIELD_TYPE_JOBTITLE:
            return CCmsContactFieldItem::ECmsJobTitle;
        case R_VPBK_FIELD_TYPE_COMPANYNAME:
            return CCmsContactFieldItem::ECmsCompanyName;
        case R_VPBK_FIELD_TYPE_DEPARTMENT:
            return CCmsContactFieldItem::ECmsDepartment;
        //case CCmsContactFieldItem::ECmsBirthday:
        //    return KErrNotFound;    //TODO!!
        case R_VPBK_FIELD_TYPE_ANNIVERSARY:
            return CCmsContactFieldItem::ECmsAnniversary;
        case R_VPBK_FIELD_TYPE_ASSTPHONE:
            return CCmsContactFieldItem::ECmsAssistantNumber;
        case R_VPBK_FIELD_TYPE_PAGERNUMBER:
            return CCmsContactFieldItem::ECmsPagerNumber;
        case R_VPBK_FIELD_TYPE_CARPHONE:
            return CCmsContactFieldItem::ECmsCarPhone;
        case R_VPBK_FIELD_TYPE_POC:
            return CCmsContactFieldItem::ECmsPushToTalk;
        case R_VPBK_FIELD_TYPE_SWIS:
            return CCmsContactFieldItem::ECmsShareView;
        case R_VPBK_FIELD_TYPE_GEOGEN:
            return CCmsContactFieldItem::ECmsAddrGeoGeneric;
        case R_VPBK_FIELD_TYPE_GEOHOME:
            return CCmsContactFieldItem::ECmsAddrGeoHome;
        case R_VPBK_FIELD_TYPE_GEOWORK:
            return CCmsContactFieldItem::ECmsAddrGeoWork;
        case R_VPBK_FIELD_TYPE_CALLEROBJIMG:
            return CCmsContactFieldItem::ECmsImageName;
        default:
            return CCmsContactFieldItem::ECmsUndefined;
        }
    }








