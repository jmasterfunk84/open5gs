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
#include "namf-build.h"

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

    return true;
}

int smsf_nsmsf_sm_service_handle_deactivate(
        smsf_ue_t *smsf_ue, ogs_sbi_stream_t *stream,
        ogs_sbi_message_t *message)
{
    ogs_assert(stream);
    ogs_assert(message);

    ogs_info("Deactivate");

    // This might be too soon.
    smsf_ue_remove(smsf_ue);

    ogs_sbi_message_t sendmsg;
    ogs_sbi_header_t header;
    ogs_sbi_response_t *response = NULL;

    memset(&header, 0, sizeof(header));
    header.service.name =
        (char *)OGS_SBI_SERVICE_NAME_NSMSF_SMS;
    header.api.version = (char *)OGS_SBI_API_V2;
    header.resource.component[0] = (char *)OGS_SBI_RESOURCE_NAME_UE_CONTEXTS;
    header.resource.component[1] = smsf_ue->supi;

    memset(&sendmsg, 0, sizeof(sendmsg));

    response = ogs_sbi_build_response(&sendmsg, OGS_SBI_HTTP_STATUS_NO_CONTENT);
    ogs_assert(response);
    ogs_assert(true == ogs_sbi_server_send_response(stream, response));

    return OGS_OK;
}

bool smsf_nsmsf_sm_service_handle_uplink_sms(
        smsf_ue_t *smsf_ue, ogs_sbi_stream_t *stream,
        ogs_sbi_message_t *message)
{
    OpenAPI_sms_record_data_t *SmsRecordData = NULL;
    OpenAPI_ref_to_binary_data_t *sms_payload = NULL;
    ogs_pkbuf_t *sms_payload_buf = NULL;

    ogs_assert(stream);
    ogs_assert(message);

    ogs_info("SMS");

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

    smsf_sms_cp_hdr_t cpheader;

    int size = 0;
    size = sizeof(cpheader);
    ogs_info("This big %d", size);

    /* could copy first byte of pkbuf into a variable to read that, then put whole thing in correct structure, then pull. */

    memcpy(&cpheader, sms_payload_buf->data, sizeof(smsf_sms_cp_hdr_t));
    ogs_pkbuf_pull(sms_payload_buf, sizeof(cpheader));

    ogs_info("MSGTYHPE: %d", cpheader.sm_service_message_type);

    /* these brackets make SWITCH CASE easier */
    switch(cpheader.sm_service_message_type) {
    case 1:
        ogs_info("CP-Data");
        /* up to 249 bytes of user data follows*/
        smsf_sms_cp_data_t cpdata;
        memcpy(&cpdata, sms_payload_buf->data, sizeof(smsf_sms_cp_data_t));
        ogs_pkbuf_pull(sms_payload_buf, sizeof(cpheader));

        ogs_info("CP Data Len: %d", cpdata.cp_user_data_length);
        ogs_info("RPDU Type: %d", cpdata.rpdu_message_type.value);

        switch(cpdata.rpdu_message_type.value) {
        case 0:
            ogs_info("RP-DATA (ms->n)");
            smsf_sms_rpdu_t rpdu;
            memcpy(&rpdu.rp_message_reference, sms_payload_buf->data, sizeof(rpdu.rp_message_reference));
            ogs_pkbuf_pull(sms_payload_buf, sizeof(rpdu.rp_message_reference));
            memcpy(&rpdu.rp_originator_address_len, sms_payload_buf->data, sizeof(rpdu.rp_originator_address_len));
            ogs_pkbuf_pull(sms_payload_buf, sizeof(rpdu.rp_originator_address_len));
            /* check if 0 */
            memcpy(&rpdu.rp_destination_address_len, sms_payload_buf->data, sizeof(rpdu.rp_destination_address_len));
            ogs_pkbuf_pull(sms_payload_buf, sizeof(rpdu.rp_destination_address_len));
            ogs_info("DA Len: [%d]", rpdu.rp_destination_address_len);

            /* maybe a memset for these variable len ones to null them out. */
            memcpy(&rpdu.rp_destination_address, sms_payload_buf->data, rpdu.rp_destination_address_len);
            ogs_pkbuf_pull(sms_payload_buf, rpdu.rp_destination_address_len);

            smsf_sms_address_t rp_da;
            char outbuf[30] = {0};
            ogs_log_hexdump(OGS_LOG_INFO, outbuf, 15);

            rp_da = rpdu.rp_destination_address;

            ogs_buffer_to_bcd(rp_da.rp_address, (rpdu.rp_destination_address_len - 1), outbuf);

            ogs_log_hexdump(OGS_LOG_INFO, outbuf, 15);

            ogs_info("I am [%s]", smsf_ue->gpsi);
            smsf_ue_t mt_smsf_ue;
            mt_smsf_ue = smsf_ue_find_by_gpsi(outbuf);
            if (!mt_smsf_ue)
                ogs_error("Can't find MT Sub");


            /* should better specify length here. */
            memcpy(&rpdu.rp_user_data, sms_payload_buf->data, sizeof(sms_payload_buf->data));
            break;

        case 2:
            ogs_info("RP-ACK (ms->n)");
            break;

        case 4:
            ogs_info("RP-ERROR (ms->n)");
            break;

        case 6:
            ogs_info("RP-SMMA (ms->n)");
            break;

        default:
            ogs_info("Undefined RPDU Message Type");
            /* goto end, send nsmf error response */
            return false;
        }

        break;
    case 4:
        ogs_info("CP-ACK");
        /* no bytes follow */
        break;
    case 16:
        ogs_info("CP-Error");
        /* 1 byte cp-cause follows */
        break;

    default:
        ogs_info("Undefined CP Message Type");
        /* goto end, send nsmf error response */
        return false;
    }

    /* i think pull removes bytes from the start of the buffer? */
    if (ogs_pkbuf_pull(sms_payload_buf, size) == NULL) {
       ogs_error("ogs_pkbuf_pull() failed [size:%d]", (int)size);
       return -1;
    }




    /* I think it should split CP data, and RP data.  RP data would normally go to SMSC
     * but, here we have to rip the userdata from the rp-data to get the destination address.
     */
    /* Check MO subscription if allowed (send 403) (after CP-ACK is sent) */
    /* check MT subscription if allowed */
    /* queue event for MT smsf_ue */
    /* if UE found, then say so.  If ue not found, respond with error. */
    /* form the SmsRecordDeliveryData */

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

    /* need xact and related stream, because your stream disappears) */


    /* Send CP-Ack to MO UE */
    //int r;
    smsf_n1_n2_message_transfer_param_t param;

    memset(&param, 0, sizeof(param));

    param.n1smbuf = ogs_pkbuf_alloc(NULL,2);
    ogs_pkbuf_put_u8(param.n1smbuf,137);        // SMS messages/TIO/TIF1
    ogs_pkbuf_put_u8(param.n1smbuf,4);          // CP-ACK

    ogs_assert(param.n1smbuf);

    smsf_namf_comm_send_n1_n2_message_transfer(smsf_ue, &param);

    return OGS_OK;
}
