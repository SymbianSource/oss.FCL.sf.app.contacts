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
*       view id changer
*
*/


#ifndef __CXSPVIEWIDCHANGER_H__
#define __CXSPVIEWIDCHANGER_H__

// INCLUDES
#include <e32base.h>
#include "MxSPViewMapper.h"
#include "TNewViewIdInfo.h"

// FORWARD DECLARATIONS
class MxSPFactory;
class CxSPViewInfo;


/**
 * View id changer
 */
class CxSPViewIdChanger : public CBase,
							public MxSPViewMapper
    {
#ifdef T_CXSPVIEWIDCHANGER_H
    friend class T_CxSPViewIdChannger;
#endif// T_CXSPVIEWIDCHANGER_H       

    public:
        /**
		 * Static factory function that performs the 2-phased construction.
         *
         * @return New instance of this class
         */
        static CxSPViewIdChanger* NewL();

        /**
         * Destructor.
         */
        ~CxSPViewIdChanger();

    public: // from MxSPViewMapper

    	TInt GetOldView( TUint32 aId, TInt aNewView, TInt& aOldView ) const;
        TInt GetNewView( TUint32 aId, TInt aOldView, TInt& aNewView ) const;

    public:

		/**
		 * Performs the view ID change operation for given extensions
		 *
		 * @param aFactories Array of extension factories
		 */
		void ChangeViewIdsL( CArrayPtrFlat<MxSPFactory>& aFactories );

		/**
		 * Gets the total number of views
		 *
		 * @param aTabs Number of tab views
		 * @param aOthres Number of other views
		 */
		void GetViewCount( TInt &aTabs, TInt &aOthers ) const;

		/**
		 * Gets a tab view's info
		 *
		 * @param aId ID related to the wanted tab view
		 * @param aInfoArray View info
		 */
		void GetTabViewInfoL( TUint32 aId, RPointerArray<CxSPViewInfo>& aInfoArray ) const;

		/**
		 * Gets all extensions tab view's info
		 *
		 * @param aInfoArray View info
		 */
		void GetTabViewInfoL( RPointerArray<CxSPViewInfo>& aInfoArray ) const;

        /**
         * Map Extension Manager's view IDs
         */
        void ChangeManagerViewIdsL();
        /**
         * write tab view names to Central Repository.
         */
        void WriteTabViewNamesToRepositoryL();

    private:

    	/**
         * Constructor.
         */
        CxSPViewIdChanger();

        /**
         * 2nd phase constructor.
         */
        void ConstructL();

    private: // Data

        /// array of tab view ids
        RArray<TNewViewIdInfo> iTabViewNewIds;
        /// array of non-tab view ids
        RArray<TNewViewIdInfo> iNonTabViewNewIds;

        /// Array of view infos
        RPointerArray<CxSPViewInfo> iViewIdMap;
    };



#endif // __CXSPVIEWIDCHANGER_H__

// End of File
