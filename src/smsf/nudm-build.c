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

#include "nudm-build.h"

ogs_sbi_request_t *smsf_nudm_uecm_build_registration(
        smsf_ue_t *smsf_ue, void *data)
{
    ogs_sbi_message_t message;
    ogs_sbi_header_t header;
    ogs_sbi_request_t *request = NULL;
    ogs_sbi_server_t *server = NULL;

    OpenAPI_smsf_registration_t Smsf3GppRegistration;

    ogs_assert(smsf_ue);
    ogs_assert(smsf_ue->supi);

    memset(&message, 0, sizeof(message));
    message.h.method = (char *)OGS_SBI_HTTP_METHOD_PUT;
    message.h.service.name = (char *)OGS_SBI_SERVICE_NAME_NUDM_UECM;
    message.h.api.version = (char *)OGS_SBI_API_V1;
    message.h.resource.component[0] = smsf_ue->supi;
    message.h.resource.component[1] =
            (char *)OGS_SBI_RESOURCE_NAME_REGISTRATIONS;
    message.h.resource.component[2] =
            (char *)OGS_SBI_RESOURCE_NAME_SMSF_3GPP_ACCESS;

    memset(&Smsf3GppRegistration, 0, sizeof(Smsf3GppRegistration));

    Smsf3GppRegistration.smsf_instance_id =
        NF_INSTANCE_ID(ogs_sbi_self()->nf_instance);
    if (!Smsf3GppRegistration.smsf_instance_id) {
        ogs_error("No smsf_instance_id");
        goto end;
    }

    server = ogs_sbi_server_first();
    if (!server) {
        ogs_error("No server");
        goto end;
    }

    memset(&header, 0, sizeof(header));
    header.service.name = (char *)OGS_SBI_SERVICE_NAME_NAMF_CALLBACK;
    header.api.version = (char *)OGS_SBI_API_V1;
    header.resource.component[0] = smsf_ue->supi;
    header.resource.component[1] = (char *)OGS_SBI_RESOURCE_NAME_DEREG_NOTIFY;

    message.Smsf3GppRegistration = &Smsf3GppRegistration;

    message.http.custom.callback =
        (char *)OGS_SBI_CALLBACK_NUDM_UECM_DEREGISTRATION_NOTIFICATION;

    request = ogs_sbi_build_request(&message);
    ogs_expect(request);

end:

    if (Smsf3GppRegistration.guami)
        ogs_sbi_free_guami(Smsf3GppRegistration.guami);
    if (Smsf3GppRegistration.dereg_callback_uri)
        ogs_free(Smsf3GppRegistration.dereg_callback_uri);

    return request;
}

ogs_sbi_request_t *smsf_nudm_uecm_build_registration_delete(
        smsf_ue_t *smsf_ue, void *data)
{
    ogs_sbi_message_t message;
    ogs_sbi_request_t *request = NULL;

    OpenAPI_smsf_registration_t Smsf3GppRegistration;

    ogs_assert(smsf_ue);
    ogs_assert(smsf_ue->supi);

    memset(&message, 0, sizeof(message));
    message.h.method = (char *)OGS_SBI_HTTP_METHOD_DELETE;
    message.h.service.name = (char *)OGS_SBI_SERVICE_NAME_NUDM_UECM;
    message.h.api.version = (char *)OGS_SBI_API_V1;
    message.h.resource.component[0] = smsf_ue->supi;
    message.h.resource.component[1] =
            (char *)OGS_SBI_RESOURCE_NAME_REGISTRATIONS;
    message.h.resource.component[2] =
            (char *)OGS_SBI_RESOURCE_NAME_AMF_3GPP_ACCESS;

    memset(&Smsf3GppRegistration, 0,
            sizeof(Smsf3GppRegistration));

    Smsf3GppRegistration.guami =
            ogs_sbi_build_guami(smsf_ue->guami);
    if (!Smsf3GppRegistration.guami) {
        ogs_error("No guami");
        goto end;
    }
    Smsf3GppRegistration.is_purge_flag = true;
    Smsf3GppRegistration.purge_flag = 1;

    message.Smsf3GppRegistration =
            &Smsf3GppRegistration;

    request = ogs_sbi_build_request(&message);
    ogs_expect(request);

end:

    if (Smsf3GppRegistration.guami)
        ogs_sbi_free_guami(Smsf3GppRegistration.guami);

    return request;
}

ogs_sbi_request_t *smsf_nudm_sdm_build_get(smsf_ue_t *smsf_ue, void *data)
{
    ogs_sbi_message_t message;
    ogs_sbi_request_t *request = NULL;

    ogs_assert(smsf_ue);
    ogs_assert(smsf_ue->supi);

    memset(&message, 0, sizeof(message));
    message.h.method = (char *)OGS_SBI_HTTP_METHOD_GET;
    message.h.service.name = (char *)OGS_SBI_SERVICE_NAME_NUDM_SDM;
    message.h.api.version = (char *)OGS_SBI_API_V2;
    message.h.resource.component[0] = smsf_ue->supi;
    message.h.resource.component[1] = data;

    request = ogs_sbi_build_request(&message);
    ogs_expect(request);

    return request;
}

ogs_sbi_request_t *smsf_nudm_sdm_build_subscription(smsf_ue_t *smsf_ue, void *data)
{
    ogs_sbi_message_t message;
    ogs_sbi_header_t header;
    ogs_sbi_request_t *request = NULL;
    ogs_sbi_server_t *server = NULL;

    OpenAPI_sdm_subscription_t SDMSubscription;

    char *monres = NULL;

    ogs_assert(smsf_ue);
    ogs_assert(smsf_ue->supi);
    ogs_assert(data);

    memset(&message, 0, sizeof(message));
    message.h.method = (char *)OGS_SBI_HTTP_METHOD_POST;
    message.h.service.name = (char *)OGS_SBI_SERVICE_NAME_NUDM_SDM;
    message.h.api.version = (char *)OGS_SBI_API_V2;
    message.h.resource.component[0] = smsf_ue->supi;
    message.h.resource.component[1] =
            (char *)OGS_SBI_RESOURCE_NAME_SDM_SUBSCRIPTIONS;

    memset(&SDMSubscription, 0, sizeof(SDMSubscription));

    SDMSubscription.nf_instance_id =
        NF_INSTANCE_ID(ogs_sbi_self()->nf_instance);

    server = ogs_sbi_server_first();
    if (!server) {
        ogs_error("No server");
        goto end;
    }

    memset(&header, 0, sizeof(header));
    header.service.name = (char *)OGS_SBI_SERVICE_NAME_NAMF_CALLBACK;
    header.api.version = (char *)OGS_SBI_API_V1;
    header.resource.component[0] = smsf_ue->supi;
    header.resource.component[1] =
            (char *)OGS_SBI_RESOURCE_NAME_SDMSUBSCRIPTION_NOTIFY;
    SDMSubscription.callback_reference = ogs_sbi_server_uri(server, &header);
    if (!SDMSubscription.callback_reference) {
        ogs_error("No callback_reference");
        goto end;
    }

    SDMSubscription.monitored_resource_uris = OpenAPI_list_create();

    monres = ogs_msprintf("%s/%s", smsf_ue->supi, (char *)data);
    if (!monres) {
        ogs_error("No monres");
        goto end;
    }

    OpenAPI_list_add(SDMSubscription.monitored_resource_uris, monres);
    SDMSubscription.is_implicit_unsubscribe = true;
    SDMSubscription.implicit_unsubscribe = 1;

    message.SDMSubscription = &SDMSubscription;

    message.http.custom.callback =
        (char *)OGS_SBI_CALLBACK_NUDM_SDM_NOTIFICATION;

    request = ogs_sbi_build_request(&message);
    ogs_expect(request);

end:

    if (monres)
        ogs_free(monres);
    OpenAPI_list_free(SDMSubscription.monitored_resource_uris);
    if (SDMSubscription.callback_reference)
        ogs_free(SDMSubscription.callback_reference);

    return request;
}

ogs_sbi_request_t *smsf_nudm_sdm_build_subscription_delete(
        smsf_ue_t *smsf_ue, void *data)
{
    ogs_sbi_message_t message;
    ogs_sbi_request_t *request = NULL;

    ogs_assert(smsf_ue);
    ogs_assert(smsf_ue->supi);
    ogs_assert(smsf_ue->data_change_subscription_id);

    memset(&message, 0, sizeof(message));
    message.h.method = (char *)OGS_SBI_HTTP_METHOD_DELETE;
    message.h.service.name = (char *)OGS_SBI_SERVICE_NAME_NUDM_SDM;
    message.h.api.version = (char *)OGS_SBI_API_V2;
    message.h.resource.component[0] = smsf_ue->supi;
    message.h.resource.component[1] =
            (char *)OGS_SBI_RESOURCE_NAME_SDM_SUBSCRIPTIONS;
    message.h.resource.component[2] = smsf_ue->data_change_subscription_id;

    request = ogs_sbi_build_request(&message);
    ogs_expect(request);

    return request;
}
