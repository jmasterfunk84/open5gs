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
#include "namf-handler.h"

bool smsf_namf_comm_handle_n1_n2_message_transfer(
        smsf_ue_t *smsf_ue, int state, ogs_sbi_message_t *recvmsg)
{
    OpenAPI_n1_n2_message_transfer_rsp_data_t *N1N2MessageTransferRspData;

    ogs_assert(smsf_ue);
    ogs_assert(recvmsg);

    N1N2MessageTransferRspData = recvmsg->N1N2MessageTransferRspData;
    if (!N1N2MessageTransferRspData) {
        ogs_error("No N1N2MessageTransferRspData [status:%d]",
                recvmsg->res_status);
        return false;
    }

    if (recvmsg->res_status == OGS_SBI_HTTP_STATUS_OK) {
        if (N1N2MessageTransferRspData->cause ==
            OpenAPI_n1_n2_message_transfer_cause_N1_MSG_NOT_TRANSFERRED) {
            /* Nothing */
        } else if (N1N2MessageTransferRspData->cause ==
            OpenAPI_n1_n2_message_transfer_cause_N1_N2_TRANSFER_INITIATED) {
            /* Nothing */
        } else {
            ogs_error("Not implemented [cause:%d]",
                    N1N2MessageTransferRspData->cause);
            ogs_assert_if_reached();
        }
    } else {
        ogs_error("[%s] HTTP response error [status:%d cause:%d]",
            smsf_ue->supi, recvmsg->res_status,
            N1N2MessageTransferRspData->cause);
    }

    return true;
}

bool smsf_namf_comm_handle_n1_n2_message_transfer_failure_notify(
        ogs_sbi_stream_t *stream, ogs_sbi_message_t *recvmsg)
{
    OpenAPI_n1_n2_msg_txfr_failure_notification_t
        *N1N2MsgTxfrFailureNotification = NULL;

    ogs_assert(stream);
    ogs_assert(recvmsg);

    N1N2MsgTxfrFailureNotification = recvmsg->N1N2MsgTxfrFailureNotification;
    if (!N1N2MsgTxfrFailureNotification) {
        ogs_error("No N1N2MsgTxfrFailureNotification");
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                recvmsg, "No N1N2MsgTxfrFailureNotification", NULL));
        return false;
    }

    if (!N1N2MsgTxfrFailureNotification->cause) {
        ogs_error("No Cause");
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                recvmsg, "No Cause", NULL));
        return false;
    }

    if (!N1N2MsgTxfrFailureNotification->n1n2_msg_data_uri) {
        ogs_error("No n1n2MsgDataUri");
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                recvmsg, "No n1n2MsgDataUri", NULL));
        return false;
    }

/*
    smsf_ue = smsf_ue_find_by_paging_n1n2message_location(
        N1N2MsgTxfrFailureNotification->n1n2_msg_data_uri);
    if (!smsf_ue) {
        ogs_error("Not found");
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_NOT_FOUND,
                recvmsg, N1N2MsgTxfrFailureNotification->n1n2_msg_data_uri,
                NULL));
        return false;
    }
*/
    /*
     * TODO:
     *
     * TS23.502 4.2.3.3 Network Triggered Service Request
     *
     * 5. [Conditional] AMF to smsf:
     * Namf_Communication_N1N2Transfer Failure Notification.
     *
     * When a Namf_Communication_N1N2Transfer Failure Notification
     * is received, smsf informs the UPF (if applicable).
     *
     * Procedure for pause of charging at smsf is specified in clause 4.4.4.
     */

    ogs_assert(true == ogs_sbi_send_http_status_no_content(stream));
    return true;
}
