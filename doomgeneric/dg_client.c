#include "dg_client.h"
#include "m_misc.h"
#include "doomstat.h"
#include "deh_main.h"
#include "g_game.h"
#include "p_local.h"
#include "d_loop.h"

void DG_CL_SpawnPlayer(int player_num) {
	if (player_num < 0 || player_num >= MAXPLAYERS) {
		return;
	}

	// We can not spawn as or replace the current
	// local console player
	if (player_num == consoleplayer) return;

	if (playeringame[player_num]) {
		// Respawn the existing player
		G_DoReborn(player_num);
		return;
	}

	// Spawn a new player
  playeringame[player_num] = true;
  P_SpawnPlayer(&playerstarts[player_num]);
}

void DG_CL_RemovePlayer(int player_num) {
	if (player_num < 0 || player_num >= MAXPLAYERS) return;

	// If it's us, don't remove the player
	if (player_num == consoleplayer) return;

	// Remove the actual character model from the world
  if (players[player_num].mo) {
      P_RemoveMobj(players[player_num].mo);
      players[player_num].mo = NULL;
  }

  playeringame[player_num] = false;
}

void DG_CL_RemoveAllPlayers() {
	for (int i = 0; i < MAXPLATS; i++) {
		DG_CL_RemovePlayer(i);
	}
}

void DG_CL_SetCmdBundle(const ticcmd_t* cmds[MAXPLAYERS], const boolean* player_mask[MAXPLAYERS]) {
	D_ReceiveTic(cmds, player_mask);
}

void DG_CL_SetLocalPlayer(int player_num) {
	if (player_num < 0 || player_num >= MAXPLAYERS) return;
	consoleplayer = player_num;
	displayplayer = player_num;
}
