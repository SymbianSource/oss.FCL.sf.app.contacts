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
* Description:  Class for handling popup query.
 *
*/


#ifndef C_CCACONTACTORPOPUPHANDLER_H
#define C_CCACONTACTORPOPUPHANDLER_H

#include <e32base.h>
#include <akninputblock.h>

//FORWARD DECLERATIONS
class CAiwServiceHandler;
class CCoeEnv;

/**
 *  CCCAContactorPopupHandler
 *  Handling launch of popup query.
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib ccacontactorservice
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CCCAContactorPopupHandler) :
    public CBase,
    public MAknInputBlockCancelHandler,
    public MAiwNotifyCallback
    {

public:

    /**
     * Two-phased constructor.
     */
    static CCCAContactorPopupHandler* NewL();

    /**
     * Destructor.
     */
    virtual ~CCCAContactorPopupHandler();

    /**
     * Launch popup.
     *
     * Leaves KErrNotFound if field data not found,
     *        KErrCancel if popup canceled
     *        KErrAbort if red key pressed
     *        KErrGeneral if error occurred
     *
     * @since S60 5.0
     * @param aCommAddressSelectType: Select Type.
     * @param aUseDefaults: Is default functionality enabled.
     */
    TDesC& LaunchPopupL(
        const TDesC8& aContactLinkArray,
        TAiwCommAddressSelectType aCommAddressSelectType,
        TBool aUseDefaults);

public:
    // from MAknInputBlockCancelHandler
    void AknInputBlockCancel();
    
private:

    // from base class MAiwNotifyCallback
    TInt HandleNotifyL(TInt aCmdId, TInt aEventId,
        CAiwGenericParamList& aEventParamList,
        const CAiwGenericParamList& aInParamList);

private:
    /**
     * Constructor.
     */
    CCCAContactorPopupHandler();
    void ConstructL();

private: //new methods
    void PrepareResourcesL();
    void ReadFieldDataL(CAiwGenericParamList& aEventParamList);
    void AsyncStop();

private:
    // data

    /**
     * Pointer to AIW service handler
     * Own.
     */
    CAiwServiceHandler* iServiceHandler;

    /**
     * Pointer to scheduler wait.
     * Own.
     */
    CActiveSchedulerWait* iSchedulerWait;

    /**
     * Environment.
     * Not Own.
     */
    CCoeEnv& iCoeEnv;

    /**
     * Offset of loaded resource file.
     * Own.
     */
    TInt iResourceOffset;

    /**
     * Result string from the AIW service.
     * Own.
     */
    HBufC* iQueryResults;

    /**
     * For error handling.
     * Own.
     */
    TInt iError;
    
    /**
     * Exit status.
     * Own.
     */
    TBool iCloseApp;

    };

#endif // C_CCACONTACTORPOPUPHANDLER_H

// End of File
