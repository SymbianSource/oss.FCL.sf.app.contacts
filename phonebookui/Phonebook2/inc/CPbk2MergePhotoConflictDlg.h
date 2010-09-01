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
#include <MPbk2ImageOperationObservers.h>

// CLASS DECLARATION
class CEikFormattedCellListBox;
class CAknNavigationDecorator;
class CFbsBitmap;
class CPbk2ImageManager;
class MPbk2ImageOperation;
class MVPbkBaseContact;
class CVPbkContactManager;

/**
 *  CPbk2MergePhotoConflictDlg
 * 
 */
NONSHARABLE_CLASS( CPbk2MergePhotoConflictDlg ) : public CAknDialog,
                                                  private MPbk2ImageGetObserver,
                                                  private MEikListBoxObserver
    {
public: // construction
    
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CPbk2MergePhotoConflictDlg* NewL( MVPbkBaseContact* aFirstContact,
                                                      MVPbkBaseContact* aSecondContact, 
                                                      TInt* aResult );
    
    /**
     * Destructor.
     */
    ~CPbk2MergePhotoConflictDlg();
        
private: // construction
    
    /**
     * Constructor for performing 1st stage construction
     */
    CPbk2MergePhotoConflictDlg( MVPbkBaseContact* aFirstContact,
                                MVPbkBaseContact* aSecondContact, 
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
    
    void SizeChanged();

private: //functions from MPbk2ImageGetObserver
    void Pbk2ImageGetComplete(
                    MPbk2ImageOperation& aOperation,
                    CFbsBitmap* aBitmap );
    
    void Pbk2ImageGetFailed(
                    MPbk2ImageOperation& aOperation,
                    TInt aError );
    
private: // new methods
    
    void SetIconsL();

    void SetItemsL();

    void SwitchRadioButtonL();
    
    void SetNaviPaneL();
    
    void SetTitlePaneL( TBool aCustom );
    
    void InitBitmapAsyncL( MVPbkBaseContact& aContact );
    
    void StopWait();
    
    void StartWait();
    
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
    /// Not own: contact to be merged
    MVPbkBaseContact* iFirstContact;
    /// not own: contact to be merged
    MVPbkBaseContact* iSecondContact;
    /// Not own: contact manager
    CVPbkContactManager* iContactManager;
    /// own: image manager
    CPbk2ImageManager* iImageManager;
    /// Own. Image operation for first contact
    MPbk2ImageOperation* iImageOperationFirst;
    
    /// Own. Image operation for first contact
    MPbk2ImageOperation* iImageOperationSecond;
    
    /// Own. size of image to be shown in photo conflict dialog
    TSize iSize;
    
    /// Own: Active scheduler waiter
    CActiveSchedulerWait* iWait;
   
    };

#endif // CPBK2MERGEPHOTOCONFLICTDLG_H
