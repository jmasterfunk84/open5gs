#define TRACE_MODULE _s1enbmsg

#include "core_debug.h"
#include "core_pkbuf.h"
#include "context.h"
#include "s1ap_build.h"
#include "s1ap_conv.h"

status_t s1ap_build_setup_req(pkbuf_t **pkbuf, c_uint32_t enb_id)
{
    int erval = -1;

    s1ap_message message;
    S1ap_S1SetupRequestIEs_t *ies;
    S1ap_PLMNidentity_t *plmnIdentity;
    S1ap_SupportedTAs_Item_t *supportedTA;

    memset(&message, 0, sizeof(s1ap_message));

    ies = &message.msg.s1ap_S1SetupRequestIEs;

    s1ap_uint32_to_ENB_ID(S1ap_ENB_ID_PR_macroENB_ID, enb_id, 
            &ies->global_ENB_ID.eNB_ID);
    s1ap_plmn_id_to_TBCD_STRING(
        &mme_self()->plmn_id, &ies->global_ENB_ID.pLMNidentity);

    supportedTA = (S1ap_SupportedTAs_Item_t *)
        core_calloc(1, sizeof(S1ap_SupportedTAs_Item_t));
    s1ap_uint16_to_OCTET_STRING(
            mme_self()->tracking_area_code, &supportedTA->tAC);
    plmnIdentity = (S1ap_PLMNidentity_t *)
        core_calloc(1, sizeof(S1ap_PLMNidentity_t));
    s1ap_plmn_id_to_TBCD_STRING(
        &mme_self()->plmn_id, plmnIdentity);
    ASN_SEQUENCE_ADD(&supportedTA->broadcastPLMNs, plmnIdentity);

    ASN_SEQUENCE_ADD(&ies->supportedTAs, supportedTA);

    ies->defaultPagingDRX = mme_self()->default_paging_drx;

    message.direction = S1AP_PDU_PR_initiatingMessage;
    message.procedureCode = S1ap_ProcedureCode_id_S1Setup;

    erval = s1ap_encode_pdu(pkbuf, &message);
    s1ap_free_pdu(&message);

    if (erval < 0)
    {
        d_error("s1ap_encode_error : (%d)", erval);
        return CORE_ERROR;
    }

    return CORE_OK;
}


