//
// dg_coop.c
//
// Local co-op / headless multi-player for doomgeneric. See dg_coop.h.
//

#include <string.h>

#include "dg_coop.h"

#include "d_loop.h"
#include "d_player.h"
#include "doomdef.h"
#include "doomstat.h"
#include "i_video.h"
#include "m_random.h"
#include "r_main.h"
#include "s_sound.h"
#include "st_stuff.h"

_Static_assert(DG_MAX_PLAYERS == MAXPLAYERS, "DG_MAX_PLAYERS must match MAXPLAYERS");

int dg_step_mode = 0;

// Index into the random table used by P_Random() (see m_random.c).
extern int prndindex;

static dg_coop_config_t dg_config;

void dg_coop_configure(const dg_coop_config_t *cfg)
{
    dg_config = *cfg;

    if (dg_config.num_players < 1)
        dg_config.num_players = 1;
    if (dg_config.num_players > DG_MAX_PLAYERS)
        dg_config.num_players = DG_MAX_PLAYERS;
    dg_config.seed &= 0xff;

    dg_step_mode = 1;
}

int dg_coop_active(void)
{
    return dg_step_mode;
}

const dg_coop_config_t *dg_coop_get_config(void)
{
    return dg_step_mode ? &dg_config : NULL;
}

void dg_coop_begin(void)
{
    // Level already loaded. The starting index changes what
    // happens from the first tic on.
    prndindex = dg_config.seed;
}

int dg_coop_step(const ticcmd_t cmds[DG_MAX_PLAYERS], unsigned active_mask) {
    boolean ingame[DG_MAX_PLAYERS];
    int i;

    if (!dg_step_mode) {
        return -1;
    }

    for (i = 0; i < DG_MAX_PLAYERS; ++i) {
        // Only players that exist in this game can take commands
        ingame[i] = (active_mask & (1u << i)) != 0 && i < dg_config.num_players;
    }

    D_StepTic(cmds, ingame, DG_MAX_PLAYERS);

    // Frees finished sound channels
    if (players[consoleplayer].mo != NULL)
        S_UpdateSounds(players[consoleplayer].mo);

    return gametic;
}

int dg_coop_gametic(void)
{
    return gametic;
}

int dg_coop_gamestate(void)
{
    switch (gamestate)
    {
        case GS_LEVEL:        return DG_STATE_LEVEL;
        case GS_INTERMISSION: return DG_STATE_INTERMISSION;
        case GS_FINALE:       return DG_STATE_FINALE;
        default:              return DG_STATE_OTHER;
    }
}

int dg_coop_player_state(int player, dg_player_state_t *out)
{
    const player_t *p;
    int i;

    memset(out, 0, sizeof(*out));

    if (!dg_step_mode || player < 0 || player >= DG_MAX_PLAYERS)
        return 0;

    p = &players[player];
    out->in_game = playeringame[player] ? 1 : 0;
    if (!out->in_game)
        return 0;

    out->alive = (p->playerstate == PST_LIVE);
    out->health = p->health;
    out->armor = p->armorpoints;
    out->armor_type = p->armortype;
    out->ready_weapon = p->readyweapon;

    for (i = 0; i < 4; ++i)
        out->ammo[i] = p->ammo[i];
    for (i = 0; i < DG_MAX_PLAYERS; ++i)
        out->frags[i] = p->frags[i];

    if (p->mo != NULL)
    {
        out->x = p->mo->x;
        out->y = p->mo->y;
        out->z = p->mo->z;
        out->angle = p->mo->angle;
        out->momx = p->mo->momx;
        out->momy = p->mo->momy;
        out->momz = p->mo->momz;
    }

    return 1;
}

int dg_coop_render_player(int player, pixel_t *out)
{
    if (!dg_step_mode || out == NULL)
        return 0;

    if (player < 0 || player >= DG_MAX_PLAYERS || !playeringame[player])
        return 0;

    if (gamestate != GS_LEVEL || players[player].mo == NULL)
        return 0;

    ST_SetPaletteFor(&players[player]);
    R_RenderPlayerView(&players[player]);
    I_BlitFrame(out);

    return 1;
}
