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

#include "namf-build.h"
//#include "gsm-build.h"
#include "ngap-build.h"

ogs_sbi_request_t *smsf_namf_comm_build_n1_n2_message_transfer(
        smsf_ue_t *smsf_ue, smsf_n1_n2_message_transfer_param_t *param)
{
    int i;
    smsf_ue_t *smsf_ue = NULL;

    ogs_sbi_message_t message;
    ogs_sbi_request_t *request = NULL;

    ogs_sbi_server_t *server = NULL;
    ogs_sbi_header_t header;

    OpenAPI_n1_n2_message_transfer_req_data_t N1N2MessageTransferReqData;

    OpenAPI_n1_message_container_t n1MessageContainer;
    OpenAPI_ref_to_binary_data_t n1MessageContent;

    OpenAPI_n2_info_container_t n2InfoContainer;
    OpenAPI_n2_sm_information_t smInfo;
    OpenAPI_n2_info_content_t n2InfoContent;
    OpenAPI_ref_to_binary_data_t ngapData;

    ogs_assert(smsf_ue);
    ogs_assert(smsf_ue->supi);

    ogs_assert(param);
    ogs_assert(param->state);
    ogs_assert(param->n1smbuf || param->n2smbuf);

    memset(&message, 0, sizeof(message));
    message.h.method = (char *)OGS_SBI_HTTP_METHOD_POST;
    message.h.service.name = (char *)OGS_SBI_SERVICE_NAME_NAMF_COMM;
    message.h.api.version = (char *)OGS_SBI_API_V1;
    message.h.resource.component[0] = (char *)OGS_SBI_RESOURCE_NAME_UE_CONTEXTS;
    message.h.resource.component[1] = smsf_ue->supi;
    message.h.resource.component[2] =
        (char *)OGS_SBI_RESOURCE_NAME_N1_N2_MESSAGES;
    message.N1N2MessageTransferReqData = &N1N2MessageTransferReqData;

    memset(&N1N2MessageTransferReqData, 0, sizeof(N1N2MessageTransferReqData));
    N1N2MessageTransferReqData.is_pdu_session_id = true;
    N1N2MessageTransferReqData.pdu_session_id = sess->psi;

    if (param->n1smbuf) {
        N1N2MessageTransferReqData.n1_message_container = &n1MessageContainer;

        memset(&n1MessageContainer, 0, sizeof(n1MessageContainer));
        n1MessageContainer.n1_message_class = OpenAPI_n1_message_class_SM;
        n1MessageContainer.n1_message_content = &n1MessageContent;

        memset(&n1MessageContent, 0, sizeof(n1MessageContent));
        n1MessageContent.content_id = (char *)OGS_SBI_CONTENT_5GNAS_SM_ID;

        message.part[message.num_of_part].pkbuf = param->n1smbuf;
        message.part[message.num_of_part].content_id =
            (char *)OGS_SBI_CONTENT_5GNAS_SM_ID;
        message.part[message.num_of_part].content_type =
            (char *)OGS_SBI_CONTENT_5GNAS_TYPE;
        message.num_of_part++;
    }

    if (param->n2smbuf) {
        N1N2MessageTransferReqData.n2_info_container = &n2InfoContainer;

        memset(&n2InfoContainer, 0, sizeof(n2InfoContainer));
        n2InfoContainer.n2_information_class = OpenAPI_n2_information_class_SM;
        n2InfoContainer.sm_info = &smInfo;

        memset(&smInfo, 0, sizeof(smInfo));
        smInfo.pdu_session_id = sess->psi;
        smInfo.n2_info_content = &n2InfoContent;

        memset(&n2InfoContent, 0, sizeof(n2InfoContent));
        switch (param->state) {
        case SMSF_UE_REQUESTED_PDU_SESSION_ESTABLISHMENT:
        case SMSF_NETWORK_TRIGGERED_SERVICE_REQUEST:
            n2InfoContent.ngap_ie_type = OpenAPI_ngap_ie_type_PDU_RES_SETUP_REQ;
            break;
        case SMSF_NETWORK_REQUESTED_PDU_SESSION_MODIFICATION:
        case SMSF_NETWORK_REQUESTED_QOS_FLOW_MODIFICATION:
            n2InfoContent.ngap_ie_type = OpenAPI_ngap_ie_type_PDU_RES_MOD_REQ;
            break;
        case SMSF_NETWORK_REQUESTED_PDU_SESSION_RELEASE:
        case SMSF_ERROR_INDICATON_RECEIVED_FROM_5G_AN:
            n2InfoContent.ngap_ie_type = OpenAPI_ngap_ie_type_PDU_RES_REL_CMD;
            break;
        default:
            ogs_fatal("Unexpected state [%d]", param->state);
            ogs_assert_if_reached();
        }
        n2InfoContent.ngap_data = &ngapData;

        memset(&ngapData, 0, sizeof(ngapData));
        ngapData.content_id = (char *)OGS_SBI_CONTENT_NGAP_SM_ID;

        message.part[message.num_of_part].pkbuf = param->n2smbuf;
        message.part[message.num_of_part].content_id =
            (char *)OGS_SBI_CONTENT_NGAP_SM_ID;
        message.part[message.num_of_part].content_type =
            (char *)OGS_SBI_CONTENT_NGAP_TYPE;
        message.num_of_part++;
    }

    if (param->n1n2_failure_txf_notif_uri == true) {
        server = ogs_sbi_server_first();
        ogs_assert(server);

        memset(&header, 0, sizeof(header));
        header.service.name = (char *)OGS_SBI_SERVICE_NAME_NSMSF_CALLBACK;
        header.api.version = (char *)OGS_SBI_API_V1;
        header.resource.component[0] =
                (char *)OGS_SBI_RESOURCE_NAME_N1_N2_FAILURE_NOTIFY;
        N1N2MessageTransferReqData.n1n2_failure_txf_notif_uri =
            ogs_sbi_server_uri(server, &header);
        ogs_assert(N1N2MessageTransferReqData.n1n2_failure_txf_notif_uri);
    }

    if (param->skip_ind == true) {
        N1N2MessageTransferReqData.is_skip_ind = true;
        N1N2MessageTransferReqData.skip_ind = true;
    }

    request = ogs_sbi_build_request(&message);
    ogs_expect(request);

    for (i = 0; i < message.num_of_part; i++)
        if (message.part[i].pkbuf)
            ogs_pkbuf_free(message.part[i].pkbuf);

    if (N1N2MessageTransferReqData.n1n2_failure_txf_notif_uri)
        ogs_free(N1N2MessageTransferReqData.n1n2_failure_txf_notif_uri);

    return request;
}
