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

    cp_data_header.cpdata.pd = SMSF_PROTOCOL_DISCRIMINATOR_SMS;
    cp_data_header.cpdata.tio = ti_flag;
    cp_data_header.cpdata.tif = ti_flag;
    cp_data_header.sm_service_message_type = SMSF_SERVICE_MESSAGE_TYPE_CP_ACK;

    pkbuf = ogs_pkbuf_alloc(NULL, 2);
    if (!pkbuf) {
        ogs_error("ogs_pkbuf_alloc() failed");
        return NULL;
    }

    memcpy(pkbuf->data, &cp_data_header, sizeof(smsf_sms_cp_hdr_t));

    return pkbuf;
}