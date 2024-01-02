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

#include "sbi-path.h"
#include "nnrf-handler.h"
#include "nsmsf-handler.h"
#include "nudm-build.h"

bool smsf_nsmsf_sm_service_handle_activate(
        smsf_ue_t *smsf_ue, ogs_sbi_stream_t *stream,
        ogs_sbi_message_t *message)
{
    ogs_info("Activate");
    OpenAPI_ue_sms_context_data_t *UeSmsContextData = NULL;

    ogs_assert(stream);
    ogs_assert(message);

    UeSmsContextData = message->UeSmsContextData;
    if (!UeSmsContextData) {
        ogs_error("[%s] No UeSmsContextData", smsf_ue->supi);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                message, "No UeSmsContextData", smsf_ue->supi));
        return false;
    }

    if (!UeSmsContextData->supi) {
        ogs_error("[%s] No supi", smsf_ue->supi);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                message, "No supi", smsf_ue->supi));
        return false;
    }
    /* shall we compare that the supi inside is the same as outside? */

    if (!UeSmsContextData->amf_id) {
        ogs_error("[%s] No amf_id", smsf_ue->supi);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                message, "No amfInstanceId", smsf_ue->supi));
        return false;
    }
    smsf_ue->amf_instance_id = ogs_strdup(UeSmsContextData->amf_id);

    if (!UeSmsContextData->access_type) {
        ogs_error("[%s] No access_type", smsf_ue->supi);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                message, "No access_type", smsf_ue->supi));
        return false;
    }
    smsf_ue->access_type = UeSmsContextData->access_type;

    if (UeSmsContextData->gpsi) {
        smsf_ue->gpsi = ogs_strdup(UeSmsContextData->gpsi);
    } else {
        ogs_info("[%s] No gpsi.  MT SMS will not be possible.", smsf_ue->supi);
    }

    // Can remove if we store the elements seperately.
    smsf_ue->ue_sms_context_data = OpenAPI_ue_sms_context_data_copy(
            smsf_ue->ue_sms_context_data,
            message->UeSmsContextData);

    int r;
    r = smsf_ue_sbi_discover_and_send(OGS_SBI_SERVICE_TYPE_NUDM_UECM, NULL,
            smsf_nudm_uecm_build_smsf_registration, smsf_ue, stream, NULL);
    ogs_expect(r == OGS_OK);
    ogs_assert(r != OGS_ERROR);

    return OGS_OK;
}

bool smsf_nsmsf_sm_service_handle_deactivate(
        smsf_ue_t *smsf_ue, ogs_sbi_stream_t *stream,
        ogs_sbi_message_t *message)
{
    ogs_assert(stream);
    ogs_assert(message);

    ogs_info("Deactivate");
    return OGS_OK;
}

bool smsf_nsmsf_sm_service_handle_uplink_sms(
        smsf_ue_t *smsf_ue, ogs_sbi_stream_t *stream,
        ogs_sbi_message_t *message)
{
    OpenAPI_sms_record_data_t *SmsRecordData = NULL;

    ogs_assert(stream);
    ogs_assert(message);

    ogs_info("SMS");

    SmsRecordData = message->SmsRecordData;
    if (!SmsRecordData) {
        ogs_error("[%s] No SmsRecordData", smsf_ue->s
        upi);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                message, "No SmsRecordData", smsf_ue->supi));
        return false;
    }

    if (!SmsRecordData->sms_record_id) {
        ogs_error("[%s] No smsRecordId", smsf_ue->supi);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                message, "No smsRecordId", smsf_ue->supi));
        return false;
    }

    if (!SmsRecordData->sms_payload) {
        ogs_error("[%s] No smsPayload", smsf_ue->supi);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                message, "No smsPayload", smsf_ue->supi));
        return false;
    }

    /* I think it should split CP data, and RP data.  RP data would normally go to SMSC
     * but, here we have to rip the userdata from the rp-data to get the destination address.
     */
    /* queue event for MT smsf_ue */
    /* if UE found, then say so.  If ue not found, respond with error. */
    /* form the SmsRecordDeliveryData */


    return OGS_OK;
}
