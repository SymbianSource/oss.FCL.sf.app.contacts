/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for IM Launcher protocol plug-ins.
*
*/

#ifndef CCMSCONTACTORIMPLUGINBASE_H
#define CCMSCONTACTORIMPLUGINBASE_H

#include <e32base.h>
#include <ecom/ecom.h>
#include <cmscontactorimpluginapi.h>
#include <cmscontactorimpluginifids.hrh>


/**
 *  Class is used to pass some parameter for CMS contactor
 *  
 *  @since S60 v5.0
 */
class MCmsContactorImPluginParameter
    {
public:
	
    /**
     * Returns the display name of the contact
     * @return the reference of the display name
     */
	virtual const TDesC& DisplayName() const = 0; 
    
    /**
     * Returns Streamed contact link array of contact who will be communicated at
     * @return the reference of ContactLink
     */
    virtual const TDesC8& ContactLinkArray() const = 0; 
    
    /**
     * Returns an extension point for this interface or NULL.
     * @param aExtensionUid Uid of extension
     * @return Extension point or NULL
     */
    virtual TAny* ParameterExtension
        ( TUid /*aExtensionUid*/ ) { return NULL; }
    
    /**
     * Destructor.
     */
    virtual ~MCmsContactorImPluginParameter() {}
    
    };

/**
 * Base class for CMS Contactor ECom implemnetation
 *
 * @since S60 v5.0
 */
class CCmsContactorImPluginBase : public CBase,
                                  public MCmsContactorImPluginApi
    {
public:
    // Implementation uid
    enum { KInterfaceId = CMS_CONTACTOR_INTERFACE_UID };


public:


    /**
     * Public destructor.
     * Plugin can be deleted through this interface.
     */
    inline ~CCmsContactorImPluginBase();



public:

    /**
     * Sets the plug-in destructor id.
     *
     * @param [in] aId
     *        Destructor id for the plug-in instance.
     */
    inline void SetDestructorId( TUid aId );
    
    /**
     * Sets parameter which provides the display name and contact link for CMS contactor.
     *
     * @param aParam provides the display name and contact link for CMS contactor,
     * the ownership of aParam is not taken
     */   
    inline void SetCmsParameter( MCmsContactorImPluginParameter* aParam );

protected:

    /**
     * C++ default constructor.
     */
    inline CCmsContactorImPluginBase();
    
    /**
     * Returns Cms Contactor Parameter
     * @return the point of Cms Contactor Parameter or NULL, the ownership is not taken
     */
    inline const MCmsContactorImPluginParameter* CmsParameter() const;


private: //Data

    /**
     * OWN: ECom destructor ID for Presence Protocol plug-in.
     * XIMPFw assignes the destructor ID during plug-in
     * object construction.
     */
    TUid iEcomDtorID;


    /**
     * REF: CmsContactor Parameter which provides the display name and 
     * contact link for CMS contactor
     */
    MCmsContactorImPluginParameter* iCmsParameter;


    /**
     * OWN: Reserved member for future extension.
     */
    TAny* iReserved2;


    /**
     * OWN: Reserved member for future extension.
     */
    TAny* iReserved3;
    };



// -----------------------------------------------------------------------------
// CCmsContactorImPluginBase::CCmsContactorImPluginBase()
// C++ default constructor.
// -----------------------------------------------------------------------------
//
inline CCmsContactorImPluginBase::CCmsContactorImPluginBase()
    {
    }


// -----------------------------------------------------------------------------
// CCmsContactorImPluginBase::~CCmsContactorImPluginBase()
// Destructor notifies the ECom from plug-in implementation destruction.
// -----------------------------------------------------------------------------
//
inline CCmsContactorImPluginBase::~CCmsContactorImPluginBase()
    {
    REComSession::DestroyedImplementation( iEcomDtorID );
    }


// -----------------------------------------------------------------------------
// CCmsContactorImPluginBase::SetDestructorId()
// -----------------------------------------------------------------------------
//
inline void CCmsContactorImPluginBase::SetDestructorId( TUid aId )
    {
    iEcomDtorID = aId;
    }

// -----------------------------------------------------------------------------
// CCmsContactorImPluginBase::SetCmsParameter()
// -----------------------------------------------------------------------------
//
inline void CCmsContactorImPluginBase::SetCmsParameter( 
        MCmsContactorImPluginParameter* aParam )
	{
	iCmsParameter = aParam;
	}

// -----------------------------------------------------------------------------
// CCmsContactorImPluginBase::CmsParameter()
// -----------------------------------------------------------------------------
//
inline const MCmsContactorImPluginParameter* CCmsContactorImPluginBase::CmsParameter()const
    {
    return iCmsParameter;
    }

#endif // CCMSCONTACTORIMPLUGINBASE_H
