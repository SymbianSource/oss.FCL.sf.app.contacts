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
* Description:  This is a class for starting services
 *                provided by CCA contactor service
 *
*/

#ifndef __CCACONTACTORSERVICE_H__
#define __CCACONTACTORSERVICE_H__

// INCLUDES
#include <e32std.h>
#include <f32file.h>
#include <VPbkFieldTypeSelectorFactory.h>

//FORWARD DECLARATIONS

class CCCAContactorServiceOperator;

/**
 *  CCAContactorService
 *
 *  Class for starting services of CCA contactor service.
 *  @code
 *      HBufC* fullName = GetFullNameLC();
 *      CVPbkContactLinkArray* linkArray = CVPbkContactLinkArray::NewLC();
 *      linkArray->AppendL(iContactLink);
 *      HBufC8* streamedArray = linkArray->PackLC();
 *
 *      //Set control flags.
 *      TUint flags;
 *      flags |= CCAContactorService::TCSParameter::EEnableDefaults;
 *
 *      CCAContactorService::TCSParameter params(VPbkFieldTypeSelectorFactory::EVoiceCallSelector, *streamedArray, flags, *fullName);
 *      CCAContactorService::ExecuteServiceL(params);
 *
 *      CleanupStack::PopAndDestroy(3); //linkArray, streamedArray, fullName
 *
 *  @endcode
 *
 *  @lib CCAContactorService
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CCAContactorService) : CBase
    {
public:

    /**
     * Parameter class for the service.
     *
     * @since S60 v5.0
     */
    class TCSParameter
        {
        public:

        /** Flags for controlling CCA Contactor Service */
        enum TControlFlags
            {
            /**
             * Enable defaults. If this is set, AIW popup query will automatically search possible
             * default settings and if found one, it doesn't show selection query at all.
             */
            EEnableDefaults  = 0x0001,
            
            /**
             * Clients may offer a particular field to be used in launching a service. Use the 
             * provided aSelectedField parameter to pass the field  data to be used. If this 
             * flag is set the AIW query will not be shown, the service will automatically launch 
             * the service. aSelectedField parameter must be valid.
             */
            EUseFieldParam = 0x0002
            };

        /*
         * Constructor
         */
        inline TCSParameter(
                VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aCommTypeSelector,
                TDesC8& aContactLinkArray, TUint aControlFlag, TDesC& aFullName, TDesC& aSelectedField) :
                iCommTypeSelector(aCommTypeSelector),
                iContactLinkArray(aContactLinkArray),
                iControlFlag(aControlFlag),
                iFullName(aFullName),
                iSelectedField(&aSelectedField)
            {
            };
        
        /*
         * Constructor
         */
        inline TCSParameter(
                VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aCommTypeSelector,
                TDesC8& aContactLinkArray, TUint aControlFlag, TDesC& aFullName) :
                iCommTypeSelector(aCommTypeSelector),
                iContactLinkArray(aContactLinkArray),
                iControlFlag(aControlFlag),
                iFullName(aFullName),
                iSelectedField(NULL)
            {
            };

        /* !DEPRICATED!
         * Constructor
         */
        inline TCSParameter(
                VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aCommTypeSelector,
                TDesC8& aContactLinkArray, TBool aEnableDefaults, TDesC& aFullName) :
                iCommTypeSelector(aCommTypeSelector),
                iContactLinkArray(aContactLinkArray),
                iEnableDefaults(aEnableDefaults),
                iFullName(aFullName),
                iSelectedField(NULL)
            {
            };

         /*
         * Enumeration for inticating which communication method is wanted to use.
         *
         * @since S60 v5.0
         */
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector iCommTypeSelector;

        /*
         * Streamed contact link array of contact who will be communicated at.
         *
         * @since S60 v5.0
         */
        TDesC8& iContactLinkArray;

        /* !DEPRICATED!
         * Use defaults. If this is enabled, AIW popup query will automatically search possible
         * default settings and if found one, it doesn't show query at all.
         *
         * @since S60 v5.0
         */
        TBool iEnableDefaults;

        /**
         * Flag for controlling behaviour of service. Currently only set of default supported.
         */
        TUint iControlFlag;

        /*
         * Some communication methods requires contacts name order to use services.
         * (for example mms/sms editor)
         *
         * @since S60 v5.0
         */
        TDesC& iFullName;
        
        /*
         * Clients may offer a particular field to be used in launching a service. This 
         * member stores the field.
         */
        TDesC* iSelectedField;
        };

    /**
     *
     */
    IMPORT_C static CCAContactorService* NewL();

    /**
     * Destructor.
     */
    virtual ~CCAContactorService();

    /**
     * Start communication service.
     *
     * If the contactlink contains a multiple contact fields for selected communication
     * method, the executed service will launch Phbk AIW selection popup. User have to select
     * wanted field to be used for communication.
     * The communication method is defined by TVPbkCommTypeSelector.
     * After selection, communication is automatically started by the service.
     *
     * @see TVPbkCommTypeSelector from VPbkFieldTypeSelectorFactory.h.
     * Leaves KErrArgument if given parameter doesn't have valid values.
     *
     * @since S60 5.0
     * @param aParameter: The Launch Parameter.
     */
    IMPORT_C void ExecuteServiceL(const TCSParameter& aParameter);

    /**
     * Is contactor service busy.
     */
    IMPORT_C TBool IsBusy() const;

private:
    /**
     * Constructor.
     */
    CCAContactorService();

    /**
     *
     */
    void ConstructL();

private:
    // data

    /**
     *
     */
    CCCAContactorServiceOperator* iOperator;
    
    /**
     * ETrue, if command is being handled at the moment
     */
    TBool iCommandIsBeingHandled;
    };

#endif      //__CCACONTACTORSERVICE_H__

// End of File
