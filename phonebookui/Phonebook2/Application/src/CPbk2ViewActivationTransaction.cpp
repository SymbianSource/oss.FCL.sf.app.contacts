/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 view activation transaction.
*
*/


#include "CPbk2ViewActivationTransaction.h"

// Phonebook 2
#include "CPbk2ViewExplorer.h"
#include "CPbk2AppUi.h"
#include <CPbk2TabGroupContainer.h>
#include <CPbk2ViewGraph.h>

// System includes
#include <aknnavi.h>
#include <aknnavide.h>
#include <akntabgrp.h>
#include <akntitle.h>
#include <akncontext.h>
#include <eikimage.h>
#include <eikspane.h>
#include <AknsUtils.h>

// --------------------------------------------------------------------------
// CPbk2ViewActivationTransaction::CPbk2ViewActivationTransaction
// --------------------------------------------------------------------------
//
CPbk2ViewActivationTransaction::CPbk2ViewActivationTransaction
        ( CPbk2AppUi& aAppUi, CPbk2ViewExplorer& aViewExplorer,
          const TUid& aViewId, const TVwsViewId& aPrevViewId,
          TUint aFlags ) :
            iAppUi( &aAppUi ),
            iViewExplorer( aViewExplorer ),
            iViewId( aViewId ),
            iPrevViewId( aPrevViewId ),
            iFlags( aFlags )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ViewActivationTransaction::~CPbk2ViewActivationTransaction
// --------------------------------------------------------------------------
//
CPbk2ViewActivationTransaction::~CPbk2ViewActivationTransaction()
    {
    // Rollback previous view (if necessary)
    Rollback();
    }

// --------------------------------------------------------------------------
// CPbk2ViewActivationTransaction::NewLC
// --------------------------------------------------------------------------
//
CPbk2ViewActivationTransaction* CPbk2ViewActivationTransaction::NewLC
        ( CPbk2AppUi& aAppUi, CPbk2ViewExplorer& aViewExplorer,
          const TUid& aViewId, const TVwsViewId& aPrevViewId,
          const TDesC* aTitlePaneText, const CEikImage* aContextPanePicture,
          TUint aFlags )
    {
    CPbk2ViewActivationTransaction* self =
        new ( ELeave ) CPbk2ViewActivationTransaction
            ( aAppUi, aViewExplorer, aViewId, aPrevViewId, aFlags );
    CleanupStack::PushL( self);
    self->ConstructL( aTitlePaneText, aContextPanePicture );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ViewActivationTransaction::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ViewActivationTransaction::ConstructL
        ( const TDesC* aTitlePaneText,
          const CEikImage* aContextPanePicture )
    {
    iNaviPane = static_cast<CAknNavigationControlContainer*>
        ( iAppUi->StatusPane()->ControlL
            ( TUid::Uid( EEikStatusPaneUidNavi ) ) );

    // Make changes to state and save rollback data
    UpdateViewNavigationState();

    if (iFlags & Phonebook2::EUpdateNaviPane)
        {
        UpdateNaviPaneL(iViewId);
        }
    if (iFlags & Phonebook2::EUpdateTitlePane)
        {
        SetTitlePaneTextL(aTitlePaneText);
        }
    if (iFlags & Phonebook2::EUpdateContextPane)
        {
        SetContextPanePictureL(aContextPanePicture);
        }
    }

// --------------------------------------------------------------------------
// CPbk2ViewActivationTransaction::Commit
// --------------------------------------------------------------------------
//
void CPbk2ViewActivationTransaction::Commit()
    {
    // Delete and reset all rollback data
    delete iSaveContextPanePicture;
    iSaveContextPanePicture = NULL;
    delete iSaveTitlePaneText;
    iSaveTitlePaneText = NULL;
    iPushedNaviDecorator = NULL;
    iTabGroup = NULL;
    iSavePreviousViewNode = NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ViewActivationTransaction::RollbackL
// --------------------------------------------------------------------------
//
void CPbk2ViewActivationTransaction::RollbackL()
    {
    // First rollback as in destructor
    Rollback();
    // Then switch back to previous view
    iViewExplorer.ActivatePreviousViewL( NULL );
    }

// --------------------------------------------------------------------------
// CPbk2ViewActivationTransaction::UpdateViewNavigationState
// --------------------------------------------------------------------------
//
void CPbk2ViewActivationTransaction::UpdateViewNavigationState()
    {
    // Reset saved state
    iSavePreviousViewNode = NULL;

    // Find activated view node in the graph
    CPbk2ViewNode* activatedNode =
        iViewExplorer.ViewGraph().FindNodeWithViewId( iViewId );

    if ( activatedNode )
        {
        // Set previous node for the newly activated view
        if ( iViewExplorer.IsPhonebook2View( iPrevViewId ) )
            {
            // Find the previous view in the graph
            CPbk2ViewNode* prevNode =
                iViewExplorer.ViewGraph().FindNodeWithViewId
                    ( iPrevViewId.iViewUid );
            while ( prevNode )
                {
                // Get transitions from the new view
                const TInt count = activatedNode->TransitionCount();
                TInt i;
                for (i = 0; i < count; ++i)
                    {
                    // Set previous view if there is a back transition
                    if ( activatedNode->TransitionAt(i).iEvent ==
                            EPbk2ViewTransitionBack &&
                         activatedNode->TransitionAt(i).iNode == prevNode )
                        {
                        iSavePreviousViewNode =
                            activatedNode->PreviousNode();
                        activatedNode->SetPreviousNode( prevNode );
                        break;
                        }
                    }
                if ( i >= count )
                    {
                    // Matching back transition not found: Move back in
                    // previous view chain
                    prevNode = prevNode->PreviousNode();
                    }
                else
                    {
                    // Transition found, exit while loop
                    break;
                    }
                }
            }
        else
            {
            iSavePreviousViewNode = activatedNode->PreviousNode();
            activatedNode->SetPreviousNode
                ( activatedNode->DefaultPreviousNode() );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ViewActivationTransaction::UpdateNaviPaneL
// --------------------------------------------------------------------------
//
void CPbk2ViewActivationTransaction::UpdateNaviPaneL( TUid aViewId )
    {
    // Reset saved state
    iPushedNaviDecorator = NULL;
    iTabGroup = NULL;
    iSaveTabIndex = 0;

    CAknNavigationDecorator* decorator=
        iAppUi->TabGroups()->TabGroupFromViewId( aViewId.iUid );

    if (decorator)
        {
        CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>
            ( decorator->DecoratedControl() );
        if (iNaviPane->Top() != decorator)
            {
            iNaviPane->PushL(*decorator);
            iPushedNaviDecorator = decorator;
            }
        iTabGroup = tabGroup;
        iSaveTabIndex = tabGroup->ActiveTabIndex();
        tabGroup->SetActiveTabById( aViewId.iUid );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ViewActivationTransaction::SetTitlePaneTextL
// --------------------------------------------------------------------------
//
void CPbk2ViewActivationTransaction::SetTitlePaneTextL
        ( const TDesC* aTitlePaneText )
    {
    // Reset saved state
    delete iSaveTitlePaneText;
    iSaveTitlePaneText = NULL;

    CEikStatusPane* statusPane = iAppUi->StatusPane();
    if (statusPane)
        {
        if (statusPane->PaneCapabilities(TUid::Uid(EEikStatusPaneUidTitle))
            .IsPresent())
            {
            iTitlePane = static_cast<CAknTitlePane*>
                (statusPane->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
            if (iTitlePane->Text())
                {
                iSaveTitlePaneText = iTitlePane->Text()->AllocL();
                }
            if (aTitlePaneText)
                {
                iTitlePane->SetTextL(*aTitlePaneText);
                }
            else
                {
                iTitlePane->SetTextToDefaultL();
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ViewActivationTransaction::SetContextPanePictureL
// --------------------------------------------------------------------------
//
void CPbk2ViewActivationTransaction::SetContextPanePictureL
        ( const CEikImage* aContextPanePicture )
    {
    // Reset saved state
    delete iSaveContextPanePicture;
    iSaveContextPanePicture = NULL;

    CEikStatusPane* statusPane = iAppUi->StatusPane();
    if (statusPane)
        {
        if (statusPane->PaneCapabilities
                (TUid::Uid(EEikStatusPaneUidContext))
                    .IsInCurrentLayout())
            {
            iContextPane = static_cast<CAknContextPane*>
                (statusPane->ControlL(TUid::Uid(EEikStatusPaneUidContext)));
            iSaveContextPanePicture = CopyImageL( iContextPane->Picture() );
            if (aContextPanePicture)
                {
                iContextPane->SetPicture
                    ( CopyImageL( *aContextPanePicture) );
                }
            else
                {
                iContextPane->SetPictureToDefaultL();
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ViewActivationTransaction::CopyImageL
// --------------------------------------------------------------------------
//
CEikImage* CPbk2ViewActivationTransaction::CopyImageL
        ( const CEikImage& aSrc )
    {
    CEikImage* copy = new ( ELeave ) CEikImage;
    CleanupStack::PushL( copy );
    copy->SetPictureOwnedExternally ( EFalse );
    CFbsBitmap* bitmap = new ( ELeave ) CFbsBitmap;
    copy->SetBitmap( bitmap );
    CFbsBitmap* mask = new ( ELeave ) CFbsBitmap;
    copy->SetMask( mask );
    User::LeaveIfError( bitmap->Duplicate( aSrc.Bitmap()->Handle() ) );
    User::LeaveIfError( mask->Duplicate( aSrc.Mask()->Handle() ) );
    CleanupStack::Pop( copy );
    return copy;
    }

// --------------------------------------------------------------------------
// CPbk2ViewActivationTransaction::Rollback
// --------------------------------------------------------------------------
//
void CPbk2ViewActivationTransaction::Rollback()
    {
    // Undo changes in reverse order
    if ( iContextPane && iSaveContextPanePicture )
        {
        // iContextPane takes ownership of iSaveContextPanePicture
        iContextPane->SetPicture( iSaveContextPanePicture );
        iSaveContextPanePicture = NULL;
        }
    if ( iTitlePane && iSaveTitlePaneText )
        {
        // iTitlePane takes ownership of iSaveTitlePaneText
        iTitlePane->SetText( iSaveTitlePaneText );
        iSaveTitlePaneText = NULL;
        }
    if ( iTabGroup && iSaveTabIndex != KErrNotFound )
        {
        iTabGroup->SetActiveTabByIndex( iSaveTabIndex );
        iSaveTabIndex = KErrNotFound;
        iTabGroup = NULL;
        }
    if ( iNaviPane && iPushedNaviDecorator )
        {
        iNaviPane->Pop( iPushedNaviDecorator );
        iPushedNaviDecorator = NULL;
        }
    if ( iSavePreviousViewNode )
        {
        CPbk2ViewNode* activatedNode =
            iViewExplorer.ViewGraph().FindNodeWithViewId( iViewId );
        if ( activatedNode )
            {
            activatedNode->SetPreviousNode( iSavePreviousViewNode );
            }
        iSavePreviousViewNode = NULL;
        }
    }

// End of File
