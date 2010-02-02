/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  BPAS information holder
*
*/



#ifndef C_BPASINFO_H
#define C_BPASINFO_H


class CBPASPresenceInfo;
class CBPASServiceInfo;
class CBPASIconInfo;


/**
 *  Bpas info class. This class contains buddy's xsp id, presence info,
 *  service info and icon info. Note that if there is no presence info service
 *  and icon infos are also not present, but buddy id is always present.
 *
 *  @lib bpas.lib
 *  @since S60 v5.0
 */
class CBPASInfo: public CBase
    {

public: 

    /**
     * Symbian constructors. Not exported since only used inside bpas
     *
     * @param aIdentity identity of the bpasinfo
     * @return CBPASInfo* newly created object
     */
    static CBPASInfo* NewL(const TDesC& aIdentity);
    static CBPASInfo* NewLC(const TDesC& aIdentity);

    /**
     * Public destructor. 
     */
    virtual ~CBPASInfo();
    
    /**
     * Returns information holder
     *
     * @return info or NULL               
     */    
    IMPORT_C CBPASPresenceInfo* PresenceInfo() const;
    
    /**
     * Returns information holder
     *
     * @return info or NULL               
     */  
    IMPORT_C CBPASServiceInfo* ServiceInfo() const;
    
    /**
     * Get identity string from presence info. The identity is in xsp format.
     *
     * @return identity, KNullDesC if the value doesn't exist
     */
    IMPORT_C const TPtrC Identity() const;
    
    /**
     * Returns information holder
     *
     * @return info or NULL               
     */  
    IMPORT_C CBPASIconInfo* IconInfo();
    
    /**
     * Finds whether this object contains presence.
     *
     * @return ETrue if this object contains presence info.
     */    
    IMPORT_C TBool HasPresence() const;
    
    /**
     * Set information holder
     *
     * @param aInfo [in] Takes ownership of the item               
     */  
    void SetPresenceInfoL( CBPASPresenceInfo* aPresenceInfo );
    
    /**
     * Set information holder
     *
     * @param aInfo [in] Takes ownership of the item               
     */  
    void SetServiceInfoL( CBPASServiceInfo* aServiceInfo );
    
    /**
     * Set information holder
     *
     * @param aInfo [in] Takes ownership of the item               
     */  
    void SetIconInfoL( CBPASIconInfo* aIconInfo );
    
    /**
     * Set identity in xsp format
     *
     * @param aIdentity to set.     
     */  
    void SetIdentityL(const TDesC& aIdentity );
  
     
private:
 
    CBPASInfo();
    void ConstructL(const TDesC& aIdentity);
    
private:

    //All owned
    HBufC*                  iIdentity;
    CBPASPresenceInfo*      iPresenceInfo;
    CBPASServiceInfo*	    iServiceInfo;// created only if presence info available
    CBPASIconInfo*          iIconInfo;// create only if service info and presence info available
    };


#endif // C_BPASINFO_H
