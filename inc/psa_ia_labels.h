/*
 * psa_ia_labels.h
 *
 * Copyright (c) 2020 Laurence Lundblade.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 *
 * Created by Laurence Lundblade on 2/3/20.
 */

#ifndef psa_ia_labels_h
#define psa_ia_labels_h

#include "eat_labels.h"

#define EAT_CBOR_ARM_LABEL_CHALLENGE  EAT_LABEL_NONCE

#define EAT_CBOR_ARM_LABEL_UEID  UEID_LABEL

#define EAT_CBOR_ARM_LABEL_BOOT_SEED 90000

#define EAT_CBOR_ARM_LABEL_HW_VERSION 90001

#define EAT_CBOR_ARM_LABEL_IMPLEMENTATION_ID 90002

#define EAT_CBOR_ARM_LABEL_CLIENT_ID 90003

#define EAT_CBOR_ARM_LABEL_SECURITY_LIFECYCLE 90004

#define EAT_CBOR_ARM_LABEL_PROFILE_DEFINITION 90005

#define EAT_CBOR_ARM_LABEL_ORIGINATION 90006

#define EAT_CBOR_ARM_LABEL_NO_SW_COMPONENTS 90010

#define EAT_CBOR_ARM_LABEL_SW_COMPONENTS 90009

#define EAT_CBOR_SW_COMPONENT_MEASUREMENT_TYPE 90011

#define EAT_CBOR_SW_COMPONENT_MEASUREMENT_VALUE 90012

#define EAT_CBOR_SW_COMPONENT_VERSION 90013

#define EAT_CBOR_SW_COMPONENT_MEASUREMENT_DESC 90014

#define EAT_CBOR_SW_COMPONENT_SIGNER_ID 90015

#define EAT_CBOR_SW_COMPONENT_SECURITY_EPOCH 90016


#endif /* psa_ia_labels_h */
