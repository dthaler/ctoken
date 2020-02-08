//
//  psa_ia_decode.c
//  CToken
//
//  Created by Laurence Lundblade on 1/31/20.
//  Copyright © 2020 Laurence Lundblade. All rights reserved.
//

#include "psa_ia_decode.h"
#include "qcbor_util.h"


/*
 * Public function. See attest_token_decode.h
 */
enum attest_token_err_t
attest_token_decode_get_iat_simple(struct attest_token_decode_context *me,
                                   struct attest_token_iat_simple_t *items)
{
    struct qcbor_util_items_to_get_t  list[NUMBER_OF_ITEMS+1];
    QCBORDecodeContext                decode_context;
    int64_t                           client_id_64;
    enum attest_token_err_t           return_value;

    /* Set all q_useful_bufs to NULL and flags to 0 */
    memset(items, 0, sizeof(struct attest_token_iat_simple_t));

    /* Re use flags as array indexes because it works nicely */
    list[NONCE_FLAG].label              = EAT_CBOR_ARM_LABEL_CHALLENGE;
    list[UEID_FLAG].label               = EAT_CBOR_ARM_LABEL_UEID;
    list[BOOT_SEED_FLAG].label          = EAT_CBOR_ARM_LABEL_BOOT_SEED;
    list[HW_VERSION_FLAG].label         = EAT_CBOR_ARM_LABEL_HW_VERSION;
    list[IMPLEMENTATION_ID_FLAG].label  = EAT_CBOR_ARM_LABEL_IMPLEMENTATION_ID;
    list[CLIENT_ID_FLAG].label          = EAT_CBOR_ARM_LABEL_CLIENT_ID;
    list[SECURITY_LIFECYCLE_FLAG].label = EAT_CBOR_ARM_LABEL_SECURITY_LIFECYCLE;
    list[PROFILE_DEFINITION_FLAG].label = EAT_CBOR_ARM_LABEL_PROFILE_DEFINITION;
    list[ORIGINATION_FLAG].label        = EAT_CBOR_ARM_LABEL_ORIGINATION;
    list[NUMBER_OF_ITEMS].label         = 0; /* terminate the list. */

    if(me->last_error != ATTEST_TOKEN_ERR_SUCCESS) {
        return_value = me->last_error;
        goto Done;
    }

    QCBORDecode_Init(&decode_context, me->payload, 0);

    return_value = qcbor_util_get_items_in_map(&decode_context,
                                               list);
    if(return_value != ATTEST_TOKEN_ERR_SUCCESS) {
        goto Done;
    }

    /* ---- NONCE ---- */
    if(list[NONCE_FLAG].item.uDataType == QCBOR_TYPE_BYTE_STRING) {
        items->nonce = list[NONCE_FLAG].item.val.string;
        items->item_flags |= CLAIM_PRESENT_BIT(NONCE_FLAG);
    }

    /* ---- UEID -------*/
    if(list[UEID_FLAG].item.uDataType == QCBOR_TYPE_BYTE_STRING) {
        items->ueid = list[UEID_FLAG].item.val.string;
        items->item_flags |= CLAIM_PRESENT_BIT(UEID_FLAG);
    }

    /* ---- BOOT SEED -------*/
    if(list[BOOT_SEED_FLAG].item.uDataType ==  QCBOR_TYPE_BYTE_STRING) {
        items->boot_seed = list[BOOT_SEED_FLAG].item.val.string;
        items->item_flags |= CLAIM_PRESENT_BIT(BOOT_SEED_FLAG);\
    }

    /* ---- HW VERSION -------*/
    if(list[HW_VERSION_FLAG].item.uDataType == QCBOR_TYPE_TEXT_STRING) {
        items->hw_version = list[HW_VERSION_FLAG].item.val.string;
        items->item_flags |= CLAIM_PRESENT_BIT(HW_VERSION_FLAG);

    }

    /* ----IMPLEMENTATION ID -------*/
    if(list[IMPLEMENTATION_ID_FLAG].item.uDataType == QCBOR_TYPE_BYTE_STRING) {
        items->implementation_id = list[IMPLEMENTATION_ID_FLAG].item.val.string;
        items->item_flags |= CLAIM_PRESENT_BIT(IMPLEMENTATION_ID_FLAG);
    }

    /* ----CLIENT ID -------*/
    if(list[CLIENT_ID_FLAG].item.uDataType == QCBOR_TYPE_INT64) {
        client_id_64 = list[CLIENT_ID_FLAG].item.val.int64;
        if(client_id_64 < INT32_MAX || client_id_64 > INT32_MIN) {
            items->client_id = (int32_t)client_id_64;
            items->item_flags |= CLAIM_PRESENT_BIT(CLIENT_ID_FLAG);
        }
    }

    /* ----SECURITY LIFECYCLE -------*/
    if(list[SECURITY_LIFECYCLE_FLAG].item.uDataType == QCBOR_TYPE_INT64) {
        if(list[SECURITY_LIFECYCLE_FLAG].item.val.int64 < UINT32_MAX) {
            items->security_lifecycle =
            (uint32_t)list[SECURITY_LIFECYCLE_FLAG].item.val.int64;
            items->item_flags |=CLAIM_PRESENT_BIT(SECURITY_LIFECYCLE_FLAG);
        }
    }

    /* ---- PROFILE_DEFINITION -------*/
    if(list[PROFILE_DEFINITION_FLAG].item.uDataType == QCBOR_TYPE_TEXT_STRING) {
        items->profile_definition=list[PROFILE_DEFINITION_FLAG].item.val.string;
        items->item_flags |= CLAIM_PRESENT_BIT(PROFILE_DEFINITION_FLAG);
    }

    /* ---- ORIGINATION -------*/
    if(list[ORIGINATION_FLAG].item.uDataType == QCBOR_TYPE_TEXT_STRING) {
        items->origination = list[ORIGINATION_FLAG].item.val.string;
        items->item_flags |= CLAIM_PRESENT_BIT(ORIGINATION_FLAG);
    }

Done:
    return return_value;
}

/** Value for EAT_CBOR_ARM_LABEL_NO_SW_COMPONENTS when present.
 * It must be this value if present.
 * Indicates that the boot status does not contain any SW components'
 * measurement
 */
#define NO_SW_COMPONENT_FIXED_VALUE 1

/*
 * Public function. See attest_token_decode.h
 */
enum attest_token_err_t
attest_token_get_num_sw_components(struct attest_token_decode_context *me,
                                   uint32_t *num_sw_components)
{
    enum attest_token_err_t return_value;
    QCBORItem               item;

    if(me->last_error != ATTEST_TOKEN_ERR_SUCCESS) {
        return_value = me->last_error;
        goto Done;
    }

    return_value = qcbor_util_get_top_level_item_in_map(me->payload,
                                                        EAT_CBOR_ARM_LABEL_SW_COMPONENTS,
                                                        QCBOR_TYPE_ARRAY,
                                                        &item);
    if(return_value != ATTEST_TOKEN_ERR_SUCCESS) {
        if(return_value != ATTEST_TOKEN_ERR_NOT_FOUND) {
            /* Something very wrong. Bail out passing on the return_value */
            goto Done;
        } else {
            /* Now decide if it was intentionally left out. */
            return_value = qcbor_util_get_top_level_item_in_map(me->payload,
                                                                EAT_CBOR_ARM_LABEL_NO_SW_COMPONENTS,
                                                                QCBOR_TYPE_INT64,
                                                                &item);
            if(return_value == ATTEST_TOKEN_ERR_SUCCESS) {
                if(item.val.int64 == NO_SW_COMPONENT_FIXED_VALUE) {
                    /* Successful omission of SW components. Pass on the
                     * success return_value */
                    *num_sw_components = 0;
                } else {
                    /* Indicator for no SW components malformed */
                    return_value = ATTEST_TOKEN_ERR_SW_COMPONENTS_MISSING;
                }
            } else if(return_value == ATTEST_TOKEN_ERR_NOT_FOUND) {
                /* Should have been an indicator for no SW components */
                return_value = ATTEST_TOKEN_ERR_SW_COMPONENTS_MISSING;
            }
        }
    } else {
        /* The SW components claim exists */
        if(item.val.uCount == 0) {
            /* Empty SW component not allowed */
            return_value = ATTEST_TOKEN_ERR_SW_COMPONENTS_MISSING;
        } else {
            /* SUCESSS! Pass on the success return_value */
            /* Note that this assumes the array is definite length */
            *num_sw_components = item.val.uCount;
        }
    }

Done:
    return return_value;
}


/**
 * \brief Decode a single SW component
 *
 * \param[in] decode_context    The CBOR decoder context to decode from
 * \param[in] sw_component_item The top-level map item for this SW
 *                              component.
 * \param[out] sw_component     The structure to fill in with decoded data.
 *
 * \return An error from \ref attest_token_err_t.
 *
 */
static inline enum attest_token_err_t
decode_sw_component(QCBORDecodeContext               *decode_context,
                    const QCBORItem                  *sw_component_item,
                    struct attest_token_sw_component_t *sw_component)
{
    enum attest_token_err_t return_value;
    QCBORItem claim_item;
    QCBORError cbor_error;
    uint_fast8_t next_nest_level; /* nest levels are 8-bit, but a uint8_t
                                   var is often slower and more code */

    if(sw_component_item->uDataType != QCBOR_TYPE_MAP) {
        return_value = ATTEST_TOKEN_ERR_CBOR_STRUCTURE;
        goto Done;
    }

    /* Zero it, setting booleans to false, pointers to NULL and
     lengths to 0 */
    memset(sw_component, 0, sizeof(struct attest_token_sw_component_t));

    return_value = ATTEST_TOKEN_ERR_SUCCESS;

    while(1) {
        cbor_error = QCBORDecode_GetNext(decode_context, &claim_item);
        if(cbor_error != QCBOR_SUCCESS) {
            /* no tolerance for any errors here */
            return_value = ATTEST_TOKEN_ERR_CBOR_NOT_WELL_FORMED;
            goto Done;
        }

        if(claim_item.uLabelType == QCBOR_TYPE_INT64) {
            switch(claim_item.label.int64) {
                case EAT_CBOR_SW_COMPONENT_MEASUREMENT_TYPE:
                    if(claim_item.uDataType != QCBOR_TYPE_TEXT_STRING) {
                        return_value = ATTETST_TOKEN_ERR_CBOR_TYPE;
                        goto Done;
                    }
                    sw_component->measurement_type = claim_item.val.string;
                    sw_component->item_flags |=
                    CLAIM_PRESENT_BIT(SW_MEASUREMENT_TYPE_FLAG);

                    break;

                case EAT_CBOR_SW_COMPONENT_MEASUREMENT_VALUE:
                    if(claim_item.uDataType != QCBOR_TYPE_BYTE_STRING) {
                        return_value = ATTETST_TOKEN_ERR_CBOR_TYPE;
                        goto Done;
                    }
                    sw_component->measurement_val = claim_item.val.string;
                    sw_component->item_flags |=
                    CLAIM_PRESENT_BIT(SW_MEASURMENT_VAL_FLAG);
                    break;

                case EAT_CBOR_SW_COMPONENT_SECURITY_EPOCH:
                    if(claim_item.uDataType != QCBOR_TYPE_INT64) {
                        return_value = ATTETST_TOKEN_ERR_CBOR_TYPE;
                        goto Done;
                    }
                    if(claim_item.val.int64 < UINT32_MAX &&
                       claim_item.val.int64 > 0) {
                        sw_component->epoch = (uint32_t)claim_item.val.int64;
                        sw_component->item_flags |=
                        CLAIM_PRESENT_BIT(SW_EPOCH_FLAG);
                    }
                    break;

                case EAT_CBOR_SW_COMPONENT_VERSION:
                    if(claim_item.uDataType != QCBOR_TYPE_TEXT_STRING) {
                        return_value = ATTETST_TOKEN_ERR_CBOR_TYPE;
                        goto Done;
                    }
                    sw_component->version = claim_item.val.string;
                    sw_component->item_flags |=
                    CLAIM_PRESENT_BIT(SW_VERSION_FLAG);
                    break;

                case EAT_CBOR_SW_COMPONENT_SIGNER_ID:
                    if(claim_item.uDataType != QCBOR_TYPE_BYTE_STRING) {
                        return_value = ATTETST_TOKEN_ERR_CBOR_TYPE;
                        goto Done;
                    }
                    sw_component->signer_id = claim_item.val.string;
                    sw_component->item_flags |=
                    CLAIM_PRESENT_BIT(SW_SIGNER_ID_FLAG);
                    break;

                case EAT_CBOR_SW_COMPONENT_MEASUREMENT_DESC:
                    if(claim_item.uDataType != QCBOR_TYPE_TEXT_STRING) {
                        return_value = ATTETST_TOKEN_ERR_CBOR_TYPE;
                        goto Done;
                    }
                    sw_component->measurement_desc = claim_item.val.string;
                    sw_component->item_flags |=
                    CLAIM_PRESENT_BIT(SW_MEASUREMENT_DESC_FLAG);
                    break;
            }
        }

        if(qcbor_util_consume_item(decode_context,
                                   &claim_item,
                                   &next_nest_level)) {
            return_value = ATTEST_TOKEN_ERR_CBOR_NOT_WELL_FORMED;
            goto Done;
        }
        if(next_nest_level < sw_component_item->uNextNestLevel) {
            /* Got all the items in the map */
            break;
        }
    }

Done:
    return return_value;
}


/*
 * Public function. See attest_token_decode.h
 */
enum attest_token_err_t
attest_token_get_sw_component(struct attest_token_decode_context *me,
                              uint32_t requested_index,
                              struct attest_token_sw_component_t *sw_components)
{
    enum attest_token_err_t return_value;
    QCBORItem               sw_components_array_item;
    QCBORDecodeContext      decode_context;
    QCBORItem               sw_component_item;
    QCBORError              qcbor_error;
    uint_fast8_t            exit_array_level;

    if(me->last_error != ATTEST_TOKEN_ERR_SUCCESS) {
        return_value = me->last_error;
        goto Done;
    }

    QCBORDecode_Init(&decode_context, me->payload, 0);

    /* Find the map containing all the SW Components */
    return_value = qcbor_util_decode_to_labeled_item(&decode_context,
                                                     EAT_CBOR_ARM_LABEL_SW_COMPONENTS,
                                                     &sw_components_array_item);
    if(return_value != ATTEST_TOKEN_ERR_SUCCESS) {
        goto Done;
    }

    if(sw_components_array_item.uDataType != QCBOR_TYPE_ARRAY) {
        return_value = ATTETST_TOKEN_ERR_CBOR_TYPE;
        goto Done;
    }

    exit_array_level = sw_components_array_item.uNextNestLevel;

    /* Loop over contents of SW Components array */
    while(1) {
        qcbor_error = QCBORDecode_GetNext(&decode_context, &sw_component_item);
        if(qcbor_error) {
            /* no tolerance for any errors here */
            return_value = ATTEST_TOKEN_ERR_CBOR_NOT_WELL_FORMED;
            goto Done;
        }

        if(sw_component_item.uNextNestLevel <= exit_array_level) {
            /* Next item will be outside the array */
            return_value = ATTEST_TOKEN_ERR_NOT_FOUND;
            /* The end of the array containing SW components
             and didn't get to the requested_index. */
            goto Done;
        }

        if(requested_index == 0) {
            /* Found the one of interest. Decode it and break out */
            return_value = decode_sw_component(&decode_context,
                                               &sw_component_item,
                                               sw_components);
            break; /* The normal, non-error exit from this loop */
        }

        /* Every member in the array counts even if they are not
         * what is expected */
        requested_index--;

        if(qcbor_util_consume_item(&decode_context, &sw_component_item, NULL)) {
            return_value = ATTEST_TOKEN_ERR_CBOR_NOT_WELL_FORMED;
            goto Done;
        }
    }

Done:
    return return_value;
}
