/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      Implementation helper class for class CPbkContactEngine.
*
*/


#ifndef __CCONTACTDBCONNECTION_H__
#define __CCONTACTDBCONNECTION_H__

// INCLUDES
#include <e32base.h>        // CBase
#include <cntviewbase.h>    // MContactViewObserver

// FORWARD DECLARATIONS
class MContactDbObserver;
class CContactDatabase;
class CContactChangeNotifier;
class CContactViewBase;
class RContactViewSortOrder;
class CContactFilteredView;
class MPbkCompressionStrategy;
class CPbkEngineExtension;
class CPbkSortOrderManager;


// CLASS DECLARATION

/**
 * Helper class for holding the contact database connection related objects
 * of CPbkContactEngine.
 */
NONSHARABLE_CLASS(CPbkContactEngine::CContactDbConnection) :
        public CBase, 
        private MContactViewObserver
    {
    public: // Interface
        /**
         * Initialisation parameters for CContactDbConnection class.
         */
        struct TParams
            {
            /// Open file server connection
            RFs iFs;
            /// Database filename
            const TDesC* iDbFileName;
            /// Replace existing database
            TBool iReplaceExistingDb;
            /// CContactDatabase observer
            MContactDbObserver* iContactDbObserver;
            /// Phonebook field types
            CPbkFieldsInfo* iFieldsInfo;
            /// Phonebook engine extension
            CPbkEngineExtension* iExtension;
            /// Any error that occured when opening the database
            TInt* iDbOpenError;
			/// unnamed constact title
			const TDesC* iUnnamedTitle;
            // contact engine
            CPbkContactEngine* iEngine;
            };

        /**
         * Creates a new instance of this class.
         *
         * @param aParams   see CContactDbConnection::TParams.
         * @param aSettingsVisible Localization defined value, is
         * the settings menu item visible or not. Affects to the 
         * name sorting.
         */
        static CContactDbConnection* NewL(const TParams& aParams,
                                          TBool aSettingsVisible);

        /**
         * Destructor.
         */
        ~CContactDbConnection();

        /**
         * Returns the contact database.
         */
        CContactDatabase& Database() const;

        /**
         * Returns the all contacts view.
         */
        CContactViewBase& AllContactsView() const;

        /**
         * Returns the all groups view.
         */
        CContactViewBase& AllGroupsViewL();

        /**
         * Returns a contact view filtered by aFilter.
         *
         * @param aFilter   see CContactDatabase::TContactViewFilter.
         */
        CContactViewBase& FilteredContactsViewL(TInt aFilter);

        /**
         * Sets the compress UI.
         */
        void SetCompressUi(MPbkCompressUi* aCompressUi);

        /**
         * Returns ETrue if background database compression is currently 
         * enabled.
         */
        TBool IsCompressionEnabled() const;

        /**
         * Returns ETrue if the database could use a compression.
         */
        TBool CheckCompress();

        /**
         * Compresses the database asynchronously. Compress UI must be set
         * and give its permission to start compression. Calls back the 
         * compress UI on the compression progress.
         */
        void CompressL();

        /**
         * Cancels compression started in CompressL().
         */
        void CancelCompress() const;

        /**
         * Compacts the database synchronously if it recommends compression.
         * If file system space is under critical level after compaction, leaves
         * with KErrDiskFull.
         */
        void CheckFileSystemSpaceAndCompressL();

        /**
         * Returns true if file system space is low on the drive where contact
         * database is located.
         */
        TBool FileSpaceLowOnDbDrive() const;

		/**
		 * Changes the sort order. Contact views will be resorted which puts
		 * them into not ready state for a while.
		 * @param aSortOrder New sort order
		 */
		void ChangeSortOrderL(const RContactViewSortOrder& aSortOrder) const;

		/**
		 * Returns the name formatting object.
		 */
		MPbkContactNameFormat& ContactNameFormatter() const;

		/**
		 * Sets aNameOrder as the contact name ordering setting.
		 */
		void SetNameDisplayOrderL(TPbkNameOrder aNameOrder);

		/**
		 * Returns the current name displaying order.
		 */
		CPbkContactEngine::TPbkNameOrder NameDisplayOrder();

        /**
         * Returns the sort order manager.
         */
        const CPbkSortOrderManager& SortOrderManager() const;
        
		/**
		 * Sets the separator char to be used between last- and first names.
		 * If value of aSeparator= 0, then separator should not be used..
		 */		
        void SetNameSeparatorL(TChar aNameSeparator);

		/**
		 * Returns the separator char to be used between last- and first names.
		 * If value = 0, then separator should not be used between names.
		 */	
        TChar NameSeparator();        

    private:  // from MContactViewObserver
	    void HandleContactViewEvent
            (const CContactViewBase& aView,const TContactViewEvent& aEvent);

    private:  // Implementation
        CContactDbConnection();
        void ConstructL(const TParams& aParams, TBool aSettingsVisible);
        TBool CheckSystemTemplateL(const CPbkFieldsInfo& aFieldsInfo);
        void CreateAllContactsViewL(const RContactViewSortOrder& aSortOrder);
        void CloseSystemTemplate();
		void InitViewL(CContactViewBase& aView);

    private:  // Data
        /// Own: open file server connection
        mutable RFs iFs;
        /// Own: Contact database
        CContactDatabase* iContactDb;
        /// Own: database drive
        TDriveNumber iDbDrive;
        /// Own: opened system template during construction
        CContactItem* iSysTemplate;
        /// Own: Contact database change notifier
        CContactChangeNotifier* iContactChangeNotifier;
        /// Own: default contact view containing all contact cards
        CContactNamedRemoteView* iAllContactsView;
        /// Own: local group list
        CContactLocalView* iGroupsLocalView;
        struct TFilteredView
            {
            TInt iFilter;
            CContactFilteredView* iFilteredView;
            };
        /// Own: filtered contact views
        RArray<TFilteredView> iFilteredViews;
        /// Own: database compression strategy
        MPbkCompressionStrategy* iCompressionStrategy;
		/// Own: Sort order manager
		CPbkSortOrderManager* iSortOrderManager;
		/// Own: Phonebook contact name formatter
        MPbkContactNameFormat* iContactNameFormatter;
    };


inline CContactDatabase& CPbkContactEngine::CContactDbConnection::Database
        (  ) const
    {
    return(*iContactDb);
    }

inline CContactViewBase& CPbkContactEngine::CContactDbConnection::AllContactsView
        (  ) const
    {
    return(*iAllContactsView);
    }

inline const CPbkSortOrderManager& 
    CPbkContactEngine::CContactDbConnection::SortOrderManager() const
    {
    return (*iSortOrderManager);
    }

#endif // __CCONTACTDBCONNECTION_H__

// End of File
