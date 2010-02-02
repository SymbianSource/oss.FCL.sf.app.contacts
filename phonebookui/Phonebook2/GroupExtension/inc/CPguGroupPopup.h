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
* Description:  Phonebook 2 group popup.
*
*/


#ifndef CPGUGROUPPOPUP_H
#define CPGUGROUPPOPUP_H

//  INCLUDES
#include <e32base.h>
#include <eikcmobs.h>
#include <MVPbkContactViewObserver.h>
#include <MVPbkContactSelector.h>

//  FORWARD DECLARATIONS
class CEikColumnListBox;
class CPbk2InputAbsorber;
class MVPbkContactLink;
class MVPbkContactLinkArray;
class MVPbkContactViewBase;

// CLASS DECLARATION

/**
 * Phonebook 2 group popup.
 * Responsible for displaying a popup dialog which displays groups.
 */
class CPguGroupPopup : public CBase,
                       private MVPbkContactViewObserver,
                       private MVPbkContactSelector,
                       private MEikCommandObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aGroupsJoined     List of groups to which
         *                          contact belongs to.
         * @param aTitleResId       Title's resource id.
         * @param aEmptyTextResId   Empty text resource id.
         * @param aSoftKeyResId     Softkey resource id.
         * @param aListBoxFlags     Listbox flags.
         * @param aAllGroupsView    All groups view.
         * @return  A new instance of this class.
         */
        static CPguGroupPopup* NewL(
                MVPbkContactLinkArray* aGroupsJoined,
                TInt32 aTitleResId,
                TInt32 aEmptyTextResId,
                TInt32 aSoftKeyResId,
                TInt32 aListBoxFlags,
                MVPbkContactViewBase& aAllGroupsView );

        /**
         * Destructor.
         */
        ~CPguGroupPopup();

    public: // Interface

        /**
         * Executes the group selection dialog.
         * Returns a link to the selected group, or NULL if user cancelled
         * the dialog. Ownership is transferred to caller.
         *
         * @return  Link to the selected group.
         */
        MVPbkContactLink* ExecuteLD();

    private: // From MVPbkContactViewObserver
        void ContactViewReady(
                MVPbkContactViewBase& aView );
        void ContactViewUnavailable(
                MVPbkContactViewBase& aView );
        void ContactAddedToView(
                MVPbkContactViewBase& aView,
                TInt aIndex,
                const MVPbkContactLink& aContactLink );
        void ContactRemovedFromView(
                MVPbkContactViewBase& aView,
                TInt aIndex,
                const MVPbkContactLink& aContactLink );
        void ContactViewError(
                MVPbkContactViewBase& aView,
                TInt aError,
                TBool aErrorNotified );

    private: // From MEikCommandObserver
        void ProcessCommandL(
                TInt aCommandId );

    private: // From MVPbkContactSelector
        TBool IsContactIncluded(
                const MVPbkBaseContact& aContact );

    private: // Implementation
        CPguGroupPopup(
                MVPbkContactLinkArray* aGroupsJoined,
                TInt32 aTitleResId,
                TInt32 aEmptyTextResId,
                TInt32 aSoftKeyResId,
                TInt32 aListBoxFlags );
        void ConstructL(
                MVPbkContactViewBase& aAllGroupsView );
        void Close();
        void RunPopupL();

    private: // Data
        /// Own: View for the groups that the iStore contact belongs to
        MVPbkContactViewBase* iGroupView;
        /// Own: Title's resource id
        TInt32 iTitleResId;
        /// Own: ListBox's empty text resource id
        TInt32 iEmptyTextResId;
        /// Own: Softkeys resource id
        TInt32 iSoftKeyResId;
        /// Own: ListBox flags
        TInt32 iListBoxFlags;
        /// Own: Selected links
        MVPbkContactLink* iSelectedGroup;
        /// Own: Popup's result
        TInt iResult;
        /// Own: Input absorber
        CPbk2InputAbsorber* iInputAbsorber;
        /// Ref: Groups that the iStoreContact belongs to
        MVPbkContactLinkArray* iGroupsJoined;
    };

#endif // CPGUGROUPPOPUP_H

// End of File
