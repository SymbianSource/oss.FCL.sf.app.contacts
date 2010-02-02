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
* Description:  Listbox model for MyCard details list
*
*/

#ifndef CCAPPMYCARDLISTBOXMODEL_H
#define CCAPPMYCARDLISTBOXMODEL_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <e32hashtab.h>

#include "ccappmycard.h"


// FORWARD DECLARATIONS
class MVPbkContactFieldData;
class MVPbkBaseContactField;
class MVPbkFieldType;
class CPbk2PresentationContactField;
class CPbk2IconArray;
class CPbk2PresentationContact;
class CEikListBox;
class CCCAppMyCardListBoxRow;
class CCoeEnv;

/**
 * Listbox model class for MyCard details list
 *
 *
 *  @lib ccappmycardplugin.dll
 *  @since S60 9.2
 */
class CCCAppMyCardListBoxModel :
    public CBase,
    public MDesCArray,
    public MMyCardObserver
{

public:

    /**
     * Two-phased constructor.
     * 
     * @param aMyCard MyCard reference
     * @param aCoeEnv UI control environment
     * @param aListBox Listbox UI control
     * @param aIconArray Listbox's icon array
     */
    static CCCAppMyCardListBoxModel* NewL(
        CCCAppMyCard& aMyCard,
        CCoeEnv& aCoeEnv,
        CEikListBox& aListBox,
        CPbk2IconArray& aIconArray );

    /**
     * Destructor.
     */
    ~CCCAppMyCardListBoxModel();


public: // From MMyCardObserver
    
    void MyCardEventL( TEvent aEvent );
    
    
public: // From MDesCArray
    
    TInt MdcaCount() const;
    TPtrC MdcaPoint(TInt aIndex) const;

public: // New methods
    
    /**
     * Get Presentation Field index for field at aIndex
     * 
     * @param aIndex Listbox row index
     * @return Presentation field index shown at aIndex  
     */
    TInt FieldIndex( TInt aIndex );
    
private: // private construction

    /**
     * Constructor
     */
    CCCAppMyCardListBoxModel(
        CCCAppMyCard& aMyCard,
        CCoeEnv& aCoeEnv,
        CEikListBox& aListBox,
        CPbk2IconArray& aIconArray );

    /**
     * 2nd constructor
     */
    void ConstructL();

private: // new implementation

    /**
     * Helper function for adding contact field items to
     * listbox data model.
     *
     */
    void AddDataL();

    /**
     * Get label for contact field
     */
    HBufC* GetLabelLC(const CPbk2PresentationContactField& aField);

    /**
     * Should field aFieldType be hidden
     */
    TBool IsHiddenField(const MVPbkFieldType* aFieldType);

    /**
     * Get field at aIndex
     */
    MVPbkBaseContactField* FieldAtLC(TInt aIndex);

    /**
     * Field type tester
     */
    TBool IsFieldTypeL(TInt aIndex, TInt aSelectorResId);

    /**
     * Format generic text -type field
     */
    void HandleTextTypeFieldL(
        TInt aIndex,
        const MVPbkContactFieldData& aFieldData,
        CCCAppMyCardListBoxRow* aRow );

    /**
     * Format Date -type field
     */
    void HandleDateTimeTypeFieldL(
        const MVPbkContactFieldData& aFieldData,
        CCCAppMyCardListBoxRow* aRow);

    /**
     * Format URI -type field
     */
    void HandleUriTypeFieldL(
        TInt aIndex,
        const MVPbkContactFieldData& aFieldData,
        CCCAppMyCardListBoxRow* aRow);

    /**
     * Clip aBuffer from beginning to fit into listbox
     */
    TBool ClipFromBeginning(TDes& aBuffer, TInt aItemIndex, TInt aSubCellNumber) const;

    /**
     * Expand column and line buffers to be sufficient for aRow
     */
    void ExpandBuffersL(CCCAppMyCardListBoxRow* aRow);

    /**
     * Expand column buffer to aRequiredLength
     * 
     * @return new column buffer
     */
    TPtr ExpandColumnBufferL(TInt aRequiredLength);


private: // data

    /**
     * Reference to control environment.
     * Not own.
     */
    CCoeEnv& iCoeEnv;
    
    /**
     * Reference to MyCard instance
     * Not own.
     */
    CCCAppMyCard& iMyCard;
    
    /**
     * Reference to listbox.
     * Not own.
     */
    CEikListBox& iListBox;

    /**
     *  Reference to listbox icon array
     *  Not own.
     */
    CPbk2IconArray& iIconArray;

    /**
     * Text row array.
     * Own.
     */
    CArrayPtrFlat<CCCAppMyCardListBoxRow> iRows;

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

    /// Not own. Presentation contact
    CPbk2PresentationContact* iPresentationContact;
    
    /// Own: Map current index to Presentation Contact index
    RHashMap<TInt, TInt> iInxToPresentationIdx; 
};

#endif // CCAPPMYCARDLISTBOXMODEL_H

// End of File
