/*
===========================================================================
Copyright (C) 2007 Amine Haddad

** This file is modified by Thomas Rinsma, I take no copyright at all

This file is part of Tremulous.

The original works of vcxzet (lamebot3) were used a guide to create TremBot.

Tremulous is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Tremulous is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Tremulous; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

//Auriga: I stripped this poor bot LOL

#include "g_local.h"

void G_BotAdd( char *name, int team, int skill ) {
	int i;
	int clientNum;
	char userinfo[MAX_INFO_STRING];
	int reservedSlots = 0;
	gentity_t *bot;

	reservedSlots = trap_Cvar_VariableIntegerValue( "sv_privateclients" );

	// find what clientNum to use for bot
	clientNum = -1;
	for( i = 0; i < reservedSlots; i++ ) { //I hate this, why does it work, and yet not work
		if( !g_entities[i].inuse ) {
			clientNum = i;
			break;
		}
	}

	if(clientNum < 0) {
		trap_Printf("no more slots for bot\n");
		return;
	}

	bot = &g_entities[ clientNum ];
	bot->r.svFlags |= SVF_BOT;
	bot->inuse = qtrue;

	bot->diedTime = -1; //heh

	// register user information
	userinfo[0] = '\0';
	Info_SetValueForKey( userinfo, "name", name );
	Info_SetValueForKey( userinfo, "rate", "25000" );
	Info_SetValueForKey( userinfo, "snaps", "20" );

	trap_SetUserinfo( clientNum, userinfo );

	// have it connect to the game as a normal client
	if(ClientConnect(clientNum, qtrue) != NULL ) {
		// won't let us join
		return;
	}

	ClientBegin( clientNum );
	G_ChangeTeam( bot, team );
}

void G_BotDel( int clientNum ) {
	gentity_t *bot;

	bot = &g_entities[clientNum];
	if( !( bot->r.svFlags & SVF_BOT ) ) {
		trap_Printf( va("'^7%s^7' is not a bot\n", bot->client->pers.netname) );
		return;
	}

	ClientDisconnect(clientNum);
}

void G_BotThink( gentity_t *self )
{
	self->client->pers.cmd.buttons = 0;
	self->client->pers.cmd.forwardmove = 0;
	self->client->pers.cmd.upmove = 0;
	self->client->pers.cmd.rightmove = 0;
}

void G_BotSpectatorThink( gentity_t *self ) {
	if( self->client->ps.pm_flags & PMF_QUEUED) {
		//we're queued to spawn, all good
		return;
	}

  	if(!level.spawnPuck) //motherpucker
		return;
		
	if( self->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		int teamnum = self->client->pers.teamSelection;
		int clientNum = self->client->ps.clientNum;

		if( teamnum == PTE_HUMANS ) {
		self->client->pers.classSelection = PCL_HUMAN;
		self->client->ps.stats[ STAT_PCLASS ] = PCL_HUMAN;
		self->client->pers.humanItemSelection = WP_HBUILD;
		G_PushSpawnQueue( &level.humanSpawnQueue, clientNum );
		} else if( teamnum == PTE_ALIENS) {
			self->client->pers.classSelection = PCL_ALIEN_LEVEL0;
			self->client->ps.stats[ STAT_PCLASS ] = PCL_ALIEN_LEVEL0;
			G_PushSpawnQueue( &level.alienSpawnQueue, clientNum );
			level.spawnPuck = qfalse;
		}
	}
}

/*
 * Called when we are in intermission.
 * Just flag that we are ready to proceed.
 */
void G_BotIntermissionThink( gclient_t *client )
{
	client->readyToExit = qtrue;
}
