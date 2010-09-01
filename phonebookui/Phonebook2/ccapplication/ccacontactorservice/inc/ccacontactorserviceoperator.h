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
* Description:  Class for handling service logic.
 *
*/


#ifndef C_CCACONTACTORSERVICEHANDLER_H
#define C_CCACONTACTORSERVICEHANDLER_H

#include <e32std.h>
#include <e32base.h>

//FORWARD DECLERATIONS
class CCCAContactorPopupHandler;

/**
 *  CCCAContactorServiceOperator
 *  Handling logic of service.
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib ccacontactorservice
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CCCAContactorServiceOperator) : public CBase
    {

public:

    /**
     * Two-phased constructor.
     */
    static CCCAContactorServiceOperator* NewLC();

    static CCCAContactorServiceOperator* NewL();

    /**
     * Destructor.
     */
    virtual ~CCCAContactorServiceOperator();

public:
    //new methods

    /**
     * Start service.
     * All leaves are trapped.
     *
     * @since S60 5.0
     */
    void Execute(const CCAContactorService::TCSParameter& aParameter);
    
    /**
     * Is call item selected
     * 
     * @since S60 5.0
     */
    TBool IsSelected();
private:
    /**
     * Constructor.
     */
    CCCAContactorServiceOperator();

    void ConstructL();

private: //new methods
    /**
     * Helper class for handling special cases.
     * Currently VOIP ServiceID implemented.
     *
     * @param aFieldData: Selected field.
     * @since S60 5.0
     */
    void ResolveSpecialCasesL(const TDesC& aFieldData);

    /**
     * Helper class for searching ServiceID
     *
     * @param aFieldData: Selected field.
     * @return ServiceId
     * @since S60 5.0
     */
    TUint32 ResolveServiceIdL(const TDesC& aFieldData);

    /**
     * Helper class for parsing xSP identification.
     *
     * @param aFieldData: Selected field.
     * @param aXSPId: Found xSP ID.
     * @return boolean if xSP is found.
     * @since S60 5.0
     */
    TBool ExtractServiceL(const TDesC& aFieldData, TPtrC& aXSPId);

    /**
     * Helper class for searching ServiceId from spsettings
     *
     * @param aFieldData: Selected field.
     * @return ServiceId
     * @since S60 5.0
     */
    TUint32 SearchServiceIdL(const TDesC& aFieldData);

    /**
     * Helper class for checking if MSISDN addressing is supported.
     *
     * @return ServiceId
     * @since S60 5.0
     */
    TUint32 ResolveMSISDNAddressingSupportedL();

    /**
     * Helper class for starting communication method.
     *
     * @param aFieldData: Selected field.
     * @since S60 5.0
     */
    void LaunchCommunicationMethodL(const TDesC& aFieldData);

    /**
     * Helper class for mapping AIW enum & selector factory enums together.
     *
     * @param aCommTypeSelector: Selector factory enum.
     * @return AIW communication enum.
     * @since S60 5.0
     */
    TAiwCommAddressSelectType CommunicationEnumMapper(
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aCommTypeSelector);


private:
    // data

    /**
     * Pointer to popup handler class
     * Own.
     */
    CCCAContactorPopupHandler* iPopupHandler;

    /**
     * Reference to the launch parameter.
     * Not own.
     */
    const CCAContactorService::TCSParameter* iParameter;

    /**
     * ServiceId for VOIP service.
     * Own.
     */
    TUint32 iServiceId;
    
    /**
     * Is call item selected
     * Own
     */
    TBool isSelected;
    };

#endif // C_CCACONTACTORSERVICEHANDLER_H

// End of File
