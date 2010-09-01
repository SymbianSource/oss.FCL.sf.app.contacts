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


#ifndef C_CMSSELECTIONDIALOG_H
#define C_CMSSELECTIONDIALOG_H

#include <e32base.h>
#include "cmscontactfield.h"

/**
 *  CCMSSelectionDialog
 *  Dialog for selecting wanted field. 
 *
 *  @code
 *   
 *  @endcode
 *
 *  @lib cmscontactor
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CCMSSelectionDialog) : public CBase
	{
public:

	/**
	 * Two-phased constructor.
	 */
	static CCMSSelectionDialog* NewL( const RPointerArray<CCmsContactField>& aFieldArray );

	/**
	 * Destructor.
	 */
	virtual ~CCMSSelectionDialog();

	/**
	 * Executes the dialog.
	 *
	 * @since S60 v5.0
	 * @param aCbaResourceId: Resource Id of softkey buttons. 
	 * @param aHeading: Resource Id of heading text.
	 * @param aFocusIndex Focused item.
	 * @return selected field item.
	 */
	const CCmsContactFieldItem* ExecuteLD
	( TInt aCbaResourceId, const TDesC& aHeading,
			TInt aFocusIndex );

private: // Implementation

	CCMSSelectionDialog( const RPointerArray<CCmsContactField>& aFieldArray );

	/**
	 * Creating and initializing actual listbox. 
	 *
	 * @since S60 v5.0
	 * @param aCbaResourceId: Resource Id of softkey buttons. 
	 * @param aHeading: Resource Id of heading text.
	 * @param aFocusIndex Focused item.
	 */
	void CreateListBoxL(
			TInt aCbaResourceId,
			const TDesC& aHeading,
			TInt aFocusIndex );

	/**
	 * Prepares the listbox model. 
	 *
	 * @since S60 v5.0
	 * @return listbox model.
	 */
	MDesCArray* CreateListItemsL( );

	/**
	 * Helper function for making correct model data for listbox.
	 *
	 * @since S60 v5.0
	 * @param aContactField: Contact field where to get the type.
	 * @return model data, ownership changed.
	 */
    TDesC* CreateModelDataL( const CCmsContactField& aContactField, const CCmsContactFieldItem& aContactFieldItem );

	/**
	 * Helper function for getting correct localized field type string.
	 *
	 * @since S60 v5.0
	 * @param aContactField: Contact field where to get the type.
	 * @return contact type string, ownership changed, set to cleanupstack. 
	 */
	TDesC* GetContactTypeStringLC( const CCmsContactField& aContactField );

	/**
	 * Helper function for getting needed contact field item. 
	 *
	 * @since S60 v5.0
	 * @param aIndex: Index of needed item. 
	 * @return contact field item.  
	 */
	const CCmsContactFieldItem& GetContactFieldItemL( TInt aIndex );

private: // data

	/// Specialized list box class
	class CListBox;
	/**
	 * List box inside the popup list.
	 * Own.  
	 */
	CListBox* iListBox;

	/// Specialized popup list class
	class CPopupList;
	/**
	 *  Popup list query.
	 * Own.  
	 */
	CPopupList* iPopupList;

	/**
	 * Array of listbox items.
	 * Not Own.  
	 */
	const RPointerArray<CCmsContactField>& iFieldArray;

	};

#endif // C_CMSSELECTIONDIALOG_H

// End of File
