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
#include "sms.h"

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

    // This might be too soon.  Maybe some state machine work to do, too.
    smsf_ue_remove(smsf_ue);

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
    uint8_t templen;
    smsf_sms_cp_hdr_t cpheader;
    smsf_n1_n2_message_transfer_param_t param;

    int size = 0;
    size = sizeof(cpheader);
    ogs_info("This big %d", size);

    /* could copy first byte of pkbuf into a variable to read that, then put whole thing in correct structure, then pull. */
    /* cast the buffer onto a header struct gsm_header = (ogs_nas_5gsm_header_t *)payload_container->buffer; */

    memcpy(&cpheader, sms_payload_buf->data, sizeof(smsf_sms_cp_hdr_t));
    ogs_pkbuf_pull(sms_payload_buf, sizeof(smsf_sms_cp_hdr_t));

    ogs_info("MSGTYHPE: %d", cpheader.sm_service_message_type);

    /* these brackets make SWITCH CASE easier */
    switch(cpheader.sm_service_message_type) {
    case 1:
        ogs_info("CP-Data");
        /* up to 249 bytes of user data follows*/
        smsf_sms_cp_data_t cpdata;
        memcpy(&cpdata, &cpheader, sizeof(smsf_sms_cp_hdr_t));
        memcpy(&cpdata.cp_user_data_length, sms_payload_buf->data, 1);
        ogs_pkbuf_pull(sms_payload_buf, 1);

        smsf_sms_rpdu_message_type_t rpheader;
        memcpy(&rpheader, sms_payload_buf->data, sizeof(smsf_sms_rpdu_message_type_t));
        ogs_pkbuf_pull(sms_payload_buf, sizeof(smsf_sms_rpdu_message_type_t));

        ogs_info("CP Data Len: %d", cpdata.cp_user_data_length);
        ogs_info("RPDU Type: %d", rpheader.value);

        switch(rpheader.value) {
        case 0:
            ogs_info("RP-DATA (ms->n)");
            smsf_sms_rpdata_t rpdu;
            memset(&rpdu, 0, sizeof(smsf_sms_rpdata_t));

            rpdu.rpdu_message_type.value = rpheader.value;
            memcpy(&rpdu.rp_message_reference, sms_payload_buf->data, 1);
            ogs_pkbuf_pull(sms_payload_buf, sizeof(rpdu.rp_message_reference));
            memcpy(&templen, sms_payload_buf->data, 1);
            if (templen)
                ogs_error("OA Length Invalid");

            ogs_pkbuf_pull(sms_payload_buf, 1);
            memcpy(&templen, sms_payload_buf->data, 1);
            if (!templen)
                ogs_error("DA Length Invalid");

            memcpy(&rpdu.rp_destination_address, sms_payload_buf->data, templen + 1);
            ogs_pkbuf_pull(sms_payload_buf, templen + 1);
            ogs_info("DA Len: [%d]", rpdu.rp_destination_address.length);

            memcpy(&rpdu.rp_user_data_length, sms_payload_buf->data, 1);
            ogs_pkbuf_pull(sms_payload_buf, 1);

            /* RP Decoding complete.  Capture the TPDU now. */
            smsf_sms_tpdu_hdr_t tpdu_hdr;

            memcpy(&tpdu_hdr, sms_payload_buf->data, sizeof(tpdu_hdr));
            ogs_info("MTI: [%d]", tpdu_hdr.tpMTI);

            switch(tpdu_hdr.tpMTI) {
            case 0:
                ogs_info("SMS-DELIVER Report (ms->n)");
                ogs_info("Sending CP-Ack");

                memset(&param, 0, sizeof(param));
                param.n1smbuf = smsf_sms_encode_cp_ack(true, cpheader.flags.tio);
                ogs_assert(param.n1smbuf);

                smsf_namf_comm_send_n1_n2_message_transfer(
                        smsf_ue, stream, &param);
                break;

            case 1:
                ogs_info("SMS-SUBMIT (ms->n)");

                smsf_sms_tpdu_submit_t tpdu_submit;
                memset(&tpdu_submit, 0, sizeof(smsf_sms_tpdu_submit_t));
                memcpy(&tpdu_submit, sms_payload_buf->data, 2);
                ogs_pkbuf_pull(sms_payload_buf, 2);
                memcpy(&templen, sms_payload_buf->data, 1);
                memcpy(&tpdu_submit.tp_destination_address, sms_payload_buf->data, ((templen + 1) / 2) + 1 + 3);
                ogs_pkbuf_pull(sms_payload_buf, ((templen + 1) / 2) + 1 + 3);
                ogs_info("TP DA Len: [%d]", tpdu_submit.tp_destination_address.addr_length);

                //memcpy(&templen, sms_payload_buf->data, 1);

                // memcpy(&tpdu_submit.tpUD, sms_payload_buf->data, tpdu_submit.tpUDL);
                // ogs_info("TP UD Len: [%d]", tpdu_submit.tpUDL);

                smsf_sms_tp_address_t tp_da;
                memset(&tp_da, 0, sizeof(smsf_sms_tp_address_t));
                char *output_bcd;
                output_bcd = ogs_calloc(1, 22+1);
                tp_da = tpdu_submit.tp_destination_address;
                ogs_buffer_to_bcd(tp_da.tp_address, (tp_da.addr_length + 1) /2, output_bcd);

                ogs_info("I am [%s]", smsf_ue->gpsi);
                smsf_ue_t *mt_smsf_ue = NULL;
                char *mt_gpsi = ogs_msprintf("msisdn-%s", output_bcd);
                ogs_info("Looking for [%s]", mt_gpsi);
                mt_smsf_ue = smsf_ue_find_by_gpsi(mt_gpsi);
                if (!mt_smsf_ue)
                    ogs_error("Can't find MT Sub");

                if (mt_gpsi)
                    ogs_free(mt_gpsi);
                if (output_bcd)
                    ogs_free(output_bcd);

                /* Send CP-Ack to MO UE */
                //int r;
                ogs_info("Sending CP-Ack");

                memset(&param, 0, sizeof(param));
                param.n1smbuf = smsf_sms_encode_cp_ack(true, cpheader.flags.tio);
                ogs_assert(param.n1smbuf);

                smsf_namf_comm_send_n1_n2_message_transfer(
                        smsf_ue, stream, &param);
                
                /* Convert SUBMIT to DELIVER and Queue towards mt_ue*/

                /* Send CP-DATA to MT UE */
                ogs_info("Sending CP-DATA");

                memset(&param, 0, sizeof(param));

                smsf_sms_tpdu_deliver_t tpduDeliver;
                memset(&tpduDeliver, 0, sizeof(smsf_sms_tpdu_deliver_t));
                tpduDeliver.header.tpMMS = 1;
                tpduDeliver.tpPID = tpdu_submit.tpPID;
                tpduDeliver.tpDCS = tpdu_submit.tpDCS;
                tpduDeliver.tpUDL = tpdu_submit.tpUDL;

                tpduDeliver.tp_originator_address.addr_length = 11;
                tpduDeliver.tp_originator_address.header.ext = 1;
                tpduDeliver.tp_originator_address.header.ton = 1;
                tpduDeliver.tp_originator_address.header.npi = 1;

                param.n1smbuf = smsf_sms_encode_rp_data(false, 0, 69, &tpduDeliver);

                ogs_assert(param.n1smbuf);

                smsf_namf_comm_send_n1_n2_message_transfer(mt_smsf_ue, stream, &param);

                /* Send RP-ACK to MO UE */
                //int r;
                ogs_info("Sending RP-Ack");

                memset(&param, 0, sizeof(param));
                param.n1smbuf = smsf_sms_encode_rp_ack(
                    true, cpheader.flags.tio, rpdu.rp_message_reference);
                ogs_assert(param.n1smbuf);

                smsf_namf_comm_send_n1_n2_message_transfer(
                        smsf_ue, stream, &param);

                break;

            case 2:
                ogs_info("SMS-COMMAND (ms->n)");
                break;

            default:
                ogs_info("Undefined TPDU Message Type for ms->n");
                /* goto end, send nsmf error response */
                return false;
            }

            // smsf_sms_address_t rp_da;
            // char outbuf[30] = {0};
            // ogs_log_hexdump(OGS_LOG_INFO, outbuf, 15);

            // /* that's just the smsc!*/
            // rp_da = rpdu.rp_destination_address;

            // ogs_buffer_to_bcd(rp_da.rp_address, (rpdu.rp_destination_address.length - 1), outbuf);

            // ogs_log_hexdump(OGS_LOG_INFO, outbuf, 15);






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
            ogs_info("Undefined RPDU Message Type for ms->n");
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
    // if (ogs_pkbuf_pull(sms_payload_buf, size) == NULL) {
    //    ogs_error("ogs_pkbuf_pull() failed [size:%d]", (int)size);
    //    return -1;
    // }

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

    return OGS_OK;
}
