/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 contact view double list box.
*
*/


#ifndef CPBK2CONTACTVIEWDOUBLELISTBOX_H
#define CPBK2CONTACTVIEWDOUBLELISTBOX_H

//  INCLUDES
#include "CPbk2ContactViewListBox.h"

#include "CPbk2ThumbnailManager.h"
#include "CPbk2ContactViewListBoxModel.h"	// params
//FORWARD
class MVPbkContactViewBase;
class CCoeControl;



/**
 * Phonebook 2 contact view double list box.
 */
NONSHARABLE_CLASS(CPbk2ContactViewDoubleListBox) : public CPbk2ContactViewListBox,
												   public MPbk2ThumbnailManagerObserver
{
     
public: // Constructors and destructor

    /**
     * Creates a new instance of this class.
     *
     * @param aContainer        The control container for this control.
     * @param aResourceReader   Resource reader used to construct
     *                          this object.
     * @param aManager          Reference to contact manager.
     * @param aView             Reference to contact view.
     * @param aNameFormatter    Reference to the name formatter.
     * @param aStoreProperties  Store property array.
     * @param aUiExtension      UI extension.
     * @param aSearchFilter PBk2 Search Filter 
     * @param aThumbManager		Thumbnail manager for the listbox
     * @return  A new instance of this class.
     */
    static CPbk2ContactViewDoubleListBox* NewL(
            CCoeControl& aContainer,
            TResourceReader& aResourceReader,
            const CVPbkContactManager& aManager,
            MPbk2FilteredViewStack& aView,
            MPbk2ContactNameFormatter& aNameFormatter,
            CPbk2StorePropertyArray& aStoreProperties,
            MPbk2ContactUiControlExtension* aUiExtension,
            CPbk2PredictiveSearchFilter& aSearchFilter,
            CPbk2ThumbnailManager& aThumbManager );

    /**
     * Destructor.
     */
    ~CPbk2ContactViewDoubleListBox();

private:	// from MPbk2ThumbnailManagerObserver
	void LoadingFinished();
	
	void ThumbnailLoadingComplete( TInt aError, TInt aIndex );
	
	void ThumbnailRemoved( const MVPbkContactLink& aLink, TInt aIndex );
	
public: // From CEikListBox
    
    void SizeChanged();

    
protected: // Implementation
    
    /**
     * Constructor
     */
    CPbk2ContactViewDoubleListBox( 
        CCoeControl& aContainer,
        MPbk2ContactUiControlExtension* aUiExtension,
        CPbk2PredictiveSearchFilter& aSearchFilter,	
        MPbk2FilteredViewStack& aView,
        CPbk2ThumbnailManager& aThumbManager );
    
    /*
     * Constructor
     */
    void ConstructL(
		CCoeControl& aContainer, 
		TResourceReader& aResourceReader,
		const CVPbkContactManager& aManager, 
		MPbk2FilteredViewStack& aView,
		MPbk2ContactNameFormatter& aNameFormatter,
		CPbk2StorePropertyArray& aStoreProperties );
    
    CPbk2ContactViewListBoxModel* DoCreateModelL( 
            		CPbk2ContactViewListBoxModel::TParams params );
    
    CPbk2ListboxModelCmdDecorator* DoCreateDecoratorL( 
            		const CPbk2IconArray& aIconArray,
            		TPbk2IconId aEmptyIconId,
            		TPbk2IconId aDefaultIconId );
    
    /**
     * Apply custom layout to listbox after size change
     */
    void SizeChangedL();


private:	//data
	//REF: View
    MPbk2FilteredViewStack& iView;
	//REF:	thumbnail manager
	CPbk2ThumbnailManager& iThumbManager;
};

#endif // CPBK2CONTACTVIEWDOUBLELISTBOX_H

// End of File
