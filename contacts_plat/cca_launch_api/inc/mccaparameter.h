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
* Description:  This is a class for creating data of contact 
 *                for launching CCA application.
 *
*/


#ifndef M_MCCAPARAMETER_H
#define M_MCCAPARAMETER_H

#include <e32std.h>
#include <cntdef.h>
#include <MVPbkContactLink.h>

//Use this UID to get pointer to access interface MCCAConnectionExt. The MCCAConnectionExt 
//is used for special MCCAConnection binary compatiblity purposes only and it does not 
//relate to MCCAParameter. Ownership of returned MCCAConnectionExt needs to be taken.
//If MCCAConnectionExt cannot be provided because e.g. lack of memory, NULL is returned.
const TUid KMCCAConnectionExtUid = { 1 };

/** @file mccaparameter.h
 *   
 * 	MCCAParameter class is used to store, control and define how the CCA application is launched 
 *  with the contact specified data.
 *  
 *  Example usage of the API: 
 *  @code
 *   MCCAParameter* aParameter = TCCAFactory::NewParameterL()
 *   aParameter->SetConnectionFlag(ENormal);
 *   aParameter->SetContactDataFlag(EContactId);
 *   aParameter->SetContactDataL(aString);
 *   
 *   aParameter->SetLaunchedViewUid(aUid);
 *
 *  @endcode
 *
 *  @lib ccaclient
 *  @since S60 v5.0
 */
class MCCAParameter
    {

public:

    /** Flags for controlling opening and closing of CCA application */
    enum TConnectionFlags
        {
        // Default flag
        ENormal = 0x00000001,
		//Upon exit close only CCA
        ESoftExit = 0x00000002 
        };

    /** 
     * Flags for indicating type of contact data 
     *
     * Notes related to specific search types below:
     * EContactId
     * EContactLink
     */
    enum TContactDataFlags
        {
        EContactNone = 0,
        EContactId,         //Depricated; not supported anymore.
        EContactLink,
        EContactMSISDN,     //Depricated; not supported anymore.
        EContactDataModel   //Internal flag for Phonebook2
        };

    /**
     * Destroy this parameter entity and all its data.
     * @since S60 5.0
     */
    virtual void Close() = 0;

    /**
     * Getter for the flags of connection.
     * @since S60 5.0
     * @return the flags.
     */
    virtual TConnectionFlags ConnectionFlag() = 0;

    /**
     * Getter for the contact data flags.
     * @since S60 5.0
     * @return the flags.
     */
    virtual TContactDataFlags ContactDataFlag() = 0;

    /**
     * Getter for the contact data.
     * @leave KErrArgument if data not set.
     * @since S60 5.0
     * @return the reference of contact data.
     */
    virtual HBufC& ContactDataL() = 0;

    /**
     * Getter for the view uid.
     * @since S60 5.0
     * @return the Uid.
     */
    virtual TUid LaunchedViewUid() = 0;

    /**
     * Getter for the version.
     * @since S60 5.0
     * @return the version.
     */
    virtual TInt32 Version() = 0;

    /**
     * Setter for the flag of connection.
     *
     * @see TConnectionFlags     
     * @since S60 5.0
     * @param aFlags: flags of connection
     */
    virtual void
            SetConnectionFlag(const TConnectionFlags aConnectionFlags) = 0;

    /**
     * Setter for the contact data flag.
     *
     * @see TContactDataFlags     
     * @since S60 5.0
     * @param aFlags: flags of connection
     */
    virtual void
            SetContactDataFlag(const TContactDataFlags aContactDataFlags) = 0;

    /**
     * Setter for the contact data.
     * If you are using TContactDataFlags::EContactLink, the MVPbkContactLink
     * should be streamed with PackLC before given for this method.
     * Ownership is NOT transferred. 
     * @see MVPbkContactLink from Virtual phonebook. 
     * @leave KErrNotFound if contact data flag is not 
     *        set before calling this method.
     * @see MCCAParameter::SetContactDataFlags
     * @since S60 5.0
     * @param aContactData: given contact data.
     */
    virtual void SetContactDataL(const TDesC& aContactData) = 0;

    /**
     * Setter for the launched view uid. Use this when there
     * is need to get a particular view opened first on launch of CCA. 
     * If view uid is not set, CCA uses the default view. 
     * @since S60 5.0
     * @param aContactData: given contact data.
     */
    virtual void SetLaunchedViewUid(const TUid aUid) = 0;

    /**
     * Returns an extension point for this interface or NULL.
     * @param aExtensionUid Uid of extension
     * @return Extension point or NULL
     */
    virtual TAny* CcaParameterExtension( 
            TUid /*aExtensionUid*/ ) { return NULL; }

    };

#endif // M_MCCAPARAMETER_H
// End of File
