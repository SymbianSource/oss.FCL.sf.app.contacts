/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contact model class for MyCard
*
*/

#ifndef CSPBCONTACTDATAMODELPRIVATE_H
#define CSPBCONTACTDATAMODELPRIVATE_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>
#include "spbcontactdatamodel.h"

// FORWARD DECLARATIONS
class MVPbkContactFieldData;
class MVPbkBaseContactField;
class MVPbkFieldType;
class MVPbkContactLink;

class MPbk2ClipListBoxText;
class CPbk2PresentationContactField;
class CPbk2PresentationContactFieldCollection;
class CPbk2IconArray;
class CPbk2PresentationContact;

class CSpbContactDataModelRow;

class CEikListBox;
class CCoeEnv;

/**
 * Listbox model class for MyCard details list
 *
 *
 *  @lib pbk2spbcontentprovider.dll
 *  @since S60 9.2
 */
NONSHARABLE_CLASS( CSpbContactDataModelPrivate ) : public CBase, public MDesCArray
{

public:

    /**
     * Two-phased constructor.
     * 
     * @param aCntManager
     * @param aCoeEnv UI control environment
     * @param aFieldTypeSelectorRes
     * @param aClip
     * @param aIconArray Listbox's icon array
     */
    static CSpbContactDataModelPrivate* NewL(
        CVPbkContactManager& aCntManager,
        CCoeEnv& aCoeEnv,
        TInt aFieldTypeSelectorRes,
        MPbk2ClipListBoxText* aClip = NULL );

    /**
     * Destructor.
     */
    ~CSpbContactDataModelPrivate();
 
public: // "From" CSpbContactDataModel
    
    TInt PresentationFieldIndex( TInt aListIndex );
    TInt ListIndex( TInt aPresentationIndex );
    void ExternalizeL( RWriteStream& aStream ) const;
    void InternalizeL( RReadStream& aStream );
    void SetDataL( 
        const CPbk2PresentationContact& aContact, 
        const CPbk2IconArray* aIconArray );
    void Reset();
    TPtrC Text( CSpbContactDataModel::TTextTypes aType ) const;
    TPtrC8 Data( CSpbContactDataModel::TBinaryTypes aType ) const;
    void UpdateIconsL( const CPbk2IconArray& aIconArray );
    void SetClipListBoxText( MPbk2ClipListBoxText* aClip );
    MVPbkContactLink* ContactLink();   
    TBool IsEmpty();

public: // From MDesCArray
    
    TInt MdcaCount() const;
    TPtrC MdcaPoint(TInt aIndex) const;
    
private: // private construction

    /**
     * Constructor
     */
    CSpbContactDataModelPrivate(
        CVPbkContactManager& aCntManager,
        CCoeEnv& aCoeEnv,
        TInt aFieldTypeSelectorRes,
        MPbk2ClipListBoxText* aClip );

    /**
     * 2nd constructor
     */
    void ConstructL();


private: // new implementation

    /**
     * Get label for contact field
     */
    HBufC* GetLabelLC(const CPbk2PresentationContactField& aField);

    /**
     * Should field aFieldType be hidden
     */
    TBool IsHiddenField(const MVPbkFieldType* aFieldType);

    /**
     * Field type tester
     */
    TBool IsFieldTypeL( const MVPbkFieldType& aFieldType, TInt aSelectorResId);

    /**
     * Format generic text -type field
     */
    void HandleTextTypeFieldL(
        const MVPbkFieldType& aFieldType,
        const MVPbkContactFieldData& aFieldData,
        CSpbContactDataModelRow* aRow );

    /**
     * Format Date -type field
     */
    void HandleDateTimeTypeFieldL(
        const MVPbkContactFieldData& aFieldData,
        CSpbContactDataModelRow* aRow);

    /**
     * Format URI -type field
     */
    void HandleUriTypeFieldL(
        const MVPbkFieldType& aFieldType,
        const MVPbkContactFieldData& aFieldData,
        CSpbContactDataModelRow* aRow);

    /**
     * Expand column and line buffers to be sufficient for aRow
     */
    void ExpandBuffersL(CSpbContactDataModelRow* aRow);

    /**
     * Expand column buffer to aRequiredLength
     * 
     * @return new column buffer
     */
    TPtr ExpandColumnBufferL(TInt aRequiredLength);

    /**
     * 
     */
    void ExternalizeDataL( RWriteStream& aStream, const TDesC16& aData ) const;
    void ExternalizeDataL( RWriteStream& aStream, const TDesC8& aData ) const;
    
    /**
     * 
     */
    void InternalizeDataL( RReadStream& aStream, HBufC16*& aData );
    void InternalizeDataL( RReadStream& aStream, HBufC8*& aData );
    
private: // data

    /**
     * Reference to control environment.
     * Not own.
     */
    CCoeEnv& iCoeEnv;
    
    /**
     * 
     * Not own.
     */
    CVPbkContactManager& iCntManager;
    
    /**
     * Reference to listbox clipper.
     * Not own.
     */
    MPbk2ClipListBoxText* iClip;

    /**
     * Text row array.
     * Own.
     */
    CArrayPtrFlat<CSpbContactDataModelRow> iRows;

    /**
     * Buffer for storing one formatted listbox row.
     * Own.
     */
    HBufC* iLineBuf;

    /**
     * Buffer for storing one formatted listbox column.
     * Own.
     */
    HBufC* iColumnBuf;
    
    /**
     * Own: Map current index to Presentation Contact index
     */
    RArray<TInt> iListIndxToPresentationIndx;

    /**
     * Field type selector resource
     */
    TInt iFieldTypeRes;

    /**
     * Own. Image data buffer
     */
    HBufC8* iImageBuffer;
    
    /**
     * Own. Contact's image filename
     */
    HBufC* iImageFileName;
    
    /**
     * Own. Contact's last name
     */
    HBufC* iLastName;
    
    /**
     * Own. Contact's first name
     */
    HBufC* iFirstName;
    
    /**
     * Own. Current contact of model
     */
    MVPbkContactLink* iContactLink;
};

#endif // CSPBCONTACTDATAMODELPRIVATE_H

// End of File
