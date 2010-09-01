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
* Description:  A class for folding view that can be expanded to its subview
*
*/


#ifndef CVPBKFOLDINGCONTACTVIEW_H
#define CVPBKFOLDINGCONTACTVIEW_H

#include <e32base.h>
#include <MVPbkContactViewBase.h>
#include <MVPbkExpandable.h>

#include "VPbkContactView.hrh"
#include <CVPbkAsyncOperation.h>

class CVPbkFoldingViewContact;
class MVPbkContactStore;
class MVPbkContactObserver;
class CVPbkContactViewDefinition;
class CVPbkContactManager;

/**
 *  A class for folding view that can be expanded to its subview
 *
 *  From the client point of view the folding view is a view contact
 *  that can be expanded to another view. The folding view must
 *  have one subview definition that is used to create the
 *  expanded view
 */
NONSHARABLE_CLASS(CVPbkFoldingContactView) : 
        public CBase,
        public MVPbkContactViewBase,
        public MVPbkExpandable
	{
public:

    /**
    * Returns new CVPbkFoldingContactView instance
    *
    * @param aObserver Observer of this class
    * @param aViewDefinition view definition
    * @param aContactManager contact manager
    * @param aSortOrder sort order
    * @return instance of CVPbkFoldingContactView
    */
    static CVPbkFoldingContactView* NewLC(
        MVPbkContactViewObserver& aObserver,
        const CVPbkContactViewDefinition& aViewDefinition,
        const CVPbkContactManager& aContactManager,
        const MVPbkFieldTypeList& aSortOrder);
    ~CVPbkFoldingContactView();

    /**
     * Returns the name of the view that is visible to client
     *
     * @return the name of the view that is visible to client
     */
    const TDesC& Name() const;
        
public: // from base class MVPbkContactViewBase
    TVPbkContactViewType Type() const;
    void ChangeSortOrderL(const MVPbkFieldTypeList& aSortOrder);
    const MVPbkFieldTypeList& SortOrder() const;
    void RefreshL();
    TInt ContactCountL() const;
    const MVPbkViewContact& ContactAtL(TInt aIndex) const;
    MVPbkContactLink* CreateLinkLC(TInt aIndex) const;
    TInt IndexOfLinkL(const MVPbkContactLink& aContactLink) const;
    void AddObserverL(MVPbkContactViewObserver& aObserver);
    void RemoveObserver(MVPbkContactViewObserver& aObserver);
    TBool MatchContactStore(const TDesC& aContactStoreUri) const;
    TBool MatchContactStoreDomain(const TDesC& aContactStoreDomain) const;
    MVPbkContactBookmark* CreateBookmarkLC(TInt aIndex) const;
    TInt IndexOfBookmarkL(
        const MVPbkContactBookmark& aContactBookmark) const;
    MVPbkContactViewFiltering* ViewFiltering();
    
public: // from base class MVPbkExpandable
    MVPbkContactViewBase* ExpandLC(
        MVPbkContactViewObserver& aObserver,
        const MVPbkFieldTypeList& aSortOrder) const;

private: // Implementation
    CVPbkFoldingContactView(
            const CVPbkContactManager& aContactManager);
    void ConstructL(
            const CVPbkContactViewDefinition& aViewDefinition,
            const MVPbkFieldTypeList& aSortOrder);
    void DoAddObserverL(MVPbkContactViewObserver& aObserver);
    void AddObserverError(MVPbkContactViewObserver& aObserver, TInt aError);
    void SendAsyncUnavailableAndReadyEventL();
    void DoSignalObserversViewReady(MVPbkContactViewObserver& aObserver);
    void DoSignalObserversViewUnavailable(MVPbkContactViewObserver& aObserver);
    void DoSignalObserversViewError(MVPbkContactViewObserver& aObserver, 
            TInt aError);

private: // Data
    /// Ref: the contact manager
    const CVPbkContactManager& iContactManager;
    /// Own: the view definition of this view.
    CVPbkContactViewDefinition* iViewDefinition;
    /// Own: The one and only view contact
    CVPbkFoldingViewContact* iFoldingContact;
    /// Own: the sort order.
    MVPbkFieldTypeList* iSortOrder;
    /// Ref: observers of the view
    RPointerArray<MVPbkContactViewObserver> iObservers;
    /// Own: an async operation for compliting async functions
    /// in VPbk interface
    VPbkEngUtils::CVPbkAsyncOperation iAsyncOperation;
    };

#endif // CVPBKFOLDINGCONTACTVIEW_H
//End of file


