/*
 * Copyright (C) 2019,2020 by Sukchan Lee <acetcom@gmail.com>
 *
 * This file is part of Open5GS.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SMSF_NSMSF_HANDLER_H
#define SMSF_NSMSF_HANDLER_H

#include "context.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 24.011 sec 7.2 */
/* create a type for CP-DATA, CP-ACK, CP-ERROR */
/* create a type for RP-DATA, RP-ACK, RP-ERROR */
/* shorten to DA and OA? */

typedef struct smsf_sms_address_s {
        ED3(uint8_t ext:1;,
            uint8_t ton:3;,
            uint8_t npi:4;)
        } header;
        uint8_t rp_address[11];
} __attribute__ ((packed)) smsf_sms_address_t;


typedef struct smsf_sms_rpdu_s {
        uint8_t rp_message_reference;
        uint8_t rp_originator_address_len;
        uint8_t rp_destination_address_len;
        smsf_sms_address_t rp_destination_address;
        uint8_t rp_user_data[233];
} __attribute__ ((packed)) smsf_sms_rpdu_t;

typedef struct smsf_sms_rpdu_message_type_s {
        ED2(uint8_t reserved:5;,
            uint8_t value:3;)
} __attribute__ ((packed)) smsf_sms_rpdu_message_type_t;

typedef struct smsf_sms_cp_data_s {
        uint8_t cp_user_data_length;
        smsf_sms_rpdu_message_type_t rpdu_message_type;
        uint8_t rpdu[248];
} __attribute__ ((packed)) smsf_sms_cp_data_t;

typedef struct smsf_sms_cp_hdr_s {
        struct {
        ED3(uint8_t pd:4;,
            uint8_t tio:3;,
            uint8_t tif:1;)
        } cpdata;
        uint8_t sm_service_message_type;
} __attribute__ ((packed)) smsf_sms_cp_hdr_t;

bool smsf_nsmsf_sm_service_handle_activate(
        smsf_ue_t *smsf_ue, ogs_sbi_stream_t *stream,
        ogs_sbi_message_t *message);

int smsf_nsmsf_sm_service_handle_deactivate(
        smsf_ue_t *smsf_ue, ogs_sbi_stream_t *stream,
        ogs_sbi_message_t *message);

bool smsf_nsmsf_sm_service_handle_uplink_sms(
        smsf_ue_t *smsf_ue, ogs_sbi_stream_t *stream,
        ogs_sbi_message_t *message);

#ifdef __cplusplus
}
#endif

#endif /* SMSF_NSMSF_HANDLER_H */
