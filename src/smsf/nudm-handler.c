/*
 * Copyright (C) 2019 by Sukchan Lee <acetcom@gmail.com>
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

#include "nudm-handler.h"

#include "sbi-path.h"

int smsf_nudm_sdm_handle_provisioned_data(
    smsf_ue_t *smsf_ue, ogs_sbi_stream_t *stream, ogs_sbi_message_t *recvmsg)
{
    OpenAPI_sms_management_subscription_data_t *SmsManagementSubscriptionData = NULL;

    SmsManagementSubscriptionData = recvmsg->SmsManagementSubscriptionData;

    if (!SmsManagementSubscriptionData) {
        ogs_error("[%s] No SmsManagementSubscriptionData", smsf_ue->supi);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                recvmsg, "No SmsManagementSubscriptionData", smsf_ue->supi));
        return false;
    }

    if (!SmsManagementSubscriptionData->mo_sms_subscribed) {
        ogs_error("[%s] No moSmsSubscribed", smsf_ue->supi);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                recvmsg, "No moSmsSubscribed", smsf_ue->supi));
        return false;
    }

    if (!SmsManagementSubscriptionData->mt_sms_subscribed) {
        ogs_error("[%s] No mtSmsSubscribed", smsf_ue->supi);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                recvmsg, "No mtSmsSubscribed", smsf_ue->supi));
        return false;
    }

    smsf_ue->sms_management_subscription_data =
        OpenAPI_sms_management_subscription_data_copy(
            smsf_ue->sms_management_subscription_data,
            SmsManagementSubscriptionData);

    return OGS_OK;
}

bool smsf_nudm_uecm_handle_smsf_registration(
    smsf_ue_t *smsf_ue, ogs_sbi_stream_t *stream, ogs_sbi_message_t *recvmsg)
{
    OpenAPI_smsf_registration_t *SmsfRegistration = NULL;

    SmsfRegistration = recvmsg->SmsfRegistration;

    if (!SmsfRegistration) {
        ogs_error("[%s] No SmsfRegistration", smsf_ue->supi);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                recvmsg, "No SmsfRegistration", smsf_ue->supi));
        return false;
    }

    if (!SmsfRegistration->smsf_instance_id) {
        ogs_error("[%s] No smsfInstanceId", smsf_ue->supi);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                recvmsg, "No smsfInstanceId", smsf_ue->supi));
        return false;
    }

    if (!SmsfRegistration->plmn_id) {
        ogs_error("[%s] No PlmnId", smsf_ue->supi);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                recvmsg, "No PlmnId", smsf_ue->supi));
        return false;
    }

    if (!SmsfRegistration->plmn_id->mnc) {
        ogs_error("[%s] No PlmnId.Mnc", smsf_ue->supi);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                recvmsg, "No PlmnId.Mnc", smsf_ue->supi));
        return false;
    }

    if (!SmsfRegistration->plmn_id->mcc) {
        ogs_error("[%s] No PlmnId.Mcc", smsf_ue->supi);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                recvmsg, "No PlmnId.Mcc", smsf_ue->supi));
        return false;
    }

    smsf_ue->smsf_registration = OpenAPI_smsf_registration_copy(
            smsf_ue->smsf_registration,
            SmsfRegistration);

    return true;
}
