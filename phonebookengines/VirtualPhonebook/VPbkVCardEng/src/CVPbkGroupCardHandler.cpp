/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handler to manage groupcard property during import and export
 *
*/


#include "CVPbkGroupCardHandler.h"

#include <MVPbkContactStore.h>
#include <MVPbkContactGroup.h>
#include <MVPbkContactStore2.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactStoreProperties2.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkContactManager.h>
#include <CVPbkContactIdConverter.h>
#include <MVPbkStoreContact2.h>

_LIT8(KGroup,"X-CATEGORIES");
_LIT16(KSemiColon,";");    
_LIT16(KVersitTokencrlf,"\r\n");

// ----------------------------------------------------------------------------
// CVPbkGroupCardHandler::CVPbkGroupCardHandler
// ----------------------------------------------------------------------------
CVPbkGroupCardHandler::CVPbkGroupCardHandler(CVPbkVCardData& aData):iData(aData)
            {
            }

// ----------------------------------------------------------------------------
// CVPbkGroupCardHandler::NewL
// ----------------------------------------------------------------------------
CVPbkGroupCardHandler* CVPbkGroupCardHandler::NewL(CVPbkVCardData& aData)
    {
    CVPbkGroupCardHandler* self = new (ELeave) CVPbkGroupCardHandler(aData);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CVPbkGroupCardHandler::ConstructL
// ----------------------------------------------------------------------------
void CVPbkGroupCardHandler::ConstructL()
    {
    iWait = new( ELeave )CActiveSchedulerWait();
    iContactsGroupsMap = NULL;
    iTargetStore = NULL;
    iStore = NULL;
    }

// ----------------------------------------------------------------------------
// CVPbkGroupCardHandler::~CVPbkGroupCardHandler
// ----------------------------------------------------------------------------
CVPbkGroupCardHandler::~CVPbkGroupCardHandler()
    {
    // The code that started iWait must know if this instance has been deleted.
    if ( iDestroyed )
        {
        *iDestroyed = ETrue;
        }
    
    if ( iSelfPtr )
        {
        *iSelfPtr = NULL;
        }
    
    delete iRetrieveOp;
    
    if ( iWait )
        {
        if( iWait->IsStarted() )
            iWait->AsyncStop();
        delete iWait;
        iWait = NULL;
        }

    if(iStore)
        {
        delete iStore;
        iStore = NULL;
        }

    if (iContactsGroupsMap)
        {   
        iContactsGroupsMap->ResetAndDestroy();
        iContactsGroupsMap->Close();
        delete iContactsGroupsMap;
        }
    }

// ----------------------------------------------------------------------------
// CVPbkGroupCardHandler::CreateXSelfPropertyL
// ----------------------------------------------------------------------------
CParserProperty* CVPbkGroupCardHandler::CreateXGroupPropertyL(const MVPbkStoreContact* aStore)
    {
    TPtrC8 text( KGroup);
    CParserProperty* property = NULL;
    CVPbkVCardParserParamArray* params = NULL;
    
    if ( aStore == NULL)
        return property;

    MVPbkContactLinkArray* groupIdArray = aStore->GroupsJoinedLC(); //Get Groupid list of contact.

    if ( groupIdArray == NULL)
        return property;

    TInt totalGroupsInContact = 0;
    totalGroupsInContact = groupIdArray->Count();

    if (totalGroupsInContact == 0)
        {
        //if contact has no groups,Append X-CATEGORIES field with NULL field Value. 
        CParserPropertyValueHBufC* tempValue = CParserPropertyValueHBufC::NewL( KNullDesC() );
        CParserPropertyValue* value = static_cast<CParserPropertyValue*>(tempValue);
        property = CParserProperty::NewL( *value,text, params);
        CleanupStack::PopAndDestroy();  //For GoupIdArray
        return property;
        }

    CDesCArrayFlat* desArray=new (ELeave)CDesCArrayFlat(totalGroupsInContact);
    CleanupStack::PushL(desArray);

    //Append all the Group Names of contact to X-CATEGORIES field value.
    TBool thisDestroyed = EFalse;
    for ( TInt m = 0 ; m < totalGroupsInContact && !thisDestroyed ; m++ )
        {                  
        const  MVPbkContactLink& groupLink = (*groupIdArray)[m];
        thisDestroyed = RetrieveContactL( groupLink, *this );
        if (!thisDestroyed && iTargetStore)
            {
            MVPbkContactGroup* contactGrp = iTargetStore->Group();
            if(contactGrp)
                {
                CParserPropertyValueHBufC* tempValue = CParserPropertyValueHBufC::NewL( contactGrp->GroupLabel() );
                CleanupStack::PushL(tempValue);
                desArray->AppendL(tempValue->Value());
                CleanupStack::PopAndDestroy(tempValue);
                }
            delete iTargetStore;
            iTargetStore = NULL;
            }
        }

    CParserPropertyValue* value = new (ELeave) CParserPropertyValueCDesCArray(desArray);
    CleanupStack::PushL(value);

    property = CParserProperty::NewL( *value, text, params); //Create the Property with field and Value

    CleanupStack::Pop(value);
    CleanupStack::Pop(desArray);
    CleanupStack::PopAndDestroy();  //For GoupIdArray

    return property;
    }

// ----------------------------------------------------------------------------
// CVPbkGroupCardHandler::DeleteContactFromGroupsL
// ----------------------------------------------------------------------------
void CVPbkGroupCardHandler::DeleteContactFromGroupsL()
    {
    if(iStore == NULL)
        return;
    //Get Groupid list a contact belongs
    MVPbkContactLinkArray* groupIds = iStore->GroupsJoinedLC(); 

    if(groupIds == NULL)
        return; 

    TInt groupIdsCount = 0;
    groupIdsCount = groupIds->Count();    
    const MVPbkContactLink* contactLink = iStore->CreateLinkLC();

    //Remove the contact from all the Groups it belongs.
    TBool thisDestroyed = EFalse;
    for(TInt i = 0;i < groupIdsCount && !thisDestroyed;i++)
        {
        const MVPbkContactLink& groupLink = (*groupIds)[i];
        thisDestroyed = RetrieveContactL( groupLink, *this );
        if (!thisDestroyed && iTargetStore)
            { 
            MVPbkContactGroup* contactGrp = iTargetStore->Group();
            if(contactGrp)
                {   
                iTargetStore->LockL(*this); //Lock the contact before editing the contact.
                //wait for LockL operation to complete
                if( ! (iWait->IsStarted()) )
                    {
                    iWait->Start();
                    }

                contactGrp->RemoveContactL(*contactLink); //Removes a contact from a Group

                iTargetStore->CommitL(*this);  //save the Changes to VPBK.
                //wait for CommitL operation to complete
                if( ! (iWait->IsStarted()) )
                    {
                    iWait->Start();
                    }
                }  

            delete iTargetStore;
            iTargetStore = NULL;
            }
        }

    CleanupStack::PopAndDestroy();  // for contactLink
    CleanupStack::PopAndDestroy(); // for groupIds
    }

// ----------------------------------------------------------------------------
// CVPbkGroupCardHandler::CreateNewGroupFromContactL
// ----------------------------------------------------------------------------
void CVPbkGroupCardHandler::CreateNewGroupFromContactL(TPtr16& aGroupLabel)
    {
    if(iStore == NULL)
        return;
    //Creates New Group 
    MVPbkContactGroup* groupItem = iStore->ParentStore().CreateNewContactGroupLC(); 

    if(groupItem)
        {                    
        groupItem->SetGroupLabelL(aGroupLabel); // set the group name.
        
        //Retrieve group store
        const MVPbkContactLink* grpContactLink = groupItem->CreateLinkLC();
        TBool thisDestroyed = RetrieveContactL( *grpContactLink, *this );
        CleanupStack::PopAndDestroy();// to clean grpContactLink

        if (!thisDestroyed && iTargetStore)
            {
            const MVPbkContactLink* contactLink = iStore->CreateLinkLC();
            groupItem->AddContactL(*contactLink);
            CleanupStack::PopAndDestroy(); //For aContactLink
            groupItem->CommitL(*this); 
            if( ! (iWait->IsStarted()) )
                {
                iWait->Start();
                }//Commit to VPBK for new grp changes.

            //Add the New Group to Hash Table
            TPtrC16 contactGroupName;
            contactGroupName.Set( aGroupLabel );
            HBufC16* grpName = contactGroupName.AllocLC();

            if(iContactsGroupsMap == NULL)
                {
                iContactsGroupsMap = new(ELeave) RPtrHashMap<TDesC16, MVPbkStoreContact>();
                }
            // iContactsGroupsMap takes the ownership of iTargetStore
            TInt res = iContactsGroupsMap->Insert(grpName, iTargetStore);
            if ( res != KErrNone )
                {
                delete iTargetStore;
                }
            iTargetStore = NULL;
            CleanupStack::Pop(); // To pop up grpName
            }
        }   

    CleanupStack::PopAndDestroy(); // To pop up and destroy groupItem  
    }

// ----------------------------------------------------------------------------
// CVPbkGroupCardHandler::DecodeContactGroupInVCardL
// ----------------------------------------------------------------------------
void CVPbkGroupCardHandler::DecodeContactGroupInVCardL(TPtr16 aValue,
        CVPbkGroupCardHandler** aSelfPointer )
    {        
    if(iStore == NULL)
        return;
    iSelfPtr = aSelfPointer;
    const MVPbkContactLink* contactLink = iStore->CreateLinkLC();

    TInt newLinePos = aValue.Find(KVersitTokencrlf());

    if (newLinePos != KErrNotFound)
        aValue = aValue.Left(newLinePos); //remove CRLF from aValue

    TInt semiColonPos = 0;
    TBool endGrpValue = EFalse;    

    DeleteContactFromGroupsL(); //Delete the contact from the Groups it belongs

    TBool destroyed = EFalse;
    while(!endGrpValue && !destroyed)
        {        
        semiColonPos = aValue.Find( KSemiColon() );
        if(semiColonPos == KErrNotFound)
            {
            semiColonPos = aValue.Length();
            endGrpValue = ETrue;  // all the Group card values are parsed
            }

        if(semiColonPos > 0)
            {          
            //Get each group card value to save OR add to Groups
            TPtrC16 desGrpValue = aValue.Left(semiColonPos); 
            HBufC16* unicodeGrpValue = desGrpValue.Alloc();
            CleanupStack::PushL( unicodeGrpValue );
            TPtr16 desGrpValueHash = unicodeGrpValue->Des();

            MVPbkContactGroup* groupContactItemId = NULL;
            MVPbkStoreContact* storeContact = NULL;
            if(iContactsGroupsMap)
                {
                // Find whether Group already present in VPBK, to get group store
                storeContact = iContactsGroupsMap->Find(desGrpValueHash);
                if (storeContact)
                    groupContactItemId = storeContact->Group();
                }

            // if group already present add the contact to existing group otherwise 
            // add the contact to new group 
            if(groupContactItemId != NULL)
                { 
                storeContact->LockL(*this);
                iDestroyed = &destroyed;
                //wait for Lock operation to complete
                if( ! (iWait->IsStarted()) )
                    {
                    iWait->Start();
                    }
                if ( !destroyed )
                    {
                    iDestroyed = NULL;
                    groupContactItemId->AddContactL(*contactLink);
                    storeContact->CommitL(*this);  // save Group modifications.
                    iDestroyed = &destroyed;
                    if( ! (iWait->IsStarted()) )
                        {
                        iWait->Start();
                        }
                    if ( !destroyed )
                        {
                        iDestroyed = NULL;
                        }
                    }
                }
            else
                {                                
                CreateNewGroupFromContactL(desGrpValueHash);
                }// End of if(err = KErrNone)

            CleanupStack::PopAndDestroy(unicodeGrpValue);
            }// End of if(semiColonPos > 0)

        //Get Next Group card Value
        if(!endGrpValue)
            aValue = aValue.Right( aValue.Length() - semiColonPos - KSemiColon().Length());
        } //End Of While loop

    CleanupStack::PopAndDestroy(); // For contactLink 

    if(!destroyed)
        {
        iSelfPtr = NULL;
        delete iStore;
        iStore = NULL;
        }

    }
// ----------------------------------------------------------------------------
// CVPbkGroupCardHandler::BuildContactGroupsHashMapL
// ----------------------------------------------------------------------------
void CVPbkGroupCardHandler::BuildContactGroupsHashMapL(
        MVPbkContactStore& aTargetContactStore, 
        CVPbkGroupCardHandler** aSelfPointer)
    {
    //Get all the Group Links in current VPBK
    MVPbkContactLinkArray* groupLinks = aTargetContactStore.ContactGroupsLC();

    if( (groupLinks == NULL || iContactsGroupsMap != NULL))
        return;

    iSelfPtr = aSelfPointer;
    
    iContactsGroupsMap = new(ELeave) RPtrHashMap<TDesC16, MVPbkStoreContact>();

    TInt groupCount = 0;
    groupCount = groupLinks->Count();

    // Get Group Name and Group store, store these values into Hash Table.
    TBool thisDestroyed = EFalse;
    for(TInt i = 0;i < groupCount && !thisDestroyed;i++)
        {
        const MVPbkContactLink& groupLink = (*groupLinks)[i];
        thisDestroyed = RetrieveContactL( groupLink, *this );
        if (!thisDestroyed && iTargetStore)
            {
            MVPbkContactGroup* contactGrp = iTargetStore->Group();
            if(contactGrp)
                {   
                TPtrC16 contactGroupName;
                contactGroupName.Set( contactGrp->GroupLabel());
                HBufC16* grpName = contactGroupName.AllocLC();
                // iContactsGroupsMap takes the ownership of iTargetStore
                TInt res = iContactsGroupsMap->Insert(grpName, iTargetStore);
                if ( res != KErrNone )
                    {
                    delete iTargetStore;
                    }
                iTargetStore = NULL;
                CleanupStack::Pop();  //grpName
                }       
            }    
        }// End of for loop

    CleanupStack::PopAndDestroy(); //for groupLinks
    
    if ( !thisDestroyed )
        {
        iSelfPtr = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CVPbkGroupCardHandler::RetrieveContactL
// ----------------------------------------------------------------------------
TBool CVPbkGroupCardHandler::RetrieveContactL( const MVPbkContactLink& aLink,
            MVPbkSingleContactOperationObserver& aObserver)
    {
    delete iTargetStore;
    iTargetStore = NULL;
    iRetrieveOp = iData.GetContactManager().RetrieveContactL( aLink, aObserver );
    
    TBool destroyed = EFalse;
    // Set iDestroyed to point to stack variable that is accessed from
    // destructor.
    iDestroyed = &destroyed; 
    //wait for GetContactGroupStoreL operation to complete,to get storecontact
    if( ! (iWait->IsStarted()) )
        {
        iWait->Start();
        }
    
    if ( !destroyed )
        {
        // Reset pointer to stack variable.
        iDestroyed = NULL;
        delete iRetrieveOp;
        iRetrieveOp = NULL;
        }
    return destroyed; 
    }

// ----------------------------------------------------------------------------
// CVPbkGroupCardHandler::VPbkSingleContactOperationComplete
// ----------------------------------------------------------------------------
void CVPbkGroupCardHandler::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkStoreContact* aContact)
    {
    iTargetStore = aContact;

    if( iWait->IsStarted() )
        iWait->AsyncStop();

    }

// ----------------------------------------------------------------------------
// CVPbkGroupCardHandler::VPbkSingleContactOperationFailed
// ----------------------------------------------------------------------------
void CVPbkGroupCardHandler::VPbkSingleContactOperationFailed
(MVPbkContactOperationBase& /*aOperation*/, TInt /*aError*/)
    {
    if( iWait->IsStarted() )
        iWait->AsyncStop();
    }

// ----------------------------------------------------------------------------
// CVPbkGroupCardHandler::GetContactGroupStoreL
// Retrieve the contact store of group contact
// ----------------------------------------------------------------------------
void CVPbkGroupCardHandler::GetContactGroupStoreL(
        const MVPbkContactLink& aContactLink, 
        CVPbkGroupCardHandler** aSelfPointer)
    {
    iSelfPtr = aSelfPointer;
    delete iStore;
    iStore = NULL;
    if ( !RetrieveContactL( aContactLink, *this ) )
        {
        iSelfPtr = NULL;
        // iStore takes the ownership.
        iStore = iTargetStore;
        iTargetStore = NULL;
        }        
    }

// ----------------------------------------------------------------------------
// CVPbkGroupCardHandler::ContactOperationCompleted
// ----------------------------------------------------------------------------
void CVPbkGroupCardHandler::ContactOperationCompleted(TContactOpResult aResult)
    {
    delete aResult.iStoreContact;
    if( iWait->IsStarted() )
        iWait->AsyncStop();
    }

// ----------------------------------------------------------------------------
// CVPbkGroupCardHandler::ContactOperationFailed
// ----------------------------------------------------------------------------
void CVPbkGroupCardHandler::ContactOperationFailed
(TContactOp /*aOpCode*/, TInt /*aErrorCode*/, TBool /*aErrorNotified*/)
    {
    if( iWait->IsStarted() )
        iWait->AsyncStop();
    }
