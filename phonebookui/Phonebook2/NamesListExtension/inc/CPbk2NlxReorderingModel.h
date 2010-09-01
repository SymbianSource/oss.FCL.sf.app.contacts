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


#ifndef CPBK2REORDERING_MODEL_H
#define CPBK2REORDERING_MODEL_H

#include <e32base.h>
#include <bamdesca.h>
#include <badesca.h> 
// FORWARD
class MVPbkContactViewBase;
class MPbk2ContactNameFormatter;
class MVPbkContactLinkArray;
class MVPbkContactLink;
class CPbk2ContactIconsUtils;
class CPbk2IconArray;
class MPbk2ContactUiControlExtension;
class MPbk2ContactUiControlUpdate;
class CVPbkContactManager;
class CPbk2StorePropertyArray;

// CLASS DECLARATIONS
/**
 * Listbox model that provides top contacts list and allows changing their
 * order.
 */
NONSHARABLE_CLASS( CPbk2NlxReorderingModel ) : public CBase, public MDesCArray
	{
public:

	/**
     * Creates a new instance of this class.
     * 
     * @param aContactManager vpbk contact manager
     * @param aView top contacts view
     * @param aFormatter Format name list items
     * @param aStoreProperties store properties
     *
     * @return  A new instance of this class.
     */
	static CPbk2NlxReorderingModel* NewL(
	        CVPbkContactManager& aContactManager,
			const MVPbkContactViewBase& aView,
			MPbk2ContactNameFormatter& aFormatter,
			CPbk2StorePropertyArray& aStoreProperties );
	
	/**
	 * Destructor
	 */ 
	~CPbk2NlxReorderingModel();

public: //	MDesCArray
	TInt MdcaCount() const;
	TPtrC MdcaPoint(TInt aIndex) const;
	
public: // new methods

	/**
     * Move item
     *
     * @param aFrom index of the intial position
     * @param aB index of the target position
     */
	void Move(TInt aFrom, TInt aTo);
	
    /**
     * Returns an array of reordered links
     *
     * @return an array of contact links
     */	
	MVPbkContactLinkArray* ReorderedLinksLC() const;
	
	/**
	 * Set focused item.
	 * 
	 * @param aFocusedLink contact link to set initial focus
	 **/	
	void SetFocusedContactL( const MVPbkContactLink& aFocusedLink );

	/**
	 * Set focused item.
	 * 
	 * @param aFocusedIndex index of focused item
	 **/	
	void SetFocusedContactL( TInt aFocusedIndex );

	/**
	 * Return focused item.
	 * 
	 * @return focused contact index or KErrNotFound
	 **/	
	TInt FocusedContactIndex() const;

	/**
	 * Return focused item contact link.
	 * 
	 * @return focused contact link or NULL
	 **/
	MVPbkContactLink*  FocusedContactLinkL() const;
	
    /**
     * Return contact's model index.
     * 
     * @return contact index or KErrNotFound
     **/    
    TInt ContactIndex(const MVPbkContactLink& aContact ) const;
	
	
	/**
	 * Returns icon array and transfers its ownership.
	 * Keeps reference for internal usage. 
	 */
	CPbk2IconArray* TakeIconArray();
	
	/**
	 * Listen to ui control extension notification.
	 */
	void SetContactUpdater( MPbk2ContactUiControlUpdate* aContactUpdator );
	
private:
	CPbk2NlxReorderingModel(
			const MVPbkContactViewBase& aView,
			MPbk2ContactNameFormatter& aFormatter);
	void ConstructL(
	        CVPbkContactManager& aContactManager,
	        CPbk2StorePropertyArray& aStoreProperties );
	
	void FormatItemL( TDes& aBuf, TInt aIndex ) const;
	void FormatEmptyItem( TDes& aBuf ) const;
	
private:
	enum
		{
		EMaxListBoxText = 256
		};
	
	/// Own: map that keeps a new order
	RArray<TInt> iReorderingMap;
	/// Ref: view with top contacts
	const MVPbkContactViewBase& iView; 
	/// Ref: name list formatter
	MPbk2ContactNameFormatter& iFormatter;
	/// Ref: Buffer to format list box text
	TBuf<EMaxListBoxText> iBuffer;
	/// Own: preselected contact, used for initial focused contact and
	///		 pass back last focused contact to UiControl
	MVPbkContactLink* iFocusedItem;
	/// Own: contact icons
	CPbk2ContactIconsUtils* iContactIcons;
	/// Own/Ref: icon array
	CPbk2IconArray* iIcons;
	/// Indicates whether iIcons is ours 
	TBool iIconsOwnership;
	/// Own: ui control exntesion, provides dynamic contact icons
	MPbk2ContactUiControlExtension* iUiExtension;
	};

#endif // CPBK2REORDERING_MODEL_H

// End of File
