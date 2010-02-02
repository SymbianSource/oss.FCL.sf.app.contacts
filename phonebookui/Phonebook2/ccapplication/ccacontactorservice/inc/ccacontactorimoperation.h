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
* Description:  Class implementing the instant messaging operation
*
*/


#ifndef CCACONTACTORIMOPERATION_H
#define CCACONTACTORIMOPERATION_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>


// CLASS DECLARATION
class CCmsContactorImPluginBase;
class CSPSettings;
class CCCAContactorOperation;
class MCmsContactorImPluginParameter;

/**
 * Class implementing CMS contactor IM plugin parameter
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib
 *  @since S60 v5.0
 */
class CCmsContactorImPluginParameter: 
    public CBase, 
    public MCmsContactorImPluginParameter
    {
public:

    /**
     * Two-phased constructor.
     * 
     * @param aDisplayName the display name of the contact
     * @param aContactLinkArray Streamed contact link array of contact to be communicated at
     */
    static CCmsContactorImPluginParameter* NewL(
		   const TDesC& aDisplayName, const TDesC8& aContactLinkArray );

    /**
     * Two-phased constructor.
     * 
     * @param aDisplayName the display name of the contact
     * @param aContactLinkArray Streamed contact link array of contact to be communicated at
     */
    static CCmsContactorImPluginParameter* NewLC(
		   const TDesC& aDisplayName, const TDesC8& aContactLinkArray );
    
    /**
    * Destructor.
    */ 
    virtual ~CCmsContactorImPluginParameter();

public: // From MCmsContactorImPluginParameter
	
    /**
     * Returns the display name of the contact
     * @return the reference of the display name
     */
    const TDesC& DisplayName() const; 
    
    /**
     * Returns Streamed contact link array of contact who will be communicated at
     * @return the reference of ContactLink
     */
    const TDesC8& ContactLinkArray() const; 
    
    /**
     * Returns an extension point for this interface or NULL.
     * @param aExtensionUid Uid of extension
     * @return Extension point or NULL
     */
    virtual TAny* ParameterExtension( TUid aExtensionUid );  
    
private: // Implementation

    CCmsContactorImPluginParameter( );
    void ConstructL( const TDesC& aDisplayName, const TDesC8& aContactLinkArray);
      
private: // data

    /*
     * Streamed contact link array of contact who will be communicated at.
     * OWN
     */
    HBufC8* iContactLinkArray; 
    
    /*
     * the display name of the contact.
     * OWN
     */
    HBufC* iDisplayName;
    };


/**
 * Class implementing the instant messaging operation
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CCCAContactorIMOperation) : public CCCAContactorOperation
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CCCAContactorIMOperation();

    /**
     * Two-phased constructor.
     * 
     * @param aParam The IM "address" to send the message to
     * @param aCmsParameter The contact link array and display name of conatct 
     * to be communicated at, the ownership of aCmsParameter is taken
     */
    static CCCAContactorIMOperation* NewL(const TDesC& aParam, 
    		MCmsContactorImPluginParameter* aCmsParameter );

    /**
     * Two-phased constructor.
     * 
     * @param aParam The IM "address" to send the message to
     * @param aCmsParameter The contact link array and display name of conatct 
     * to be communicated at, the ownership of aCmsParameter is taken
     */
    static CCCAContactorIMOperation* NewLC(const TDesC& aParam,
    		MCmsContactorImPluginParameter* aCmsParameter);
    
    /**
     * Executes the operation (opens the message editor)
     */
    virtual void ExecuteLD();

private:

    /**
     * Constructor for performing 1st stage construction
     * 
     * @param aParam The IM "address" to send the message to
     * @param aCmsParameter The contact link array and display name of conatct 
     * who will be communicated at,the ownership of aCmsParameter is taken
     */
    CCCAContactorIMOperation(const TDesC& aParam, 
    		MCmsContactorImPluginParameter* aCmsParameter);

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();
    
private:
    /**
     * Resolve IM Launcher ECom UID
     * 
     * @since S60 v5.0     
     * @param aServiceId service id 
     * @return ECom UID
     */ 
    TUid ResolveEcomUidL( const TDesC& aServiceId );     
    
    /**
     * Resolve IM Launcher ECom UID
     * 
     * @since S60 v5.0     
     * @param aServiceId service id 
     * @return Ecom UId in interegr
     */ 
    TInt DoResolveEcomUidL( const TDesC& aServiceId );     
                    
    /**
     * Load the ECom plugin
     * 
     * @since S60 v5.0       
     * @param aUidImp implementation UID
     * @param aXspId service provider id
     */
    void LoadEcomL( TUid aUidImp, const TDesC& aXspId );
    
    /**
     * Extracts service part from XspId
     * 
     * @since S60 v5.0       
     * @param aXspId service provider id   
     * @return service part (uri scheme part)
     */
    TPtrC ExtractService( const TDesC& aXspId );
private:
    
    /**
     * Im Plugin. 
     * OWN
     */
    CCmsContactorImPluginBase* iPlugin;        

    /**
     * Service Provider Settings
     * OWN
     */
    CSPSettings* iSPSettings;

    /**
     * Cms Contactor ImPlugin Parameter
     * OWN
     */
    MCmsContactorImPluginParameter* iCmsParameter;
    };

#endif // CCACONTACTORIMOPERATION_H
// End of File
