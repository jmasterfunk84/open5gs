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
#include "nsmsf-handler.h"
#include "nudm-handler.h"

void smsf_ue_state_initial(ogs_fsm_t *s, smsf_event_t *e)
{
    ogs_assert(s);

    OGS_FSM_TRAN(s, &smsf_ue_state_operational);
}

void smsf_ue_state_final(ogs_fsm_t *s, smsf_event_t *e)
{
}

void smsf_ue_state_operational(ogs_fsm_t *s, smsf_event_t *e)
{
    smsf_ue_t *smsf_ue = NULL;

    ogs_sbi_stream_t *stream = NULL;
    ogs_sbi_message_t *message = NULL;
    int r;

    ogs_assert(s);
    ogs_assert(e);

    smsf_sm_debug(e);

    smsf_ue = e->smsf_ue;
    ogs_assert(smsf_ue);

    switch (e->h.id) {
    case OGS_FSM_ENTRY_SIG:
        break;

    case OGS_FSM_EXIT_SIG:
        break;

    case OGS_EVENT_SBI_SERVER:
        message = e->h.sbi.message;
        ogs_assert(message);
        stream = e->h.sbi.data;
        ogs_assert(stream);

        SWITCH(message->h.service.name)
        CASE(OGS_SBI_SERVICE_NAME_NSMSF_SMS)
            SWITCH(message->h.method)
            CASE(OGS_SBI_HTTP_METHOD_POST)
                SWITCH(message->h.resource.component[1])
                CASE(OGS_SBI_RESOURCE_NAME_SEND_SMS)
                    smsf_nsmsf_sm_service_handle_uplink_sms(smsf_ue, stream, message);
                    break;
                DEFAULT
                    ogs_error("[%s] Invalid resource name [%s]",
                            smsf_ue->supi, message->h.resource.component[1]);
                    ogs_assert(true ==
                        ogs_sbi_server_send_error(stream,
                            OGS_SBI_HTTP_STATUS_BAD_REQUEST, message,
                            "Invalid resource name", message->h.method));
                END
                break;

            CASE(OGS_SBI_HTTP_METHOD_PUT)
                SWITCH(message->h.resource.component[1])
                CASE(OGS_SBI_RESOURCE_NAME_AUTH_EVENTS)
                    smsf_nsmsf_sm_service_handle_activate(
                            smsf_ue, stream, message);
                    break;
                DEFAULT
                    ogs_error("[%s] Invalid resource name [%s]",
                            smsf_ue->supi, message->h.resource.component[1]);
                    ogs_assert(true ==
                        ogs_sbi_server_send_error(stream,
                            OGS_SBI_HTTP_STATUS_BAD_REQUEST, message,
                            "Invalid resource name", message->h.method));
                END
                break;

            DEFAULT
                ogs_error("[%s] Invalid HTTP method [%s]",
                        smsf_ue->supi, message->h.method);
                ogs_assert(true ==
                    ogs_sbi_server_send_error(stream,
                        OGS_SBI_HTTP_STATUS_FORBIDDEN, message,
                        "Invalid HTTP method", message->h.method));
            END
            break;

        DEFAULT
            ogs_error("Invalid API name [%s]", message->h.service.name);
            ogs_assert(true ==
                ogs_sbi_server_send_error(stream,
                    OGS_SBI_HTTP_STATUS_BAD_REQUEST, message,
                    "Invalid API name", message->h.service.name));
        END
        break;

    case OGS_EVENT_SBI_CLIENT:
        message = e->h.sbi.message;
        ogs_assert(message);

        smsf_ue = e->smsf_ue;
        ogs_assert(smsf_ue);
        stream = e->h.sbi.data;
        ogs_assert(stream);

        SWITCH(message->h.service.name)
        CASE(OGS_SBI_SERVICE_NAME_NUDM_UECM)
            SWITCH(message->h.resource.component[1])
            CASE(OGS_SBI_RESOURCE_NAME_REGISTRATIONS)
                SWITCH(message->h.resource.component[2])
                CASE(OGS_SBI_RESOURCE_NAME_SMSF_3GPP_ACCESS)
                    smsf_nsmsf_sm_service_handle_activate(
                            smsf_ue, stream, message);
                    break;

                DEFAULT
                    SWITCH(message->h.resource.component[3])
                    CASE(OGS_SBI_RESOURCE_NAME_PROVISIONED_DATA)
                        smsf_nsmsf_sm_service_handle_activate(
                                smsf_ue, stream, message);
                        break;

                    DEFAULT
                        ogs_error("Invalid resource name [%s]",
                                message->h.resource.component[2]);
                        ogs_assert_if_reached();
                    END
                END
                break;

            DEFAULT
                ogs_error("Invalid resource name [%s]",
                        message->h.resource.component[1]);
                ogs_assert_if_reached();
            END
            break;

        DEFAULT
            ogs_error("Invalid API name [%s]", message->h.service.name);
            ogs_assert(true ==
                ogs_sbi_server_send_error(stream,
                    OGS_SBI_HTTP_STATUS_BAD_REQUEST, message,
                    "Invalid API name", message->h.resource.component[0]));
        END
        break;

    default:
        ogs_error("[%s] Unknown event %s", smsf_ue->supi, smsf_event_get_name(e));
        break;
    }
}

void smsf_ue_state_exception(ogs_fsm_t *s, smsf_event_t *e)
{
    smsf_ue_t *smsf_ue = NULL;
    ogs_assert(s);
    ogs_assert(e);

    smsf_sm_debug(e);

    smsf_ue = e->smsf_ue;
    ogs_assert(smsf_ue);

    switch (e->h.id) {
    case OGS_FSM_ENTRY_SIG:
        break;

    case OGS_FSM_EXIT_SIG:
        break;

    default:
        ogs_error("[%s] Unknown event %s", smsf_ue->supi, smsf_event_get_name(e));
        break;
    }
}
