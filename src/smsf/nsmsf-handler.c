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

    if (!UeSmsContextData->amf_id) {
        ogs_error("[%s] No amf_id", smsf_ue->supi);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                message, "No amfInstanceId", smsf_ue->supi));
        return false;
    }

    if (!UeSmsContextData->supi) {
        ogs_error("[%s] No supi", smsf_ue->supi);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                message, "No supi", smsf_ue->supi));
        return false;
    }

    if (UeSmsContextData->gpsi) {
        smsf_ue->gpsi = ogs_strdup(UeSmsContextData->gpsi);
    } else {
        ogs_info("[%s] No gpsi.  MT SMS will not be possible.", smsf_ue->supi);
    }

    int r;
    r = smsf_ue_sbi_discover_and_send(OGS_SBI_SERVICE_TYPE_NUDM_UECM, NULL,
            smsf_nudm_uecm_build_registration, smsf_ue, stream, NULL);
    ogs_expect(r == OGS_OK);
    ogs_assert(r != OGS_ERROR);

    return OGS_OK;
 
    /* Must hand off to PCF for AM POLICY.  ../src/amf/nudm-handler.c:327
    Or is it done in smsf-sm? to handle error cases.*/

    return OGS_OK;
}

bool smsf_nsmsf_sm_service_handle_deactivate(
        smsf_ue_t *smsf_ue, ogs_sbi_stream_t *stream,
        ogs_sbi_message_t *message)
{
    ogs_info("Deactivate");
    return OGS_OK;
}

bool smsf_nsmsf_sm_service_handle_uplink_sms(
        smsf_ue_t *smsf_ue, ogs_sbi_stream_t *stream,
        ogs_sbi_message_t *message)
{
    ogs_info("SMS");
    return OGS_OK;
}

/*bool smsf_nnrf_nsselection_handle_get(
        ogs_sbi_stream_t *stream, ogs_sbi_message_t *recvmsg)
{
    int status = OGS_SBI_HTTP_STATUS_OK;
    char *strerror = NULL;

    OpenAPI_authorized_network_slice_info_t AuthorizedNetworkSliceInfo;


    ogs_sbi_message_t sendmsg;
    ogs_sbi_response_t *response = NULL;

    ogs_assert(stream);
    ogs_assert(recvmsg);

    if (!recvmsg->param.nf_id) {
        status = OGS_SBI_HTTP_STATUS_BAD_REQUEST;
        strerror = ogs_msprintf("No nf-id");
        goto cleanup;
    }

    if (!recvmsg->param.nf_type) {
        status = OGS_SBI_HTTP_STATUS_BAD_REQUEST;
        strerror = ogs_msprintf("No nf-type");
        goto cleanup;
    }

    if (!recvmsg->param.slice_info_request_for_pdu_session_presence) {
        status = OGS_SBI_HTTP_STATUS_BAD_REQUEST;
        strerror = ogs_msprintf("Not implemented except PDU session");
        goto cleanup;
    }


    memset(&AuthorizedNetworkSliceInfo, 0, sizeof(AuthorizedNetworkSliceInfo));

    memset(&sendmsg, 0, sizeof(sendmsg));
    sendmsg.AuthorizedNetworkSliceInfo = &AuthorizedNetworkSliceInfo;

    response = ogs_sbi_build_response(&sendmsg, OGS_SBI_HTTP_STATUS_OK);
    ogs_assert(response);
    ogs_assert(true == ogs_sbi_server_send_response(stream, response));

    return true;

cleanup:
    ogs_assert(strerror);

    ogs_error("%s", strerror);
    ogs_sbi_server_send_error(stream, status, recvmsg, strerror, NULL);
    ogs_free(strerror);

    return false;
}
*/
