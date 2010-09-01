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
* Description:  Class that contains util-functions for logsserviceextension.
*
*/



#ifndef T_TLOGSEXTUTIL_H
#define T_TLOGSEXTUTIL_H


class RFile;

NONSHARABLE_CLASS(TLogsExtUtil) 
    {
  
public:

    /**
     * This function takes a service id and a element id as input arguments
     * and returns a file handle and bitmap ids of the requested bitmaps.
     * 
     * Note: Ownership of the file handle is transferred to the caller.
     *       Remember to close the file handle.
     *
     * @since S60 v3.2
     * @param aServiceId a service id
     * @param aElementId an element id, this is the element id defined in the 
     *        xml-sheet of the branding package, e.g."person:open:image"
     * @param aBitmapFile a file handle to the file that contains the bitmaps
     * @param aBitmapId a bitmap id
     * @param aBitmapMaskId a bitmapmask id   
     */
    static void GetFileHandleL( 
        const TUint32 aServiceId,
        const TDesC8& aElementId,    
        RFile& aBitmapFile,
        TInt& aBitmapId,   
        TInt& aBitmapMaskId );    

    };
        
#endif // T_TLOGSEXTUTIL_H
