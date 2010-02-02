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
* Description:  The virtual phonebook contact id converter
*
*/


#include "CContactIdConverter.h"
#include "CContactLink.h"
#include "CContactStore.h"
#include "CContact.h"

#include <CVPbkContactManager.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactStoreList.h>
#include <VPbkStoreUriLiterals.h>
#include <MVPbkContactLink.h>
#include <MVPbkStoreContactField.h>

#include <cntdef.h>

// From Phonebook
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>

namespace VPbkCntModel {

_LIT( KCntDbURIDomain, "cntdb" );

CContactIdConverter::CContactIdConverter( MVPbkContactStore& aContactStore ) :
    iContactStore( static_cast<CContactStore&>(aContactStore) )
    {
    }

CContactIdConverter* CContactIdConverter::NewL( TAny* aParam )
    {
    MVPbkContactStore& store = *static_cast<MVPbkContactStore*>( aParam );
    
    if ( store.StoreProperties().Uri().Compare(
            KCntDbURIDomain, 
            TVPbkContactStoreUriPtr::EContactStoreUriStoreType) != 0 )
        {
        User::Leave( KErrNotSupported );
        }
    
    CContactIdConverter* self = new( ELeave ) CContactIdConverter( store );    
    return self; 
    }
    
CContactIdConverter::~CContactIdConverter()
    {
    delete iPbkEngine;
    }
    
TInt32 CContactIdConverter::LinkToIdentifier( const MVPbkContactLink& aLink ) const
    {
    TInt32 result = KNullContactId;
    if ( &aLink.ContactStore() == &iContactStore )
        {
        const CContactLink& link = static_cast<const CContactLink&>( aLink );
        result = link.ContactId();
        }
    return result;
    }
    
MVPbkContactLink* CContactIdConverter::IdentifierToLinkLC( TInt32 aIdentifier ) const
    {
    return CContactLink::NewLC( iContactStore, aIdentifier );
    }
    
TInt CContactIdConverter::PbkEngFieldIndexL( 
            const MVPbkStoreContactField& aField ) const
    {
    TInt result = KErrNotFound;
    if ( &aField.ContactStore() == &iContactStore )
        {       
        const TContactField& field = static_cast<const TContactField&>( aField );
        CContact& contact = static_cast<CContact&>( aField.ParentContact() );

        CPbkContactItem* ci = PbkEngineL()->ReadContactLC( 
                                    contact.NativeContact()->Id() );
        
        CPbkFieldArray& fields = ci->CardFields();
        const TInt fieldCount = fields.Count();
        for ( TInt i = 0; i < fieldCount; ++i )
            {
            if ( fields[i].ItemField().Id() == field.NativeField()->Id() )
                {
                result = i;
                break;
                }
            }
        
        CleanupStack::PopAndDestroy(); // ci
        }
    return result;
    }
    
CPbkContactItem* CContactIdConverter::LinkToPbkContactItemLC(
                         const MVPbkContactLink& aLink ) const
    {
    CPbkContactItem* contact = NULL;
    TInt32 linkId( LinkToIdentifier(aLink) );
    if ( linkId != KNullContactId )
        {
        contact = PbkEngineL()->ReadContactLC( linkId );
        }
    return contact;
    }    
    
CPbkContactItem* CContactIdConverter::LinkToOpenPbkContactItemLCX(
                                    const MVPbkContactLink& aLink ) const
    {
    CPbkContactItem* contact = NULL;
    TInt32 linkId( LinkToIdentifier(aLink) );
    if ( linkId != KNullContactId )
        {       
        contact = PbkEngineL()->OpenContactLCX( linkId );
        }
    return contact;    
    }
    
void CContactIdConverter::CommitOpenContactItemL( CPbkContactItem& aContactItem )
    {
    PbkEngineL()->CommitContactL( aContactItem );
    }
    
CPbkContactEngine* CContactIdConverter::PbkEngineL() const 
    {
    if (!iPbkEngine)
        {
        iPbkEngine = CPbkContactEngine::NewL();
        }
    return iPbkEngine;
    }

} // namespace VPbkCntModel

// End of File
