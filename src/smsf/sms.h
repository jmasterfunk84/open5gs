/*
 * Copyright (C) 2019 by Sukchan Lee <acetcom@gmail.com>
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

#ifndef SMSF_SMS_H
#define SMSF_SMS_H

#include "ogs-core.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct smsf_sms_address_s {
        uint8_t length;
        struct {
        ED3(uint8_t ext:1;,
            uint8_t ton:3;,
            uint8_t npi:4;)
        } header;
        uint8_t rp_address[11];
} __attribute__ ((packed)) smsf_sms_address_t;

typedef struct smsf_sms_tp_address_s {
        uint8_t addr_length;
        struct {
        ED3(uint8_t ext:1;,
            uint8_t ton:3;,
            uint8_t npi:4;)
        } header;
        uint8_t tp_address[10];
} __attribute__ ((packed)) smsf_sms_tp_address_t;

typedef struct smsf_sms_rpdu_message_type_s {
        ED2(uint8_t reserved:5;,
            uint8_t value:3;)
} __attribute__ ((packed)) smsf_sms_rpdu_message_type_t;

typedef struct smsf_sms_rpdata_s {
        smsf_sms_rpdu_message_type_t rpdu_message_type;
        uint8_t rp_message_reference;
        smsf_sms_address_t rp_originator_address;
        smsf_sms_address_t rp_destination_address;
        uint8_t rp_user_data_length;
} __attribute__ ((packed)) smsf_sms_rpdu_t;

typedef struct smsf_sms_cp_hdr_s {
        struct {
        ED3(uint8_t tif:1;,
            uint8_t tio:3;,
            uint8_t pd:4;)
        } cpdata;
        uint8_t sm_service_message_type;
} __attribute__ ((packed)) smsf_sms_cp_hdr_t;

typedef struct smsf_sms_cp_data_s {
        smsf_sms_cp_hdr_t header;
        uint8_t cp_user_data_length;
} __attribute__ ((packed)) smsf_sms_cp_data_t;

typedef struct smsf_sms_tpdu_hdr_s {
        ED2(uint8_t spare:6;,
            uint8_t tpMTI:2;)
} __attribute__ ((packed)) smsf_sms_tpdu_hdr_t;

typedef struct smsf_sms_tpdu_submit_s {
        struct {
        ED6(uint8_t tpRP:1;,
            uint8_t tpUDHI:1;,
            uint8_t tpSRR:1;,
            uint8_t tpVPF:2;,
            uint8_t tpRD:1;,
            uint8_t tpMTI:2;)
        } header;
        uint8_t tpMR;
        smsf_sms_tp_address_t tp_destination_address;
        uint8_t tpPID;
        uint8_t tpDCS;
        uint8_t tpUDL;
        uint8_t tpUD[140];
} __attribute__ ((packed)) smsf_sms_tpdu_submit_t;

typedef struct smsf_sms_tpdu_deliver_s {
        struct {
        ED6(uint8_t tpRP:1;,
            uint8_t tpUDHI:1;,
            uint8_t tpSRI:1;,
            uint8_t tpReserved:2;,
            uint8_t tpMMS:1;,
            uint8_t tpMTI:2;)
        } header;
        uint8_t tpMR;
        smsf_sms_tp_address_t tp_originator_address;
        uint8_t tpPID;
        uint8_t tpDCS;
        uint8_t tpSCTS[7];
        uint8_t tpUDL;
        uint8_t tpUD[140];
} __attribute__ ((packed)) smsf_sms_tpdu_deliver_t;

#define SMSF_SERVICE_MESSAGE_TYPE_CP_ACK            4

#define SMSF_PROTOCOL_DISCRIMINATOR_SMS             0x9

ogs_pkbuf_t *smsf_sms_encode_cp_ack(bool ti_flag, int ti_o);

#ifdef __cplusplus
}
#endif

#endif /* SMSF_SMS_H */