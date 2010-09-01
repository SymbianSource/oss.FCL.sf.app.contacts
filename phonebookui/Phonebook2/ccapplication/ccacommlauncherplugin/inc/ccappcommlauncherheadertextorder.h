/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class which takes care of placing the correct texts in the header
*
*/


#ifndef CCAPPCOMMLAUNCHERHEADERTEXTORDER_H_
#define CCAPPCOMMLAUNCHERHEADERTEXTORDER_H_

// INCLUDES
#include "ccappcommlauncherheaders.h"

const TInt KCCAppCommLauncherHeaderTextOrderLineCount = 3;
const TInt KCCAppCommLauncherHeaderTextOrderFirstField = CCmsContactFieldItem::ECmsLastName; 
const TInt KCCAppCommLauncherHeaderTextOrderLastField = CCmsContactFieldItem::ECmsShareView;


// CLASS DECLARATION
/**
 * Class which takes care of placing the correct texts in the header
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ccappcommlauncherplugin.dll
 *  @since S60 v5.0
 */
class CCCAppCommLauncherHeaderTextOrder : public CBase
    {
public:
    // Construction & destruction
    /**
     * Two-phased constructor.
     */
    static CCCAppCommLauncherHeaderTextOrder* NewL();
 
    /**
     * Destructor.
     */
    ~CCCAppCommLauncherHeaderTextOrder();

    /**
     * Gets the text for a header row
     * 
     * @param aRow Row number (0-)
     * @return const TDesC& the text
     */
    const TDesC& GetTextForRow(TInt aRow);
    

    /**
     * Processes the contact data and determines which fields are used
     * 
     * @param aContactField The contact field info
     */
    void ProcessContactFieldsL(RArray<CCmsContactField> aCmsContactFields);
    
    /**
     * Resets the header texts
     */
    void Reset();
    
    /**
     * Clips the text
     * 
     * @param aFont The font which will be used for the text
     * @param aLineWidth Maximun width of a row in pixels
     */
    void ClipL(const CFont& aFont, TInt aLineWidth);
private:
    /**
     * Constructor for performing 1st stage construction
     */
    CCCAppCommLauncherHeaderTextOrder();

    /**
     * Constructor for performing 2nd stage construction
     */
    void ConstructL();

    /**
     * Processes a contact field and puts the data in a row
     * 
     * @param aContactField The contact field
     */
    void ProcessContactFieldL(const CCmsContactField& aContactField);
        
    /**
     * Sets the priority of the given contact field type
     * 
     * @param aContactField The contact field
     * @param iGroup ETrue to set the group priority, 
     * @param aPriority Priority
     */
    void SetPriority(CCmsContactFieldItem::TCmsContactField aContactField, TBool aGroup, TInt aPriority);
    
    /**
     * Gets the priority of the given contact field type
     * 
     * @param aContactField The contact field
     * @return TInt priority
     */
    TInt GetPriority(CCmsContactFieldItem::TCmsContactField aContactField, TBool aGroup);

    /**
     * Determines whether the Contact Field is to used or skipped.
     * 
     * @param aContactField the field  
     * @return TInt aTopGroupPriority group to be used when determining. 
     */    
    void SelectContactFieldL(const CCmsContactField& aContactField, TInt aTopGroupPriority);

    /**
     * Determines stripped length of Contact Field Item
     * 
     * @param aCmsContactFieldItem the item 
     * @return TInt stripped length 
     */    
    TInt StrippedLengthL( const CCmsContactFieldItem& aCmsContactFieldItem);

    /**
     * Deletes a char from descriptor
     * 
     * @param aDest descriptor 
     * @param aChar character to be removed 
     */
    void DeleteCharL(TDes& aDest, TChar aChar);    
    
    /**
     * Maps a string to the corresponding contact field type
     * 
     * @param aDesc The string
     * @param aType Contact field is returned through the passed reference
     * @return TInt error code, KErrNotFound if there is no type corresponding to the string, KErrNone otherwise
     */
    TInt MapStringToContactField(const TDesC& aDesc, CCmsContactFieldItem::TCmsContactField& aType);

    /**
     * Tells which is the correct clipping direction for a contact field type
     * 
     * @param aType Contact field
     * @return TInt error code, KErrNotFound if there is no type corresponding to the string, KErrNone otherwise
     */
    AknTextUtils::TClipDirection MapContactFieldToClipDirection(CCmsContactFieldItem::TCmsContactField aType);
    
    /**
     * Count number of items corresponding current database
     * 
     * @param CCmsContactField Field to be checked
     * @return TInt number of items 
     */    
    TInt ItemCountForCurrentDbL( const CCmsContactField& aContactField );
    
    RPointerArray<HBufC> iHeaderRows;
    RPointerArray<HBufC> iHeaderRowsClipped;
    TInt iHeaderTypes[KCCAppCommLauncherHeaderTextOrderLineCount];
    
    TBool iHeadersWrapped;
    
    TInt iGroupPriorities[KCCAppCommLauncherHeaderTextOrderLastField - KCCAppCommLauncherHeaderTextOrderFirstField + 1];
    TInt iIndividualPriorities[KCCAppCommLauncherHeaderTextOrderLastField - KCCAppCommLauncherHeaderTextOrderFirstField + 1];
    HBufC* iCurrentDbName;
    };

#endif /*CCAPPCOMMLAUNCHERHEADERTEXTORDER_H_*/
