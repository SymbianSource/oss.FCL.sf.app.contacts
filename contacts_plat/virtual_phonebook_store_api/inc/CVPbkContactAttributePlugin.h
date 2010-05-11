/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An ECOM interface for the virtual phonebook contact attributes.
*                Contact attributes are e.g speed dial and defaults.
*
*/


#ifndef CVPBKCONTACTATTRIBUTEPLUGIN_H
#define CVPBKCONTACTATTRIBUTEPLUGIN_H

// INCLUDES FILES
#include <ecom/ecom.h>
#include <MVPbkContactAttributeManager.h>

// FORWARD DECLARATIONS
class CVPbkContactManager;
class MVPbkContactOperation;
class MVPbkContactFindObserver;
class MVPbkContactAttribute;

/**
 * ECom plugin interface for contact attribute plugins.
 */
class CVPbkContactAttributePlugin : 
        public CBase,
        public MVPbkContactAttributeManager
    {
    public: // Construction and destruction
        class TParam
            {
            public:
                /**
                 * Constructor.
                 *
                 * @param aContactManager  Reference to the contact manager.
                 */
                TParam(CVPbkContactManager& aContactManager);
            public:
                ///Ref: Contact manager
                CVPbkContactManager& iContactManager;
                ///Own: Spare for future extension
                TAny* iSpare;
            };
        
        /**
         * Two-phase constructor for the plugin.
         *
         * @param aUid    Plugin attribute type.
         * @param aParam  Plugin parameters, see
         *                CVPbkContactAttributePlugin::TParam.
         * @return Plugin instance.
         */
        static CVPbkContactAttributePlugin* NewL(TUid aUid, TParam& aParam);
        ~CVPbkContactAttributePlugin();
        
    public: // Interface
        /**
         * @return The attirbute type of this plugin.
         */
        TUid AttributeType() const;
        
        /**
         * Creates an operation object instance for listing contacts.
         *
         * @param aAttributeType  Attribute type for the operation.
         * @param aObserver       Operation observer.
         * @return  Operation object.
         */
        virtual MVPbkContactOperation* CreateListContactsOperationL(
            TUid aAttributeType, 
            MVPbkContactFindObserver& aObserver) =0;
            
        /**
         * Creates an operation object instance for listing contacts.
         *
         * @param aAttributeType  Attribute for the operation.
         * @param aObserver       Operation observer.
         * @return  Operation object.
         */
        virtual MVPbkContactOperation* CreateListContactsOperationL(
            const MVPbkContactAttribute& aAttribute, 
            MVPbkContactFindObserver& aObserver) =0;
        
    public: // From MVPbkContactAttributeManager
    
        /**
         * This functionality has been implemented in terms of
         * HasFieldAttributeL so plugins don't need to implement
         * it by their own.
         *
         * @param aAttr     Contact attributes to search for.
         * @param aContact  Contact from which to look for a field.
         * @return  Pointer to a field matcing the given attribute.
         * @exception KErrNotSupported The default implementation always
         *                             leaves with KErrNotSupported.
         */
        inline MVPbkStoreContactField* FindFieldWithAttributeL(
            const MVPbkContactAttribute& aAttr,
            MVPbkStoreContact& aContact) const;
        
    private: // Data
        ///Own: destructor id key
        TUid iDtorIDKey;
        ///Own: attribute type
        TUid iAttributeType;
    };
    
inline CVPbkContactAttributePlugin::TParam::TParam(
        CVPbkContactManager& aContactManager) :
    iContactManager(aContactManager)
    {
    }
    
inline CVPbkContactAttributePlugin* CVPbkContactAttributePlugin::NewL(
        TUid aUid, TParam& aParam)
    {
    TAny* ptr = NULL;
    ptr = REComSession::CreateImplementationL(
            aUid, 
            _FOFF(CVPbkContactAttributePlugin, iDtorIDKey),
            static_cast<TAny*>(&aParam));

    CVPbkContactAttributePlugin* plugin = 
        reinterpret_cast<CVPbkContactAttributePlugin*>(ptr);
    if (plugin)
        {
        plugin->iAttributeType = aUid;
        }

    return plugin;
    }
    
inline CVPbkContactAttributePlugin::~CVPbkContactAttributePlugin()
    {
    REComSession::DestroyedImplementation(iDtorIDKey);
    }
    
inline TUid CVPbkContactAttributePlugin::AttributeType() const
    {
    return iAttributeType;
    }

inline MVPbkStoreContactField* 
    CVPbkContactAttributePlugin::FindFieldWithAttributeL(
            const MVPbkContactAttribute& /*aAttr*/,
            MVPbkStoreContact& /*aContact*/) const
    {
    // This functionality has been implemented in terms of
    // HasFieldAttributeL so plugins don't need to implement
    // it by their own.
    User::Leave( KErrNotSupported );
    return NULL;
    }
#endif // CVPBKCONTACTATTRIBUTEPLUGIN_H

// End of File
