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

int smsf_nudm_sdm_handle_provisioned(
        smsf_ue_t *smsf_ue, int state, ogs_sbi_message_t *recvmsg)
{
/*
Can't just do this.  you need to trigger an event back on nsmsf.
    ogs_sbi_message_t sendmsg;
    ogs_sbi_response_t *response = NULL;

    memset(&sendmsg, 0, sizeof(sendmsg));

    response = ogs_sbi_build_response(
            &sendmsg, OGS_SBI_HTTP_STATUS_NO_CONTENT);
    ogs_assert(response);
    ogs_assert(true == ogs_sbi_server_send_response(stream, response));

*/
/*
    int i, r;
    ran_ue_t *ran_ue = NULL;

    ogs_assert(smsf_ue);
    ogs_assert(recvmsg);

    ran_ue = ran_ue_cycle(smsf_ue->ran_ue);
    if (!ran_ue) {

        ogs_error("NG context has already been removed");
        r = nas_5gs_send_gmm_reject(
                smsf_ue, OGS_5GMM_CAUSE_5GS_SERVICES_NOT_ALLOWED);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return OGS_ERROR;
    }

    SWITCH(recvmsg->h.resource.component[1])
    CASE(OGS_SBI_RESOURCE_NAME_AM_DATA)

        if (recvmsg->AccessAndMobilitySubscriptionData) {
            OpenAPI_list_t *gpsiList =
                recvmsg->AccessAndMobilitySubscriptionData->gpsis;
            OpenAPI_ambr_rm_t *SubscribedUeAmbr =
                recvmsg->AccessAndMobilitySubscriptionData->subscribed_ue_ambr;
            OpenAPI_nssai_t *NSSAI =
                recvmsg->AccessAndMobilitySubscriptionData->nssai;
            OpenAPI_list_t *RatRestrictions =
                recvmsg->AccessAndMobilitySubscriptionData->rat_restrictions;

            OpenAPI_lnode_t *node = NULL;


            for (i = 0; i < smsf_ue->num_of_msisdn; i++) {
                ogs_assert(smsf_ue->msisdn[i]);
                ogs_free(smsf_ue->msisdn[i]);
            }
            smsf_ue->num_of_msisdn = 0;

            if (gpsiList) {
                OpenAPI_list_for_each(gpsiList, node) {
                    if (node->data) {
                        char *gpsi = NULL;

                        gpsi = ogs_id_get_type(node->data);
                        if (gpsi) {
                            if (strncmp(gpsi, OGS_ID_GPSI_TYPE_MSISDN,
                                    strlen(OGS_ID_GPSI_TYPE_MSISDN)) == 0) {
                                smsf_ue->msisdn[smsf_ue->num_of_msisdn] =
                                    ogs_id_get_value(node->data);
                                ogs_assert(smsf_ue->
                                        msisdn[smsf_ue->num_of_msisdn]);

                                smsf_ue->num_of_msisdn++;
                            }
                            ogs_free(gpsi);
                        }
                    }
                }
            }


            smsf_ue->ue_ambr.uplink = 0;
            smsf_ue->ue_ambr.downlink = 0;

            if (SubscribedUeAmbr) {
                smsf_ue->ue_ambr.uplink =
                    ogs_sbi_bitrate_from_string(SubscribedUeAmbr->uplink);
                smsf_ue->ue_ambr.downlink =
                    ogs_sbi_bitrate_from_string(SubscribedUeAmbr->downlink);
            }

            if (NSSAI) {
                OpenAPI_list_t *DefaultSingleNssaiList = NULL;
                OpenAPI_list_t *SingleNssaiList = NULL;


                amf_clear_subscribed_info(smsf_ue);

                DefaultSingleNssaiList = NSSAI->default_single_nssais;
                if (DefaultSingleNssaiList) {
                    OpenAPI_list_for_each(DefaultSingleNssaiList, node) {
                        OpenAPI_snssai_t *Snssai = node->data;

                        ogs_slice_data_t *slice =
                            &smsf_ue->slice[smsf_ue->num_of_slice];
                        if (Snssai) {
                            slice->s_nssai.sst = Snssai->sst;
                            slice->s_nssai.sd =
                                ogs_s_nssai_sd_from_string(Snssai->sd);
                        }


                        slice->default_indicator = true;

                        smsf_ue->num_of_slice++;
                    }

                    SingleNssaiList = NSSAI->single_nssais;
                    if (SingleNssaiList) {
                        OpenAPI_list_for_each(SingleNssaiList, node) {
                            OpenAPI_snssai_t *Snssai = node->data;

                            ogs_slice_data_t *slice =
                                &smsf_ue->slice[smsf_ue->num_of_slice];
                            if (Snssai) {
                                slice->s_nssai.sst = Snssai->sst;
                                slice->s_nssai.sd =
                                    ogs_s_nssai_sd_from_string(Snssai->sd);
                            }


                            slice->default_indicator = false;

                            smsf_ue->num_of_slice++;
                        }
                    }
                }
            }

            OpenAPI_list_clear(smsf_ue->rat_restrictions);
            if (RatRestrictions) {
                OpenAPI_list_for_each(RatRestrictions, node) {
                    OpenAPI_list_add(smsf_ue->rat_restrictions, node->data);
                }
            }
        }

        if (amf_update_allowed_nssai(smsf_ue) == false) {
            ogs_error("No Allowed-NSSAI");
            r = nas_5gs_send_gmm_reject(
                    smsf_ue, OGS_5GMM_CAUSE_NO_NETWORK_SLICES_AVAILABLE);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return OGS_ERROR;
        }

        if (smsf_ue_is_rat_restricted(smsf_ue)) {
            ogs_error("Registration rejected due to RAT restrictions");
            r = nas_5gs_send_gmm_reject(
                    smsf_ue, OGS_5GMM_CAUSE_5GS_SERVICES_NOT_ALLOWED);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return OGS_ERROR;
        }

        r = smsf_ue_sbi_discover_and_send(
                OGS_SBI_SERVICE_TYPE_NUDM_SDM, NULL,
                smsf_nudm_sdm_build_get,
                smsf_ue, state, (char *)OGS_SBI_RESOURCE_NAME_SMF_SELECT_DATA);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        break;

    CASE(OGS_SBI_RESOURCE_NAME_SMF_SELECT_DATA)
        if (recvmsg->SmfSelectionSubscriptionData) {
            OpenAPI_list_t *SubscribedSnssaiInfoList = NULL;
            OpenAPI_map_t *SubscribedSnssaiInfoMap = NULL;
            OpenAPI_snssai_info_t *SubscribedSnssaiInfo = NULL;

            OpenAPI_list_t *DnnInfoList = NULL;
            OpenAPI_dnn_info_t *DnnInfo = NULL;

            OpenAPI_lnode_t *node = NULL, *node2 = NULL;

            SubscribedSnssaiInfoList = recvmsg->
                SmfSelectionSubscriptionData->subscribed_snssai_infos;
            if (SubscribedSnssaiInfoList) {

                OpenAPI_list_for_each(SubscribedSnssaiInfoList, node) {
                    SubscribedSnssaiInfoMap = node->data;
                    if (SubscribedSnssaiInfoMap &&
                            SubscribedSnssaiInfoMap->key) {
                        ogs_slice_data_t *slice = NULL;
                        ogs_s_nssai_t s_nssai;

                        bool rc = ogs_sbi_s_nssai_from_string(
                                &s_nssai, SubscribedSnssaiInfoMap->key);
                        if (rc == false) {
                            ogs_error("Invalid S-NSSAI format [%s]",
                                SubscribedSnssaiInfoMap->key);
                            continue;
                        }

                        slice = ogs_slice_find_by_s_nssai(
                                    smsf_ue->slice, smsf_ue->num_of_slice,
                                    &s_nssai);
                        if (!slice) {
                            ogs_error("Cannt find S-NSSAI[SST:%d SD:0x%x]",
                                    s_nssai.sst, s_nssai.sd.v);
                            continue;
                        }

                        SubscribedSnssaiInfo = SubscribedSnssaiInfoMap->value;
                        if (SubscribedSnssaiInfo) {
                            DnnInfoList = SubscribedSnssaiInfo->dnn_infos;
                            if (DnnInfoList) {
                                OpenAPI_list_for_each(DnnInfoList, node2) {
                                    DnnInfo = node2->data;
                                    if (DnnInfo) {
                                        ogs_session_t *session =
                                            &slice->session
                                                [slice->num_of_session];
                                        session->name =
                                            ogs_strdup(DnnInfo->dnn);
                                        ogs_assert(session->name);
                                        if (DnnInfo->is_default_dnn_indicator ==
                                                true) {
                                            session->default_dnn_indicator =
                                                DnnInfo->default_dnn_indicator;
                                        }
                                        slice->num_of_session++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        r = smsf_ue_sbi_discover_and_send(
                OGS_SBI_SERVICE_TYPE_NUDM_SDM, NULL,
                smsf_nudm_sdm_build_get,
                smsf_ue, state,
                (char *)OGS_SBI_RESOURCE_NAME_UE_CONTEXT_IN_SMF_DATA);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        break;

    CASE(OGS_SBI_RESOURCE_NAME_UE_CONTEXT_IN_SMF_DATA)
        if (smsf_ue->data_change_subscription_id) {
            r = smsf_ue_sbi_discover_and_send(
                    OGS_SBI_SERVICE_TYPE_NPCF_AM_POLICY_CONTROL, NULL,
                    amf_npcf_am_policy_control_build_create,
                    smsf_ue, state, NULL);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
        }
        else {
            ogs_info("EntryA - Potential Code Change");
            r = smsf_ue_sbi_discover_and_send(
                    OGS_SBI_SERVICE_TYPE_NUDM_SDM, NULL,
                    smsf_nudm_sdm_build_subscription,
                    smsf_ue, state, (char *)OGS_SBI_RESOURCE_NAME_AM_DATA);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
        }
        break;

    CASE(OGS_SBI_RESOURCE_NAME_SDM_SUBSCRIPTIONS)

        int rv;
        ogs_sbi_message_t message;
        ogs_sbi_header_t header;

        if (!recvmsg->http.location) {
            ogs_error("[%s] No http.location", smsf_ue->supi);
            r = nas_5gs_send_gmm_reject_from_sbi(
                    smsf_ue, OGS_SBI_HTTP_STATUS_INTERNAL_SERVER_ERROR);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return OGS_ERROR;
        }

        memset(&header, 0, sizeof(header));
        header.uri = recvmsg->http.location;

        rv = ogs_sbi_parse_header(&message, &header);
        if (rv != OGS_OK) {
            ogs_error("[%s] Cannot parse http.location [%s]",
                smsf_ue->supi, recvmsg->http.location);
            r = nas_5gs_send_gmm_reject_from_sbi(
                    smsf_ue, OGS_SBI_HTTP_STATUS_INTERNAL_SERVER_ERROR);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return OGS_ERROR;
        }

        if (!message.h.resource.component[2]) {
            ogs_error("[%s] No Subscription ID [%s]",
                smsf_ue->supi, recvmsg->http.location);

            ogs_sbi_header_free(&header);
            r = nas_5gs_send_gmm_reject_from_sbi(
                    smsf_ue, OGS_SBI_HTTP_STATUS_INTERNAL_SERVER_ERROR);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return OGS_ERROR;
        }

        if (smsf_ue->data_change_subscription_id)
            ogs_free(smsf_ue->data_change_subscription_id);
        smsf_ue->data_change_subscription_id =
            ogs_strdup(message.h.resource.component[2]);

        ogs_sbi_header_free(&header);

        ogs_info("EntryB - Finding SMSF not PCF");
        r = smsf_ue_sbi_discover_and_send(
                OGS_SBI_SERVICE_TYPE_NSMSF_SMS, NULL,
                amf_nsmsf_sm_service_build_activate, smsf_ue, 0, NULL);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        break;

    DEFAULT
    END
*/

    return OGS_OK;
}

bool smsf_nudm_uecm_handle_smsf_registration(
    smsf_ue_t *smsf_ue, ogs_sbi_stream_t *stream, ogs_sbi_message_t *recvmsg)
{
    ogs_info("got the data");
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
