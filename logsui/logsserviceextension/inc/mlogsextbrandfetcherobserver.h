/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An observer for the various fetchers.
*
*/



#ifndef M_MLOGSEXTBRANDFETCHEROBSERVER_H
#define M_MLOGSEXTBRANDFETCHEROBSERVER_H


class CFbsBitmap;

/**
 * Callback for the fetcher objects. Implemented by CLogsUiControlExtension
 * 
 * @lib logsserviceextension.dll
 * @since S60 v3.2
 */
class MLogsExtBrandFetcherObserver
    {
    public:         
        
        /**
         * Handles the brand fetcher's callback.(e.g.adds the icon to the 
         * listbox's icon array and stores the position of the added icon)
         *
         * @since S60 v3.2
         * @param aServiceId service id
         * @param aBitmapFile file handle to bitmap file
         * @param aBitmapId id of the presence image bitmap in the file
         * @param aBitmapMaskId id of the presence image mask in the file        
        **/
        virtual void BrandIconFetchDoneL( const TUint32 aServiceId,
                                          RFile& aBitmapFile,
                                          const TInt aBitmapId,   
                                          const TInt aBitmapMaskId) = 0;
                                          
    };

#endif // M_MLOGSEXTBRANDFETCHEROBSERVER_H
