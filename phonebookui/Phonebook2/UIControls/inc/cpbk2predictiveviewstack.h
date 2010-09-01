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
* Description:  Phonebook 2 Predictive search view stack.
*
*/

#ifndef CPBK2PREDICTIVEVIEWSTACK_H
#define CPBK2PREDICTIVEVIEWSTACK_H

// INCLUDES
#include <e32base.h>
#include <MVPbkContactViewObserver.h>
#include <MVPbkContactViewBase.h>
#include <badesca.h>
#include "MPbk2FilteredViewStack.h"
#include <MPsResultsObserver.h>
#include <CPsClientData.h>
#include <CPsPattern.h>
#include <CVPbkContactLinkArray.h>

// FORWARD DECLARATIONS
class CVPbkContactFindPolicy;
class MVPbkContactBookmarkCollection;
class CPSRequestHandler;
class CPsSettings;
class CVPbkContactIdConverter;
class CPsQuery;
class CVPbkTopContactManager;
class CPbk2ContactPositionInfo;
class CPbk2PredictiveSearchFilter;
class MPbk2ContactNameFormatter;

// CLASS DECLARATION
/**
 * Implements Predictive Searching for Pbk2 UI. Uses Predictive Search Engine.
 */
NONSHARABLE_CLASS(CPbk2PredictiveViewStack) : public CActive,
                                            public MPbk2FilteredViewStack,
                                            private MVPbkContactViewObserver,
                                            public MPsResultsObserver
    {
    public: // Constructors and destructor
            
        /**
         * Creates a new instance of this class.
         *
         * @param aBaseView The non-filtered base view.
         * @param aSearchFilter SearchPane Filter
         * @param aNameformatter PBk2 contact name formatter interface
         * @return  A new instance of this class.
         */
        static CPbk2PredictiveViewStack* NewL(
                MVPbkContactViewBase& aBaseView,
                CPbk2PredictiveSearchFilter& aSearchFilter,
                MPbk2ContactNameFormatter& aNameformatter );

        /**
         * Destructor.
         */
        ~CPbk2PredictiveViewStack();

    public: // From MPbk2FilteredViewStack

        void UpdateFilterL(
                const MDesCArray& aFindStrings, 
                const MVPbkContactBookmarkCollection* aAlwaysincluded,
                TBool aAlwaysIncludedChanged );
        
        void Reset();
        
        MVPbkContactViewBase& BaseView() const;
        
        void SetNewBaseViewL(
                MVPbkContactViewBase& aBaseView );
                
        TInt Level() const;

    public: // New methods

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

    private: // from MPsResultsObserver

        void HandlePsResultsUpdate(
            RPointerArray<CPsClientData>& searchResults,
            RPointerArray<CPsPattern>& searchSeqs);

        void HandlePsError(TInt aErrorCode);

        void CachingStatus(TCachingStatus& aStatus, TInt& aError);
        
    private: // From CActive
    
        void RunL();
        void DoCancel();
        TInt RunError( TInt aError );

    private: // Implementation
        CPbk2PredictiveViewStack( CPbk2PredictiveSearchFilter& aSearchFilter,
                MPbk2ContactNameFormatter& aNameformatter );

        void ConstructL(
                MVPbkContactViewBase& aBaseView );

        void InitializePCSEngineL();
        void HandleContactDeletionL(MVPbkContactViewBase& aView, TInt aIndex,
                const MVPbkContactLink& aContactLink);
        void CalculateSearchResultIndexesL(RPointerArray<CPsClientData>& aSearchResults);

        TInt BaseViewIndex( TInt aSearchResultIndex ) const;
        void SendTopViewChangedEvent( MVPbkContactViewBase& aOldTopView );
        void SendTopViewUpdatedEvent();
        void SendBaseViewChangedEvent();
        void ClearSearch();
        
    public:
        /**
         * Return last Predictive Search query.
         * This object is used in underlying and bookmark restoring.
         * All Predictive Search result is valid only for one query.
         * @return pointer to the last PCS query object.
         */
        const CPsQuery* LastPCSQuery() const;
        
        /**
        * Returns the instance of the request handler object for the Predicitve Search application
        * 
        * @return pointer to the CPSRequestHandler. 
        */
        CPSRequestHandler* PSHandler() const;
    
        /**
        * Get mathes part from data model object. 
        * Data model uses mathes patterns returned from PCS server.
        * This method is added for bidirectional languages.

        * @param aSearchData  The input data to be searched.
        * @param aMatchLocation The list matched index and length of match
        */
        void GetMatchingPartsL( const TDesC& aSearchData,
                              RArray<TPsMatchLocation>& aMatchLocation) const;
        
        /**
         * Sends no matched event of the last predictive search
         * to the search filter.        
         */
        void SendPSNoResultsEventL();
							  
        /**
         * Set the group contact link for curently open group. 
         *    
         * @param aGroupLinktoSet  The input group contact link
         */
        void SetCurrentGroupLinkL( MVPbkContactLink* aGroupLinktoSet);    
    private:        
        /**
         * Create copy of matches patterns from PCS server
         * Also creates patterns to be used in bidirectional matches functional
         */
        void CreatePatternsL(RPointerArray<CPsPattern>& searchSeqs);
		
	   
        // Perform the psengine settings.
        void SetPsSettingL( RPointerArray<TDesC>& aDatabases);
        
        /**
         * save marked contacts info to iMarkedContactsPositionInfoArray, marked
         * top contacts are excluded
         */
        void SaveBookmarkContatsInfoL();
        
        /**
         * move given array to top of searched result
         * @param aContactInfoArray array to be moved.
         */
        void MoveArrayToSearchedResultTopL( 
                RPointerArray<CPbk2ContactPositionInfo>& aContactInfoArray );
        
        /**
         * find from iTopContactPositionInfoArray by index of contact in iBaseView
         * @return KErrNotFound if not found
         */
        TInt MatchingTopContactFind( const TInt aIndexInBaseView );
        
        /**
         * sort the give array by position in main view.
         * @param aContactInfoArray array to be sorted.
         */
        void SortbyPositionInMainView( 
                RPointerArray<CPbk2ContactPositionInfo>& aContactInfoArray );
   
    public:
        
        /**
         * @param index of contact
         * @return true if the contact of index belongs to non matching marked contacts.
         */
        TBool IsNonMatchingMarkedContact( const TInt aIndex );
        
    private: // Data
        
        RPointerArray<MVPbkContactViewObserver> iViewObservers;// Ref: View observers
        
        RPointerArray<MPbk2FilteredViewStackObserver> iStackObservers;// Ref: Stack observers
        
        /**The base view of CPbk2PredictiveViewStack
        *  Not own
        */
        MVPbkContactViewBase* iBaseView; //The base view of CPbk2PredictiveViewStack
                
        TBool iViewReady;//ETrue if view is ready 
        
        TBool iSearchedState; //ETrue if Predictive search view is created
        
        /**Contact id converter for DefaultCntDbUri()
         * own
         */
        CVPbkContactIdConverter* iConverterDefaultStore; 
        
        /**Predictive search handler
         * own
         */
        CPSRequestHandler* iPsHandler; //Predictive search handler
        
        
        /** Query passed to PCS engine
         * own
         */
        CPsQuery* iPsQuery;//
        
        /**Contact link of searched contacts
         * own
         */
        CVPbkContactLinkArray*  iPredictiveSearchResultContactLinkArrray; //
        
        //Owns : Array that holds the result pattern for predictive search
        RPointerArray< CPsPattern > iPatternsCollection;
        
        /// Own: Array of bidirectional patterns
        RPointerArray<HBufC> iBidiPatterns;
		
	    /** 
		 * CVPbkTopContactManager used for checking if contact is top contact
		 * own
		 */
        CVPbkTopContactManager* iTopContactManager;
		
	    /**
         * temporary array contains top contact position infomation from iBaseView,
         * those top contacts are from PS search results
         * own
         */
        RPointerArray<CPbk2ContactPositionInfo> iTopContactPositionInfoArray;

        
        //Owns : Group contact link for the currently open Group
        MVPbkContactLink* iCurrentGroupLink;
		
	    /// Own: True if predictive search is enabled
        TBool iPredictiveSearch;
        
        /// Own: Text from search control 
        RBuf iSearchText;
		
	    /**
         * store marked contact's links
         * not own
         */
        MVPbkContactBookmarkCollection* iBookMarkCollection;
        
        /**
         * temporary array contains marked contact position infomation from iBaseView,
         * these contacts non matching contacts from search result.
         * own
         */
        RPointerArray<CPbk2ContactPositionInfo> iMarkedContactsPositionInfoArray;
        
        /** 
		 * Predictive search filter
		 * ref
		 */
        CPbk2PredictiveSearchFilter& iSearchFilter;
        
        ///Ref : PBk2 contact name formatter interface
        MPbk2ContactNameFormatter& iNameformatter;

        //Flag to indicate Feature manager initilization
        TBool iFeatureManagerInitilized ;
        
        //Stores the Start Index of the Marked Contact that is not
        //part of our searched contact list
        TInt iNonMatchedMarkedContactStartIndex; 
        
        //Stores the End Index of the Marked Contact that is not
        //part of our searched contact list
        TInt iNonMatchedMarkedContactEndIndex;
};

#endif // CPBK2_PREDICTIVEVIEWSTACK_H

// End of File
