/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contact model class
*
*/

#ifndef CSPBCONTACTDATAMODEL_H
#define CSPBCONTACTDATAMODEL_H

// INCLUDES
#include <e32base.h>
#include <bamdesca.h>

// FORWARD DECLARATIONS
class CPbk2IconArray;
class CPbk2PresentationContact;
class CCoeEnv;
class MVPbkContactLink;
class MPbk2ClipListBoxText;
class CSpbContactDataModelPrivate;
class CVPbkContactManager;
class RWriteStream;
class RReadStream;

/**
 *  Contact data model class
 *
 *  @lib pbk2spbcontentprovider.dll
 *  @since S60 9.2
 */
class CSpbContactDataModel : public CBase
{
public:

    /**
     * Two-phased constructor.
     * 
     * @param aCntManager Contact manager
     * @param aCoeEnv UI control environment
     * @param aFieldTypeSelectorRes field type selector. 
     *        Used to determine fields that get clipped from the beginning.
     * @param aClip listbox text clipper
     * @param aIconArray Listbox's icon array
     */
    IMPORT_C static CSpbContactDataModel* NewL(
        CVPbkContactManager& aCntManager,
        CCoeEnv& aCoeEnv,
        TInt aFieldTypeSelectorRes,
        MPbk2ClipListBoxText* aClip = NULL );

    /**
     * Destructor.
     */
    ~CSpbContactDataModel();
    

public: // data types
    
    /// Text types (16-bit) 
    enum TTextTypes
        {
        /// Contact's first name
        ETextFirstName,
        /// Contact's last name
        ETextLastName,
        /// Filename of contact's image
        ETextImageFileName,
        };
    
    /// Data types (8-bit)
    enum TBinaryTypes
        {
        /// Thumbnail data of contact 
        EDataImageContent
        };
    
public: // New methods
    
    /**
     * Get Presentation Field index for field at aIndex
     * 
     * @param aListIndex Listbox row index
     * @return Presentation field index shown at aListIndex  
     */
    IMPORT_C TInt PresentationFieldIndex( TInt aListIndex );

    /**
     * Externalize model into stream
     */
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
    
    /**
     * Internalize model from stream. 
     * Any previous data will be deleted.
     */
    IMPORT_C void InternalizeL( RReadStream& aStream );
    
    /**
     * Sets the models content according to given contact. 
     * Any previous data will be deleted.
     *  
     * @param aContact new contact
     * @param aIconArray icon array that holds the field icons (can be NULL)
     */
    IMPORT_C void SetDataL( 
        const CPbk2PresentationContact& aContact,
        const CPbk2IconArray* aIconArray );
    
    /**
     * Reset Model's content.
     */
    IMPORT_C void Reset();
    
    /**
     * Getter for text elements
     * 
     * @param aType text id
     */
    IMPORT_C TPtrC Text( TTextTypes aType ) const;

    /**
     * Getter for data (binary) elements
     * 
     * @param aType element id
     */
    IMPORT_C TPtrC8 Data( TBinaryTypes aType ) const;
    
    /**
     * Update icon indexes according to aIconArray.
     * 
     * @param aIconArray new icon array
     */
    IMPORT_C void UpdateIconsL( const CPbk2IconArray& aIconArray );

    /**
     * Set list box clipper for the model. Used to clip long list text.
     * 
     * @param aClip text clipper 
     */
    IMPORT_C void SetClipListBoxText( MPbk2ClipListBoxText* aClip );
    
    /**
     * Link of the active contact. Link refers to contact set with SetDataL().
     */
    IMPORT_C MVPbkContactLink* ContactLink();
    
    /**
     * Check if model is empty
     */
    IMPORT_C TBool IsEmpty();

    /**
     * Listbox model of the contact's details.
     */
    IMPORT_C MDesCArray& ListBoxModel();
    
private: // data

    /// Own. Private implementation
    CSpbContactDataModelPrivate* iImpl;
};

#endif // CSPBCONTACTDATAMODEL_H

// End of File
