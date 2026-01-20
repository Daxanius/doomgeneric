//
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// Network client code
//

#ifndef DG_CLIENT_H
#define DG_CLIENT_H

#include "doomdef.h"
#include "doomtype.h"
#include "d_ticcmd.h"
#include "sha1.h"
#include "net_defs.h"

// void DG_CL_Init(void);
// void DG_CL_Run(void);

// Implement the functions below with your own networking logic:
void DG_CL_GetSettings(net_gamesettings_t *_settings);

// Implement this one to send a ticc command to the server
// using whichever method you prefer
void DG_CL_SendTiccmd(ticcmd_t* cmd, int maketic);

// The following can be used to control the games multiplayer state

// Set the command bundle received from the server
// Player mask is a list of all connected players
// If one of the players in the mask is false, that player will be promptly removed from the local session
void DG_CL_SetCmdBundle(const ticcmd_t *cmds[MAXPLAYERS], const boolean* player_mask[MAXPLAYERS]);

// Use these to control players within the current game
void DG_CL_SetLocalPlayer(int player_num);

// The server should be in control of player IDs
// this spawns a player in for a given ID if the player does not
// exist already, otherwise it will respawn the player
void DG_CL_SpawnPlayer(int player_num);

// Remove a player from the local game. Will ignore if it's the current player
// Or if the number is invalid
void DG_CL_RemovePlayer(int player_num);

// Removes all players from the local game. Handy if we're disconnecting
void DG_CL_RemoveAllPlayers();

extern boolean net_client_connected;
extern char *net_player_name;

extern sha1_digest_t net_server_wad_sha1sum;
extern sha1_digest_t net_server_deh_sha1sum;
extern unsigned int net_server_is_freedoom;
extern sha1_digest_t net_local_wad_sha1sum;
extern sha1_digest_t net_local_deh_sha1sum;
extern unsigned int net_local_is_freedoom;

extern boolean drone;

#endif /* #ifndef NET_CLIENT_H */
