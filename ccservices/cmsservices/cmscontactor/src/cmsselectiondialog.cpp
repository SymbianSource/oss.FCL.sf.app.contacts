/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Dialog for selecting wanted field. 
 *
*/


// System includes
#include <avkon.rsg>
#include <aknPopup.h>
#include <aknlists.h>
#include <eikclbd.h>
#include <barsread.h>

#include "cmscontactorheaders.h"

#include "cmscontactfielditem.h"

/// Unnamed namespace for local definitions
namespace
	{

#ifdef _DEBUG
	enum TPanicCode
		{
		EPanicPreCond_ExecuteLD = 1
		};

	void Panic(TPanicCode aReason)
		{
		_LIT(KPanicText, "CCMSSelectionDialog");
		User::Panic (KPanicText, aReason);
		}
#endif // _DEBUG

	} /// namespace


/**
 * PopupList for possible specialized cases.
 */
NONSHARABLE_CLASS( CCMSSelectionDialog::CPopupList ) :
public CAknPopupList
	{
public: // Interface

	/**
	 * Like CAknPopupList::NewL.
	 * @see CAknPopupList::NewL
	 * @since S60 v5.0
	 */
	static CPopupList* NewL(
			CEikListBox* aListBox,
			TInt aCbaResource,
			AknPopupLayouts::TAknPopupLayouts aType =
			AknPopupLayouts::EMenuWindow );
    
	/**
	 * Like CAknPopupList::AttemptExitL.
	 * @see CAknPopupList::AttemptExitL
	 * @since S60 v5.0
	 */
	inline void AttemptExitL(
			TBool aAccept );


private: // From CAknPopupList
	void SetupWindowLayout(
			AknPopupLayouts::TAknPopupLayouts aType );


private: // Implementation
	inline CPopupList();

private: // Data
	/**
	 * Focus index.
	 * Own.  
	 */
	TInt iFocusIndex;
	};

/**
 * Listbox class for possible specialized cases.
 */
NONSHARABLE_CLASS( CCMSSelectionDialog::CListBox ) :
//public CAknDoubleGraphicPopupMenuStyleListBox
public CAknDouble2PopupMenuStyleListBox
	{
public: // Interface

	/**
	 * Constructor.
	 */
	inline CListBox();
	
	/**
	 * From CAknDouble2PopupMenuStyleListBox
	 * overridden to handle send key
	 */
	TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType);
	
	};

// ================= MEMBER FUNCTIONS =======================
//


// --------------------------------------------------------------------------
// CCMSSelectionDialog::CListBox::CPopupList
// --------------------------------------------------------------------------
//
inline CCMSSelectionDialog::CPopupList::CPopupList():
	iFocusIndex( KErrNotFound)
	{
	}

// --------------------------------------------------------------------------
// CCMSSelectionDialog::CListBox::NewL
// --------------------------------------------------------------------------
//
CCMSSelectionDialog::CPopupList* CCMSSelectionDialog::CPopupList::NewL(
		CEikListBox* aListBox, TInt aCbaResource,
		AknPopupLayouts::TAknPopupLayouts aType)
	{
	CPopupList* self = new ( ELeave ) CPopupList();
	CleanupStack::PushL ( self);
	self->ConstructL ( aListBox, aCbaResource, aType);
	CleanupStack::Pop ( self);
	return self;
	}

// --------------------------------------------------------------------------
// CCMSSelectionDialog::CPopupList::AttemptExitL
// --------------------------------------------------------------------------
//
inline void CCMSSelectionDialog::CPopupList::AttemptExitL( TBool aAccept)
	{
	CAknPopupList::AttemptExitL ( aAccept);
	return;
	}

// --------------------------------------------------------------------------
// CCMSSelectionDialog::CListBox::SetupWindowLayout
// --------------------------------------------------------------------------
//

void CCMSSelectionDialog::CPopupList::SetupWindowLayout(
		AknPopupLayouts::TAknPopupLayouts aType)
	{
	// Call base class
	CAknPopupList::SetupWindowLayout ( aType);
	return; 
	}

// --------------------------------------------------------------------------
// CCMSSelectionDialog::CListBox::CListBox
// --------------------------------------------------------------------------
//
inline CCMSSelectionDialog::CListBox::CListBox()
	{
	return; 
	}

// --------------------------------------------------------------------------
// CCMSSelectionDialog::CListBox::OfferKeyEventL
// --------------------------------------------------------------------------
//
TKeyResponse CCMSSelectionDialog::CListBox::OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType)
    {
    TKeyEvent keyToForward = aKeyEvent;
    if (EKeyYes == aKeyEvent.iCode)
        {
        keyToForward.iCode = EKeyEnter;
        }
    if (EStdKeyYes == aKeyEvent.iScanCode)
        {
        keyToForward.iScanCode = EStdKeyEnter;   
        }
    return CAknDouble2PopupMenuStyleListBox::OfferKeyEventL(keyToForward, aType);
    }

// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CCMSSelectionDialog::NewL
// ----------------------------------------------------------
//
CCMSSelectionDialog* CCMSSelectionDialog::NewL(
		const RPointerArray<CCmsContactField>& aFieldArray)
	{
	CCMSSelectionDialog* self = new (ELeave) CCMSSelectionDialog( aFieldArray );
	return self;
	}

// ----------------------------------------------------------
// CCMSSelectionDialog::~CCMSSelectionDialog
// ----------------------------------------------------------
//
CCMSSelectionDialog::~CCMSSelectionDialog()
	{
	CMS_DP (KCMSContactorLoggerFile,
			CMS_L ( "CCMSSelectionDialog::~CCMSSelectionDialog()"));

	delete iListBox;
	iListBox = NULL;

	}

// --------------------------------------------------------------------------
// CCMSSelectionDialog::ExecuteLD
// --------------------------------------------------------------------------
//
const CCmsContactFieldItem* CCMSSelectionDialog::ExecuteLD(TInt aCbaResourceId,
		const TDesC& aHeading, TInt aFocusIndex)

	{
	CMS_DP (KCMSContactorLoggerFile,
			CMS_L ( "CCMSSelectionDialog::ExecuteLD()"));

	__ASSERT_DEBUG (!iListBox && !iPopupList, Panic (EPanicPreCond_ExecuteLD));

	// "D" function semantics
	CleanupStack::PushL ( this);

	const CCmsContactFieldItem* result= NULL;

	// Create list box
	CreateListBoxL ( aCbaResourceId, aHeading, aFocusIndex);

	// Show the list query
	TInt dlgResult = 0;
	TRAPD ( err, dlgResult = iPopupList->ExecuteLD ());
	if ( KErrNone != err )
		{
		CMS_DP(KCMSContactorLoggerFile, CMS_L( "CCMSSelectionDialog::ExecuteLD(): Err: %d"),err);		
		User::Leave ( err);
		}

	if ( dlgResult)
		{		
		TInt index = iListBox->CurrentItemIndex ();
		result = &GetContactFieldItemL(index);
		}
	else
		{
		//Operation cancelled. 
		CMS_DP (KCMSContactorLoggerFile,
				CMS_L ( "CCMSSelectionDialog::ExecuteLD(): Cancelled."));
		User::Leave (KErrCancel);
		}

	CleanupStack::PopAndDestroy ( this);

	CMS_DP (KCMSContactorLoggerFile,
			CMS_L ( "CCMSSelectionDialog::ExecuteLD(): Done."));

	return result;
	}

// ----------------------------------------------------------
// CCMSSelectionDialog::CCMSSelectionDialog
// ----------------------------------------------------------
//
CCMSSelectionDialog::CCMSSelectionDialog(
		const RPointerArray<CCmsContactField>& aFieldArray) :
	iFieldArray(aFieldArray)
	{
	return; 
	}

// --------------------------------------------------------------------------
// CCMSSelectionDialog::CreateListBoxL
// --------------------------------------------------------------------------
//
void CCMSSelectionDialog::CreateListBoxL( TInt aCbaResourceId,
		const TDesC& aHeading, TInt /*aFocusIndex*/)
	{
	CMS_DP (KCMSContactorLoggerFile,
			CMS_L ( "CCMSSelectionDialog::CreateListBoxL()"));

	// Create a list box
	iListBox = new(ELeave) CListBox;

	// Create a popup list
	CPopupList* popupList = CPopupList::NewL (iListBox, aCbaResourceId,
			AknPopupLayouts::EMenuDoubleLargeGraphicWindow);
	CleanupStack::PushL ( popupList);

	// Init list box
	iListBox->ConstructL ( popupList, CEikListBox::ELeftDownInViewRect);

	// Set title of popuplist
	if ( 0 < aHeading.Length () )
		{
		popupList->SetTitleL ( aHeading);
		}

	iListBox->Model()->SetItemTextArray ( CreateListItemsL ());
	iListBox->Model()->SetOwnershipType ( ELbmOwnsItemArray);
	iListBox->Reset ();

	iListBox->CreateScrollBarFrameL ( ETrue);
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL (CEikScrollBarFrame::EOff,
			CEikScrollBarFrame::EAuto);

	CleanupStack::Pop (); // listBox
	
	delete iPopupList; 
	iPopupList = NULL; 
	
	iPopupList = popupList;

	CMS_DP (KCMSContactorLoggerFile,
			CMS_L ( "CCMSSelectionDialog::CreateListBoxL(): Done."));

	return;
	}

// -----------------------------------------------------------------------------
// CCMSSelectionDialog::CreateListItemsL
// -----------------------------------------------------------------------------
//    
MDesCArray* CCMSSelectionDialog::CreateListItemsL()
	{
	CMS_DP (KCMSContactorLoggerFile,
			CMS_L ( "CCMSSelectionDialog::CreateListItemsL()"));

	const TInt granularity = 4;
	CDesCArrayFlat* itemArray = new(ELeave) CDesCArrayFlat(granularity);
	CleanupStack::PushL (itemArray);


	for (TInt a = 0; a < iFieldArray.Count (); a++)
		{
		CCmsContactField* contactfield = iFieldArray[a];

		for (TInt i = 0; i < contactfield->ItemCount (); i++)
			{			
			const CCmsContactFieldItem
					* contactfielditem = &contactfield->ItemL(i);

            TDesC* modeldata = CreateModelDataL(*contactfield, *contactfielditem);
            CleanupStack::PushL( modeldata );

			itemArray->AppendL( *modeldata );

			CleanupStack::PopAndDestroy (modeldata);			
			}

		}

	CleanupStack::Pop (itemArray);

	CMS_DP (KCMSContactorLoggerFile,
			CMS_L ( "CCMSSelectionDialog::CreateListItemsL(): Done."));

	return itemArray;
	}


// -----------------------------------------------------------------------------
// CCMSSelectionDialog::CreateModelDataL
// -----------------------------------------------------------------------------
//    
TDesC* CCMSSelectionDialog::CreateModelDataL(
		const CCmsContactField& aContactField, const CCmsContactFieldItem& aContactFieldItem )
	{
	CMS_DP (KCMSContactorLoggerFile,
			CMS_L ( "CCMSSelectionDialog::CreateModelDataL()"));

	//Format of listbox model
	// "FirstLabel\tSecondLabel"

	TDesC* itemtypedata = GetContactTypeStringLC ( aContactField );
	TDesC* itemdata = aContactFieldItem.Data().AllocLC ();

	HBufC* modeldata = HBufC::NewLC (itemdata->Size ()+ itemtypedata->Size ()+2); //2 -> tab.

	modeldata->Des().Append (*itemtypedata);
	modeldata->Des().Append (KCMSContactorTabLit);
	modeldata->Des().Append (*itemdata);


	CleanupStack::Pop (modeldata);
	CleanupStack::PopAndDestroy (itemdata);
	CleanupStack::PopAndDestroy (itemtypedata);

    
	CMS_DP (KCMSContactorLoggerFile,
			CMS_L ( "CCMSSelectionDialog::CreateModelDataL(): Done."));
    return modeldata; 
	}

// -----------------------------------------------------------------------------
// CCMSSelectionDialog::GetContactTypeStringLC
// -----------------------------------------------------------------------------
//    
TDesC* CCMSSelectionDialog::GetContactTypeStringLC(
		const CCmsContactField& aContactField)
	{
	CMS_DP (KCMSContactorLoggerFile,
			CMS_L ( "CCMSSelectionDialog::GetContactTypeStringL()"));

	CCmsContactFieldItem::TCmsContactField type = aContactField.Type ();

	HBufC* typestring= NULL;

	switch (aContactField.Type ())
		{
		case CCmsContactFieldItem::ECmsMobilePhoneHome:
		case CCmsContactFieldItem::ECmsMobilePhoneGeneric:
		case CCmsContactFieldItem::ECmsMobilePhoneWork:
			{
			typestring = StringLoader::LoadLC ( R_QTN_PHOB_LBL_NUMBER_MOBILE );
			break;
			}

		case CCmsContactFieldItem::ECmsLandPhoneGeneric:
		case CCmsContactFieldItem::ECmsLandPhoneHome:
		case CCmsContactFieldItem::ECmsLandPhoneWork:
			{
			typestring = StringLoader::LoadLC ( R_QTN_PHOB_LBL_NUMBER_STANDARD );

			break;
			}

		case CCmsContactFieldItem::ECmsEmailHome:
		case CCmsContactFieldItem::ECmsEmailWork:
		case CCmsContactFieldItem::ECmsEmailGeneric:
			{
			typestring = StringLoader::LoadLC ( R_QTN_PHOB_LBL_EMAIL );

			break;
			}

		case CCmsContactFieldItem::ECmsVoipNumberHome:
		case CCmsContactFieldItem::ECmsVoipNumberWork:
		case CCmsContactFieldItem::ECmsVoipNumberGeneric:				
			{
			typestring = StringLoader::LoadLC ( R_QTN_PHOB_LBL_VOIP );

			break;
			}
		case CCmsContactFieldItem::ECmsUrlGeneric:
		case CCmsContactFieldItem::ECmsUrlHome:
		case CCmsContactFieldItem::ECmsUrlWork:
		    {
		    typestring = StringLoader::LoadLC ( R_QTN_PHOB_LBL_URL );
		    
		    break;
		    }
		case CCmsContactFieldItem::ECmsVideoNumberGeneric:
        case CCmsContactFieldItem::ECmsVideoNumberHome:
        case CCmsContactFieldItem::ECmsVideoNumberWork:
            {
            typestring = StringLoader::LoadLC ( R_QTN_PHOB_LBL_VIDEO );
            break;
            }
		    
        // TODO2: Add here IM specific field detection

		default:
			{
			typestring = StringLoader::LoadLC ( R_QTN_PHOB_LBL_UNKNOWN);
			break;
			}
		}

	CMS_DP (KCMSContactorLoggerFile,
			CMS_L ( "CCMSSelectionDialog::GetContactTypeStringL(): Done."));
	return typestring;
	}

// -----------------------------------------------------------------------------
// CCMSSelectionDialog::GetContactFieldItemL
// -----------------------------------------------------------------------------
//    
const CCmsContactFieldItem& CCMSSelectionDialog::GetContactFieldItemL(
		TInt aIndex)
	{
	CMS_DP (KCMSContactorLoggerFile,
			CMS_L ( "CCMSSelectionDialog::GetContactFieldItemL()"));

	const CCmsContactFieldItem* result= NULL;
	TInt counter = 0;

	for (TInt a = 0; a < iFieldArray.Count (); a++)
		{
		CCmsContactField* contactfield = iFieldArray[a];

		for (TInt i = 0; i < contactfield->ItemCount (); i++)
			{
			if ( aIndex==counter)
				{
				result = &contactfield->ItemL(i);
				return *result;
				}
			counter++;
			}
		}

	CMS_DP (KCMSContactorLoggerFile,
			CMS_L ( "CCMSSelectionDialog::GetContactFieldItemL(): Done."));
	return *result;
	}

// End of file
