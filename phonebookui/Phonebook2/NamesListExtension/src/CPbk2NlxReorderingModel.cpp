/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 reordering model.
*
*/


// INCLUDE FILES
#include "CPbk2NlxReorderingModel.h"

#include <CPbk2ContactIconsUtils.h>

// Virtual Phonebook
#include <CVPbkContactLinkArray.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactViewBase.h>
#include <MVPbkContactLink.h>
#include <MPbk2ContactNameFormatter.h>
#include <MVPbkViewContact.h>
#include <Pbk2UIControls.rsg>
#include <CPbk2IconArray.h>
#include <MPbk2ContactUiControlExtension.h>
#include <MPbk2ContactUiControlExtension2.h>
#include <MPbk2UIExtensionFactory.h>
#include <CPbk2UIExtensionManager.h>

namespace
	{
_LIT(KListItemFormat, "\t%S");
_LIT(KListItemFormatIcon, "%d\t%S");	

const TInt KDefaultListFormatting =
        MPbk2ContactNameFormatter::EUseSeparator |
        MPbk2ContactNameFormatter::EPreserveLeadingSpaces;

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_IconArrayNotOurs = 1
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2NlxReorderingModel");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG
	}

// --------------------------------------------------------------------------
// CPbk2NlxReorderingModel::CPbk2NlxReorderingModel
// --------------------------------------------------------------------------
CPbk2NlxReorderingModel::CPbk2NlxReorderingModel(
		const MVPbkContactViewBase& aView,
		MPbk2ContactNameFormatter& aFormatter ):
		iView(aView),
		iFormatter(aFormatter),
		iIconsOwnership(ETrue)
	{
	}

// --------------------------------------------------------------------------
// CPbk2NlxReorderingModel::~CPbk2NlxReorderingModel
// --------------------------------------------------------------------------
CPbk2NlxReorderingModel::~CPbk2NlxReorderingModel()
	{
	delete iContactIcons;
	delete iUiExtension;
	if ( iIconsOwnership )
	    {
	    delete iIcons;
	    }
	iReorderingMap.Close();
	delete iFocusedItem;
	}

// --------------------------------------------------------------------------
// CPbk2NlxReorderingModel::NewL
// --------------------------------------------------------------------------
CPbk2NlxReorderingModel* CPbk2NlxReorderingModel::NewL(
        CVPbkContactManager& aContactManager,
        const MVPbkContactViewBase& aView,
        MPbk2ContactNameFormatter& aFormatter,
        CPbk2StorePropertyArray& aStoreProperties )
	{
	CPbk2NlxReorderingModel* self = new(ELeave) CPbk2NlxReorderingModel(
			aView, aFormatter);
	CleanupStack::PushL(self);
	self->ConstructL( aContactManager, aStoreProperties );
	CleanupStack::Pop(self);
	return self;
	}

// --------------------------------------------------------------------------
// CPbk2NlxReorderingModel::ConstructL
// --------------------------------------------------------------------------
void CPbk2NlxReorderingModel::ConstructL(
        CVPbkContactManager& aContactManager,
        CPbk2StorePropertyArray& aStoreProperties )
	{
	iIcons = CPbk2ContactIconsUtils::CreateListboxIconArrayL(R_PBK2_CONTACT_LIST_ICONS);
	
    CPbk2UIExtensionManager* extManager =
        CPbk2UIExtensionManager::InstanceL();
    extManager->PushL();

    iUiExtension = 
        extManager->FactoryL()->CreatePbk2UiControlExtensionL( aContactManager );
    CleanupStack::PopAndDestroy(); // extManager
    
    if ( iUiExtension )
        {
        MPbk2ContactUiControlExtension2* ext = reinterpret_cast
            <MPbk2ContactUiControlExtension2*>( iUiExtension->
                ContactUiControlExtensionExtension(
                    KMPbk2ContactUiControlExtensionExtension2Uid ) );
        if ( ext != NULL )
            {
            CPbk2IconArray* extIconArray = const_cast<CPbk2IconArray*>(iIcons);
            ext->SetIconArray( *extIconArray );
            }
        }

	iContactIcons = CPbk2ContactIconsUtils::NewL(
	        aStoreProperties,
	        iUiExtension );

	//init mapping array
	for ( TInt i = 0; i < iView.ContactCountL(); i++ )
		{
		User::LeaveIfError( iReorderingMap.Append( i ) );
		}
	}

// --------------------------------------------------------------------------
// CPbk2NlxReorderingModel::Move
// --------------------------------------------------------------------------
void CPbk2NlxReorderingModel::Move(TInt aFrom, TInt aTo)
	{
	if ( aFrom > aTo )
		{
		iReorderingMap.Insert( iReorderingMap[aFrom], aTo);
		iReorderingMap.Remove( aFrom + 1 );
		}
	else if ( aTo > aFrom )
		{
		iReorderingMap.Insert( iReorderingMap[aFrom], aTo + 1 );
		iReorderingMap.Remove( aFrom );
		}
	}

// --------------------------------------------------------------------------
// CPbk2NlxReorderingModel::MdcaCount
// --------------------------------------------------------------------------
TInt CPbk2NlxReorderingModel::MdcaCount() const
	{
	return iReorderingMap.Count();
	}

// --------------------------------------------------------------------------
// Formats contact item for listbox.
// --------------------------------------------------------------------------
inline void CPbk2NlxReorderingModel::FormatItemL(TDes& aBuf, TInt aIndex) const
	{
	RArray<TPbk2IconId> ids;
	CleanupClosePushL( ids );
	iContactIcons->GetIconIdsForContactL( iView.ContactAtL(aIndex), ids );
	
	HBufC* title = iFormatter.GetContactTitleL(
			iView.ContactAtL(aIndex).Fields(),
			KDefaultListFormatting );
	CleanupStack::PushL(title);
	TInt iconIndex = KErrNotFound;
	if ( ids.Count() > 0 )
	    {
	    iconIndex = iIcons->FindIcon( ids[0] );
	    }
	if ( iconIndex == KErrNotFound)
	    {
	    aBuf.Format(KListItemFormat, title);
	    }
	else
	    {
	    aBuf.Format(KListItemFormatIcon, iconIndex, title);
	    }
	CleanupStack::PopAndDestroy(title);
	CleanupStack::PopAndDestroy(); //ids
	}

// --------------------------------------------------------------------------
// Backup formatting for error cases.
// --------------------------------------------------------------------------
inline void CPbk2NlxReorderingModel::FormatEmptyItem(TDes& aBuf ) const
	{
	TPtrC emptyTitle( KNullDesC );
	aBuf.Format(KListItemFormat, &emptyTitle);
	}

// --------------------------------------------------------------------------
// CPbk2NlxReorderingModel::MdcaPoint
// --------------------------------------------------------------------------
TPtrC CPbk2NlxReorderingModel::MdcaPoint(TInt aIndex) const
	{
	//remove const from iBuffer to be able to format in const method
	TRAPD(err, FormatItemL( const_cast< TBuf<EMaxListBoxText>& >( iBuffer ),
						   iReorderingMap[aIndex]));
	if ( err != KErrNone )
		{
		FormatEmptyItem( const_cast< TBuf<EMaxListBoxText>& >( iBuffer ) );		
		}
	return iBuffer;
	}

// --------------------------------------------------------------------------
// CPbk2NlxReorderingModel::FocusedContactIndex
// --------------------------------------------------------------------------
TInt CPbk2NlxReorderingModel::FocusedContactIndex() const
	{
	TInt result = KErrNotFound;
	if ( iFocusedItem )
		{
		result = ContactIndex( *iFocusedItem );
		}
	return result;
	}

// --------------------------------------------------------------------------
// CPbk2NlxReorderingModel::ContactIndex
// --------------------------------------------------------------------------
TInt CPbk2NlxReorderingModel::ContactIndex(
        const MVPbkContactLink& aContact ) const
    {
    TInt result = KErrNotFound;
    TInt viewIndex = KErrNotFound;
    TRAP_IGNORE( viewIndex = iView.IndexOfLinkL( aContact ) );
    if ( viewIndex >= 0 )
        {
        result = iReorderingMap.Find( viewIndex );
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2NlxReorderingModel::FocusedItem
// --------------------------------------------------------------------------
void CPbk2NlxReorderingModel::SetFocusedContactL(
		const MVPbkContactLink& aFocusedLink )
	{
	delete iFocusedItem;
	iFocusedItem = NULL;
	iFocusedItem = aFocusedLink.CloneLC();
	CleanupStack::Pop();
	}

// --------------------------------------------------------------------------
// CPbk2NlxReorderingModel::FocusedItem
// --------------------------------------------------------------------------
void CPbk2NlxReorderingModel::SetFocusedContactL( TInt aFocusedIndex )
	{
	delete iFocusedItem;
	iFocusedItem = NULL;
	iFocusedItem = iView.CreateLinkLC( iReorderingMap[aFocusedIndex] );
	CleanupStack::Pop();
	}

// --------------------------------------------------------------------------
// CPbk2NlxReorderingModel::FocusedItem
// --------------------------------------------------------------------------
MVPbkContactLink*  CPbk2NlxReorderingModel::FocusedContactLinkL() const
	{
	MVPbkContactLink* result = NULL;
	if ( iFocusedItem )
	    {
	    result = iFocusedItem->CloneLC();
	    CleanupStack::Pop();
	    }
	return result;
	}

// --------------------------------------------------------------------------
// CPbk2NlxReorderingModel::ReorderedLinksLC
// --------------------------------------------------------------------------
MVPbkContactLinkArray* CPbk2NlxReorderingModel::ReorderedLinksLC() const
	{
	CVPbkContactLinkArray* result = CVPbkContactLinkArray::NewLC();
	for( TInt i = 0; i < iReorderingMap.Count(); i++ )
		{
		MVPbkContactLink* link = iView.CreateLinkLC( iReorderingMap[i] );
		result->AppendL( link );
		CleanupStack::Pop();
		}
	return result;
	}

CPbk2IconArray* CPbk2NlxReorderingModel::TakeIconArray()
    {
    __ASSERT_DEBUG( iIconsOwnership, Panic(EPanicPreCond_IconArrayNotOurs));
    iIconsOwnership = EFalse;
    return iIcons;
    }
// --------------------------------------------------------------------------
// CPbk2NlxReorderingModel::SetContactUpdater
// --------------------------------------------------------------------------
void CPbk2NlxReorderingModel::SetContactUpdater(
        MPbk2ContactUiControlUpdate* aContactUpdator )
    {
    if ( iUiExtension )
        {
        iUiExtension->SetContactUiControlUpdate( aContactUpdator );
        }
    }
// End of File
