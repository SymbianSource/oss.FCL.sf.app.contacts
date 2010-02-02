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
* Description:  Provides Image conflict resolution dialog
*               for Contact Merge command.
*
*/

#ifndef CPBK2MERGEPHOTOCONFLICTDLG_H
#define CPBK2MERGEPHOTOCONFLICTDLG_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <AknDialog.h>
#include <eiklbo.h>

// CLASS DECLARATION
class CEikFormattedCellListBox;
class CAknNavigationDecorator;
class CFbsBitmap;

/**
 *  CPbk2MergePhotoConflictDlg
 * 
 */
NONSHARABLE_CLASS( CPbk2MergePhotoConflictDlg ) : public CAknDialog,
                                                  private MEikListBoxObserver
    {
public: // construction
    
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CPbk2MergePhotoConflictDlg* NewL( CFbsBitmap* aFirstImage,
                                                      CFbsBitmap* aSecondImage, 
                                                      TInt* aResult );
    
    /**
     * Destructor.
     */
    ~CPbk2MergePhotoConflictDlg();
        
private: // construction
    
    /**
     * Constructor for performing 1st stage construction
     */
    CPbk2MergePhotoConflictDlg( CFbsBitmap* aFirstImage,
                                CFbsBitmap* aSecondImage, 
                                TInt* aResult );
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
    
private: // new methods
    
    void SetIconsL();

    void SetItemsL();

    void SwitchRadioButtonL();
    
    void SetNaviPaneL();
    
    void SetTitlePaneL( TBool aCustom );
    
private: // new methods
    
    /// Not own: Pointer for list control
    CEikFormattedCellListBox* iListBox;
    /// Own: Navi pane control
    CAknNavigationDecorator* iNaviDecorator;
    /// Not own: first bitmap to display
    CFbsBitmap* iFirstImage; 
    /// Not own: second bitmap to display
    CFbsBitmap* iSecondImage;
    /// Not own: index of selected image
    TInt* iSelectedItem;
    /// Own: Selection indicator string
    HBufC* iSelectedString;
    };

#endif // CPBK2MERGEPHOTOCONFLICTDLG_H
