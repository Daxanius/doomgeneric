//
// dg_coop.h
//
// Local co-op / headless multi-player API for doomgeneric.
//
// One engine instance simulates several players at once. The host drives the
// game by calling dg_coop_step() once per game tic with one command per player,
// and can render any player's view on demand.
//
// This makes desyncs effectively impossible
// Do NOT CALL doomgeneric_Tick() in this mode!!
//

#ifndef DG_COOP_H
#define DG_COOP_H

#include <stdint.h>
#include "d_ticcmd.h"
#include "doomgeneric.h"

#ifdef __cplusplus
extern "C" {
#endif

// Matches MAXPLAYERS in the engine (vanilla Doom is limited to 4).
#define DG_MAX_PLAYERS 4

typedef struct
{
    int num_players;       // 1 - DG_MAX_PLAYERS
    int deathmatch;        // 0 = co-op, 1 = deathmatch, 2 = altdeath
    int episode;
    int map;
    int skill;
    int nomonsters;
    int respawn_monsters;
    int fast_monsters;
    int timelimit;
    int seed;
} dg_coop_config_t;

typedef struct
{
    int      in_game;
    int      alive;
    int      health;
    int      armor;
    int      armor_type;
    int32_t  x, y, z;
    uint32_t angle;
    int32_t  momx, momy, momz;
    int      ready_weapon;
    int      ammo[4];
    int      frags[DG_MAX_PLAYERS];
} dg_player_state_t;

// Must be called before doomgeneric_Create()
void dg_coop_configure(const dg_coop_config_t *cfg);

// True after configure was called
int dg_coop_active(void);

// Run one game tic. Bit i of active_mask says whether player i takes part;
// clearing a bit removes that player from the game (like leaving a game).
// Returns the new game tic number.
int dg_coop_step(const ticcmd_t cmds[DG_MAX_PLAYERS], unsigned active_mask);

int dg_coop_gametic(void);

// Players can only be rendered while the
// state is DG_STATE_LEVEL.
#define DG_STATE_LEVEL        0
#define DG_STATE_INTERMISSION 1
#define DG_STATE_FINALE       2
#define DG_STATE_OTHER        3
int dg_coop_gamestate(void);

// Returns 0 if the player slot does not exist.
int dg_coop_player_state(int player, dg_player_state_t *out);

// Render the given player's view
int dg_coop_render_player(int player, pixel_t *out);

// Some internal bullcrap
extern int dg_step_mode;
const dg_coop_config_t *dg_coop_get_config(void);
void dg_coop_begin(void);   // called once the level is loaded

#ifdef __cplusplus
}
#endif

#endif // DG_COOP_H
