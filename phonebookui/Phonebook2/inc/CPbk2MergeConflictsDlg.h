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
* Description:  Provides Conflict resolution dialog
*               for Contact Merge command.
*
*/

#ifndef CPBK2MERGECONFLICTSDLG_H
#define CPBK2MERGECONFLICTSDLG_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <AknDialog.h>
#include <eiklbo.h>

// CLASS DECLARATION
class CEikColumnListBox;
class MPbk2MergeResolver;
class CAknNavigationDecorator;

/**
 *  CPbk2MergeConflictsDlg
 * 
 */
NONSHARABLE_CLASS( CPbk2MergeConflictsDlg ) : public CAknDialog,
                                              private MEikListBoxObserver
    {
public: // construction
    
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CPbk2MergeConflictsDlg* NewL( MPbk2MergeResolver* aConflictResolver, TBool aPhotoConflictExpected );
    
    /**
     * Destructor.
     */
    ~CPbk2MergeConflictsDlg();
        
private: // construction
    
    /**
     * Constructor for performing 1st stage construction
     */
    CPbk2MergeConflictsDlg( MPbk2MergeResolver* aConflictResolver, TBool aPhotoConflictExpected );
    
    /**
     * Default constructor for performing 2nd stage construction
     */
    void ConstructL();

private: // from MEikListBoxObserver
    
    void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
    
private: // from CAknDialog
        
    void PreLayoutDynInitL();

    TBool OkToExitL( TInt aButtonId );

    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

private:

    enum TRowType
        {
        ELabelRow = 1,
        ESelectionRow
        };
    
    struct TRadioControl
        {
        /// Own: index on list box
        TInt iBeginIndex;
        /// Own: selected radio option starting from 1
        TInt iSelected;
        };
    
private: // new methods
    
    void SetIconsL();

    void SetItemsL();
    
    TInt RowType( TInt aIndex );

    void SwitchRadioButtonL();
    
    TBool IsRadioSelected( TInt aIndex );
    
    TInt IndexOfNewRadioControl();
    
    void ResolveConflicts();
    
    void SetNaviPaneL();
    
    void SetTitlePaneL( TBool aCustom );
    
private: // new methods
    
    /// Not own: Pointer for list control
    CEikColumnListBox* iListBox;
    /// Not own: Pointer for list control
    MPbk2MergeResolver* iConflictResolver;
    /// Own: Stores radio buttons controls information
    RArray <TRadioControl> iRadioControls;
    /// Own: Navi pane control
    CAknNavigationDecorator* iNaviDecorator;
    /// Own: Dialog buttons are set based on this value
    TBool iPhotoConflictExpected;
    };

#endif // CPBK2MERGECONFLICTSDLG_H
