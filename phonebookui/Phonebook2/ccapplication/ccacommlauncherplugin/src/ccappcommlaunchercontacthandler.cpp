/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of details view plugin
*
*/


#include "ccappcommlauncherheaders.h"
#include <centralrepository.h>
#include <VPbkVariant.hrh>

// ======== LOCAL FUNCTIONS ========

// --------------------------------------------------------------------------
// MatchContactField
// --------------------------------------------------------------------------
//
TBool MatchContactField( 
    const CCmsContactFieldItem::TCmsContactField* aKey, 
    const CCmsContactField& aField )
    {
    if ( *aKey == aField.Type() )
        return ETrue;
    else
        return EFalse;
    }

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::CCCAppCommLauncherContactHandler
// --------------------------------------------------------------------------
//
CCCAppCommLauncherContactHandler::CCCAppCommLauncherContactHandler(
    MCCAppContactHandlerNotifier& aObserver,
    CCCAppCommLauncherPlugin& aPlugin):
        iObserver( aObserver ),
        iDynamicAddressGroupSet( 0 ),
        iDefaultAttributesBitMask( 0 ),
        iPlugin(aPlugin)
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("CCCAppCommLauncherContactHandler()"));
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::~CCCAppCommLauncherContactHandler
// --------------------------------------------------------------------------
//
CCCAppCommLauncherContactHandler::~CCCAppCommLauncherContactHandler()
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("~CCCAppCommLauncherContactHandler()"));
    if ( iCmsWrapper )
        {
        iCmsWrapper->RemoveObserver( *this );
        iCmsWrapper->Release();
        }
    iDynamicVoipAddressGroup.Close();
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::NewL
// --------------------------------------------------------------------------
//
CCCAppCommLauncherContactHandler* CCCAppCommLauncherContactHandler::NewL(
    MCCAppContactHandlerNotifier& aObserver,
    CCCAppCommLauncherPlugin& aPlugin )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherContactHandler::NewL()"));
    CCCAppCommLauncherContactHandler* self =
        new ( ELeave ) CCCAppCommLauncherContactHandler( aObserver,
            aPlugin ) ;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherContactHandler::NewL()"));
    return self;
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::ConstructL
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherContactHandler::ConstructL()
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherContactHandler::ConstructL()"));
    iCmsWrapper = CCCAppCmsContactFetcherWrapper::InstanceL();
    
    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherContactHandler::ConstructL()"));
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::RequestContactDataL
// --------------------------------------------------------------------------
//    
void CCCAppCommLauncherContactHandler::RequestContactDataL()
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherContactHandler::RequestContactDataL()"));
    
    // 1st fetch whatever fetched so far
    const CCmsContactFieldInfo* info = iCmsWrapper->ContactInfo();//not owned
    if ( info )// if NULL, info fetching not completed
        {
        CCA_DP(KCommLauncherLogFile, CCA_L("::RequestContactDataL() - info available"));
        ContactInfoFetchedNotifyL( *info );
        // if info is fetched, there can be actual fields fetched also
        RPointerArray<CCmsContactField> fieldArray = iCmsWrapper->ContactFieldDataArray();//not owned
        const TInt count = fieldArray.Count();
        CCA_DP(KCommLauncherLogFile, CCA_L("::RequestContactDataL - fetched field array count: %d"), count );
        for ( TInt i = 0; i < count; i++ )
            {
            
            CCmsContactField& contactField = *fieldArray[i];
            ContactFieldFetchedNotifyL( contactField );           
            
            if ( CCmsContactFieldItem::ECmsFullName == contactField.Type() )
                {// update titlepane with the name 
                TPtrC newTxt = contactField.ItemL( 0 ).Data();
                iPlugin.SetTitleL( newTxt );
                }                
            }
        iObserver.ContactFieldFetchingCompletedL();
        }

    // 2nd register as observer for following events
    iCmsWrapper->AddObserverL( *this );
        
    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherContactHandler::RequestContactDataL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::ContactFieldDataObserverNotifyL
// ---------------------------------------------------------------------------
//  
void CCCAppCommLauncherContactHandler::ContactFieldDataObserverNotifyL( 
        MCCAppContactFieldDataObserver::TParameter& aParameter )
    {
    if ( MCCAppContactFieldDataObserver::TParameter::EContactInfoAvailable 
        == aParameter.iType && NULL != aParameter.iContactInfo )
        {
        iDynamicAddressGroupSet = EFalse;
        ContactInfoFetchedNotifyL( *aParameter.iContactInfo );
        }
    else if ( MCCAppContactFieldDataObserver::TParameter::EContactDataFieldAvailable 
        == aParameter.iType && NULL != aParameter.iContactField )
        {
        TRAP_IGNORE( ContactFieldFetchedNotifyL( *aParameter.iContactField ));
        if ( CCCAppCmsContactFetcherWrapper::EOperationsCompleted 
             & aParameter.iStatusFlag )
            {
            iObserver.ContactFieldFetchingCompletedL();
            }
        }
    else if ( MCCAppContactFieldDataObserver::TParameter::EContactsChanged 
        == aParameter.iType )
        {
        iDynamicAddressGroupSet = EFalse;
        iObserver.ContactsChangedL();
        }
    else if (MCCAppContactFieldDataObserver::TParameter::EContactPresenceChanged == aParameter.iType)
        {
        iObserver.ContactPresenceChangedL(*aParameter.iContactField);
        }
   }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::ContactFieldDataObserverHandleErrorL
// ---------------------------------------------------------------------------
//  
void CCCAppCommLauncherContactHandler::ContactFieldDataObserverHandleErrorL( 
    TInt aState, TInt aError )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("::ContactFieldDataObserverHandleErrorL - aState: %d, aError: %d"), aState, aError );
    //todo; what kind of behaviour is wanted in error case?
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::ContactInfoFetchedNotifyL
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherContactHandler::ContactInfoFetchedNotifyL( 
    const CCmsContactFieldInfo& aContactFieldInfo )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherContactHandler::ContactInfoFetchedNotifyL()"));

    iDefaultAttributesBitMask = 0;

    //PERFORMANCE LOGGING: 9. Contact info notify received
    WriteToPerfLog();

    iObserver.ContactInfoFetchedNotifyL( aContactFieldInfo );

    //PERFORMANCE LOGGING: 10. Contact info notify consumed
    WriteToPerfLog();

    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherContactHandler::ContactInfoFetchedNotifyL()"));
    }
    
// --------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::ContactFieldFetchedNotifyL
// --------------------------------------------------------------------------
//   
void CCCAppCommLauncherContactHandler::ContactFieldFetchedNotifyL( 
    /*const*/ CCmsContactField& aContactField )//todo; should be const!
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherContactHandler::ContactFieldFetchedNotifyL()"));
    
    UpdateDefaultAttributes( aContactField );
    iObserver.ContactFieldFetchedNotifyL( aContactField );

    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherContactHandler::ContactFieldFetchedNotifyL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::AddressAmount
// ---------------------------------------------------------------------------
//
TInt CCCAppCommLauncherContactHandler::AddressAmount( 
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction )
    {
    return iCmsWrapper->GetContactActionFieldCount(aContactAction);
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::IsServiceAvailable
// ---------------------------------------------------------------------------
//
TInt CCCAppCommLauncherContactHandler::IsServiceAvailable( 
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction )
    {
    return iCmsWrapper->IsServiceAvailable( aContactAction );
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::ContactStore
// --------------------------------------------------------------------------
// 
TCmsContactStore CCCAppCommLauncherContactHandler::ContactStore() const
    {
    return iCmsWrapper->ContactStore();
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::ContactFieldDataArray
// ---------------------------------------------------------------------------
//
RPointerArray<CCmsContactField>& CCCAppCommLauncherContactHandler::ContactFieldDataArray()
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("CCCAppCommLauncherContactHandler::ContactFieldDataArray()"));    
    return iCmsWrapper->ContactFieldDataArray();
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::ContactIdentifierLC
// ---------------------------------------------------------------------------
//
HBufC8* CCCAppCommLauncherContactHandler::ContactIdentifierLC()
    {
    return iCmsWrapper->ContactIdentifierLC();
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::UpdateDefaultAttributes
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherContactHandler::UpdateDefaultAttributes(
    const CCmsContactField& aContactField )
    {
    iDefaultAttributesBitMask |= aContactField.HasDefaultAttribute();         
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::MapContactorTypeToCMSDefaultType
// ---------------------------------------------------------------------------
//
CCmsContactFieldItem::TCmsDefaultAttributeTypes 
    CCCAppCommLauncherContactHandler::MapContactorTypeToCMSDefaultType( 
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction )
    {
    CCmsContactFieldItem::TCmsDefaultAttributeTypes returnValue
        = CCmsContactFieldItem::ECmsDefaultTypeUndefined; 
    
    switch( aContactAction )
        {
        case VPbkFieldTypeSelectorFactory::EVoiceCallSelector:        
            returnValue = CCmsContactFieldItem::ECmsDefaultTypePhoneNumber;
            break;
        case VPbkFieldTypeSelectorFactory::EUniEditorSelector:        
            returnValue = CCmsContactFieldItem::ECmsDefaultTypeMms;
            break;
        case VPbkFieldTypeSelectorFactory::EEmailEditorSelector:        
            returnValue = CCmsContactFieldItem::ECmsDefaultTypeEmail;
            break;
        case VPbkFieldTypeSelectorFactory::EInstantMessagingSelector:
            returnValue = CCmsContactFieldItem::ECmsDefaultTypeImpp;
            break;
        case VPbkFieldTypeSelectorFactory::EVOIPCallSelector:        
            returnValue = CCmsContactFieldItem::ECmsDefaultTypeVoIP;
            break;
        case VPbkFieldTypeSelectorFactory::EURLSelector:
            returnValue = CCmsContactFieldItem::ECmsDefaultTypeUrl;
            break;
        case VPbkFieldTypeSelectorFactory::EVideoCallSelector:        
            returnValue = CCmsContactFieldItem::ECmsDefaultTypeVideoNumber;
            break;
        case VPbkFieldTypeSelectorFactory::EPocSelector:
            returnValue = CCmsContactFieldItem::ECmsDefaultTypePOC;
            break;
        default:
            break;
        }
    return returnValue;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::HasDefaultAttribute
// ---------------------------------------------------------------------------
//
TBool CCCAppCommLauncherContactHandler::HasDefaultAttribute(
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction )
    {
    return MapContactorTypeToCMSDefaultType( aContactAction ) & iDefaultAttributesBitMask;
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::ContactFieldItemDataL
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherContactHandler::ContactFieldItemDataL(
    const CCmsContactFieldItem::TCmsContactField aFieldType, TPtrC& aData )
    {
    RPointerArray<CCmsContactField>& cntFieldArray = 
        iCmsWrapper->ContactFieldDataArray(); //not own
    const TInt index = cntFieldArray.Find( aFieldType, MatchContactField );

    if ( index >= 0 && index < cntFieldArray.Count() 
         && cntFieldArray[index]->ItemCount() > 0 )
        {
        aData.Set( cntFieldArray[index]->ItemL( 0 ).Data() );
        }
    else
        {
        aData.Set( KNullDesC() );
        }
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::ContactFieldTypeAndContactActionMatch
// --------------------------------------------------------------------------
//
TBool CCCAppCommLauncherContactHandler::ContactFieldTypeAndContactActionMatch(
    CCmsContactFieldItem::TCmsContactField aContactField,        
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherContactHandler::ContactFieldTypeAndContactActionMatch()"));    
    CCA_DP(KCommLauncherLogFile, CCA_L("::ContactFieldTypeAndContactActionMatch() - aContactAction: %d"), aContactAction );    
    CCA_DP(KCommLauncherLogFile, CCA_L("::ContactFieldTypeAndContactActionMatch() - aContactField: %d"), aContactField );    
    
    TBool isMatch = EFalse;
    
    switch( aContactAction )
        {
        case VPbkFieldTypeSelectorFactory::EVoiceCallSelector:
            isMatch = FieldArrayAndContactActionMatch( 
                aContactField, KCCAppCommLauncherVoiceCallGroup, 
                CCACOMMLAUNCHER_ROMARRAY_SIZE( KCCAppCommLauncherVoiceCallGroup )); 
            break;
        case VPbkFieldTypeSelectorFactory::EUniEditorSelector: 
        	 TInt flags = 0;
        	 CRepository* key = CRepository::NewL(TUid::Uid(KCRUidPhonebook));
        	 TInt err = key->Get(KPhonebookLocalVariationFlags, flags);
        	            
        	 if ( flags & EVPbkLVShowEmailInSendMsg)
        		 {
        		 isMatch = FieldArrayAndContactActionMatch( 
        		          aContactField, KCCAppCommLauncherMessageEmailGroup, 
        		          CCACOMMLAUNCHER_ROMARRAY_SIZE( KCCAppCommLauncherMessageEmailGroup ));
        		 }
        	 else
        		 {       	
        		 isMatch = FieldArrayAndContactActionMatch( 
        				 aContactField, KCCAppCommLauncherMessageGroup, 
        				 CCACOMMLAUNCHER_ROMARRAY_SIZE( KCCAppCommLauncherMessageGroup )); 
        		 }
        	 delete key;
        	 break;
        case VPbkFieldTypeSelectorFactory::EEmailEditorSelector:        
            isMatch = FieldArrayAndContactActionMatch( 
                aContactField, KCCAppCommLauncherEmailGroup, 
                CCACOMMLAUNCHER_ROMARRAY_SIZE( KCCAppCommLauncherEmailGroup )); 
            break;
        case VPbkFieldTypeSelectorFactory::EInstantMessagingSelector:
            isMatch = FieldArrayAndContactActionMatch( 
                aContactField, KCCAppCommLauncherChatGroup, 
                CCACOMMLAUNCHER_ROMARRAY_SIZE( KCCAppCommLauncherChatGroup )); 
            break;
        case VPbkFieldTypeSelectorFactory::EVOIPCallSelector:
            // note; dynamic construction required
            ConstructDynamicFieldArrayIfNeeded( aContactAction );
            isMatch = ( KErrNotFound != iDynamicVoipAddressGroup.Find( aContactField ));             
            break;
        case VPbkFieldTypeSelectorFactory::EURLSelector:        
            isMatch = FieldArrayAndContactActionMatch( 
                aContactField, KCCAppCommLauncherUrlGroup, 
                CCACOMMLAUNCHER_ROMARRAY_SIZE( KCCAppCommLauncherUrlGroup )); 
            break;            
        case VPbkFieldTypeSelectorFactory::EVideoCallSelector:        
            isMatch = FieldArrayAndContactActionMatch( 
                aContactField, KCCAppCommLauncherVideoCallGroup, 
                CCACOMMLAUNCHER_ROMARRAY_SIZE( KCCAppCommLauncherVideoCallGroup )); 
            break;
        case VPbkFieldTypeSelectorFactory::EFindOnMapSelector:
            isMatch = FieldArrayAndContactActionMatch( 
                aContactField, KCCAppCommLauncherAddressGroup, 
                CCACOMMLAUNCHER_ROMARRAY_SIZE(
                		KCCAppCommLauncherAddressGroup )); 
            break;
        default:
            break;
        }

    CCA_DP(KCommLauncherLogFile, CCA_L("::ContactFieldTypeAndContactActionMatch() - isMatch: %d"), isMatch );    
    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherContactHandler::ContactFieldTypeAndContactActionMatch()"));    
    return isMatch;
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::FieldArrayAndContactActionMatch
// --------------------------------------------------------------------------
//
TBool CCCAppCommLauncherContactHandler::FieldArrayAndContactActionMatch(
    CCmsContactFieldItem::TCmsContactField aContactField,
    const TInt aFieldArray[], const TInt aCount )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("CCCAppCommLauncherContactHandler::FieldArrayAndContactActionMatch()"));    
    
    for( TInt i = 0;i < aCount;i++ )
        {
        if ( aContactField == aFieldArray[i] )
            return ETrue;
        }

    return EFalse;
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::ConstructDynamicFieldArrayIfNeeded
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherContactHandler::ConstructDynamicFieldArrayIfNeeded(
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherContactHandler::ConstructDynamicFieldArrayIfNeededL()"));    
    
    switch( aContactAction )
        {
        case VPbkFieldTypeSelectorFactory::EVOIPCallSelector: 
            {
            if ( !iDynamicAddressGroupSet )
                {
                CreateDynamicVoipAddressGroup( IsServiceAvailable( aContactAction ));
                iDynamicAddressGroupSet = ETrue;
                }
            }
            break;
        default:
            break;
        }

    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherContactHandler::ConstructDynamicFieldArrayIfNeededL()"));    
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::CreateDynamicVoipAddressGroup
// --------------------------------------------------------------------------
// 
void CCCAppCommLauncherContactHandler::CreateDynamicVoipAddressGroup(
    const TInt aVoipSupportFlag )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherContactHandler::CreateDynamicVoipAddressGroup()"));    
    CCA_DP(KCommLauncherLogFile, CCA_L("::CreateDynamicVoipAddressGroup() - aVoipSupportFlag: %d"), aVoipSupportFlag );    
   
    iDynamicVoipAddressGroup.Reset();
    if ( ECmsVoIPSupportBasic & aVoipSupportFlag )
        {
        iDynamicVoipAddressGroup.Append( CCmsContactFieldItem::ECmsVoipNumberGeneric );
        iDynamicVoipAddressGroup.Append( CCmsContactFieldItem::ECmsVoipNumberHome );
        iDynamicVoipAddressGroup.Append( CCmsContactFieldItem::ECmsVoipNumberWork );
        iDynamicVoipAddressGroup.Append( CCmsContactFieldItem::ECmsPushToTalk );
        iDynamicVoipAddressGroup.Append( CCmsContactFieldItem::ECmsShareView );
        iDynamicVoipAddressGroup.Append( CCmsContactFieldItem::ECmsSipAddress );
        }
    if ( ECmsVoIPSupportXspId & aVoipSupportFlag )
        {
        iDynamicVoipAddressGroup.Append( CCmsContactFieldItem::ECmsImpp );
        }
    if ( ECmsVoIPSupportCallout & aVoipSupportFlag )
        {
        iDynamicVoipAddressGroup.Append( CCmsContactFieldItem::ECmsMobilePhoneGeneric );
        iDynamicVoipAddressGroup.Append( CCmsContactFieldItem::ECmsMobilePhoneHome );
        iDynamicVoipAddressGroup.Append( CCmsContactFieldItem::ECmsMobilePhoneWork );
        iDynamicVoipAddressGroup.Append( CCmsContactFieldItem::ECmsLandPhoneGeneric );
        iDynamicVoipAddressGroup.Append( CCmsContactFieldItem::ECmsLandPhoneHome );
        iDynamicVoipAddressGroup.Append( CCmsContactFieldItem::ECmsLandPhoneWork );
        iDynamicVoipAddressGroup.Append( CCmsContactFieldItem::ECmsAssistantNumber );
        iDynamicVoipAddressGroup.Append( CCmsContactFieldItem::ECmsCarPhone );                    
        }
    
    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherContactHandler::CreateDynamicVoipAddressGroup()"));    
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::IsItNumberAddress
// --------------------------------------------------------------------------
// 
TBool CCCAppCommLauncherContactHandler::IsItNumberAddress(
     VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction )
    {
    if ( VPbkFieldTypeSelectorFactory::EVoiceCallSelector == aContactAction
        || VPbkFieldTypeSelectorFactory::EUniEditorSelector == aContactAction
        || VPbkFieldTypeSelectorFactory::EVOIPCallSelector == aContactAction
        || VPbkFieldTypeSelectorFactory::EVideoCallSelector == aContactAction )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::IsItNumberAddress
// --------------------------------------------------------------------------
// 
void CCCAppCommLauncherContactHandler::RefetchContactL()
    {
    iCmsWrapper->RefetchContactL();
    iCmsWrapper->AddObserverL( *this );
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherContactHandler::IsTopContact
// --------------------------------------------------------------------------
// 
TBool CCCAppCommLauncherContactHandler::IsTopContact()
    {
    return iCmsWrapper->IsTopContact();
    }

// End of file
