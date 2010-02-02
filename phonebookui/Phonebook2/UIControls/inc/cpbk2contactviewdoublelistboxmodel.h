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
* Description:  Phonebook 2 contact view double list box model.
*
*/


#ifndef CPBK2CONTACTVIEWDOUBLELISTBOXMODEL_H
#define CPBK2CONTACTVIEWDOUBLELISTBOXMODEL_H

//  INCLUDES
#include "CPbk2ContactViewListBoxModel.h"

//FORWARD
class CPbk2ThumbnailManager;
class MPbk2ContactUiControlDoubleListboxExtension;
class CPbk2ContactViewDoubleListboxDataElement;

// CLASS DECLARATION

/**
 * Phonebook 2 contact view double list box model.
 */
NONSHARABLE_CLASS(CPbk2ContactViewDoubleListBoxModel) :
    public CPbk2ContactViewListBoxModel
    {
public:  // Constructors and destructor

    /**
     * Creates a new instance of this class.
     *
     * @param aParams   Parameters for the model.
     * @return  A new instance of this class.
     */
     static CPbk2ContactViewDoubleListBoxModel* NewL(
            CPbk2ContactViewListBoxModel::TParams& aParams,
            CPbk2ThumbnailManager& aThumbManager );

    /**
     * Destructor.
     */
    ~CPbk2ContactViewDoubleListBoxModel();

    
    
protected:
    
    /**
     * Constructor
     */
    CPbk2ContactViewDoubleListBoxModel(
        CPbk2ContactViewListBoxModel::TParams& aParams,
        CPbk2ThumbnailManager& aThumbManager );

protected: // new 
    
    /**
     * Format whole line buffer based on aElement's content.
     * 
     * @param aElement data element which stores the line content 
     */
    void FormatBufferFromElement(
        const CPbk2ContactViewDoubleListboxDataElement& aElement ) const;
    
    /**
     * Append text into line buffer with trailing tab character. Removes 
     * illegal characters from the aText before appending. aText is subject to
     * modifications during this call.
     * 
     * @param aText text to append into line buffer 
     */
    void AppendText( TDes& aText ) const;
    
    /*
     * Appends icon index with trailing tab character if it's found from the icon array
     * 
     * @param aIconId 	Id of the icon in icon array
     */
    void AppendIconIndexIfFound(  const TPbk2IconId& aIconId ) const;
        
public: // From CPbk2ContactViewListBoxModel
    
    /// See CPbk2ContactViewListBoxModel
    void FormatBufferForContactL(
        const MVPbkViewContact& aViewContact, 
        TInt aIndex ) const;
    
private:	//data
	
	//REF:	thumbnail manager
	CPbk2ThumbnailManager& iThumbManager;
    
	/// Ref
	MPbk2ContactUiControlDoubleListboxExtension* iDoubleListExtensionPoint;
	
    };

#endif // CPBK2CONTACTVIEWDOUBLELISTBOXMODEL_H

// End of File
