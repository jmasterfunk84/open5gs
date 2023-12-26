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

#include "nsmsf-build.h"

ogs_sbi_request_t *amf_nsmsf_sm_service_build_activate(
        amf_ue_t *amf_ue, void *data)
{
    ogs_info("temp: activate");
}

ogs_sbi_request_t *amf_nsmsf_sm_service_build_deactivate(
        amf_ue_t *amf_ue, void *data)
{
    ogs_info("temp: deactivate");
}

ogs_sbi_request_t *amf_nsmsf_sm_service_build_uplink_sms(
        amf_ue_t *amf_ue, void *data)
{
    ogs_info("temp: uplinksms");
/*
    ogs_sbi_message_t message;
    ogs_sbi_header_t header;
    ogs_sbi_request_t *request = NULL;
    ogs_sbi_server_t *server = NULL;

    OpenAPI_sms_record_data_t SmsRecordData;

    int i;
    OpenAPI_lnode_t *node = NULL;

    OpenAPI_user_location_t ueLocation;
    OpenAPI_sms_payload_t smsPayload;

    ogs_assert(amf_ue);
    ogs_assert(amf_ue->supi);
    ogs_assert(ran_ue_cycle(amf_ue->ran_ue));

    memset(&message, 0, sizeof(message));
    message.h.method = (char *)OGS_SBI_HTTP_METHOD_POST;
    message.h.service.name =
        (char *)OGS_SBI_SERVICE_NAME_NSMSF_SMS;
    message.h.api.version = (char *)OGS_SBI_API_V2;
    message.h.resource.component[0] = (char *)OGS_SBI_RESOURCE_NAME_UE_CONTEXTS;
    message.h.resource.component[1] = smf_ue->supi;
    message.h.resource.component[2] = (char *)OGS_SBI_RESOURCE_NAME_SEND_SMS;


    memset(&SmsRecordData, 0, sizeof(SmsRecordData));
    memset(&SmsPayload, 0, sizeof(SmsPayload));
    memset(&ueLocation, 0, sizeof(ueLocation));
    memset(&UeAmbr, 0, sizeof(UeAmbr));

    server = ogs_sbi_server_first();
    if (!server) {
        ogs_error("No server");
        goto end;
    }

    memset(&header, 0, sizeof(header));
    header.service.name = (char *)OGS_SBI_SERVICE_NAME_NAMF_CALLBACK;
    header.api.version = (char *)OGS_SBI_API_V1;
    header.resource.component[0] = amf_ue->supi;
    header.resource.component[1] =
            (char *)OGS_SBI_RESOURCE_NAME_UE_CONTEXTS;
    SmsRecordData.notification_uri =
                        ogs_sbi_server_uri(server, &header);
    if (!SmsRecordData.notification_uri) {
        ogs_error("No notifiation_uri");
        goto end;
    }

    SmsRecordData.supi = amf_ue->supi;

    if (amf_ue->num_of_msisdn) {
        if (amf_ue->msisdn[0]) {
            SmsRecordData.gpsi = ogs_msprintf("%s-%s",
                        OGS_ID_GPSI_TYPE_MSISDN, amf_ue->msisdn[0]);
            if (!SmsRecordData.gpsi) {
                ogs_error("No gpsi");
                goto end;
            }
        }
    }

    SmsRecordData.access_type = amf_ue->nas.access_type; 
    SmsRecordData.pei = amf_ue->pei;

    ueLocation.nr_location = ogs_sbi_build_nr_location(
            &amf_ue->nr_tai, &amf_ue->nr_cgi);
    if (!ueLocation.nr_location) {
        ogs_error("No ueLocation.nr_location");
        goto end;
    }
    ueLocation.nr_location->ue_location_timestamp =
        ogs_sbi_gmtime_string(amf_ue->ue_location_timestamp);
    if (!ueLocation.nr_location->ue_location_timestamp) {
        ogs_error("No ueLocation.nr_location->ue_location_timestamp");
        goto end;
    }
    SmsRecordData.user_loc = &ueLocation;

    SmsRecordData.time_zone =
        ogs_sbi_timezone_string(ogs_timezone());
    if (!PolicyAssociationRequest.time_zone) {
        ogs_error("No time_zone");
        goto end;
    }

    SmsRecordData.smsPayload = data;
    if (!SmsRecordData.smsPayload) {
        ogs_error("No SMSPayload");
        goto end;
    }

    message.SmsRecordData = &SmsRecordData;

    request = ogs_sbi_build_request(&message);
    ogs_expect(request);

end:
    if (SmsRecordData.notification_uri)
        ogs_free(SmsRecordData.notification_uri);
    if (SmsRecordData.smsPayload)
        ogs_free(SmsRecordData.smsPayload;

    if (SmsRecordData.gpsi)
        ogs_free(SmsRecordData.gpsi);

    if (ueLocation.nr_location) {
        if (ueLocation.nr_location->ue_location_timestamp)
            ogs_free(ueLocation.nr_location->ue_location_timestamp);
        ogs_sbi_free_nr_location(ueLocation.nr_location);
    }
    if (PolicyAssociationRequest.time_zone)
        ogs_free(PolicyAssociationRequest.time_zone);

    return request;
*/
}
