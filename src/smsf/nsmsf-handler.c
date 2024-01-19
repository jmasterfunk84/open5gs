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
    int r;

    ogs_assert(smsf_ue);
    ogs_assert(stream);
    ogs_assert(message);
    
    ogs_debug("[%s] Activate SMService", smsf_ue->supi);

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
        ogs_error("[%s] No gpsi.  MT SMS will not be possible.", smsf_ue->supi);
    }

    // Can remove if we store the elements seperately.
    smsf_ue->ue_sms_context_data = OpenAPI_ue_sms_context_data_copy(
            smsf_ue->ue_sms_context_data,
            message->UeSmsContextData);

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

    ogs_debug("[%s] Deactivate SMService", smsf_ue->supi);

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
    uint8_t templen;
    smsf_sms_cp_hdr_t cpheader;
    smsf_n1_n2_message_transfer_param_t param;

    int size = 0;
    size = sizeof(cpheader);

    /* could copy first byte of pkbuf into a variable to read that, then put whole thing in correct structure, then pull. */
    /* cast the buffer onto a header struct gsm_header = (ogs_nas_5gsm_header_t *)payload_container->buffer; */

    memcpy(&cpheader, sms_payload_buf->data, sizeof(smsf_sms_cp_hdr_t));
    ogs_pkbuf_pull(sms_payload_buf, sizeof(smsf_sms_cp_hdr_t));

    ogs_debug("[%s] CP Header Message Type [%d]", smsf_ue->supi,
            cpheader.sm_service_message_type);

    /* these brackets make SWITCH CASE easier */
    switch(cpheader.sm_service_message_type) {
    case 1:
        ogs_debug("[%s] CP-Data", smsf_ue->supi);
        /* up to 249 bytes of user data follows*/
        smsf_sms_cp_data_t cpdata;
        memcpy(&cpdata, &cpheader, sizeof(smsf_sms_cp_hdr_t));
        memcpy(&cpdata.cp_user_data_length, sms_payload_buf->data, 1);
        ogs_pkbuf_pull(sms_payload_buf, 1);

        smsf_sms_rpdu_message_type_t rpheader;
        memcpy(&rpheader, sms_payload_buf->data,
                sizeof(smsf_sms_rpdu_message_type_t));
        ogs_pkbuf_pull(sms_payload_buf, sizeof(smsf_sms_rpdu_message_type_t));

        switch(rpheader.value) {
        case 0:
            ogs_debug("[%s] RP-DATA (ms->n)", smsf_ue->supi);
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

            memcpy(&rpdu.rp_destination_address,
                    sms_payload_buf->data, templen + 1);
            ogs_pkbuf_pull(sms_payload_buf, templen + 1);

            memcpy(&rpdu.rp_user_data_length, sms_payload_buf->data, 1);
            ogs_pkbuf_pull(sms_payload_buf, 1);

            /* RP Decoding complete.  Capture the TPDU now. */
            smsf_sms_tpdu_hdr_t tpdu_hdr;

            memcpy(&tpdu_hdr, sms_payload_buf->data, sizeof(tpdu_hdr));

            switch(tpdu_hdr.tpMTI) {
            case 0:
                ogs_debug("[%s] SMS-DELIVER Report (ms->n)", smsf_ue->supi);

                memset(&param, 0, sizeof(param));
                param.n1smbuf = smsf_sms_encode_cp_ack(true,
                        cpheader.flags.tio);
                ogs_assert(param.n1smbuf);

                smsf_namf_comm_send_n1_n2_message_transfer(
                        smsf_ue, stream, &param);
                break;

            case 1:
                ogs_debug("[%s] SMS-SUBMIT (ms->n)", smsf_ue->supi);

                smsf_sms_tpdu_submit_t tpdu_submit;
                memset(&tpdu_submit, 0, sizeof(smsf_sms_tpdu_submit_t));
                memcpy(&tpdu_submit, sms_payload_buf->data, 2);
                ogs_pkbuf_pull(sms_payload_buf, 2);
                memcpy(&templen, sms_payload_buf->data, 1);
                memcpy(&tpdu_submit.tp_destination_address,
                        sms_payload_buf->data, 2 + ((templen + 1) / 2));
                ogs_pkbuf_pull(sms_payload_buf, 2 + ((templen + 1) / 2));   // Address length, Address type, address itself
                memcpy(&tpdu_submit.tpPID, sms_payload_buf->data, 3);       // PID, DCS, UDL
                ogs_pkbuf_pull(sms_payload_buf, 3);

                int tpdurealbytes;
                if (!tpdu_submit.tpDCS) {
                    tpdurealbytes = ((tpdu_submit.tpUDL+1)*7/8);
                } else {
                    tpdurealbytes = tpdu_submit.tpUDL;
                }

                memcpy(&tpdu_submit.tpUD, sms_payload_buf->data, tpdurealbytes);

                /* Begin looking for our TPDU destination smsf_ue */

                smsf_sms_tp_address_t tp_da;
                memset(&tp_da, 0, sizeof(smsf_sms_tp_address_t));
                char *output_bcd;
                output_bcd = ogs_calloc(1, 22+1);
                tp_da = tpdu_submit.tp_destination_address;
                ogs_buffer_to_bcd(tp_da.tp_address, (tp_da.addr_length + 1) /2,
                        output_bcd);

                /* Look for the MT MSISDN */
                smsf_ue_t *mt_smsf_ue = NULL;
                char *mt_gpsi = ogs_msprintf("msisdn-%s", output_bcd);
                ogs_debug("[%s] Looking for [%s]", smsf_ue->supi, mt_gpsi);
                mt_smsf_ue = smsf_ue_find_by_gpsi(mt_gpsi);
                if (!mt_smsf_ue)
                    ogs_error("Can't find MT Sub");

                if (mt_gpsi)
                    ogs_free(mt_gpsi);
                if (output_bcd)
                    ogs_free(output_bcd);

                /* Send CP-Ack to MO UE */
                //int r;
                ogs_debug("[%s] SMS-SUBMIT (ms->n)", smsf_ue->supi);

                memset(&param, 0, sizeof(param));
                param.n1smbuf = smsf_sms_encode_cp_ack(true,
                        cpheader.flags.tio);
                ogs_assert(param.n1smbuf);

                smsf_namf_comm_send_n1_n2_message_transfer(
                        smsf_ue, stream, &param);
                
                /* Convert SUBMIT to DELIVER and Queue towards mt_ue*/

                if (mt_smsf_ue) {
                    /* Send CP-DATA to MT UE */
                    ogs_debug("[%s] Sending CP-DATA", mt_smsf_ue->supi);

                    memset(&param, 0, sizeof(param));

                    smsf_sms_tpdu_deliver_t tpduDeliver;
                    //smsf_copy_submit_to_deliver(&tpduDeliver, const? &tpdu_submit, mt_smsf_ue, smsf_ue)
                    memset(&tpduDeliver, 0, sizeof(smsf_sms_tpdu_deliver_t));
                    tpduDeliver.header.tpUDHI = tpdu_submit.header.tpUDHI;
                    tpduDeliver.header.tpMMS = 1;  // Could eval DCS for concatenation

                    /* Populate the Sender's MSISDN */
                    char *msisdn;
                    msisdn = ogs_calloc(1, 15);
                    memcpy(msisdn, (smsf_ue->gpsi) + 7,
                            strlen(smsf_ue->gpsi) - 7);
                    char *msisdn_bcd;
                    msisdn_bcd = ogs_calloc(1, 64);
                    int msisdn_bcd_len;
                    ogs_bcd_to_buffer(msisdn, msisdn_bcd, &msisdn_bcd_len);
                    if (msisdn)
                        ogs_free(msisdn);
                    tpduDeliver.tp_originator_address.addr_length =
                            strlen(msisdn);
                    tpduDeliver.tp_originator_address.header.ext = 1;
                    tpduDeliver.tp_originator_address.header.ton = 1;
                    tpduDeliver.tp_originator_address.header.npi = 1;
                    memcpy(&tpduDeliver.tp_originator_address.tp_address,
                            msisdn_bcd, msisdn_bcd_len);

                    tpduDeliver.tpPID = tpdu_submit.tpPID;
                    tpduDeliver.tpDCS = tpdu_submit.tpDCS;
                    smsf_sms_set_sc_timestamp(&tpduDeliver.tpSCTS);
                    tpduDeliver.tpUDL = tpdu_submit.tpUDL;

                    memcpy(&tpduDeliver.tpUD, &tpdu_submit.tpUD, tpdurealbytes);

                    mt_smsf_ue->mt_message_reference += 1;
                    if (mt_smsf_ue->mt_message_reference == 0)
                        mt_smsf_ue->mt_message_reference = 1;

                    mt_smsf_ue->mt_tio += 1;
                    if (mt_smsf_ue->mt_tio > 7)
                        mt_smsf_ue->mt_tio = 0;

                    param.n1smbuf = smsf_sms_encode_rp_data(false,
                            mt_smsf_ue->mt_tio,
                            mt_smsf_ue->mt_message_reference,
                            &tpduDeliver);

                    ogs_assert(param.n1smbuf);

                    smsf_namf_comm_send_n1_n2_message_transfer(mt_smsf_ue,
                            stream, &param);

                    if (msisdn_bcd)
                        ogs_free(msisdn_bcd);
                }
                /* Send RP-ACK to MO UE */
                //int r;
                ogs_debug("[%s] Sending RP-Ack", smsf_ue->supi);

                memset(&param, 0, sizeof(param));
                param.n1smbuf = smsf_sms_encode_rp_ack(
                    true, cpheader.flags.tio, rpdu.rp_message_reference);
                ogs_assert(param.n1smbuf);

                smsf_namf_comm_send_n1_n2_message_transfer(
                        smsf_ue, stream, &param);

                break;

            case 2:
                ogs_debug("[%s] SMS-COMMAND (ms->n)", smsf_ue->supi);
                break;

            default:
                ogs_error("[%s] Undefined TPDU Message Type for ms->n [%d]",
                        smsf_ue->supi, tpdu_hdr.tpMTI);

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

        case 2:
            ogs_debug("[%s] RP-ACK (ms->n)", smsf_ue->supi);
            // Sending CP-ACK //
            memset(&param, 0, sizeof(param));
            param.n1smbuf = smsf_sms_encode_cp_ack(false, cpheader.flags.tio);
            ogs_assert(param.n1smbuf);

            smsf_namf_comm_send_n1_n2_message_transfer(
                    smsf_ue, stream, &param);
            break;

        case 4:
            ogs_debug("[%s] RP-ERROR (ms->n)", smsf_ue->supi);
            break;

        case 6:
            ogs_debug("[%s] RP-SMMA (ms->n)", smsf_ue->supi);
            break;

        default:
            ogs_error("[%s] Undefined RPDU Message Type for ms->n [%d]",
                    smsf_ue->supi, rpheader.value);
            /* goto end, send nsmf error response */
            return false;
        }

        break;
    case 4:
        ogs_debug("[%s] CP-ACK", smsf_ue->supi);
        /* no bytes follow */
        break;
    case 16:
        ogs_debug("[%s] CP-ERROR", smsf_ue->supi);
        /* 1 byte cp-cause follows */
        break;

    default:
        ogs_error("[%s] Undefined CP Message Type [%d]",
                smsf_ue->supi, cpheader.sm_service_message_type);
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
