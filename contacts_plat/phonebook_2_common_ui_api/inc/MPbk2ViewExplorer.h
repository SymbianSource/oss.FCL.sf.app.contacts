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
* Description:  Phonebook 2 view explorer interface.
*
*/


#ifndef MPBK2VIEWEXPLORER_H
#define MPBK2VIEWEXPLORER_H

// INCLUDES
#include <e32base.h>
#include <pbk2viewid.hrh>
#include <vwsdef.h> // TVwsViewId

// FORWARD DECLARATIONS
class MPbk2ViewActivationTransaction;
class CPbk2ViewState;
class CPbk2ViewGraph;
class CEikImage;

namespace Phonebook2 {

/**
 * Flags for HandleViewActivationLC.
 * @see HandleViewActivationLC
 */
enum TPbk2ViewActivationFlags
    {
    /// Update title pane
    EUpdateTitlePane    = 0x0001,
    /// Update context pane
    EUpdateContextPane  = 0x0002,
    /// Update navi pane
    EUpdateNaviPane     = 0x0004,
    /// Update all panes
    EUpdateAll          = 0xffff
    };

/// Names list view UID
static const TUid KPbk2NamesListViewUid =       { EPbk2NamesListViewId };
/// Contact info view UID
static const TUid KPbk2ContactInfoViewUid =     { EPbk2ContactInfoViewId };
/// Settings view UID
static const TUid KPbk2SettingsViewUid =        { EPbk2SettingsViewId };

} /// namespace


// CLASS DECLARATION

/**
 * Phonebook 2 view explorer interface.
 * View explorer can be used to activate views and to navigate
 * between the views.
 */
class MPbk2ViewExplorer
    {
    public:  // Interface

        /**
         * Returns true if given view id indicates a Phonebook 2 view.
         *
         * @param aViewId   The id of the view to query.
         * @return  ETrue if the view is a Phonebook 2 view,
         *          EFalse otherwise.
         */
        virtual TBool IsPhonebook2View(
                const TVwsViewId& aViewId ) const = 0;

        /**
         * Activates a Phonebook 2 view.
         *
         * @param aViewId       Id of the view to activate.
         * @param aViewState    The state to pass to the activated view,
         *                      can be NULL.
         * @see CAknViewAppUi::ActivateLocalViewL(TUid)
         */
        virtual void ActivatePhonebook2ViewL(
                TUid aViewId,
                const CPbk2ViewState* aViewState ) const = 0;

        /**
         * Activates the previous view.
         *
         * @param aViewState    Desired view state to restore, can be NULL.
         */
        virtual void ActivatePreviousViewL(
                const CPbk2ViewState* aViewState ) = 0;

        /**
         * Updates application-level objects (view navigation state and
         * status pane) leave-safely at view activation. Called from each
         * application view's DoActivateL().
         *
         * @param aViewId               The activated view's id.
         * @param aPrevViewId           Previously active view's id.
         * @param aTitlePaneText        Text to set to title pane. If NULL
         *                              default text is used.
         * @param aContextPanePicture   Picture to set into context pane.
         *                              If NULL default picture is used.
         * @param aFlags                Combination of TPbk2ViewActivationFlags
         *                              telling which panes are to be updated.
         * @return  A transaction object. Commit the object before
         *          destroying it to keep the changes to application-level
         *          objects.
         */
        virtual MPbk2ViewActivationTransaction* HandleViewActivationLC(
                const TUid& aViewId,
                const TVwsViewId& aPrevViewId,
                const TDesC* aTitlePaneText,
                const CEikImage* aContextPanePicture,
                TUint aFlags ) =0;

        /**
         * Returns Phonebook 2 view graph object.
         *
         * @return  View graph.
         */
        virtual CPbk2ViewGraph& ViewGraph() const = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* ViewExplorerExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }

    protected: // Disabled functions
        virtual ~MPbk2ViewExplorer()
                {}
    };

#endif // MPBK2VIEWEXPLORER_H

// End of File
