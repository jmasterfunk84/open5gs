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

    int tpdu_oa_real_length;

    memset(&cp_data, 0, sizeof(smsf_sms_cp_data_t));

    int tpdurealbytes;
    if (!tpdu->tpDCS) {
        tpdurealbytes = ((tpdu->tpUDL+1)*7/8);
    } else {
        tpdurealbytes = tpdu->tpUDL;
    }

    tpdu_oa_real_length = ((tpdu->tp_originator_address.addr_length + 1) /2);

    ogs_info("Real Bytes: [%d]", tpdurealbytes);

    cp_data.header.flags.pd = SMSF_PROTOCOL_DISCRIMINATOR_SMS;
    cp_data.header.flags.tio = ti_o;
    cp_data.header.flags.tif = ti_flag;
    cp_data.header.sm_service_message_type = SMSF_SERVICE_MESSAGE_TYPE_CP_DATA;
    cp_data.cp_user_data_length = 25 + tpdurealbytes + tpdu_oa_real_length;

    pkbuf = ogs_pkbuf_alloc(NULL, 240);
    if (!pkbuf) {
        ogs_error("ogs_pkbuf_alloc() failed");
        return NULL;
    }

    ogs_pkbuf_put_u8(pkbuf, cp_data.header.flags.octet);
    ogs_pkbuf_put_u8(pkbuf, cp_data.header.sm_service_message_type);
    ogs_pkbuf_put_u8(pkbuf, cp_data.cp_user_data_length);

    ogs_pkbuf_put_u8(pkbuf, 1); // Mesage Type = RP-Data N2MS
    ogs_pkbuf_put_u8(pkbuf, rp_message_reference); // rp_message_reference
    ogs_pkbuf_put_data(pkbuf, (char *)"\x07\x91\x31\x60\x26\x00\x50\xf1", 8); // rp-oa
    ogs_pkbuf_put_u8(pkbuf, 0); // rp-da
    // 13 is the size of the tpdu stuff.
    ogs_pkbuf_put_u8(pkbuf, tpdurealbytes + tpdu_oa_real_length + 13);

    ogs_pkbuf_put_u8(pkbuf, tpdu->header.octet);
    ogs_pkbuf_put_u8(pkbuf, tpdu->tp_originator_address.addr_length);
    ogs_pkbuf_put_u8(pkbuf, tpdu->tp_originator_address.header.octet);
    ogs_pkbuf_put_data(pkbuf, tpdu->tp_originator_address.tp_address,
            tpdu_oa_real_length);
    ogs_pkbuf_put_u8(pkbuf, tpdu->tpPID);
    ogs_pkbuf_put_u8(pkbuf, tpdu->tpDCS);
    ogs_pkbuf_put_u8(pkbuf, tpdu->tpSCTS.year);
    ogs_pkbuf_put_u8(pkbuf, tpdu->tpSCTS.month);
    ogs_pkbuf_put_u8(pkbuf, tpdu->tpSCTS.day);
    ogs_pkbuf_put_u8(pkbuf, tpdu->tpSCTS.hour);
    ogs_pkbuf_put_u8(pkbuf, tpdu->tpSCTS.minute);
    ogs_pkbuf_put_u8(pkbuf, tpdu->tpSCTS.second);
    ogs_pkbuf_put_u8(pkbuf, tpdu->tpSCTS.timezone);
    ogs_pkbuf_put_u8(pkbuf, tpdu->tpUDL);
    ogs_pkbuf_put_data(pkbuf, &tpdu->tpUD,tpdurealbytes);

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

void smsf_sms_set_sc_timestamp(smsf_sms_tpscts_t *sc_timestamp)
{
    struct timeval tv;
    struct tm local;
    int local_time_zone;

    ogs_gettimeofday(&tv);
    ogs_localtime(tv.tv_sec, &local);

    if (local.tm_gmtoff >= 0) {
        local_time_zone = OGS_SCTS_TIME_TO_BCD(local.tm_gmtoff / 900);
    } else {
        local_time_zone = OGS_SCTS_TIME_TO_BCD((-local.tm_gmtoff) / 900);
        local_time_zone |= 0x08;
    }

    sc_timestamp->year = OGS_SCTS_TIME_TO_BCD(local.tm_year % 100);
    sc_timestamp->month = OGS_SCTS_TIME_TO_BCD(local.tm_mon+1);
    sc_timestamp->day = OGS_SCTS_TIME_TO_BCD(local.tm_mday);
    sc_timestamp->hour = OGS_SCTS_TIME_TO_BCD(local.tm_hour);
    sc_timestamp->minute = OGS_SCTS_TIME_TO_BCD(local.tm_min);
    sc_timestamp->second = OGS_SCTS_TIME_TO_BCD(local.tm_sec);
    sc_timestamp->timezone = local_time_zone;

}
