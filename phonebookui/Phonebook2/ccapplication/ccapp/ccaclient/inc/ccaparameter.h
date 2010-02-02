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

#ifndef C_CCAPARAMETER_H
#define C_CCAPARAMETER_H

#include <e32std.h>
#include "mccaparameter.h"


/**
 *  MCCAParameter
 *
 *  Class for creating data of contact. 
 *  @code
 *   MCCAParameter* aParameter = TCCAFactory::NewParameterL()
 *   aParameter->SetConnectionFlag(ENormal);
 *   aParameter->SetContactDataFlag(EContactId);
 *   aParameter->SetContactDataL(aString);
 *   aParameter->SetLaunchedViewUid(aUid);
 *   aParameter->SetVersion(1);
 *
 *  @endcode
 *
 *  @lib ccaclient
 *  @since S60 v5.0
 */
class CCCAParameter : public CBase, 
					  public MCCAParameter
    {

public:

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CCCAParameter* NewL( );

    /**
    * Destructor.
    */
    virtual ~CCCAParameter();

public: // new

    /**
     * For externalize data of parameter.
     *
     * @since S60 v5.0
     * @param aStream Stream where to externalize the data.
     */
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;

    /**
     * For internalize data of parameter.
     *
     * @since S60 v5.0
     * @param aStream Stream where to internalize the data.
     */
    IMPORT_C void InternalizeL( RReadStream& aStream );
    
    /**
     * Getter for the contact id.
     * If contact id not present, leaves with KErrNotFound.
     *
     * @since S60 5.0
     * @return the contact id.
     */
    IMPORT_C TInt32 ContactIdL();

    /**
     * Getter for the contact link.
     * If contact link not present, leaves with KErrNotFound.
     *
     * @since S60 5.0
     * @return the contact id.
     */
    IMPORT_C HBufC8* ContactLinkL();

// from base class MCCAParameter

    /**
     * From MCCAParameter
     * Destroy this parameter entity and all its data.
     * @since S60 5.0
     */
    virtual void Close();

    /**
     * From MCCAParameter
     * Getter for the flags of connection.
     * @since S60 5.0
     * @return the flags.
     */
    virtual TConnectionFlags ConnectionFlag();

    /**
     * From MCCAParameter
     * Getter for the contact data flags.
     * @since S60 5.0
     * @return the flags.
     */
    virtual TContactDataFlags ContactDataFlag();
    
    /**
     * From MCCAParameter
     * Getter for the contact data.
     * @leave KErrArgument if data not set.     
     * @since S60 5.0
     * @return the reference of contact data.
     */
    virtual HBufC& ContactDataL();
    
    /**
     * From MCCAParameter
     * Getter for the view uid.
     * @since S60 5.0
     * @return the Uid.
     */
    virtual TUid LaunchedViewUid();        
        
    /**
     * From MCCAParameter
     * Getter for the version.
     * @since S60 5.0
     * @return the version.
     */
    virtual TInt32 Version();
		
    /**
     * From MCCAParameter
     * Setter for the flag of connection.
     *
     * @see TConnectionFlags     
     * @since S60 5.0
     * @param aFlags: flags of connection
     */
    virtual void SetConnectionFlag( const TConnectionFlags aConnectionFlags );

    /**
     * From MCCAParameter
     * Setter for the contact data flag.
     *
     * @see TContactDataFlags     
     * @since S60 5.0
     * @param aFlags: flags of connection
     */
    virtual void SetContactDataFlag( const TContactDataFlags aContactDataFlags );

    /**
     * From MCCAParameter
     * Setter for the contact data.
     * If you are using TContactDataFlags::EContactLink, the MVPbkContactLink
     * should be streamed with PackLC before given for this method.
     * @see MVPbkContactLink from Virtual phonebook. 
     * Leaves KErrNotFound if contact data flag is not 
     *        set before calling this method.
     * @see MCCAParameter::SetContactDataFlags
     * @since S60 5.0
     * @param aContactData: given contact data.
     */
    virtual void SetContactDataL( const TDesC& aContactData );

    /**
     * From MCCAParameter
     * Setter for the launched view uid. Use this when there
     * is need to get a particular view opened first on launch of CCA. 
     * If view uid is not set, CCA uses the default view. 
     * @since S60 5.0
     * @param aContactData: given contact data.
     */
    virtual void SetLaunchedViewUid( const TUid aUid );

    /**
     * From MCCAParameter 
     * @see MCCAParameter::CcaParameterExtension 
     * @since S60 5.0
     */
    virtual TAny* CcaParameterExtension( TUid aExtensionUid );    
private:

    CCCAParameter();
    void ConstructL( );
    
private: // data

    /** Enum for externalize/internalize data */
    enum TProvidedData
        {
        EDataProvided = 1,
        EDataConnectionFlag = 2,
        EDataContactDataFlag = 4,
        EDataContactData = 8,
        EDataViewUid = 16,
        EDataVersion = 32
        };

    /**
     * Connection flag.
     * Own.  
     */
    TConnectionFlags iConnectionFlag;
    /**
     * Contact data flag.
     * Own.  
     */
    TContactDataFlags iContactDataFlag;    
    /**
     * Streamed contact data.
     * Own.  
     */
    HBufC* iContactData;    
    /**
     * View uid.
     * Own.  
     */
    TUid  iUid;
    /**
     * Versio.
     * Own.  
     */
    TInt32  iVersion;
    };

#endif// C_CCAPARAMETER_H

// End of File
