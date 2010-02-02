/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*       Implementation for CxSPSortView.
*
*/


// INCLUDE FILES
#include "CxSPSortView.h"
#include "CxSPSortViewControl.h"

// System includes
#include <AknGlobalNote.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <StringLoader.h>
#include <eikbtgpc.h>
#include <eikspane.h>
#include <avkon.hrh>
#include <avkon.rsg>

//Pbk2
#include <MPbk2AppUi.h>
#include <CPbk2UIExtensionView.h>
#include <MPbk2ViewActivationTransaction.h>
#include <MPbk2ViewExplorer.h>
#include <CPbk2ViewState.h>
#include <MPbk2StartupMonitor.h>
#include <Pbk2Debug.h>

// internal
#include "ExtensionManager.hrh"
#include <ExtensionManagerRes.rsg>
#include "ExtensionManagerUID.H"
#include "CxSPLoader.h"
#include "MxSPFactory.h"
#include "CxSPViewIdChanger.h"


using namespace Phonebook2;

// ================= MEMBER FUNCTIONS =======================

inline CxSPSortView::CxSPSortView( CxSPViewIdChanger& aViewIdChanger,
								   MxSPContactManager& aContactManager,
								   CPbk2UIExtensionView& aView,
								   CxSPArray& aExtensions ) :
									CxSPBaseView( aViewIdChanger, aContactManager, aView ),
									iViewIdChanger( aViewIdChanger ),
									iExtensions( aExtensions )
    {
    }

inline void CxSPSortView::ConstructL()
    {
    ViewBaseConstructL( KEXTENSIONMANAGERIMPLEMENTATIONUID,
    					EExtensionManagerSortViewId );        
    }

CxSPSortView* CxSPSortView::NewL( CxSPViewIdChanger& aViewIdChanger,
								   MxSPContactManager& aContactManager,
								   CPbk2UIExtensionView& aView,
								   CxSPArray& aExtensions )
    {
    CxSPSortView* self = new(ELeave) CxSPSortView( aViewIdChanger, 
    											   aContactManager,
    											   aView,
    											   aExtensions );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CxSPSortView::~CxSPSortView()
    {
    delete iNaviDecorator;
    if (iContainer)
	    {
        CCoeEnv::Static()->AppUi()->RemoveFromStack(iContainer);
        }
    delete iContainer;
    }

void CxSPSortView::HandleCommandL
        (TInt aCommand)
    {
    switch (aCommand)
        {
        case EAknSoftkeyExit: // Exit softkey
            {
            TBool changes = iContainer->CommitSortL( this );
            if( !changes )
            	{
        		Pbk2AppUi()->Pbk2ViewExplorer()->
            		ActivatePhonebook2ViewL( TUid::Uid(
                		EPbk2NamesListViewId ), NULL );
				}
            // No else. If there were changes in the sort order we will be
            // notified via MGlobalNoteObserver API when the user has closed
            // the resulting global note
            break;
            }
        case EExtensionManagerCmdMove: // Move softkey
        	{
        	iContainer->SetCurrentItemMarkedL( ETrue );
        	iView.Cba()->SetCommandSetL( R_AVKON_SOFTKEYS_OK_CANCEL );
        	iView.Cba()->DrawDeferred();
        	break;
        	}
        case EAknSoftkeyOk: // Ok softkey
        	{
        	iContainer->MoveMarkedItemL();
        	iView.Cba()->SetCommandSetL( R_EXTENSION_MANAGER_SORT_VIEW_SOFTKEYS );
        	iView.Cba()->DrawDeferred();
        	break;
        	}
        case EAknSoftkeyCancel: // Cancel softkey
        	{
        	iContainer->SetCurrentItemMarkedL( EFalse );
        	iView.Cba()->SetCommandSetL( R_EXTENSION_MANAGER_SORT_VIEW_SOFTKEYS );
        	iView.Cba()->DrawDeferred();
        	break;
        	}
        default:
            {
            Pbk2AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

void CxSPSortView::DoActivateViewL( const TVwsViewId& aPrevViewId,
                         TUid /*aCustomMessageId*/,
                         const TDesC8& /*aCustomMessage*/ )
	{
    MPbk2ViewActivationTransaction* viewActivationTransaction =
        Pbk2AppUi()->Pbk2ViewExplorer()->HandleViewActivationLC(
            iView.Id(),
            aPrevViewId,
            NULL,
            NULL,
            EUpdateAll );

    // Notifies startup monitor of view activation
    if ( Pbk2AppUi()->Pbk2StartupMonitor() )
        {
        Pbk2AppUi()->Pbk2StartupMonitor()->
            NotifyViewActivationL( iView.Id() );
        }
	 
    if (!iContainer)
        {
        iContainer = CxSPSortViewControl::NewL( iViewIdChanger, iExtensions );
        CCoeEnv::Static()->AppUi()->AddToStackL( iView, iContainer );        
        iContainer->SetMopParent( &iView );        
        iContainer->SetRect( iView.ClientRect() );
        iContainer->ActivateL();
        
        // Load the default cba for the sort view
        iView.Cba()->SetCommandSetL( R_EXTENSION_MANAGER_SORT_VIEW_SOFTKEYS );
        
        // Invoke DrawNow() to avoid the cba flicker
        iView.Cba()->DrawNow();
        } 

    // Commit application-wide state changes
    viewActivationTransaction->Commit();
    CleanupStack::PopAndDestroy(); //viewActivationTransaction
	
	// Remove the tabs from the navipane
    CEikStatusPane* sp = CEikonEnv::Static()->AppUiFactory()->StatusPane();
    CAknNavigationControlContainer* np =
        static_cast<CAknNavigationControlContainer*>(
            sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    delete iNaviDecorator;
    iNaviDecorator = NULL;

    iNaviDecorator = np->CreateNavigationLabelL();
    np->PushL( *iNaviDecorator );
    sp->DrawNow();
	}

void CxSPSortView::DoDeactivate()
	{
    if (iContainer)
        {
        CCoeEnv::Static()->AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        }

	if( iNaviDecorator )
		{
		// Remove the view-specific navipane decorations
	    CEikStatusPane* sp = CEikonEnv::Static()->AppUiFactory()->StatusPane();
	    TRAP_IGNORE( CAknNavigationControlContainer* np =
	         		 static_cast<CAknNavigationControlContainer*>(
	            	 sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi )));  // CSI: 42 #
			    	 np->Pop( iNaviDecorator );
				   );
		}
	}

void CxSPSortView::HandleStatusPaneSizeChange()
    {
    // Resize the container to fill the client rectangle
    if (iContainer)
        {
        iContainer->SetRect( iView.ClientRect() );
        }
    }

void CxSPSortView::GlobalNoteClosed( const TInt aResult )
    {
    PBK2_DEBUG_PRINT(
            PBK2_DEBUG_STRING("CxSPSortView::GlobalNoteClosed [%i]"), aResult );

    // This event results from a call to iContainer->CommitSortL when sorting
    // has been changed and the user has closed the resulting global note,
    // so we need to exit the application
    Pbk2AppUi()->ExitApplication();
    }

void CxSPSortView::GlobalNoteError( const TInt aErr )
    {
    PBK2_DEBUG_PRINT(
            PBK2_DEBUG_STRING("CxSPSortView::GlobalNoteError [%i]"), aErr );

    // This event results from a call to iContainer->CommitSortL when sorting
    // has been changed and there was a problem with the resulting global note,
    // so we need to exit the application.
    // We could also try to show the error to the user.
    Pbk2AppUi()->ExitApplication();
    }

//  End of File
