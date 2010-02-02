
KCommunicationMethodOrder - variation of communication methods
##############################################################

The order of communication methods in the communication launcher view can be variated. This can be done via using the KCommunicationMethodOrder-key in keys_ccappcommlauncher.xls-file or by editing the ccappcommlauncher.confml-file.

KCommunicationMethodOrder-key is a string consisting numbers. The numbers mean the types of communication methods and they are directly mapped to values of VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector-enum. This is defined in VPbkFieldTypeSelectorFactory.h.

Here is the current enumeration, but I suggest to check most-recent values directly from the header!


       /**
         * Defines predefined contact action type selectors.
         */
        enum TVPbkContactActionTypeSelector
            {
            /// VoiceCall selector
            EEmptySelector = 0x50,
            EVoiceCallSelector,
            EUniEditorSelector,
            EEmailEditorSelector,
            EInstantMessagingSelector,
            EVOIPCallSelector,
            EURLSelector,
            EVideoCallSelector
            }; 

Ok, if the key is like "81 85 82 83 84 86 87", it means the following. In case needed information is available from the specific contact and from the platform, following communication methods are offered in following order when communication launcher view is activated :

- Starting a voice call
- Starting a VOIP call
- Launching a SMS/MMS common/uni-editor
- Launching a email-editor
- Launching instant messaging application
- Starting web browser to a URL-address
- Starting a video call

So, the numbers defined 1st (leftmost) are on top and numbers defined last (rightmost) are on bottom. 0x50 hex is 80 in decimals.

If the communication method type is not defined to the string, then it will not be shown in the view at all. For example string "86 83" would show only items for starting web browser and launching email editor.


KHeaderTextPriorities - variation of header priority order
##########################################################

The order in which contact data fields are selected to the three header rows is
stored in cenrep repository KCRUidCCACommLauncher with keys KHeaderTextPriorities
(and KHeaderTextPriorities2) as a string that has the following format.

"ECmsUrlGeneric 1 1 ECmsEmailGeneric 1 3 ECmsMobilePhoneGeneric 1 4 ECmsFirstName 2 3"

The string consists of field names followed by their respective priority values.
The names and values are separated by whitespace. The first value specifies the
primary (group) priority of the data field and the second value specifies the
secondary (individual) priority of the field. A higher priority value means
higher priority. Only the available fields with the highest primary (group)
priority are shown in the header, in the order specified by their secondary
priorities (higher above lower). If there is only one field with the highest
primary priority it is wrapped across all three rows.

If the priorities of a field are not specified (the field name is not in the
string) it will be given a default priority. A field with primary priority of 0
or lower is never shown. All the fields are listed below with their default
priorities. Note that a cenrep string cannot be longer than 1024 characters, so
if your string becomes longer than that, you need to divide it between
KHeaderTextPriorities and KHeaderTextPriorities2. When there is data in both
strings, either KHeaderTextPriorities needs to end with whitespace or
KHeaderTextPriorities2 start with whitespace. 

ECmsUndefined 0 0
ECmsPresenceData 0 0
ECmsBrandedAvailability 0 0
ECmsThumbnailPic 0 0
ECmsFaxNumberHome 0 0
ECmsVoipNumberHome 0 0
ECmsVoipNumberWork 0 0
ECmsAddrLabelGeneric 0 0
ECmsAddrPOGeneric 0 0
ECmsAddrExtGeneric 0 0
ECmsAddrPostcodeGeneric 0 0
ECmsVoipNumberGeneric 0 0
ECmsAddrLabelHome 0 0
ECmsAddrPOHome 0 0
ECmsAddrExtHome 0 0
ECmsAddrPostcodeHome 0 0
ECmsAddrLabelWork 0 0
ECmsAddrPOWork 0 0
ECmsAddrExtWork 0 0
ECmsAddrPostcodeWork 0 0
ECmsFullName 0 0
ECmsBirthday 0 0
ECmsAnniversary 0 0
ECmsImpp 1 1
ECmsUrlGeneric 1 1
ECmsUrlHome 1 1
ECmsUrlWork 1 1
ECmsSipAddress 1 2
ECmsEmailGeneric 1 3
ECmsEmailHome 1 3
ECmsEmailWork 1 3
ECmsMobilePhoneGeneric 1 4
ECmsMobilePhoneHome 1 4
ECmsMobilePhoneWork 1 4
ECmsLandPhoneGeneric 1 4
ECmsLandPhoneHome 1 4
ECmsLandPhoneWork 1 4
ECmsVideoNumberGeneric 1 4
ECmsVideoNumberHome 1 4
ECmsVideoNumberWork 1 4
ECmsLastName 2 1
ECmsMiddleName 2 2
ECmsFirstName 2 3
ECmsAddrCountryGeneric 3 1
ECmsAddrRegionGeneric 3 2
ECmsAddrLocalGeneric 3 3
ECmsAddrStreetGeneric 3 4
ECmsAddrCountryWork 4 1
ECmsAddrRegionWork 4 2
ECmsAddrLocalWork 4 3
ECmsAddrStreetWork 4 4
ECmsAddrCountryHome 5 1
ECmsAddrRegionHome 5 2
ECmsAddrLocalHome 5 3
ECmsAddrStreetHome 5 4
ECmsDepartment 6 1
ECmsCompanyName 6 2
ECmsJobTitle 6 3
ECmsNickname 6 4
ECmsNote 7 1

