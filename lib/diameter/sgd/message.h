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

#if !defined(OGS_DIAMETER_INSIDE) && !defined(OGS_DIAMETER_COMPILATION)
#error "This header cannot be included directly."
#endif

#ifndef OGS_DIAM_SGD_MESSAGE_H
#define OGS_DIAM_SGD_MESSAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#define OGS_DIAM_SGD_APPLICATION_ID 16777313

extern struct dict_object *ogs_diam_sgd_application = NULL;

extern struct dict_object *ogs_diam_sgd_cmd_ofr = NULL;
extern struct dict_object *ogs_diam_sgd_cmd_ofa = NULL;
extern struct dict_object *ogs_diam_sgd_cmd_tfr = NULL;
extern struct dict_object *ogs_diam_sgd_cmd_tfa = NULL;
extern struct dict_object *ogs_diam_sgd_cmd_alr = NULL;
extern struct dict_object *ogs_diam_sgd_cmd_ala = NULL;

extern struct dict_object *ogs_diam_sgd_sc_address = NULL;
extern struct dict_object *ogs_diam_sgd_sm_rp_ui = NULL;
extern struct dict_object *ogs_diam_sgd_tfr_flags = NULL;
extern struct dict_object *ogs_diam_sgd_sm_delivery_failure_cause = NULL;
extern struct dict_object *ogs_diam_sgd_sm_diagnostic_info = NULL;
extern struct dict_object *ogs_diam_sgd_sm_delivery_timer = NULL;
extern struct dict_object *ogs_diam_sgd_sm_delivery_start_time = NULL;
extern struct dict_object *ogs_diam_sgd_mi_correlation_id = NULL;
extern struct dict_object *ogs_diam_sgd_hss_id = NULL;
extern struct dict_object *ogs_diam_sgd_originating_sip_uri = NULL;
extern struct dict_object *ogs_diam_sgd_destination_sip_uri = NULL;
extern struct dict_object *ogs_diam_sgd_ofr_flags = NULL;
extern struct dict_object *ogs_diam_sgd_maximum_retransmission_time = NULL;
extern struct dict_object *ogs_diam_sgd_requested_retransmission_time = NULL;
extern struct dict_object *ogs_diam_sgd_sms_gmsc_address = NULL;

typedef struct ogs_diam_sgd_message_s {
#define OGS_DIAM_SGD_CMD_SESSION_TERMINATION                        1
    uint16_t            cmd_code;
    uint32_t            result_code;
    uint32_t            *err;
    uint32_t            *exp_err;
} ogs_diam_sgd_message_t;

int ogs_diam_sgd_init(void);

#ifdef __cplusplus
}
#endif

#endif /* OGS_DIAM_OGS_DIAM_SGD_MESSAGE_H */
