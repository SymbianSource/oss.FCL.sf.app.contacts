/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Temporary listbox model.
*
*/

#ifndef C_CCAPPDETAILSVIEWLISTBOXMODEL_H
#define C_CCAPPDETAILSVIEWLISTBOXMODEL_H

#include <e32base.h>
#include <badesca.h>
#include <e32hashtab.h>

#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactObserver.h>

#include "MPbk2EditedContactObserver.h"

class CEikListBox;

class CPbk2ApplicationServices;
class MVPbkContactLinkArray;
class CVPbkContactLinkArray;
class MVPbkContactLink;
class MVPbkStoreContact;
class MVPbkStoreContactField;
class MVPbkBaseContactField;
class MVPbkContactOperationBase;
class MVPbkFieldType;
class MVPbkContactFieldData;

class MPbk2FieldPropertyArray;
class CPbk2FieldPropertyArray;
class CPbk2StoreSpecificFieldPropertyArray;
class CPbk2PresentationContact;
class CPbk2PresentationContactField;
class CPbk2IconArray;
class CCCAppDetailsViewPlugin;

/**
 * Details view list box model row.
 *
 */
class CCCAppDetailsViewListBoxRow : public CBase
{
public: // Constructors and destructor

    /**
     * Creates a new instance of this class.
     *
     * @return  A new instance of this class.
     */
    static CCCAppDetailsViewListBoxRow* NewL();

    /**
     * Destructor.
     */
    ~CCCAppDetailsViewListBoxRow();

public: // Interface

    /**
     * Returns the number of columns in this row.
     *
     * @return  Number of columns in this row.
     */
    TInt ColumnCount() const;

    /**
     * Returns the descriptor at the given position.
     *
     * @param aColumnIndex  Column index.
     * @return  The descriptor at the given index.
     */
    TPtrC At(TInt aColumnIndex) const;

    /**
     * Appends a given column to the row.
     *
     * @param aColumnText   Column to append.
     */
    void AppendColumnL(const TDesC& aColumnText);

    /**
     * Is clip required.
     */
    TBool IsClipRequired() const;

    /**
     * Clip text from beginning if field is numeric field or e-mail field.
     */
    void SetClipRequired(TBool aFlag);

    /**
     * Returns the total descriptor length of all the columns.
     *
     * @return  Total row length.
     */
    TInt TotalLength() const;

    /**
     * Returns the maximum column length of this row.
     *
     * @return  Maximum column length.
     */
    TInt MaxColumnLength() const;

private: // Implementation

    CCCAppDetailsViewListBoxRow();

private: // Data

    /// Own: Column array
    RPointerArray<HBufC> iColumns;

    /// Is clip required.
    TBool iClipBeginning;
};

/**
 *
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ccappdetailsviewplugin.dll
 */
class CCCAppDetailsViewListBoxModel :
    public CActive,
    public MDesCArray,
    public MVPbkSingleContactOperationObserver,
    public MVPbkContactObserver,
    public MPbk2EditedContactObserver
{

public:

    /**
     * Column indexes.
     */
    enum TColumnIndex
    {
        /// Icon column
        EIconColumn = 0,
        /// Label column
        ELabelColumn,
        /// Content column
        EContentColumn
    };

    /**
     * Two-phased constructor.
     * @param aListBox
     */
    static CCCAppDetailsViewListBoxModel* NewL(
        CCoeEnv& aCoeEnv,
        CEikListBox& aListBox,
        CPbk2ApplicationServices* aApplicationServices,
        CPbk2IconArray& aIconArray,
        CCCAppDetailsViewPlugin& aPlugin);

    /**
     * Destructor.
     */
    ~CCCAppDetailsViewListBoxModel();

    /**
     *
     */
    void SetLinks(MVPbkContactLinkArray* aLinks);

    /**
     *
     */
    MVPbkContactLinkArray* Links() const;

    /**
     *
     */
    void OpenEditorL();

    /**
     * Get field index of focused list item.
     */
    TInt FocusedFieldIndex();

    /**
     * Get index of focused list item.
     */
    TInt FocusedListIndex();

    /**
     * Set initial focused list index.
     */
    void SetInitialFocusIndex(TInt aIndex);
    
    /**
     *
     */
    MVPbkBaseContactField* FocusedFieldLC();

    /**
     *
     */
    MVPbkStoreContact* StoreContact();

    /**
     *
     */
    HBufC* GetContactTitleL();

    /**
     *
     */
    TBool IsFocusedFieldTypeL(TInt aSelectorResId);

    /**
     *
     */
    TBool HasContactFieldTypeL(TInt aSelectorResId);

    /**
     *
     */
    TBool IsTopContact() const;

    /**
     *
     */
    TBool IsReadOnlyContact() const;

    /**
     *
     */
    TBool IsSimContact() const;

    /**
     *
     */
    TBool IsSdnContact() const;

    /**
     *
     */
    TBool HasSpeedDialL(TInt aIndex);

    /**
     *
     */
    TBool HasDefaultsL(TInt aIndex);

    /**
     * Handles an edit request if the contact is editable.
     */
    void HandleEditRequestL();

    /**
     * Handles a delete request if the contact is editable.
     */
    void HandleDeleteRequestL();
    
    /**
     * Judge If the fileds mapped
     */
    TBool IsFieldMapped()const;

private: // From CActive

    void RunL();

    TInt RunError(TInt aError);

    void DoCancel();

private: // From MVPbkContactObserver

    void ContactOperationCompleted(TContactOpResult aResult);

    void ContactOperationFailed(
        TContactOp aOpCode,
        TInt aErrorCode,
        TBool aErrorNotified);

private: // From MPbk2EditedContactObserver

    void ContactEditingComplete(MVPbkStoreContact* aEditedContact);

    void ContactEditingDeletedContact(MVPbkStoreContact* aEditedContact);

    void ContactEditingAborted();

public: // From MVPbkSingleContactOperationObserver

    void VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& aOperation,
        MVPbkStoreContact* aContact);

    void VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& aOperation,
        TInt aError);

private: // From MDesCArray

    TInt MdcaCount() const;

    TPtrC MdcaPoint(TInt aIndex) const;

private:

    /**
     * Set focused list item.
     */
    void SetFocusedListIndex(TInt aIndex);

    CCCAppDetailsViewListBoxModel(
        CCoeEnv& aCoeEnv,
        CEikListBox& aListBox,
        CPbk2ApplicationServices* aApplicationServices,
        CPbk2IconArray& aIconArray,
        CCCAppDetailsViewPlugin& aPlugin);

    void ConstructL();

private: // new implementation

    enum TCCAppDetailsViewCommand
    {
        ECCAppDetailsViewCommandNone = 0,
        ECCAppDetailsViewCommandEdit = 1
    };

    /**
     *
     */
    void IssueRequest(TInt aError = KErrNone);

    /**
     *
     */
    void CreatePresentationContactL();

    /**
     * Helper function for adding contact field items to
     * listbox data model.
     *
     */
    void AddDataL();

    /**
     *
     */
    void ExecuteEditorL();

    /**
     *
     */
    HBufC* GetLabelLC(const CPbk2PresentationContactField& aField);

    /**
     *
     */
    void SetEmptyTextL(TInt aResourceId);

    /**
     *
     */
    TBool IsHiddenField(const MVPbkFieldType* aFieldType);

    /**
     *
     */
    MVPbkBaseContactField* FieldAtLC(TInt aIndex);

    /**
     *
     */
    TBool IsFieldTypeL(TInt aIndex, TInt aSelectorResId);

    /**
     *
     */
    TBool HandleTextTypeFieldL(
        TInt aIndex,
        const MVPbkContactFieldData& aFieldData,
        CCCAppDetailsViewListBoxRow* aRow,
        TInt aResId);

    /**
     *
     */
    void HandleDateTimeTypeFieldL(
        const MVPbkContactFieldData& aFieldData,
        CCCAppDetailsViewListBoxRow* aRow);

    /**
     *
     */
    void HandleUriTypeFieldL(
        TInt aIndex,
        const MVPbkContactFieldData& aFieldData,
        CCCAppDetailsViewListBoxRow* aRow);

    /**
     *
     */
    void AddIconsToEndL(
        TInt aIndex,
        CCCAppDetailsViewListBoxRow* aRow);

    /**
     *
     */
    TBool ClipFromBeginning(TDes& aBuffer, TInt aItemIndex, TInt aSubCellNumber) const;

    /**
     *
     */
    void ExpandBuffersL(CCCAppDetailsViewListBoxRow* aRow);

    /**
     *
     */
    TPtr ExpandColumnBufferL(TInt aRequiredLength);

    /*
     * Sets the title to the plugin
     * @param aTitle - title to be set
     */
    void SetTitleL();

    /**
     * Returns ETrue if the contact is editable.
     */
    TBool IsContactEditable();

    /**
     * Sets the CBA labels.
     */
    void UpdateMSKinCbaL();
    
    /**
	 * Check if address has geocoordinates.
	 */
    TBool IsAddressValidated(TInt aGroupId);

private: // data

    /**
     * Reference to control environment.
     * Not own.
     */
    CCoeEnv& iCoeEnv;

    /**
     * Reference to listbox.
     * Not own.
     */
    CEikListBox& iListBox;

    /**
     * Text row array.
     * Own.
     */
    CArrayPtrFlat<CCCAppDetailsViewListBoxRow> iRows;

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
     * Reference to Application services.
     * Not own.
     */
    CPbk2ApplicationServices* iAppServices;

    /**
     *
     */
    MVPbkContactLinkArray* iLinks;

    /**
     *
     */
    const MVPbkContactLink* iCurrentLink;

    /**
     *
     */
    MVPbkContactOperationBase* iRetrieveOperation;

    /**
     *
     */
    MVPbkStoreContact* iStoreContact;

    /// Own: Phonebook 2 field properties
    MPbk2FieldPropertyArray* iFieldProperties;

    /// Own: Store specific properties
    CPbk2StoreSpecificFieldPropertyArray* iSpecificFieldProperties;

    /// Own: Presentation level contact
    CPbk2PresentationContact* iPresentationContact;

    /// Ref: Icon array
    const CPbk2IconArray& iIconArray;

    /**
     *
     */
    TCCAppDetailsViewCommand iCommand;

    /**
     * The flag indicating if the editing has been aborted
     */
    TBool iIsEditingAborted;

    /**
     *
     */
    TInt iInitialFocusIndex;

    /**
     * Ref to plugin.
     * Not own.
     */
    CCCAppDetailsViewPlugin& iPlugin;
    
    /// Own: Map current index to Presentation Contact index
    RHashMap<TInt, TInt> iInxToPresentationIdx; 
};

#endif // CCADETAILSVIEWLISTBOXMODEL_H

// End of File
