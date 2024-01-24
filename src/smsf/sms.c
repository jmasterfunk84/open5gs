/*
 * Copyright (C) 2024 by Sukchan Lee <acetcom@gmail.com>
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
    tpdurealbytes = smsf_sms_get_user_data_byte_length(!tpdu->tpDCS,
            tpdu->tpUDL);

    tpdu_oa_real_length = ((tpdu->tp_originator_address.addr_length + 1) /2);

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

    ogs_pkbuf_put_u8(pkbuf, SMSF_RP_MESSAGE_TYPE_N2MS_DATA);
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

ogs_pkbuf_t *smsf_sms_encode_rp_ack(bool ti_flag, int ti_o,
        int rp_message_reference)
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

    ogs_pkbuf_put_u8(pkbuf, cp_data.header.flags.octet);
    ogs_pkbuf_put_u8(pkbuf, cp_data.header.sm_service_message_type);
    ogs_pkbuf_put_u8(pkbuf, cp_data.cp_user_data_length);

    ogs_pkbuf_put_u8(pkbuf, SMSF_RP_MESSAGE_TYPE_N2MS_ACK);
    ogs_pkbuf_put_u8(pkbuf, rp_message_reference);
    ogs_pkbuf_put_u8(pkbuf, 65); // Element ID 0x41
    ogs_pkbuf_put_u8(pkbuf, 0); // Length: 0

    return pkbuf;
}

ogs_pkbuf_t *smsf_sms_encode_rp_error(bool ti_flag, int ti_o,
        int rp_message_reference)
{
    ogs_pkbuf_t *pkbuf = NULL;
    smsf_sms_cp_data_t cp_data;

    memset(&cp_data, 0, sizeof(smsf_sms_cp_data_t));

    cp_data.header.flags.pd = SMSF_PROTOCOL_DISCRIMINATOR_SMS;
    cp_data.header.flags.tio = ti_o;
    cp_data.header.flags.tif = ti_flag;
    cp_data.header.sm_service_message_type = SMSF_SERVICE_MESSAGE_TYPE_CP_DATA;
    cp_data.cp_user_data_length = 6;

    pkbuf = ogs_pkbuf_alloc(NULL, 9);
    if (!pkbuf) {
        ogs_error("ogs_pkbuf_alloc() failed");
        return NULL;
    }

    ogs_pkbuf_put_u8(pkbuf, cp_data.header.flags.octet);
    ogs_pkbuf_put_u8(pkbuf, cp_data.header.sm_service_message_type);
    ogs_pkbuf_put_u8(pkbuf, cp_data.cp_user_data_length);

    ogs_pkbuf_put_u8(pkbuf, SMSF_RP_MESSAGE_TYPE_N2MS_ERROR);
    ogs_pkbuf_put_u8(pkbuf, rp_message_reference);
    ogs_pkbuf_put_u8(pkbuf, 1); // RP-Cause Length: 1
    ogs_pkbuf_put_u8(pkbuf, 50); // RP-Cause: Requested facility not subscribed
    ogs_pkbuf_put_u8(pkbuf, 65); // Element ID 0x41
    ogs_pkbuf_put_u8(pkbuf, 0); // Length: 0

    return pkbuf;
}

int smsf_sms_get_user_data_byte_length(smsf_sms_tpdcs_t data_coding_scheme,
    int user_data_length)
{
    int user_data_bytes;

    if (data_coding_scheme == 0) {
        user_data_bytes = ((user_data_length + 1)*7/8);
    } else {
       user_data_bytes = user_data_length;
    }

    return user_data_bytes;
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

void smsf_copy_submit_to_deliver(smsf_sms_tpdu_deliver_t *tpdu_deliver,
                const smsf_sms_tpdu_submit_t *tpdu_submit,
                const smsf_ue_t *mt_smsf_ue, const smsf_ue_t *smsf_ue)
{
    ogs_assert(tpdu_deliver);
    ogs_assert(tpdu_submit);
    ogs_assert(mt_smsf_ue);
    ogs_assert(smsf_ue);

    int tpdurealbytes;

    tpdu_deliver->header.tpUDHI = tpdu_submit->header.tpUDHI;
    tpdu_deliver->header.tpMMS = 1;  // Could eval DCS for concatenation

    /* Populate the Sender's MSISDN */
    char *oa_msisdn;
    if (strncmp(smsf_ue->gpsi, OGS_ID_GPSI_TYPE_MSISDN,
            strlen(OGS_ID_GPSI_TYPE_MSISDN)) == 0) {
        oa_msisdn = ogs_id_get_value(smsf_ue->gpsi);
        ogs_assert(oa_msisdn);
    } else {
        ogs_error("SMS-MO without MSISDN");
    }

    char *oa_msisdn_bcd;
    oa_msisdn_bcd = ogs_calloc(1, OGS_MAX_MSISDN_BCD_LEN+1);
    int oa_msisdn_bcd_len;
    ogs_bcd_to_buffer(oa_msisdn, oa_msisdn_bcd,
            &oa_msisdn_bcd_len);
    if (oa_msisdn)
        ogs_free(oa_msisdn);
    tpdu_deliver->tp_originator_address.addr_length =
            strlen(oa_msisdn);
    tpdu_deliver->tp_originator_address.header.ext = 1;
    tpdu_deliver->tp_originator_address.header.ton = 1;
    tpdu_deliver->tp_originator_address.header.npi = 1;
    memcpy(&tpdu_deliver->tp_originator_address.tp_address,
            oa_msisdn_bcd, oa_msisdn_bcd_len);

    if (oa_msisdn_bcd)
        ogs_free(oa_msisdn_bcd);

    tpdu_deliver->tpPID = tpdu_submit->tpPID;
    tpdu_deliver->tpDCS.octet = tpdu_submit->tpDCS.octet;
    smsf_sms_set_sc_timestamp(&tpdu_deliver->tpSCTS);
    tpdu_deliver->tpUDL = tpdu_submit->tpUDL;

    tpdurealbytes = smsf_sms_get_user_data_byte_length(
            tpdu_submit->tpDCS, tpdu_submit->tpUDL);

    memcpy(&tpdu_deliver->tpUD, &tpdu_submit->tpUD, tpdurealbytes);
}