/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*     Implements AIW assign provider.
*
*/


#ifndef __CPbkAiwCmdAssignProvider_H__
#define __CPbkAiwCmdAssignProvider_H__

//  INCLUDES
#include <RPbkResourceFile.h>
#include "CPbkAiwProviderBase.h"

//  FORWARD DECLARATIONS
class CPbkDrmManager;

//  CLASS DECLARATION

class CPbkAiwCmdAssignProvider :
        public CPbkAiwProviderBase
    {
    public: // constructor & destructor
        static CPbkAiwCmdAssignProvider* NewL();
        ~CPbkAiwCmdAssignProvider();

    private: // from CAiwServiceIfMenu
        void HandleServiceCmdL(const TInt& aCmdId,
                               const CAiwGenericParamList& aInParamList,
                               CAiwGenericParamList& aOutParamList,
                               TUint aCmdOptions = 0,
                               const MAiwNotifyCallback* aCallback = NULL);
        void InitializeMenuPaneL(CAiwMenuPane& aMenuPane,
                                 TInt aIndex,
                                 TInt aCascadeId,
                                 const CAiwGenericParamList& aInParamList);
        void HandleMenuCmdL(TInt aMenuCmdId,
                            const CAiwGenericParamList& aInParamList,
                            CAiwGenericParamList& aOutParamList,
                            TUint aCmdOptions = 0,
                            const MAiwNotifyCallback* aCallback = NULL);

    private: // implementation
        CPbkAiwCmdAssignProvider();
        void ConstructL();
        void CmdAssignL(
            TInt aMenuCmdId,
            const TDesC& aFileName,
            const CAiwGenericParamList& aInParamList,
            const MAiwNotifyCallback* aCallback);
        void VerifyMimeTypeL(
            TInt aMenuCmdId,
            const TDesC& aMimeTypeString);

        TInt CheckFileSizeLimit( const TDesC& aFileName );
        void GetMaxToneFileSizeL( TInt& aMaxSizeKB ) const;
        TInt CheckToneFileSize( const TDesC& aFile, TInt aSizeLimitKB );
        void ShowSizeErrorNoteL( TInt aSizeLimitKB ) const;

    private: // data
        //Own: DMR manager
        CPbkDrmManager* iDrmManager;
        /// Own: view resource file
        RPbkResourceFile iViewResourceFile;
        /// Own: Voice call ringing tone file size max value.
        TInt iToneFileSizeLimitKB;
    };

#endif // __CPbkAiwCmdAssignProvider_H__

// End of File
