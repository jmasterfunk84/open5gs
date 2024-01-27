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

#include "sbi-path.h"
#include "nnrf-handler.h"
#include "nsmsf-handler.h"
#include "nudm-build.h"
#include "namf-build.h"
#include "sms.h"

bool smsf_nsmsf_sm_service_handle_activate(
        smsf_ue_t *smsf_ue, ogs_sbi_stream_t *stream,
        ogs_sbi_message_t *message)
{
    int r;

    ogs_assert(smsf_ue);
    ogs_assert(stream);
    ogs_assert(message);
    
    ogs_info("[%s] Activate SMService", smsf_ue->supi);

    OpenAPI_ue_sms_context_data_t *UeSmsContextData = NULL;

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
        ogs_error("[%s] No gpsi.  SMS-MT will not be possible", smsf_ue->supi);
    }

    r = smsf_ue_sbi_discover_and_send(OGS_SBI_SERVICE_TYPE_NUDM_UECM, NULL,
            smsf_nudm_uecm_build_smsf_registration, smsf_ue, stream, NULL);
    ogs_expect(r == OGS_OK);
    ogs_assert(r != OGS_ERROR);

    return true;
}

int smsf_nsmsf_sm_service_handle_deactivate(
        smsf_ue_t *smsf_ue, ogs_sbi_stream_t *stream,
        ogs_sbi_message_t *message)
{
    int r;

    ogs_assert(smsf_ue);
    ogs_assert(stream);
    ogs_assert(message);

    ogs_info("[%s] Deactivate SMService", smsf_ue->supi);

    /* do we go to the nf we already subscribed to? */
    r = smsf_ue_sbi_discover_and_send(OGS_SBI_SERVICE_TYPE_NUDM_SDM, NULL,
            smsf_nudm_sdm_build_subscription_delete, smsf_ue, stream, NULL);
    ogs_expect(r == OGS_OK);
    ogs_assert(r != OGS_ERROR);

    return OGS_OK;
}

bool smsf_nsmsf_sm_service_handle_uplink_sms(
        smsf_ue_t *smsf_ue, ogs_sbi_stream_t *stream,
        ogs_sbi_message_t *message)
{
    ogs_assert(smsf_ue);
    ogs_assert(stream);
    ogs_assert(message);

    ogs_debug("[%s] UplinkSMS", smsf_ue->supi);

    OpenAPI_sms_record_data_t *SmsRecordData = NULL;
    OpenAPI_ref_to_binary_data_t *sms_payload = NULL;
    ogs_pkbuf_t *sms_payload_buf = NULL;

    SmsRecordData = message->SmsRecordData;
    if (!SmsRecordData) {
        ogs_error("[%s] No SmsRecordData", smsf_ue->supi);
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

    sms_payload = SmsRecordData->sms_payload;
    if (!sms_payload || !sms_payload->content_id) {
        ogs_error("[%s] No sms_payload and/or content_id", smsf_ue->supi);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                message, "No smsPayload and/or content_id", smsf_ue->supi));
        return false;
    }

    sms_payload_buf = ogs_sbi_find_part_by_content_id(
            message, sms_payload->content_id);
    if (!sms_payload_buf) {
        ogs_error("[%s] No SMS Payload Content", smsf_ue->supi);
        return false;
    }

    /* do like ogs_nas_eps_decode_eps_network_feature_support*/
    /* cast the buffer onto a header struct gsm_header = (ogs_nas_5gsm_header_t *)payload_container->buffer; */
    smsf_sms_cp_hdr_t cpheader;
    smsf_n1_n2_message_transfer_param_t param;

    memcpy(&cpheader, sms_payload_buf->data, sizeof(smsf_sms_cp_hdr_t));
    ogs_pkbuf_pull(sms_payload_buf, sizeof(smsf_sms_cp_hdr_t));

    ogs_debug("[%s] CP Header Message Type [%d]", smsf_ue->supi,
            cpheader.sm_service_message_type);

    switch(cpheader.sm_service_message_type) {
    case SMSF_SERVICE_MESSAGE_TYPE_CP_DATA:
        ogs_debug("[%s] CP-Data", smsf_ue->supi);
        /* up to 249 bytes of user data follows*/
        smsf_sms_cp_data_t cpdata;
        memcpy(&cpdata, &cpheader, sizeof(smsf_sms_cp_hdr_t));
        memcpy(&cpdata.cp_user_data_length, sms_payload_buf->data, 1);
        ogs_pkbuf_pull(sms_payload_buf, 1);

        ogs_debug("[%s] Sending CP-ACK", smsf_ue->supi);
        memset(&param, 0, sizeof(param));
        int ti_flag_ack = true;
        if (cpheader.flags.tif)
            ti_flag_ack = false;
        param.n1smbuf = smsf_sms_encode_cp_ack(ti_flag_ack,
                cpheader.flags.tio);
        ogs_assert(param.n1smbuf);
        smsf_namf_comm_send_n1_n2_message_transfer(
                smsf_ue, stream, &param);

        /* The SMSF would normally send the CP-DATA payload to the SMSC.
         * This could be done either Diameter SGD, MAP, or SBI.
         * We will process the message here to allow for local delivery. 
         */

        memset(&param, 0, sizeof(param));
        ogs_pkbuf_t *fromsmsc;
        fromsmsc = send_to_local_smsc(smsf_ue, stream, sms_payload_buf);
        if (fromsmsc) {
            param.n1smbuf = smsf_sms_encode_cp_data(ti_flag_ack,
                    cpheader.flags.tio, fromsmsc);
            ogs_assert(param.n1smbuf);
            smsf_namf_comm_send_n1_n2_message_transfer(
                    smsf_ue, stream, &param);
        }
        break;
    case SMSF_SERVICE_MESSAGE_TYPE_CP_ACK:
        ogs_debug("[%s] CP-ACK", smsf_ue->supi);
        /* no bytes follow */
        break;
    case SMSF_SERVICE_MESSAGE_TYPE_CP_ERROR:
        ogs_debug("[%s] CP-ERROR", smsf_ue->supi);
        /* 1 byte cp-cause follows */
        int cp_cause = 0;
        if (sms_payload_buf->data = 1) {
            memcpy(cp_cause, sms_payload_buf->data, 1);
            ogs_error("[%s] CP-ERROR[%d]", smsf_ue->supi, cp_cause);
        }
        break;

    default:
        ogs_error("[%s] Undefined CP Message Type [%d]",
                smsf_ue->supi, cpheader.sm_service_message_type);
        /* goto end, send nsmf error response */
        return false;
    }

    ogs_sbi_message_t sendmsg;
    ogs_sbi_header_t header;
    ogs_sbi_response_t *response = NULL;

    OpenAPI_sms_record_delivery_data_t SmsRecordDeliveryData;

    memset(&SmsRecordDeliveryData, 0, sizeof(SmsRecordDeliveryData));

    SmsRecordDeliveryData.sms_record_id = SmsRecordData->sms_record_id;
    SmsRecordDeliveryData.delivery_status = 
            OpenAPI_sms_delivery_status_SMS_DELIVERY_SMSF_ACCEPTED;

    memset(&header, 0, sizeof(header));
    header.service.name =
        (char *)OGS_SBI_SERVICE_NAME_NSMSF_SMS;
    header.api.version = (char *)OGS_SBI_API_V2;
    header.resource.component[0] = (char *)OGS_SBI_RESOURCE_NAME_UE_CONTEXTS;
    header.resource.component[1] = smsf_ue->supi;
    header.resource.component[2] = (char *)OGS_SBI_RESOURCE_NAME_SEND_SMS;

    memset(&sendmsg, 0, sizeof(sendmsg));
    sendmsg.SmsRecordDeliveryData = &SmsRecordDeliveryData;

    response = ogs_sbi_build_response(&sendmsg, OGS_SBI_HTTP_STATUS_OK);
    ogs_assert(response);
    ogs_assert(true == ogs_sbi_server_send_response(stream, response));

    return OGS_OK;
}
