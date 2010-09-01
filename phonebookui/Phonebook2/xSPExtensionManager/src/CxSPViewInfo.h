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
*       xSP view info
*
*/


#ifndef __CXSPVIEWINFO_H__
#define __CXSPVIEWINFO_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class TResourceReader;
class TNewViewIdInfo;
class MxSPViewInfo;

/**
 * xSP view info
 */
class CxSPViewInfo : public CBase
    {

    public:

        /**
         * Static factory function that performs the 2-phased construction.
         *
         * @param aReader Reference to resource reader to be used
         * @param aId Id of the component owning the view IDs
         * @return New instance of this class
         */
        static CxSPViewInfo* NewLC( TResourceReader& aReader, TUint32 aId );

        /**
         * Static factory function that performs the 2-phased construction.
         *
         * @param MxSPViewInfo Reference to view info got from extension
         * @param aId Id of the component owning the view IDs
         * @return New instance of this class
         */
        static CxSPViewInfo* NewLC( const MxSPViewInfo& aInfo, TUint32 aId );

        /**
         * Destructor.
         */
        ~CxSPViewInfo();

    public:

        /**
         * Gets the ID related this view
         *
         * @return ID related this view
         */
        TUint32 Id() const;

        /**
         * Gets the new mapped view ID
         *
         * @return New mapped view ID
         */
        TInt32 NewViewId() const;

        /**
         * Gets the old/original view ID
         *
         * @return Old/original view ID
         */
        TInt32 OldViewId() const;

        /**
         * Gets info whether this view is a tabbed view
         *
         * @return ETrue if this view uses a tab, otherwise EFalse
         */
        TBool TabView() const;

        /**
         * Gets the tab icon ID of this tab view
         *
         * @return The tab icon ID of this tab view
         */
        TInt32 TabIconId() const;

        /**
         * Gets the tab icon mask ID of this tab view
         *
         * @return The tab icon mask ID of this tab view
         */
        TInt32 TabMaskId() const;

        /**
         * Gets the tab icon file of this tab view
         *
         * @return The tab icon file of this tab view
         */
        const TDesC& TabIconFile() const;

        // ADDITIONS FOR TAB VIEWS SORTING

        /**
         * Gets the view name
         *
         * @return view name or KNullDesC
         */
        const TDesC& Name() const;

        /**
         * Gets the sort icon ID
         *
         * @return The sort icon ID or -1
         */
        TInt32 SortIconId() const;

        /**
         * Gets the sort icon mask ID
         *
         * @return The sort icon mask ID or -1
         */
        TInt32 SortMaskId() const;

        /**
         * Gets the sort icon file
         *
         * @return The sort icon file
         */
        const TDesC& SortIconFile() const;

    private:

    	friend class CxSPViewIdChanger;

        /**
         * Constructor.
         */
        CxSPViewInfo( TUint32 aId );

        /**
         * 2nd phase constructor.
         */
        void ConstructL( TResourceReader& aReader );

        /**
         * 2nd phase constructor.
         */
        void ConstructL( const MxSPViewInfo& aInfo );

        /**
         * Set new id
         *
         * @param aTabViewNewIds New view ID pool
         * @return error code
         */
        TInt SetNewId( RArray<TNewViewIdInfo>& aPool );

        /**
         * 	Check whether bitmap can be created out of given
         *	parameters
         *
         * @param aFileName name of the graphics file
         * @param aId bitmap id
         */
    	TBool IsBitmapCreatable( const TDesC& aFileName,
    									TInt aId );

    private: // Data

        // plugin id
        TUint32 iId;

        // new/mapped view id
        TInt32 iNewViewId;

        // old/original view id
        TInt32 iOldViewId;

        // flag whether view uses a tab
        TInt32 iTabView;

        // tab icon id
        TInt32 iTabIconId;

        // tab icon mask id
        TInt32 iTabMaskId;

        ///Own: tab icon file name
        HBufC* iTabIconFile;

        // ADDITIONS FOR TAB VIEWS SORTING

        ///Own: view name
        HBufC* iName;

        ///Own: sort icon id
        TInt32 iSortIconId;

        ///Own: sort mask id
        TInt32 iSortMaskId;

         ///Own: sort icon file name
        HBufC* iSortIconFile;
    };


#endif // __CXSPVIEWINFO_H__

// End of File
