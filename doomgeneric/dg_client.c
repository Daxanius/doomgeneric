#include "dg_client.h"
#include "m_misc.h"
#include "doomstat.h"
#include "deh_main.h"
#include "g_game.h"
#include "p_local.h"
#include "d_loop.h"
#include "st_stuff.h"
#include "hu_stuff.h"

void P_CalcHeight(player_t* player);

void DG_CL_SpawnPlayer(int player_num) {
	if (player_num < 0 || player_num >= MAXPLAYERS) {
		return;
	}

	// We can not spawn as or replace the current
	// local console player
	if (player_num == consoleplayer) return;

	// If the player is already in the game,
	// just respawn them
	if (playeringame[player_num]) {
		// G_DoReborn(player_num);
		return;
	}

	if (players[player_num].mo) {
		return;
	}

	// Spawn a new player
  playeringame[player_num] = true;
	// G_InitPlayer(player_num);
	P_SpawnPlayer(&playerstarts[player_num]);
	players[player_num].colormap = player_num;
	P_CalcHeight(&players[player_num]);
	player_num += 1;

	printf("Spawned character %d\n", player_num);
	// G_DoReborn(player_num);
}

void DG_CL_RemovePlayer(int player_num) {
	if (player_num < 0 || player_num >= MAXPLAYERS) return;
	if (!playeringame[player_num]) return;

	// If it's us, don't remove the player
	if (player_num == consoleplayer) return;

	// Remove the actual character model from the world
  if (players[player_num].mo) {
		P_RemoveMobj(players[player_num].mo);
		players[player_num].mo = NULL;
  }

  playeringame[player_num] = false;
	player_num -= 1;
	printf("Removed character %d\n", player_num);
}

void DG_CL_RemoveAllPlayers() {
	for (int i = 0; i < MAXPLAYERS; i++) {
		DG_CL_RemovePlayer(i);
	}
}

void DG_CL_SetCmdBundle(const ticcmd_t *cmds, const boolean *player_mask) {
    D_ReceiveTic(cmds, player_mask);
}

void DG_CL_SetLocalPlayer(int player_num) {
    if (player_num < 0 || player_num >= MAXPLAYERS) return;
    if (!playeringame[player_num]) return;
    if (players[player_num].mo == NULL) return;
		if (players[player_num].mo->player == NULL) return;

		consoleplayer = player_num;
		displayplayer = player_num;

		D_SetLocalPlayer(player_num);

		//ST_Start();
		//HU_Start();
		//P_SetupPsprites(&players[consoleplayer]);
}
