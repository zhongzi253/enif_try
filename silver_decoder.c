#include <stdbool.h>  
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <ucontext.h>
#include "erl_nif.h"  

enum ie_tags {
    ENUM_RecommendedCellItem = 214,
    ENUM_RecommendedEnbItem = 215
};

typedef enum _ie_criticality {
    ENUM_REJECT = 0,
    ENUM_IGNORE,
    ENUM_NOTIFY
}ie_criticality;

typedef struct _mvsgT_ecgi {
    ErlNifBinary plmnId;//:24;  //string
    unsigned char pad_len;
    ErlNifBinary eci;//:28;
} mvsgT_ecgi;

typedef struct _s1apT_recommendedCell {
    ie_criticality criti;   
    mvsgT_ecgi ecgi;
    int time_stay_in_cell;
} s1apT_recommendedCell;
typedef struct _ctsT_globalEnodebId {
    ErlNifBinary plmnId;//:24;  //string
    unsigned char pad_len;
    ErlNifBinary enodebId;//:20; //union  28
} ctsT_globalEnodebId;
typedef struct _s1apT_recommendedEnb {
    ie_criticality criti;
    unsigned char choice;
    ctsT_globalEnodebId globalEnodebId; //union mvsgT_tai
} s1apT_recommendedEnb;

#define NUM_CELL 16
#define NUM_ENB 16

typedef struct _s1apT_recommended_cells_enbs {
    s1apT_recommendedCell cellList[NUM_CELL];
    int numCell;
    s1apT_recommendedEnb  enbList[NUM_ENB];
    int numEnb;
} s1apT_recommended_cells_enbs;

    static void
n2h2(unsigned char *psrc, unsigned char *pdst)
{
    pdst[0] = psrc[1];
    pdst[1] = psrc[0];
}

    static ERL_NIF_TERM
make_criticality(ErlNifEnv* env, const ie_criticality cri)
{
    ERL_NIF_TERM res;
    switch(cri) {
        case ENUM_REJECT:
            res = enif_make_atom(env, "reject");
            break;
        case ENUM_IGNORE:
            res = enif_make_atom(env, "ignore");
            break;
        case ENUM_NOTIFY:
            res = enif_make_atom(env, "notify");
            break;
        default:
            memset(&res, 0, sizeof(ERL_NIF_TERM)); //make bad arg
            printf("silver_debug[%d]: should not be here.\n");
    }

    return res;
}
/*
   typedef struct struct _s1apT_ueContextReleaseComplete {
   int mme_s1apid,
   int enb_s1apid,              
   s1apT_critDiag crit_diag,   
   s1apT_userLocation userLocation,   
   s1apT_recommended_cells_enbs recommended_cells_enbs, 
   s1apT_cellIdCeLevel  cell_id_ce_level,       
   } s1apT_ueContextReleaseComplete;
   */


static ERL_NIF_TERM make_EUTRAN_CGI(ErlNifEnv* env, mvsgT_ecgi * ecgi)
{
    ERL_NIF_TERM eci = enif_make_tuple(env,
            2,
            enif_make_int(env, ecgi->pad_len),
            enif_make_binary(env, &ecgi->eci)
            );
    return enif_make_tuple(
            env,
            4,
            enif_make_atom(env, "EUTRAN-CGI"),
            enif_make_string_len(env, ecgi->plmnId.data, 3, ERL_NIF_LATIN1),
            eci,
            enif_make_atom(env, "asn1_NOVALUE")
            );

}

static ERL_NIF_TERM make_RecommendedCellItem(ErlNifEnv* env, s1apT_recommendedCell * cells_ptr)
{
    ERL_NIF_TERM atom1 = enif_make_atom(env, "RecommendedCellItem");
    ERL_NIF_TERM atom2 = enif_make_atom(env, "asn1_NOVALUE");
    ERL_NIF_TERM int1 = enif_make_int(env, cells_ptr->time_stay_in_cell);
    ERL_NIF_TERM res = enif_make_tuple(
            env,
            4,
            atom1,
            make_EUTRAN_CGI(env, &cells_ptr->ecgi),
            int1,//enif_make_int(env, cells_ptr->time_stay_in_cell),
            atom2//enif_make_atom(env, "asn1_NOVALUE")
            );

    return enif_make_tuple(
            env,
            4,
            enif_make_atom(env, "ProtocolIE-Field"),
            enif_make_int(env, ENUM_RecommendedCellItem),
            make_criticality(env, cells_ptr->criti),
            res
            );


}

static ERL_NIF_TERM make_RecommendedCellsForPaging(ErlNifEnv* env, s1apT_recommendedCell cells_ptr[][NUM_CELL], const int size)
{
    int i = 0;
    s1apT_recommendedCell * cells = *cells_ptr;

    ERL_NIF_TERM res = enif_make_list(env, 0);
    for(i = 0; i < size; ++i)
    {
        res = enif_make_list_cell(env, make_RecommendedCellItem(env, &(cells[size - 1 - i])), res);
    }

    return enif_make_tuple(
            env,
            3,
            enif_make_atom(env, "RecommendedCellsForPaging"),
            res,
            enif_make_atom(env, "asn1_NOVALUE")
            );
}

static ERL_NIF_TERM make_RecommendedENBItem(ErlNifEnv* env, s1apT_recommendedEnb * enbs)
{
    ERL_NIF_TERM enodeb = enif_make_tuple(
            env,
            2, 
            enif_make_int(env, enbs->globalEnodebId.pad_len), 
            enif_make_binary(env, &enbs->globalEnodebId.enodebId)
            );
    ERL_NIF_TERM macroENB = enif_make_tuple(
            env,
            2,
            enif_make_atom(env, "macroENB-ID"),
            enodeb
            );
    ERL_NIF_TERM res_1 = enif_make_tuple(
            env,
            4,
            enif_make_atom(env, "Global-ENB-ID"),
            enif_make_string_len(env, enbs->globalEnodebId.plmnId.data, 3, ERL_NIF_LATIN1),
            macroENB,
            enif_make_atom(env, "asn1_NOVALUE")
            );

    ERL_NIF_TERM res_2 = enif_make_tuple(
            env,
            2,
            enif_make_atom(env, "global-ENB-ID"),
            res_1
            );
    ERL_NIF_TERM res = enif_make_tuple(
            env,
            3,
            enif_make_atom(env, "RecommendedENBItem"),
            res_2,
            enif_make_atom(env, "asn1_NOVALUE")
            );
    return enif_make_tuple(
            env,
            4,
            enif_make_atom(env, "ProtocolIE-Field"),
            enif_make_int(env, ENUM_RecommendedEnbItem),
            make_criticality(env, enbs->criti), 
            res
            );

}

static ERL_NIF_TERM make_RecommendedENBsForPaging(ErlNifEnv* env, s1apT_recommendedEnb  enbs_ptr[][NUM_ENB], const int size)
{
    s1apT_recommendedEnb * enbs = * enbs_ptr;
    int i = 0;

    ERL_NIF_TERM res = enif_make_list(env, 0);
    for(i = 0; i < size; ++i)
    {
        res = enif_make_list_cell(env, make_RecommendedENBItem(env, &enbs[size - i - 1]), res);
    }

    return enif_make_tuple(
            env,
            3,
            enif_make_atom(env, "RecommendedENBsForPaging"),
            res,
            enif_make_atom(env, "asn1_NOVALUE")
            );
}

static ERL_NIF_TERM make_InformationOnRecommendedCellsAndENBsForPaging(ErlNifEnv* env, s1apT_recommended_cells_enbs *ce)
{
    ERL_NIF_TERM recommendedCellsForPaging, recommendedENBsForPaging, result;
    recommendedCellsForPaging = make_RecommendedCellsForPaging(env, &(ce->cellList), ce->numCell);
    recommendedENBsForPaging = make_RecommendedENBsForPaging(env, &(ce->enbList), ce->numEnb);

    result = enif_make_tuple(
            env,
            4,
            enif_make_atom(env, "InformationOnRecommendedCellsAndENBsForPaging"),
            recommendedCellsForPaging,
            recommendedENBsForPaging,
            enif_make_atom(env, "asn1_NOVALUE")
            );
    return result;
}

unsigned char * dec_RecommendedCellItem(unsigned char * const data, s1apT_recommendedCell * cell)
{    
    unsigned char * p = data;

    unsigned short int tag = 0;
    n2h2(p, &tag);
    if (ENUM_RecommendedCellItem != tag) {
        printf("silver_debug[%d]: expect for ENUM_RecommendedCellItem[%d], get[%d]\n", __LINE__, ENUM_RecommendedCellItem, tag);
        return NULL;
    }
    p += 2;

    cell->criti = (*p++ & 0xc0) >> 6;

    //open type length
    int len = *p++;

    int bitmask_self = 0xd0 & *p;
    int bitmask_ecgi = 0x18 & *p;
    ++p;

    enif_alloc_binary(3, &cell->ecgi.plmnId);
    memcpy(cell->ecgi.plmnId.data, p, 3);
    p += 3;

    cell->ecgi.pad_len = 4;
    enif_alloc_binary(4, &cell->ecgi.eci); //TODO: 4 LSB pad bits
    memcpy(cell->ecgi.eci.data, p, 4);
    p += 4;

    n2h2(p, &cell->time_stay_in_cell); 
    p += 2;

    return p;
}


unsigned char *  dec_RecommendedCellList(unsigned char * const data, const int num, const unsigned char bitmask_cells, s1apT_recommendedCell ptr_cellList[][NUM_CELL])
{
    //ignore pad
    int i = 0;
    s1apT_recommendedCell *cellList = *ptr_cellList;
    unsigned char * p = data;

    while(i < num) {
        p = dec_RecommendedCellItem(p, cellList + i);
        ++i;
    }

    //TODO test bitmask, currently, nothing from Dallas
    return p;
}

unsigned char *  dec_RecommendedENBItem(unsigned char * const data, s1apT_recommendedEnb * enb)
{
    unsigned char * p = data;

    unsigned short int tag;
    n2h2(p, &tag);
    if (ENUM_RecommendedEnbItem != tag) {
        printf("silver_debug[%d]: expect for ENUM_RecommendedEnbItem[%d], get[%d]\n", __LINE__, ENUM_RecommendedEnbItem, tag);
        return NULL;
    }
    p += 2;

    enb->criti = (*p++ & 0xc0) >> 6;

    //open type length
    int len = *p++;

    unsigned char bitmask_self = 0xd0 & *p;
    enb->choice = (0x10 & *p) >> 4;
    unsigned char bitmask_enb = 0x0c & *p;
    ++p;

    if (0 == enb->choice)
    {
        enif_alloc_binary(3, &enb->globalEnodebId.plmnId);
        memcpy(enb->globalEnodebId.plmnId.data, p, 3);
        p += 3;

        unsigned char bitmask_choice_macroENB = 0xc0 & *p++;

        if(0 == bitmask_choice_macroENB) {
            enb->globalEnodebId.pad_len = 4;
            enif_alloc_binary(3, &enb->globalEnodebId.enodebId);
            memcpy(enb->globalEnodebId.enodebId.data, p, 3); //TODO: scrach out 4 LSB pad bits
            p += 3;
        }
        else {
            printf("silver_debug[%d]:  should not be here\n", __LINE__);
            p = NULL;
        }

    }
    else {
        printf("silver_debug[%d]:  should not be here\n", __LINE__);
        p = NULL;
    }
    //TODO test bitmask, currently, nothing from Dallas
    return p;

}

unsigned char *  dec_RecommendedENBList(unsigned char * const data, const int size, const unsigned char bitmask_cells, s1apT_recommendedEnb  ptr_enbList[][NUM_ENB])
{
    int i = 0;
    s1apT_recommendedEnb *enbList = *ptr_enbList;
    unsigned char * p = data;

    while(i < size) {
        p = dec_RecommendedENBItem(p, enbList + i);
        ++i;
    }

    //TODO test bitmask, currently, nothing from Dallas
    return p;
}

int dec_InformationOnRecommendedCellsAndENBsForPaging(unsigned char * const data, const int size, s1apT_recommended_cells_enbs * ce)
{

#ifdef DEBUG
    enif_alloc_binary(3, &ce->cellList[0].ecgi.plmnId);
    strcpy(ce->cellList[0].ecgi.plmnId.data, "ab");
    enif_alloc_binary(4, &ce->cellList[0].ecgi.eci);
    strcpy(ce->cellList[0].ecgi.eci.data, "abc");
    enif_alloc_binary(3, &ce->cellList[1].ecgi.plmnId);
    strcpy(ce->cellList[1].ecgi.plmnId.data, "ab");
    enif_alloc_binary(4, &ce->cellList[1].ecgi.eci);
    strcpy(ce->cellList[1].ecgi.eci.data, "abc");
    enif_alloc_binary(3, &ce->enbList[0].globalEnodebId.plmnId);
    strcpy(ce->enbList[0].globalEnodebId.plmnId.data, "ab");
    enif_alloc_binary(3, &ce->enbList[0].globalEnodebId.enodebId);
    strcpy(ce->enbList[0].globalEnodebId.enodebId.data, "ab");

    ce->numCell = 2;
    ce->cellList[0].time_stay_in_cell = 30;
    ce->cellList[1].time_stay_in_cell = 40;

    ce->numEnb = 1;
#endif

    unsigned char * p = data;
    unsigned char bitmask_self = (0xc0 & *p) >> 6;
    unsigned char bitmask_son = (0x30 & *p) >> 4;
    ce->numCell = (0x0f & *p) + 1;
    ++p;
    p = dec_RecommendedCellList(p, ce->numCell, bitmask_son, &(ce->cellList));

    bitmask_son = (0xc0 && *p) >> 6;
    ce->numEnb = ((0x3c & *p) >> 2) + 1;
    ++p;
    p = dec_RecommendedENBList(p, ce->numEnb, bitmask_son, &ce->enbList);

    //ignore bit mask
}

static ERL_NIF_TERM nif_dec_InformationOnRecommendedCellsAndENBsForPaging(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ErlNifBinary bin;
    unsigned i;
    s1apT_recommended_cells_enbs ce;
    memset(&ce, 0, sizeof(s1apT_recommended_cells_enbs));
    if (argc != 1 || !enif_inspect_binary(env, argv[0], &bin))
        return enif_make_badarg(env);
/*    printf("silver_debug: received binary of length %zu\r\ndata: ", bin.size);
    for (i = 0; i < bin.size; ++i)
        printf("%x ", bin.data[i]);
    printf("\r\n");
*/
    dec_InformationOnRecommendedCellsAndENBsForPaging(bin.data, bin.size, &ce);
    return make_InformationOnRecommendedCellsAndENBsForPaging(env, &ce);
}

static ERL_NIF_TERM nif_create_tuple(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]){
    char atom[20] = "";
    int a1 = 0;
    int a2 = 0;

    enif_get_atom(env, argv[0], atom, 20, ERL_NIF_LATIN1);
    enif_get_int(env, argv[1], &a1); 
    enif_get_int(env, argv[1], &a2); 

    printf("Get att: a1=%s, a2=%d, a3=%d\r\n", atom, a1, a2);

    return enif_make_tuple(
            env, 
            3, 
            enif_make_atom(env, atom),
            enif_make_int(env, a1), 
            enif_make_int(env, a2)
            );
}

static ErlNifFunc nif_funcs[] = {  
    {"dec_InformationOnRecommendedCellsAndENBsForPaging", 1, nif_dec_InformationOnRecommendedCellsAndENBsForPaging}
};

static void unload(ErlNifEnv* env, void* priv)
{
    return;
}
static int upgrade(ErlNifEnv* env, void** priv, void** old_priv, ERL_NIF_TERM load_info)
{
    return 0;
}
void *getMcontextEip(ucontext_t *uc)
{
#if defined(Linux) || defined(linux)
#if defined(__i386__) || defined(__i686__)
    return (void*) uc->uc_mcontext.gregs[14]; /* Linux 32 */
#elif defined(__X86_64__) || defined(__x86_64__)
    return (void*) uc->uc_mcontext.gregs[16]; /* Linux 64 */
#elif defined(__ia64__) /* Linux IA64 */
    return (void*) uc->uc_mcontext.sc_ip;
#else
    unsupported_CPU
        return NULL;
#endif
#else
    unsupported_OS
#endif
}

void sigsegvHandler(int sig, siginfo_t *info, void *secret)
{
#define BACK_TRACE_SIZE 100
    void *trace[BACK_TRACE_SIZE];
    char **messages = NULL;
    int i, trace_size = 0;
    ucontext_t *uc = (ucontext_t*) secret;
    struct sigaction act;

    trace_size = backtrace(trace, BACK_TRACE_SIZE);

    if (getMcontextEip(uc) != NULL) {
        trace[1] = getMcontextEip(uc);
    }
    messages = backtrace_symbols(trace, trace_size);

    for (i=1; i<trace_size; ++i)
        printf("%s", messages[i]);

    if (sig != SIGUSR1 && sig != SIGUSR2)
    {
        sigemptyset (&act.sa_mask);
        act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND;
        act.sa_handler = SIG_DFL;
        sigaction (sig, &act, NULL);
        kill(getpid(), sig);
    }
}

void setupSignalHandlers(void)
{
    struct sigaction act;

    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_SIGINFO | SA_RESETHAND;
    act.sa_sigaction = sigsegvHandler;
    sigaction(SIGSEGV, &act, NULL);

}

    static int
load(ErlNifEnv* env, void** priv, ERL_NIF_TERM load_info)
{
    setupSignalHandlers();
    return 0;
}

ERL_NIF_INIT(silver_decoder, nif_funcs, &load, NULL, &upgrade, &unload)  
