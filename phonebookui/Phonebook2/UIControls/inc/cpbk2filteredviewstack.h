/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A stack for the base view and filtered views.
*
*/


#ifndef CPBK2FILTEREDVIEWSTACK_H
#define CPBK2FILTEREDVIEWSTACK_H

// INCLUDES
#include <e32base.h>
#include <MVPbkContactViewObserver.h>
#include <MVPbkContactViewBase.h>
#include <badesca.h>
#include "MPbk2FilteredViewStack.h"
// FORWARD DECLARATIONS
class CVPbkContactFindPolicy;
class MVPbkContactBookmarkCollection;
class MPbk2FilteredViewStackElement;
class CElementStack;

// CLASS DECLARATION

/**
 * A stack for the base view and filtered views that can be used
 * as a contact view. MVPbkContactViewBase reuqests are forwarded
 * to the top view.
 *
 * Responsible for:
 * -creating filtered views based on given find string.
 * -managing the created views, keeping them on stacks
 */
NONSHARABLE_CLASS(CPbk2FilteredViewStack) : public CBase,
                                            public MPbk2FilteredViewStack,
                                            public MVPbkContactViewObserverExtension,
                                            private MVPbkContactViewObserver
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aBaseView The non-filtered base view.
         * @param aConfigurationFlags a set of TConfigurationFlags
         * @return  A new instance of this class.
         */
        static CPbk2FilteredViewStack* NewL(
                MVPbkContactViewBase& aBaseView );

        /**
         * Destructor.
         */
        ~CPbk2FilteredViewStack();

    public: // Interface


        void UpdateFilterL(
                const MDesCArray& aFindStrings,
                const MVPbkContactBookmarkCollection* aAlwaysincluded,
                TBool aAlwaysIncludedChanged );

        void Reset();

        MVPbkContactViewBase& BaseView() const;


        void SetNewBaseViewL(
                MVPbkContactViewBase& aBaseView );


        TInt Level() const;

        void AddStackObserverL(
                MPbk2FilteredViewStackObserver& aStackObserver );


        void RemoveStackObserver(
                MPbk2FilteredViewStackObserver& aStackObserver );


    public:  // From MVPbkContactViewBase
        TVPbkContactViewType Type() const;
        void ChangeSortOrderL(
                const MVPbkFieldTypeList& aSortOrder );
        const MVPbkFieldTypeList& SortOrder() const;
        void RefreshL();
        TInt ContactCountL() const;
        const MVPbkViewContact& ContactAtL(
                TInt aIndex ) const;
        MVPbkContactLink* CreateLinkLC(
                TInt aIndex ) const;
        TInt IndexOfLinkL(
                const MVPbkContactLink& aContactLink ) const;
        void AddObserverL(
                MVPbkContactViewObserver& aObserver );
        void RemoveObserver(
                MVPbkContactViewObserver& aObserver );
        TBool MatchContactStore(
                const TDesC& aContactStoreUri ) const;
        TBool MatchContactStoreDomain(
                const TDesC& aContactStoreDomain ) const;
        MVPbkContactBookmark* CreateBookmarkLC(
                TInt aIndex ) const;
        TInt IndexOfBookmarkL(
                const MVPbkContactBookmark& aContactBookmark ) const;
        MVPbkContactViewFiltering* ViewFiltering();

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
        TAny* ContactViewObserverExtension(TUid aExtensionUid );

    private: // From MVPbkContactViewObserverExtension
        void FilteredContactRemovedFromView(
			 MVPbkContactViewBase& aView );

    private: // Implementation
        CPbk2FilteredViewStack();
        void ConstructL(
                MVPbkContactViewBase& aBaseView );
        MPbk2FilteredViewStackElement& TopElement() const;
        MVPbkContactViewBase& TopView() const;
        void DoHandleContactViewReadyL(
                MVPbkContactViewBase& aView );
        void UpdateStackL();
        class CCallback;
        void DoAddObserverL(
                MVPbkContactViewObserver& aObserver,
                CCallback& aCallback );
        void DoAddObserverError(
                TInt aError,
                CCallback& aCallback );
        void SendTopViewChangedEvent(
                MVPbkContactViewBase& aOldTopView );
        void SendTopViewUpdatedEvent();
        void SendBaseViewChangedEvent();
        void DeleteCallback(
                CCallback& aCallback );
        TInt CountLengthOfStrings(
                const MDesCArray& aStringArray ) const;
        MDesCArray* CopySearchWordsByLevelL(
                const MDesCArray& aStringArray,
                TInt aLevel );
        TBool IsElementsUnderDestruction();

    private: // Data
        /// Own: the stacked views that are currently used
        CElementStack* iViewStack;
        /// Own: Array construction views
        CElementStack* iConstructionStack;
        /// Own: An element that contains the base view
        MPbk2FilteredViewStackElement* iBaseViewElement;
        /// Ref: View observers
        RPointerArray<MVPbkContactViewObserver> iViewObservers;
        /// Ref: Stack observers
        RPointerArray<MPbk2FilteredViewStackObserver> iStackObservers;
        /// Own: View observer callbacks
        RPointerArray<CCallback> iCallbacks;
        /// Own: Contact find policy
        CVPbkContactFindPolicy* iFindPolicy;

 #ifdef _DEBUG
        void __DbgTestInvariant() const;
 #endif // _DEBUG
    };

#endif // CPBK2FILTEREDVIEWSTACK_H

// End of File
