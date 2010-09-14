/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*       Provides methods for Phonebook 2 field analyzer.
*
*/


// INCLUDE FILES
#include "CPbk2FieldAnalyzer.h"

// Phonebook 2
#include <CPbk2PresentationContactField.h>

// Virtual Phonebook
#include <MVPbkStoreContactField.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactStore.h>
#include <CVPbkContactManager.h>
#include <CVPbkSpeedDialAttribute.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <vpbkeng.rsg>
#include <MVPbkFieldType.h>
#include <CVPbkContactFieldIterator.h>
#include <MVPbkContactFieldUriData.h>

// System
#include <featmgr.h>
#include <spsettings.h>
#include <spentry.h>
#include <spproperty.h>
#include <crcseprofileregistry.h>

namespace   // Unnamed namespace. Used to break down the VoipSupportL method
{
/*
*	Check whether there is any voip capable settings in the 
*   phone's Service Provider Settings
*/

void BasicSupportL(TInt& aVoipFlag)
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
    
    if (supported)
        {
        aVoipFlag |= MPbk2FieldAnalyzer::EVoIPSupportBasic;
        }
	
    if( settings->IsFeatureSupported( ESupportCallOutFeature ) )
        {
        aVoipFlag |= MPbk2FieldAnalyzer::EVoIPSupportCallout;
		}
    
    CleanupStack::PopAndDestroy(2); // settings, idArray
    }
/*
*	Check whether there is any SIP settings in the phone's Service Provider Settings
*/

void CleanupResetAndDestroy(TAny*  aItem)
	{
	RPointerArray<CRCSEProfileEntry> *pMyArray = (RPointerArray<CRCSEProfileEntry>*) aItem;
	pMyArray->ResetAndDestroy();
	pMyArray->Close();
	}

void SipSupportL(TInt& aVoipFlag)
    { 
    TBool found( EFalse );
    RPointerArray<CRCSEProfileEntry> entries;
    CleanupStack::PushL( TCleanupItem( CleanupResetAndDestroy, &entries ) );
        
    CSPSettings* settings = CSPSettings::NewLC();
    RIdArray idArray;
    CleanupClosePushL(idArray);
        
    User::LeaveIfError( settings->FindServiceIdsL(idArray) );
    CRCSEProfileRegistry* profileRegistry = CRCSEProfileRegistry::NewLC();
	// Check if we have atleast one SPSetting entry
	// Any entry in this array uses SIP protocol for VoIP
    TInt count = idArray.Count();
	for (TInt i = 0; !found && i < count; ++i)
        {
        profileRegistry->FindByServiceIdL( idArray[i], entries );
        if (entries.Count() > 0)
            {
            aVoipFlag |= MPbk2FieldAnalyzer::EVoIPSupportSip;
            found = ETrue;
            }
        }       
    
    CleanupStack::PopAndDestroy( 4 ); // entries, idArray, settings, profileRegistry
    }
/*
*	Check whether the Xsp fields specified in the IMPP field of the contact
* 	support voip (not necessarily SIP)
*/

void XspSupportL(CVPbkContactManager& aContactManager, 
        const MVPbkBaseContactFieldCollection& aFields, TInt& aVoipFlag)
    {
    
    TBool found( EFalse );
    RIdArray ids;
    CleanupClosePushL( ids );
    CSPSettings* settings = CSPSettings::NewLC();
    
    CDesCArrayFlat* nameArray = new ( ELeave ) CDesCArrayFlat( 2 );
    CleanupStack::PushL( nameArray );
    
    TInt error = settings->FindServiceIdsL( ids );  
    User::LeaveIfError( error );     
    error = settings->FindServiceNamesL( ids, *nameArray );
	User::LeaveIfError( error );
    const TInt count = nameArray->MdcaCount();	

    const MVPbkFieldType* type = aContactManager.FieldTypes().Find( R_VPBK_FIELD_TYPE_IMPP );
    CVPbkBaseContactFieldTypeIterator* itr = CVPbkBaseContactFieldTypeIterator::NewLC( *type, aFields );
    
    while( itr->HasNext() && !found )
        {
        const MVPbkBaseContactField* field = itr->Next();
        const MVPbkContactFieldUriData& uri = MVPbkContactFieldUriData::Cast( field->FieldData() );
        TPtrC scheme( uri.Scheme() );
        
        for ( TInt i = 0; !found && i < count; i++ )
            {
            TPtrC desc = nameArray->MdcaPoint( i );
            if ( !desc.CompareF( scheme ) )
                {
                CSPProperty* property = CSPProperty::NewLC();
                error = settings->FindPropertyL( ids[i], EPropertyServiceAttributeMask, *property );
                if ( KErrNone == error )
                    {
                    TInt value = 0; 
                    property->GetValue( value );                    
                    if ( value & ESupportsInternetCall )
                        {
                        found = ETrue;
                        aVoipFlag |= MPbk2FieldAnalyzer::EVoIPSupportXspId;                
                        }
                    }
                CleanupStack::PopAndDestroy();  //property
                break;
                }
            }
        }
    CleanupStack::PopAndDestroy( 4 );  //nameArray, settings, ids, itr    
    }
}

// --------------------------------------------------------------------------
// CPbk2FieldAnalyzer::CPbk2FieldAnalyzer
// --------------------------------------------------------------------------
//
CPbk2FieldAnalyzer::CPbk2FieldAnalyzer( 
        CVPbkContactManager& aContactManager ) : 
    iContactManager( aContactManager ),
    iVoiceTaggedFields (NULL)
	{
	// Do nothing
	}

// --------------------------------------------------------------------------
// CPbk2FieldAnalyzer::NewL
// --------------------------------------------------------------------------
//
CPbk2FieldAnalyzer* CPbk2FieldAnalyzer::NewL( 
        CVPbkContactManager& aContactManager )
    {
    CPbk2FieldAnalyzer* self = new (ELeave) CPbk2FieldAnalyzer( 
        aContactManager);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2FieldAnalyzer::~CPbk2FieldAnalyzer
// --------------------------------------------------------------------------
//
CPbk2FieldAnalyzer::~CPbk2FieldAnalyzer()
    {
    delete iVoiceTaggedFields;
    }

// --------------------------------------------------------------------------
// CPbk2FieldAnalyzer::HasSpeedDialL
// --------------------------------------------------------------------------
//
TBool CPbk2FieldAnalyzer::HasSpeedDialL( 
        const MVPbkStoreContactField& aField ) const
    {
    return iContactManager.ContactAttributeManagerL().HasFieldAttributeL
           ( CVPbkSpeedDialAttribute::Uid(), aField );
    }


// --------------------------------------------------------------------------
// CPbk2FieldAnalyzer::HasVoiceTagL
// --------------------------------------------------------------------------
//
TBool CPbk2FieldAnalyzer::HasVoiceTagL( 
        const MVPbkStoreContactField& aField ) const
    {
    TBool ret = EFalse;

    if (iVoiceTaggedFields)
        {
        // look into voice tag container to see if this field had a voice tag    
        const TInt count = iVoiceTaggedFields->FieldCount();
        for ( TInt i = 0; i < count && !ret; ++i )
            {
            MVPbkStoreContactField& voiceTagField = iVoiceTaggedFields->FieldAt(i);            
            if (aField.IsSame(voiceTagField))
                {
                ret = ETrue;
                }
            }
        }
    return ret;    
    }


// --------------------------------------------------------------------------
// CPbk2FieldAnalyzer::IsHiddenField
// --------------------------------------------------------------------------
//
TBool CPbk2FieldAnalyzer::IsHiddenField( 
        const CPbk2PresentationContactField& aField ) const
    {
    return !aField.IsVisibleInDetailsView();
    }

// --------------------------------------------------------------------------
// CPbk2FieldAnalyzer::SetVoiceTagFields
// --------------------------------------------------------------------------
//
void CPbk2FieldAnalyzer::SetVoiceTagFields(
        MVPbkStoreContactFieldCollection* aFields)
    {
    delete iVoiceTaggedFields;
    iVoiceTaggedFields = aFields;
    }

const TInt CPbk2FieldAnalyzer::VoipSupportL( const MVPbkBaseContactFieldCollection& aFields ) const
    {
    TInt supported( 0 );
    
    if ( FeatureManager::FeatureSupported(KFeatureIdCommonVoip) )
    	{
        BasicSupportL( supported );
        SipSupportL( supported );
        XspSupportL( iContactManager, aFields, supported );
    	}
    return supported;    
    
    }

TBool CPbk2FieldAnalyzer::IsFieldVoipCapable( TInt aVpbkFieldResId, const TInt aVoipFlag ) const
    {
    TBool ret( EFalse );
    
    if ( FeatureManager::FeatureSupported(KFeatureIdCommonVoip) )
        {
        if ( EVoIPSupportBasic & aVoipFlag && 
           ( R_VPBK_FIELD_TYPE_VOIPGEN == aVpbkFieldResId 
          || R_VPBK_FIELD_TYPE_VOIPHOME == aVpbkFieldResId
          || R_VPBK_FIELD_TYPE_VOIPWORK == aVpbkFieldResId
          || R_VPBK_FIELD_TYPE_POC == aVpbkFieldResId
          || R_VPBK_FIELD_TYPE_SWIS == aVpbkFieldResId ) )
            {
            ret = ETrue;
            }
        if ( EVoIPSupportSip & aVoipFlag && 
          ( R_VPBK_FIELD_TYPE_SIP == aVpbkFieldResId ) )
            {
            ret = ETrue;
            }
        if ( EVoIPSupportXspId & aVoipFlag && 
          ( R_VPBK_FIELD_TYPE_IMPP == aVpbkFieldResId  ) )
            {
            ret = ETrue;
            }
        if ( EVoIPSupportCallout & aVoipFlag && 
           ( R_VPBK_FIELD_TYPE_MOBILEPHONEGEN == aVpbkFieldResId
          || R_VPBK_FIELD_TYPE_MOBILEPHONEHOME == aVpbkFieldResId
          || R_VPBK_FIELD_TYPE_MOBILEPHONEWORK == aVpbkFieldResId
          || R_VPBK_FIELD_TYPE_LANDPHONEGEN == aVpbkFieldResId
          || R_VPBK_FIELD_TYPE_LANDPHONEHOME == aVpbkFieldResId
          || R_VPBK_FIELD_TYPE_LANDPHONEWORK == aVpbkFieldResId
          || R_VPBK_FIELD_TYPE_ASSTPHONE == aVpbkFieldResId
          || R_VPBK_FIELD_TYPE_CARPHONE == aVpbkFieldResId ) )
            {
            ret = ETrue;
            }
        }
    
    return ret;
    }


//  End of File  
