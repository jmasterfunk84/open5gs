/*
 * Copyright (C) 2019-2023 by Sukchan Lee <acetcom@gmail.com>
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

#include "sms.h"

ogs_pkbuf_t *smsf_sms_encode_cp_ack(bool ti_flag, int ti_o)
{
    ogs_pkbuf_t *pkbuf = NULL;
    smsf_sms_cp_hdr_t cp_data_header;

    memset(&cp_data_header, 0, sizeof(smsf_sms_cp_hdr_t));

    cp_data_header.flags.pd = SMSF_PROTOCOL_DISCRIMINATOR_SMS;
    cp_data_header.flags.tio = ti_o;
    cp_data_header.flags.tif = ti_flag;
    cp_data_header.sm_service_message_type = SMSF_SERVICE_MESSAGE_TYPE_CP_ACK;

    pkbuf = ogs_pkbuf_alloc(NULL, 2);
    if (!pkbuf) {
        ogs_error("ogs_pkbuf_alloc() failed");
        return NULL;
    }

    ogs_pkbuf_put_u8(pkbuf,cp_data_header.flags.octet);
    ogs_pkbuf_put_u8(pkbuf,cp_data_header.sm_service_message_type);

    return pkbuf;
}

ogs_pkbuf_t *smsf_sms_encode_rp_data(bool ti_flag, int ti_o, 
                int rp_message_reference, smsf_sms_tpdu_deliver_t *tpdu)
{
    ogs_pkbuf_t *pkbuf = NULL;
    smsf_sms_cp_data_t cp_data;

    memset(&cp_data, 0, sizeof(smsf_sms_cp_data_t));

    int tpdurealbytes;
    if (!tpdu->tpDCS) {
        tpdurealbytes = ((tpdu->tpUDL+1)*7/8);
    } else {
        tpdurealbytes = tpdu->tpUDL;
    }
    
    cp_data.header.flags.pd = SMSF_PROTOCOL_DISCRIMINATOR_SMS;
    cp_data.header.flags.tio = ti_o;
    cp_data.header.flags.tif = ti_flag;
    cp_data.header.sm_service_message_type = SMSF_SERVICE_MESSAGE_TYPE_CP_DATA;
    cp_data.cp_user_data_length = 37 + tpdurealbytes;

    pkbuf = ogs_pkbuf_alloc(NULL, 64);
    if (!pkbuf) {
        ogs_error("ogs_pkbuf_alloc() failed");
        return NULL;
    }

    ogs_pkbuf_put_u8(pkbuf,cp_data.header.flags.octet);
    ogs_pkbuf_put_u8(pkbuf,cp_data.header.sm_service_message_type);
    ogs_pkbuf_put_u8(pkbuf,cp_data.cp_user_data_length);

    ogs_pkbuf_put_u8(pkbuf,1); // Mesage Type = RP-Data N2MS
    ogs_pkbuf_put_u8(pkbuf, rp_message_reference); // rp_message_reference
    ogs_pkbuf_put_data(pkbuf, (char *)"\x07\x91\x31\x60\x26\x00\x50\xf1", 8); // rp-oa
    ogs_pkbuf_put_u8(pkbuf,0); // rp-da
    ogs_pkbuf_put_u8(pkbuf,tpdurealbytes+19); // rpud len

    ogs_pkbuf_put_data(pkbuf,tpdu, 17 + tpdurealbytes);
    ogs_pkbuf_put_u8(pkbuf,tpdurealbytes);
    ogs_pkbuf_put_data(pkbuf,&tpdu->tpUD,tpdurealbytes);

    return pkbuf;
}

ogs_pkbuf_t *smsf_sms_encode_rp_ack(bool ti_flag, int ti_o, int rp_message_reference)
{
    ogs_pkbuf_t *pkbuf = NULL;
    smsf_sms_cp_data_t cp_data;

    memset(&cp_data, 0, sizeof(smsf_sms_cp_data_t));

    cp_data.header.flags.pd = SMSF_PROTOCOL_DISCRIMINATOR_SMS;
    cp_data.header.flags.tio = ti_o;
    cp_data.header.flags.tif = ti_flag;
    cp_data.header.sm_service_message_type = SMSF_SERVICE_MESSAGE_TYPE_CP_DATA;
    cp_data.cp_user_data_length = 4;

    pkbuf = ogs_pkbuf_alloc(NULL, 7);
    if (!pkbuf) {
        ogs_error("ogs_pkbuf_alloc() failed");
        return NULL;
    }

    ogs_pkbuf_put_u8(pkbuf,cp_data.header.flags.octet);
    ogs_pkbuf_put_u8(pkbuf,cp_data.header.sm_service_message_type);
    ogs_pkbuf_put_u8(pkbuf,cp_data.cp_user_data_length);

    ogs_pkbuf_put_u8(pkbuf,3); // Mesage Type = RP-ACK
    ogs_pkbuf_put_u8(pkbuf,rp_message_reference);
    ogs_pkbuf_put_u8(pkbuf,65); // Element ID 0x41
    ogs_pkbuf_put_u8(pkbuf,0); // Length: 0


    return pkbuf;
}
