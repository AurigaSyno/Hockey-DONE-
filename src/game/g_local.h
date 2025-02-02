/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2000-2006 Tim Angus

This file is part of Tremulous.

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

// g_local.h -- local definitions for game module

#include "../qcommon/q_shared.h"
#include "bg_public.h"
#include "g_public.h"

typedef struct gentity_s gentity_t;
typedef struct gclient_s gclient_t;

#include "g_admin.h"

//==================================================================

#define INFINITE      1000000

#define FRAMETIME     100         // msec
#define CARNAGE_REWARD_TIME 3000
#define REWARD_SPRITE_TIME  2000

#define INTERMISSION_DELAY_TIME 1000
#define SP_INTERMISSION_DELAY_TIME 5000

// gentity->flags
#define FL_GODMODE        0x00000010
#define FL_NOTARGET       0x00000020
#define FL_TEAMSLAVE      0x00000400  // not the first on the team
#define FL_NO_KNOCKBACK   0x00000800
#define FL_DROPPED_ITEM   0x00001000
#define FL_NO_BOTS        0x00002000  // spawn point not for bot use
#define FL_NO_HUMANS      0x00004000  // spawn point just for bots
#define FL_FORCE_GESTURE  0x00008000  // spawn point just for bots

typedef struct
{
  qboolean	isNB;
  float		Area;
  float		Height;
} noBuild_t;

typedef struct
{
  gentity_t	*Marker;
  vec3_t	Origin;
} nbMarkers_t;

// movers are things like doors, plats, buttons, etc
typedef enum
{
  MOVER_POS1,
  MOVER_POS2,
  MOVER_1TO2,
  MOVER_2TO1,

  ROTATOR_POS1,
  ROTATOR_POS2,
  ROTATOR_1TO2,
  ROTATOR_2TO1,

  MODEL_POS1,
  MODEL_POS2,
  MODEL_1TO2,
  MODEL_2TO1
} moverState_t;

#define SP_PODIUM_MODEL   "models/mapobjects/podium/podium4.md3"

//============================================================================

struct gentity_s
{
  entityState_t     s;        // communicated by server to clients
  entityShared_t    r;        // shared by both the server system and game

  // DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER
  // EXPECTS THE FIELDS IN THAT ORDER!
  //================================

  struct gclient_s  *client;        // NULL if not a client

  qboolean          inuse;

  char              *classname;     // set in QuakeEd
  int               spawnflags;     // set in QuakeEd

  qboolean          neverFree;      // if true, FreeEntity will only unlink
                                    // bodyque uses this

  int               flags;          // FL_* variables

  char              *model;
  char              *model2;
  int               freetime;       // level.time when the object was freed

  int               eventTime;      // events will be cleared EVENT_VALID_MSEC after set
  qboolean          freeAfterEvent;
  qboolean          unlinkAfterEvent;

  qboolean          physicsObject;  // if true, it can be pushed by movers and fall off edges
                                    // all game items are physicsObjects,
  float             physicsBounce;  // 1.0 = continuous bounce, 0.0 = no bounce
  int               clipmask;       // brushes with this content value will be collided against
                                    // when moving.  items and corpses do not collide against
                                    // players, for instance

  // movers
  moverState_t      moverState;
  int               soundPos1;
  int               sound1to2;
  int               sound2to1;
  int               soundPos2;
  int               soundLoop;
  gentity_t         *parent;
  gentity_t         *nextTrain;
  gentity_t         *prevTrain;
  vec3_t            pos1, pos2;
  float             rotatorAngle;
  gentity_t         *clipBrush;     // clipping brush for model doors

  char              *message;

  int               timestamp;      // body queue sinking, etc

  float             angle;          // set in editor, -1 = up, -2 = down
  char              *target;
  char              *targetname;
  char              *team;
  char              *targetShaderName;
  char              *targetShaderNewName;
  gentity_t         *target_ent;

  float             speed;
  float             lastSpeed;      // used by trains that have been restarted
  vec3_t            movedir;

  // acceleration evaluation
  qboolean          evaluateAcceleration;
  vec3_t            oldVelocity;
  vec3_t            acceleration;
  vec3_t            oldAccel;
  vec3_t            jerk;

  int               nextthink;
  void              (*think)( gentity_t *self );
  void              (*reached)( gentity_t *self );  // movers call this when hitting endpoint
  void              (*blocked)( gentity_t *self, gentity_t *other );
  void              (*touch)( gentity_t *self, gentity_t *other, trace_t *trace );
  void              (*use)( gentity_t *self, gentity_t *other, gentity_t *activator );
  void              (*pain)( gentity_t *self, gentity_t *attacker, int damage );
  void              (*die)( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod );

  int               pain_debounce_time;
  int               fly_sound_debounce_time;  // wind tunnel
  int               last_move_time;

  int               health;
  int               lastHealth; // currently only used for overmind

  qboolean          takedamage;

  int               damage;
  int               splashDamage; // quad will increase this without increasing radius
  int               splashRadius;
  int               methodOfDeath;
  int               splashMethodOfDeath;
  int               chargeRepeat;

  int               count;

  gentity_t         *chain;
  gentity_t         *enemy;
  gentity_t         *activator;
  gentity_t         *teamchain;   // next entity in team
  gentity_t         *teammaster;  // master of the team

  int               watertype;
  int               waterlevel;

  int               noise_index;

  // timing variables
  float             wait;
  float             random;

  pTeam_t           stageTeam;
  stage_t           stageStage;

  int               biteam;             // buildable item team
  gentity_t         *parentNode;        // for creep and defence/spawn dependencies
  qboolean          active;             // for power repeater, but could be useful elsewhere
  qboolean          powered;            // for human buildables
  int               builtBy;            // clientNum of person that built this
  gentity_t         *dccNode;           // controlling dcc
  gentity_t         *overmindNode;      // controlling overmind
  qboolean          dcced;              // controlled by a dcc or not?
  qboolean          spawned;            // whether or not this buildable has finished spawning
  int               shrunkTime;         // time when a barricade shrunk or zero
  int               buildTime;          // when this buildable was built
  int               animTime;           // last animation change
  int               time1000;           // timer evaluated every second
  qboolean          deconstruct;        // deconstruct if no BP left
  int               deconstructTime;    // time at which structure marked
  int               overmindAttackTimer;
  int               overmindDyingTimer;
  int               overmindSpawnsTimer;
  int               nextPhysicsTime;    // buildables don't need to check what they're sitting on
                                        // every single frame.. so only do it periodically
  int               clientSpawnTime;    // the time until this spawn can spawn a client
  qboolean          lev1Grabbed;        // for turrets interacting with lev1s
  int               lev1GrabTime;       // for turrets interacting with lev1s
  int               spawnBlockTime;

  int               credits[ MAX_CLIENTS ];     // human credits for each client
  qboolean          creditsHash[ MAX_CLIENTS ]; // track who has claimed credit
  int               killedBy;                   // clientNum of killer

  gentity_t         *targeted;          // true if the player is currently a valid target of a turret
  vec3_t            turretAim;          // aim vector for turrets

  vec4_t            animation;          // animated map objects

  gentity_t         *builder;           // occupant of this hovel

  qboolean          nonSegModel;        // this entity uses a nonsegmented player model

  buildable_t       bTriggers[ BA_NUM_BUILDABLES ]; // which buildables are triggers
  pClass_t          cTriggers[ PCL_NUM_CLASSES ];   // which classes are triggers
  weapon_t          wTriggers[ WP_NUM_WEAPONS ];    // which weapons are triggers
  upgrade_t         uTriggers[ UP_NUM_UPGRADES ];   // which upgrades are triggers

  int               triggerGravity;                 // gravity for this trigger

  int               suicideTime;                    // when the client will suicide

  int               lastDamageTime;
  
  int               bdnumb;     // buildlog entry ID
  
  // For nobuild!
  noBuild_t	    noBuild;

  qboolean diedTime;
};

typedef enum
{
  CON_DISCONNECTED,
  CON_CONNECTING,
  CON_CONNECTED
} clientConnected_t;

typedef enum
{
  SPECTATOR_NOT,
  SPECTATOR_FREE,
  SPECTATOR_LOCKED,
  SPECTATOR_FOLLOW,
  SPECTATOR_SCOREBOARD
} spectatorState_t;

typedef enum
{
  TEAM_BEGIN,   // Beginning a team game, spawn at base
  TEAM_ACTIVE   // Now actively playing
} playerTeamStateState_t;

typedef struct
{
  playerTeamStateState_t  state;

  int     location;

  int     captures;
  int     basedefense;
  int     carrierdefense;
  int     flagrecovery;
  int     fragcarrier;
  int     assists;

  float   lasthurtcarrier;
  float   lastreturnedflag;
  float   flagsince;
  float   lastfraggedcarrier;
} playerTeamState_t;

// the auto following clients don't follow a specific client
// number, but instead follow the first two active players
#define FOLLOW_ACTIVE1  -1
#define FOLLOW_ACTIVE2  -2

// client data that stays across multiple levels or tournament restarts
// this is achieved by writing all the data to cvar strings at game shutdown
// time and reading them back at connection time.  Anything added here
// MUST be dealt with in G_InitSessionData() / G_ReadSessionData() / G_WriteSessionData()
typedef struct
{
  team_t            sessionTeam;
  pTeam_t         restartTeam; //for !restart keepteams and !restart switchteams
  int               spectatorTime;    // for determining next-in-line to play
  spectatorState_t  spectatorState;
  int               spectatorClient;  // for chasecam and follow mode
  int               wins, losses;     // tournament stats
  qboolean          invisible;       // for being invisible on the server - ghosts!
  qboolean          teamLeader;       // true when this client is a team leader
  clientList_t      ignoreList;
} clientSession_t;

#define MAX_NETNAME       36

// data to store details of clients that have abnormally disconnected
typedef struct connectionRecord_s
{
  int       clientNum;
  pTeam_t   clientTeam;
  int       clientCredit;
  int       clientScore;
  int       clientEnterTime;

  int       ptrCode;
} connectionRecord_t;

typedef struct
{
  short kills;
  short deaths;
  short feeds;
  short suicides;
  short assists;
  int dmgdone;
  int ffdmgdone;
  int structdmgdone;
  short structsbuilt;
  short repairspoisons;
  short structskilled;
  int timealive;
  int timeinbase;
  short headshots;
  int hits;
  int hitslocational;
  short teamkills;
  int dretchbasytime;
  int jetpackusewallwalkusetime;
  int timeLastViewed;
  int AllstatstimeLastViewed;
  int spreebleeds;
} statsCounters_t;

typedef struct
{
  int kills;
  int deaths;
  int feeds;
  int suicides;
  int assists;
  long dmgdone;
  long ffdmgdone;
  long structdmgdone;
  int structsbuilt;
  int repairspoisons;
  int structskilled;
  long timealive;
  long timeinbase;
  int headshots;
  long hits;
  long hitslocational;
  int teamkills;
  long dretchbasytime;
  long jetpackusewallwalkusetime;
  long timeLastViewed;
} statsCounters_level;

// client data that stays across multiple respawns, but is cleared
// on each level change or team change at ClientBegin()
typedef struct
{
  clientConnected_t   connected;
  usercmd_t           cmd;                // we would lose angles if not persistant
  qboolean            localClient;        // true if "ip" info key is "localhost"
  qboolean            initialSpawn;       // the first spawn should be at a cool location
  qboolean            predictItemPickup;  // based on cg_predictItems userinfo
  qboolean            pmoveFixed;         //
  char                netname[ MAX_NETNAME ];
  int                 maxHealth;          // for handicapping
  int                 enterTime;          // level.time the client entered the game
  playerTeamState_t   teamState;          // status in teamplay games
  int                 voteCount;          // to prevent people from constantly calling votes
  qboolean            teamInfo;           // send team overlay updates?

  pClass_t            classSelection;     // player class (copied to ent->client->ps.stats[ STAT_PCLASS ] once spawned)
  float               evolveHealthFraction;
  weapon_t            humanItemSelection; // humans have a starting item
  pTeam_t             teamSelection;      // player team (copied to ps.stats[ STAT_PTEAM ])

  int                 teamChangeTime;     // level.time of last team change
  qboolean            joinedATeam;        // used to tell when a PTR code is valid
  connectionRecord_t  *connection;

  int                 nameChangeTime;
  int                 nameChanges;

  // used to save playerState_t values while in SPECTATOR_FOLLOW mode
  int                 score;
  int                 credit;
  int                 ping;

  int                 lastTeamStatus;
  int                 lastFreekillTime;

  int                 lastFloodTime;         // level.time of last flood-limited command
  int                 floodDemerits;         // number of flood demerits accumulated

  char                lastMessage[ MAX_SAY_TEXT ];  // last message said by this player
  int                 lastMessageTime;              // level.time of last message said by this player

  int                 lastTeamKillTime;      // level.time of last team kill
  int                 teamKillDemerits;      // number of team kill demerits accumulated

  vec3_t              lastDeathLocation;
  char                guid[ 33 ];
  char                ip[ 16 ];
  qboolean            paused;
  qboolean            muted;
  int                 muteExpires;           // level.time at which a player is unmuted
  qboolean            ignoreAdminWarnings;
  qboolean            denyBuild;
  int                 specExpires;          // level.time at which a player can join a team again after being forced into spectator
  int                 denyHumanWeapons;
  int                 denyAlienClasses;
  int                 adminLevel;
  char                adminName[ MAX_NETNAME ];
  qboolean            designatedBuilder;
  qboolean            firstConnect;        // This is the first map since connect
  qboolean            useUnlagged;
  statsCounters_t     statscounters;
  int                 bubbleTime;
  int                 godMode;
} clientPersistant_t;

#define MAX_UNLAGGED_MARKERS 256
typedef struct unlagged_s {
  vec3_t      origin;
  vec3_t      mins;
  vec3_t      maxs;
  qboolean    used;
} unlagged_t;

typedef struct {
	float rangeBoost;
} adminRangeBoosts_t;

// this structure is cleared on each ClientSpawn(),
// except for 'client->pers' and 'client->sess'
struct gclient_s
{
  // ps MUST be the first element, because the server expects it
  playerState_t       ps;       // communicated by server to clients

  // exported into pmove, but not communicated to clients
  pmoveExt_t          pmext;

  // the rest of the structure is private to game
  clientPersistant_t  pers;
  clientSession_t     sess;

  qboolean            readyToExit;    // wishes to leave the intermission

  qboolean            noclip;

  int                 lastCmdTime;    // level.time of last usercmd_t, for EF_CONNECTION
                                      // we can't just use pers.lastCommand.time, because
                                      // of the g_sycronousclients case
  int                 buttons;
  int                 oldbuttons;
  int                 latched_buttons;

  vec3_t              oldOrigin;

  // sum up damage over an entire frame, so
  // shotgun blasts give a single big kick
  int                 damage_armor;     // damage absorbed by armor
  int                 damage_blood;     // damage taken out of health
  int                 damage_knockback; // impact damage
  vec3_t              damage_from;      // origin for vector calculation
  qboolean            damage_fromWorld; // if true, don't use the damage_from vector

  //
  int                 lastkilled_client;// last client that this client killed
  int                 lasthurt_client;  // last client that damaged this client
  int                 lasthurt_mod;     // type of damage the client did

  // timers
  int                 respawnTime;      // can respawn when time > this
  int                 inactivityTime;   // kick players when time > this
  qboolean            inactivityWarning;// qtrue if the five seoond warning has been given
  int                 rewardTime;       // clear the EF_AWARD_IMPRESSIVE, etc when time > this

  int                 airOutTime;

  qboolean            fireHeld;         // used for hook
  qboolean            fire2Held;        // used for alt fire
  gentity_t           *hook;            // grapple hook if out

  int                 switchTeamTime;   // time the player switched teams

  // timeResidual is used to handle events that happen every second
  // like health / armor countdowns and regeneration
  // two timers, one every 100 msecs, another every sec
  int                 time100;
  int                 time1000;
  int                 time10000;

  char                *areabits;

  gentity_t           *hovel;

  int                 lastPoisonTime;
  int                 poisonImmunityTime;
  gentity_t           *lastPoisonClient;
  int                 lastPoisonCloudedTime;
  gentity_t           *lastPoisonCloudedClient;
  int                 grabExpiryTime;
  int                 lastLockTime;
  int                 lastSlowTime;
  int                 lastBoostedTime;
  int                 lastMedKitTime;
  int                 medKitHealthToRestore;
  int                 medKitIncrementTime;
  int                 lastCreepSlowTime;    // time until creep can be removed

  qboolean            allowedToPounce;

  qboolean            charging;

  float               jetpackfuel;

  vec3_t              hovelOrigin;          // player origin before entering hovel

  int                 lastFlameBall;        // s.number of the last flame ball fired

#define RAM_FRAMES  1                       // number of frames to wait before retriggering
  int                 retriggerArmouryMenu; // frame number to retrigger the armoury menu

  unlagged_t          unlaggedHist[ MAX_UNLAGGED_MARKERS ];
  unlagged_t          unlaggedBackup;
  unlagged_t          unlaggedCalc;
  int                 unlaggedTime;
  
  int               tkcredits[ MAX_CLIENTS ];

  adminRangeBoosts_t newRange;

};


typedef struct spawnQueue_s
{
  int clients[ MAX_CLIENTS ];

  int front, back;
} spawnQueue_t;

#define QUEUE_PLUS1(x)  (((x)+1)%MAX_CLIENTS)
#define QUEUE_MINUS1(x) (((x)+MAX_CLIENTS-1)%MAX_CLIENTS)

void      G_InitSpawnQueue( spawnQueue_t *sq );
int       G_GetSpawnQueueLength( spawnQueue_t *sq );
int       G_PopSpawnQueue( spawnQueue_t *sq );
int       G_PeekSpawnQueue( spawnQueue_t *sq );
qboolean  G_SearchSpawnQueue( spawnQueue_t *sq, int clientNum );
qboolean  G_PushSpawnQueue( spawnQueue_t *sq, int clientNum );
qboolean  G_RemoveFromSpawnQueue( spawnQueue_t *sq, int clientNum );
int       G_GetPosInSpawnQueue( spawnQueue_t *sq, int clientNum );


#define MAX_LOCDAMAGE_TEXT    8192
#define MAX_LOCDAMAGE_REGIONS 16

// store locational damage regions
typedef struct damageRegion_s
{
  float     minHeight, maxHeight;
  int       minAngle, maxAngle;

  float     modifier;

  qboolean  crouch;
} damageRegion_t;

#define MAX_ARMOUR_TEXT    8192
#define MAX_ARMOUR_REGIONS 16

// store locational armour regions
typedef struct armourRegion_s
{
  float     minHeight, maxHeight;
  int       minAngle, maxAngle;

  float     modifier;

  qboolean  crouch;
} armourRegion_t;

//status of the warning of certain events
typedef enum
{
  TW_NOT = 0,
  TW_IMMINENT,
  TW_PASSED
} timeWarning_t;

typedef enum
{
  BF_BUILT,
  BF_DECONNED,
  BF_DESTROYED,
  BF_TEAMKILLED
} buildableFate_t;

// record all changes to the buildable layout - build, decon, destroy - and
// enough information to revert that change
typedef struct buildHistory_s buildHistory_t;
struct buildHistory_s
{
  int ID; // persistent ID to aid in specific reverting
  gentity_t *ent; // who, NULL if they've disconnected (or aren't an ent)
  char name[ MAX_NETNAME ]; // who, saves name if ent is NULL
  int buildable; // what
  vec3_t origin; // where
  vec3_t angles; // which way round
  vec3_t origin2; // I don't know what the hell these are, but layoutsave saves
  vec3_t angles2; // them so I will do the same
  buildableFate_t fate; // was it built, destroyed or deconned
  buildHistory_t *next; // next oldest change
  buildHistory_t *marked; // linked list of markdecon buildings taken
};

//
// this structure is cleared as each map is entered
//
#define MAX_SPAWN_VARS      64
#define MAX_SPAWN_VARS_CHARS  4096

typedef struct
{
  struct gclient_s  *clients;   // [maxclients]

  struct gentity_s  *gentities;
  int               gentitySize;
  int               num_entities;   // current number, <= MAX_GENTITIES

  fileHandle_t      logFile;

  // store latched cvars here that we want to get at often
  int               maxclients;

  int               framenum;
  int               time;                         // in msec
  int               previousTime;                 // so movers can back up when blocked
  int               frameMsec;                    // trap_Milliseconds() at end frame

  int               startTime;                    // level.time the map was started

  int               teamScores[ TEAM_NUM_TEAMS ];
  int               lastTeamLocationTime;         // last time of client team location update

  qboolean          newSession;                   // don't use any old session data, because
                                                  // we changed gametype

  qboolean          restarted;                    // waiting for a map_restart to fire

  int               numConnectedClients;
  int               numNonSpectatorClients;       // includes connecting clients
  int               numPlayingClients;            // connected, non-spectators
  int               sortedClients[MAX_CLIENTS];   // sorted by score

  int               numNewbies;                   // number of UnnamedPlayers who have been renamed this round.

  int               snd_fry;                      // sound index for standing in lava

  // voting state
  char              voteString[MAX_STRING_CHARS];
  char              voteDisplayString[MAX_STRING_CHARS];
  int               votePassThreshold;
  int               voteTime;                     // level.time vote was called
  int               voteExecuteTime;              // time the vote is executed
  int               voteYes;
  int               voteNo;
  int               numVotingClients;             // set by CalculateRanks
  int               votedHow[MAX_CLIENTS];       // 0 is "hasn't voted", 1 is "voted yes", -1 is voted "no"

  // team voting state
  char              teamVoteString[ 2 ][ MAX_STRING_CHARS ];
  char              teamVoteDisplayString[ 2 ][ MAX_STRING_CHARS ];
  int               teamVoteTime[ 2 ];            // level.time vote was called
  int               teamVoteYes[ 2 ];
  int               teamVoteNo[ 2 ];
  int               numteamVotingClients[ 2 ];    // set by CalculateRanks
  int               teamVotedHow[ 2 ][ MAX_CLIENTS ];

  // spawn variables
  qboolean          spawning;                     // the G_Spawn*() functions are valid
  int               numSpawnVars;
  char              *spawnVars[ MAX_SPAWN_VARS ][ 2 ];  // key / value pairs
  int               numSpawnVarChars;
  char              spawnVarChars[ MAX_SPAWN_VARS_CHARS ];

  // intermission state
  int               intermissionQueued;           // intermission was qualified, but
                                                  // wait INTERMISSION_DELAY_TIME before
                                                  // actually going there so the last
                                                  // frag can be watched.  Disable future
                                                  // kills during this delay
  int               intermissiontime;             // time the intermission was started
  char              *changemap;
  qboolean          readyToExit;                  // at least one client wants to exit
  int               exitTime;
  vec3_t            intermission_origin;          // also used for spectator spawns
  vec3_t            intermission_angle;

  qboolean          locationLinked;               // target_locations get linked
  gentity_t         *locationHead;                // head of the location list

  int               numAlienSpawns;
  int               numHumanSpawns;

  int               numAlienClients;
  int               numHumanClients;

  float             averageNumAlienClients;
  int               numAlienSamples;
  float             averageNumHumanClients;
  int               numHumanSamples;

  int               numLiveAlienClients;
  int               numLiveHumanClients;

  int               alienBuildPoints;
  int               humanBuildPoints;
  int               humanBuildPointsPowered;

  gentity_t         *markedBuildables[ MAX_GENTITIES ];
  int               numBuildablesForRemoval;

  int               alienKills;
  int               humanKills;

  qboolean          reactorPresent;
  qboolean          overmindPresent;
  qboolean          overmindMuted;

  int               humanBaseAttackTimer;

  pTeam_t           lastWin;

  int               suddenDeathABuildPoints;
  int               suddenDeathHBuildPoints;
  qboolean          suddenDeath;
  int               suddenDeathBeginTime;
  timeWarning_t     suddenDeathWarning;
  timeWarning_t     timelimitWarning;
  int               extend_vote_count;

  spawnQueue_t      alienSpawnQueue;
  spawnQueue_t      humanSpawnQueue;

  int               alienStage2Time;
  int               alienStage3Time;
  int               humanStage2Time;
  int               humanStage3Time;

  qboolean          uncondAlienWin;
  qboolean          uncondHumanWin;
  qboolean          alienTeamLocked;
  qboolean          humanTeamLocked;
  qboolean          paused;
  int               pauseTime;
  float             pause_speed;
  float             pause_gravity;
  float             pause_knockback;
  int               pause_ff;
  int               pause_ffb;

  int               lastCreditedAlien;
  int               lastCreditedHuman;

  int unlaggedIndex;
  int unlaggedTimes[ MAX_UNLAGGED_MARKERS ];

  char              layout[ MAX_QPATH ];

  pTeam_t           surrenderTeam;
  buildHistory_t    *buildHistory;
  int               lastBuildID;
  int               lastTeamUnbalancedTime;
  int               numTeamWarnings;  
  int               lastMsgTime;
  int               mapRotationVoteTime;
  
  statsCounters_level alienStatsCounters;
  statsCounters_level humanStatsCounters;

  qboolean          spawnPuck;
  qboolean          puckIsSpawned;
  
  qboolean	    noBuilding;
  float		    nbArea;
  float		    nbHeight;
  
  nbMarkers_t	    nbMarkers[ MAX_GENTITIES ];
} level_locals_t;

#define CMD_CHEAT         0x01
#define CMD_MESSAGE       0x02 // sends message to others (skip when muted)
#define CMD_TEAM          0x04 // must be on a team
#define CMD_NOTEAM        0x08 // must not be on a team
#define CMD_ALIEN         0x10
#define CMD_HUMAN         0x20
#define CMD_LIVING        0x40
#define CMD_INTERMISSION  0x80 // valid during intermission

typedef struct
{
  char *cmdName;
  int  cmdFlags;
  void ( *cmdHandler )( gentity_t *ent );
} commands_t;

//
// g_spawn.c
//
qboolean  G_SpawnString( const char *key, const char *defaultString, char **out );
// spawn string returns a temporary reference, you must CopyString() if you want to keep it
qboolean  G_SpawnFloat( const char *key, const char *defaultString, float *out );
qboolean  G_SpawnInt( const char *key, const char *defaultString, int *out );
qboolean  G_SpawnVector( const char *key, const char *defaultString, float *out );
void      G_SpawnEntitiesFromString( void );
char      *G_NewString( const char *string );

//
// g_cmds.c
//
void      Cmd_Score_f( gentity_t *ent );
qboolean  G_RoomForClassChange( gentity_t *ent, pClass_t class, vec3_t newOrigin );
void      G_StopFromFollowing( gentity_t *ent );
void      G_StopFollowing( gentity_t *ent );
qboolean  G_FollowNewClient( gentity_t *ent, int dir );
void      G_ToggleFollow( gentity_t *ent );
qboolean  G_MatchOnePlayer( int *plist, char *err, int len );
int       G_ClientNumbersFromString( char *s, int *plist );
void G_Say( gentity_t *ent, gentity_t *target, int mode, const char *chatText );
int       G_SayArgc( void );
qboolean  G_SayArgv( int n, char *buffer, int bufferLength );
char      *G_SayConcatArgs( int start );
void      G_DecolorString( char *in, char *out );
void      G_ParseEscapedString( char *buffer );
void      G_LeaveTeam( gentity_t *self );
void      G_ChangeTeam( gentity_t *ent, pTeam_t newTeam );
void      G_SanitiseString( char *in, char *out, int len );
void      G_PrivateMessage( gentity_t *ent );
char      *G_statsString( statsCounters_t *sc, pTeam_t *pt );
void      Cmd_Share_f( gentity_t *ent );
void      Cmd_Donate_f( gentity_t *ent );
void      Cmd_TeamVote_f( gentity_t *ent ); 
void      Cmd_Builder_f( gentity_t *ent );
void      G_WordWrap( char *buffer, int maxwidth );
void      G_CP( gentity_t *ent );
qboolean  G_IsMuted( gclient_t *ent );
qboolean  G_TeamKill_Repent( gentity_t *ent );

//
// g_physics.c
//
void G_Physics( gentity_t *ent, int msec );

//
// g_buildable.c
//

#define MAX_ALIEN_BBOX  25

typedef enum
{
  IBE_NONE,

  IBE_NOOVERMIND,
  IBE_OVERMIND,
  IBE_NOASSERT,
  IBE_SPWNWARN,
  IBE_NOCREEP,
  IBE_HOVEL,
  IBE_HOVELEXIT,

  IBE_REACTOR,
  IBE_REPEATER,
  IBE_TNODEWARN,
  IBE_RPTWARN,
  IBE_RPTWARN2,
  IBE_NOPOWER,
  IBE_NODCC,

  IBE_NORMAL,
  IBE_NOROOM,
  IBE_PERMISSION,

  IBE_MAXERRORS
} itemBuildError_t;

qboolean          AHovel_Blocked( gentity_t *hovel, gentity_t *player, qboolean provideExit );
gentity_t         *G_CheckSpawnPoint( int spawnNum, vec3_t origin, vec3_t normal,
                    buildable_t spawn, vec3_t spawnOrigin );

qboolean          G_IsPowered( vec3_t origin );
qboolean          G_IsDCCBuilt( void );
qboolean          G_IsOvermindBuilt( void );

void              G_BuildableThink( gentity_t *ent, int msec );
qboolean          G_BuildableRange( vec3_t origin, float r, buildable_t buildable );
itemBuildError_t  G_CanBuild( gentity_t *ent, buildable_t buildable, int distance, vec3_t origin );
qboolean G_BuildingExists( int bclass ) ;
qboolean          G_BuildIfValid( gentity_t *ent, buildable_t buildable );
void              G_SetBuildableAnim( gentity_t *ent, buildableAnimNumber_t anim, qboolean force );
void              G_SetIdleBuildableAnim( gentity_t *ent, buildableAnimNumber_t anim );
void              G_SpawnBuildable(gentity_t *ent, buildable_t buildable);
void              FinishSpawningBuildable( gentity_t *ent );
void              G_CheckDBProtection( void );
void              G_LayoutSave( char *name );
int               G_LayoutList( const char *map, char *list, int len );
void              G_LayoutSelect( void );
void              G_LayoutLoad( void );
void              G_BaseSelfDestruct( pTeam_t team );
gentity_t         *G_InstantBuild( buildable_t buildable, vec3_t origin, vec3_t angles, vec3_t origin2, vec3_t angles2 );
void              G_SpawnRevertedBuildable( buildHistory_t *bh, qboolean mark );
void              G_CommitRevertedBuildable( gentity_t *ent );
qboolean          G_RevertCanFit( buildHistory_t *bh );
int               G_LogBuild( buildHistory_t *new );
int               G_CountBuildLog( void );
char             *G_FindBuildLogName( int id );
void		  G_NobuildSave( void );
void 		  G_NobuildLoad( void );

//
// g_utils.c
//
int         G_ParticleSystemIndex( char *name );
int         G_ShaderIndex( char *name );
int         G_ModelIndex( char *name );
int         G_SoundIndex( char *name );
void        G_TeamCommand( pTeam_t team, char *cmd );
void        G_KillBox (gentity_t *ent);
gentity_t   *G_Find (gentity_t *from, int fieldofs, const char *match);
gentity_t   *G_PickTarget (char *targetname);
void        G_UseTargets (gentity_t *ent, gentity_t *activator);
void        G_SetMovedir ( vec3_t angles, vec3_t movedir);

void        G_InitGentity( gentity_t *e );
gentity_t   *G_Spawn( void );
gentity_t   *G_TempEntity( vec3_t origin, int event );
void        G_Sound( gentity_t *ent, int channel, int soundIndex );
void        G_FreeEntity( gentity_t *e );
qboolean    G_EntitiesFree( void );

void        G_TouchTriggers( gentity_t *ent );
void        G_TouchSolids( gentity_t *ent );

float       *tv( float x, float y, float z );
char        *vtos( const vec3_t v );

float       vectoyaw( const vec3_t vec );

void        G_AddPredictableEvent( gentity_t *ent, int event, int eventParm );
void        G_AddEvent( gentity_t *ent, int event, int eventParm );
void        G_BroadcastEvent( int event, int eventParm );
void        G_SetOrigin( gentity_t *ent, vec3_t origin );
void        AddRemap(const char *oldShader, const char *newShader, float timeOffset);
const char  *BuildShaderStateConfig( void );


qboolean    G_ClientIsLagging( gclient_t *client );

void        G_TriggerMenu( int clientNum, dynMenu_t menu );
void        G_CloseMenus( int clientNum );

qboolean    G_Visible( gentity_t *ent1, gentity_t *ent2 );
gentity_t   *G_ClosestEnt( vec3_t origin, gentity_t **entities, int numEntities );

//
// g_combat.c
//
qboolean  CanDamage( gentity_t *targ, vec3_t origin );
void      G_Damage( gentity_t *targ, gentity_t *inflictor, gentity_t *attacker,
                    vec3_t dir, vec3_t point, int damage, int dflags, int mod );
void      G_SelectiveDamage( gentity_t *targ, gentity_t *inflictor, gentity_t *attacker, vec3_t dir,
                             vec3_t point, int damage, int dflags, int mod, int team );
qboolean  G_RadiusDamage( vec3_t origin, gentity_t *attacker, float damage, float radius,
                          gentity_t *ignore, int mod );
qboolean  G_SelectiveRadiusDamage( vec3_t origin, gentity_t *attacker, float damage, float radius,
                                   gentity_t *ignore, int mod, int team );
void      G_Knockback( gentity_t *targ, vec3_t dir, int knockback );
void      body_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath );
void      AddScore( gentity_t *ent, int score );

void      G_InitDamageLocations( void );

// damage flags
#define DAMAGE_RADIUS         0x00000001  // damage was indirect
#define DAMAGE_NO_ARMOR       0x00000002  // armour does not protect from this damage
#define DAMAGE_NO_KNOCKBACK   0x00000004  // do not affect velocity, just view angles
#define DAMAGE_NO_PROTECTION  0x00000008  // armor, shields, invulnerability, and godmode have no effect
#define DAMAGE_NO_LOCDAMAGE   0x00000010  // do not apply locational damage

//
// g_missile.c
//
void      G_RunMissile( gentity_t *ent );

gentity_t *fire_flamer( gentity_t *self, vec3_t start, vec3_t aimdir );
gentity_t *fire_blaster( gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_pulseRifle( gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_luciferCannon( gentity_t *self, vec3_t start, vec3_t dir, int damage, int radius );
gentity_t *fire_lockblob( gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_paraLockBlob( gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_slowBlob( gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_bounceBall( gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *fire_hive( gentity_t *self, vec3_t start, vec3_t dir );
gentity_t *launch_grenade( gentity_t *self, vec3_t start, vec3_t dir );


//
// g_mover.c
//
void G_RunMover( gentity_t *ent );
void Touch_DoorTrigger( gentity_t *ent, gentity_t *other, trace_t *trace );
void manualTriggerSpectator( gentity_t *trigger, gentity_t *player );

//
// g_trigger.c
//
void trigger_teleporter_touch( gentity_t *self, gentity_t *other, trace_t *trace );
void G_Checktrigger_stages( pTeam_t team, stage_t stage );


//
// g_misc.c
//
void TeleportPlayer( gentity_t *player, vec3_t origin, vec3_t angles );
void ShineTorch( gentity_t *self );

//
// g_weapon.c
//

#define MAX_ZAP_TARGETS LEVEL2_AREAZAP_MAX_TARGETS

typedef struct zap_s
{
  qboolean      used;

  gentity_t     *creator;
  gentity_t     *targets[ MAX_ZAP_TARGETS ];
  int           numTargets;

  int           timeToLive;
  int           damageUsed;

  gentity_t     *effectChannel;
} zap_t;

void      G_ForceWeaponChange( gentity_t *ent, weapon_t weapon );
void      G_GiveClientMaxAmmo( gentity_t *ent, qboolean buyingEnergyAmmo );
void      CalcMuzzlePoint( gentity_t *ent, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint );
void      SnapVectorTowards( vec3_t v, vec3_t to );
qboolean  CheckVenomAttack( gentity_t *ent );
void      CheckGrabAttack( gentity_t *ent );
qboolean  CheckPounceAttack( gentity_t *ent );
void      ChargeAttack( gentity_t *ent, gentity_t *victim );
void      G_UpdateZaps( int msec );


//
// g_client.c
//
void      G_AddCreditToClient( gclient_t *client, short credit, qboolean cap );
team_t    TeamCount( int ignoreClientNum, int team );
void      G_SetClientViewAngle( gentity_t *ent, vec3_t angle );
gentity_t *G_SelectTremulousSpawnPoint( pTeam_t team, vec3_t preference, vec3_t origin, vec3_t angles );
gentity_t *G_SelectSpawnPoint( vec3_t avoidPoint, vec3_t origin, vec3_t angles );
gentity_t *G_SelectAlienLockSpawnPoint( vec3_t origin, vec3_t angles );
gentity_t *G_SelectHumanLockSpawnPoint( vec3_t origin, vec3_t angles );
void      SpawnCorpse( gentity_t *ent );
void      respawn( gentity_t *ent );
void      BeginIntermission( void );
void      ClientSpawn( gentity_t *ent, gentity_t *spawn, vec3_t origin, vec3_t angles );
void      player_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod );
qboolean  SpotWouldTelefrag( gentity_t *spot );
char     *G_NextNewbieName( gentity_t *ent );

//
// g_svcmds.c
//
qboolean  ConsoleCommand( void );
void      G_ProcessIPBans( void );
qboolean  G_FilterPacket( char *from );

//
// g_weapon.c
//
void FireWeapon( gentity_t *ent );
void FireWeapon2( gentity_t *ent );
void FireWeapon3( gentity_t *ent );

//
// g_cmds.c
//

//
// g_main.c
//
void ScoreboardMessage( gentity_t *client );
void MoveClientToIntermission( gentity_t *client );
void G_MapConfigs( const char *mapname );
void CalculateRanks( void );
void FindIntermissionPoint( void );
void G_RunThink( gentity_t *ent );
void QDECL G_LogPrintf( const char *fmt, ... );
void QDECL G_LogPrintfColoured( const char *fmt, ... );
void QDECL G_LogOnlyPrintf( const char *fmt, ... );
void QDECL G_AdminsPrintf( const char *fmt, ... );
void QDECL G_WarningsPrintf( char *flag, const char *fmt, ... );
void QDECL G_LogOnlyPrintf( const char *fmt, ... );
void SendScoreboardMessageToAllClients( void );
void QDECL G_Printf( const char *fmt, ... );
void QDECL G_Error( const char *fmt, ... );
void CheckVote( void );
void CheckTeamVote( int teamnum );
void LogExit( const char *string );
int  G_TimeTilSuddenDeath( void );
void CheckMsgTimer( void );
qboolean G_Flood_Limited( gentity_t *ent );

//
// g_client.c
//
char *ClientConnect( int clientNum, qboolean firstTime );
void ClientUserinfoChanged( int clientNum, qboolean forceName );
void ClientDisconnect( int clientNum );
void ClientBegin( int clientNum );
void ClientCommand( int clientNum );

//
// g_active.c
//
void G_UnlaggedStore( void );
void G_UnlaggedClear( gentity_t *ent );
void G_UnlaggedCalc( int time, gentity_t *skipEnt );
void G_UnlaggedOn( gentity_t *attacker, vec3_t muzzle, float range );
void G_UnlaggedOff( void );
void ClientThink( int clientNum );
void ClientEndFrame( gentity_t *ent );
void G_RunClient( gentity_t *ent );

//
// g_team.c
//
qboolean  OnSameTeam( gentity_t *ent1, gentity_t *ent2 );
gentity_t *Team_GetLocation( gentity_t *ent );
qboolean  Team_GetLocationMsg( gentity_t *ent, char *loc, int loclen );
void      TeamplayInfoMessage( gentity_t *ent );
void      CheckTeamStatus( void );

//
// g_mem.c
//
void *G_Alloc( int size );
void G_InitMemory( void );
void G_Free( void *ptr );
void G_DefragmentMemory( void );
void Svcmd_GameMem_f( void );

//
// g_session.c
//
void G_ReadSessionData( gclient_t *client );
void G_InitSessionData( gclient_t *client, char *userinfo );
void G_WriteSessionData( void );

//
// g_bot.c
//
void G_BotAdd( char *name, int team, int skill );
void G_BotThink( gentity_t *self );
void G_BotSpectatorThink( gentity_t *self );
void G_BotIntermissionThink( gclient_t *client );

//
// g_maprotation.c
//
#define MAX_MAP_ROTATIONS       64
#define MAX_MAP_ROTATION_MAPS   64
#define MAX_MAP_COMMANDS        16
#define MAX_MAP_ROTATION_CONDS  8

#define NOT_ROTATING          -1

typedef enum
{
  MCV_ERR,
  MCV_RANDOM,
  MCV_NUMCLIENTS,
  MCV_LASTWIN,
  MCV_VOTE,
  MCV_SELECTEDRANDOM
} mapConditionVariable_t;

typedef enum
{
  MCO_LT,
  MCO_EQ,
  MCO_GT
} mapConditionOperator_t;

typedef enum
{
  MCT_ERR,
  MCT_MAP,
  MCT_ROTATION
} mapConditionType_t;

typedef struct mapRotationCondition_s
{
  char                    dest[ MAX_QPATH ];

  qboolean                unconditional;

  mapConditionVariable_t  lhs;
  mapConditionOperator_t  op;

  int                     numClients;
  pTeam_t                 lastWin;
} mapRotationCondition_t;

typedef struct mapRotationEntry_s
{
  char                    name[ MAX_QPATH ];

  char                    postCmds[ MAX_MAP_COMMANDS ][ MAX_STRING_CHARS ];
  char                    layouts[ MAX_CVAR_VALUE_STRING ];
  int                     numCmds;

  mapRotationCondition_t  conditions[ MAX_MAP_ROTATION_CONDS ];
  int                     numConditions;
} mapRotationEntry_t;

typedef struct mapRotation_s
{
  char                name[ MAX_QPATH ];

  mapRotationEntry_t  maps[ MAX_MAP_ROTATION_MAPS ];
  int                 numMaps;
  int                 currentMap;
} mapRotation_t;

typedef struct mapRotations_s
{
  mapRotation_t   rotations[ MAX_MAP_ROTATIONS ];
  int             numRotations;
} mapRotations_t;

void      G_PrintRotations( void );
qboolean  G_AdvanceMapRotation( void );
qboolean  G_StartMapRotation( char *name, qboolean changeMap );
void      G_StopMapRotation( void );
qboolean  G_MapRotationActive( void );
void      G_InitMapRotations( void );
qboolean  G_MapExists( char *name );
int       G_GetCurrentMap( int rotation );

qboolean G_CheckMapRotationVote( void );
qboolean G_IntermissionMapVoteWinner( void );
void G_IntermissionMapVoteMessage( gentity_t *ent );
void G_IntermissionMapVoteMessageAll( void );
void G_IntermissionMapVoteCommand( gentity_t *ent, qboolean next, qboolean choose );

//
// g_ptr.c
//
void                G_UpdatePTRConnection( gclient_t *client );
connectionRecord_t  *G_GenerateNewConnection( gclient_t *client );
void                G_ResetPTRConnections( void );
connectionRecord_t  *G_FindConnectionForCode( int code );


//some maxs
#define MAX_FILEPATH      144

extern  level_locals_t  level;
extern  gentity_t       g_entities[ MAX_GENTITIES ];

#define FOFS(x) ((int)&(((gentity_t *)0)->x))

extern  vmCvar_t  g_dedicated;
extern  vmCvar_t  g_cheats;
extern  vmCvar_t  g_maxclients;     // allow this many total, including spectators
extern  vmCvar_t  g_maxGameClients;   // allow this many active
extern  vmCvar_t  g_restarted;
extern  vmCvar_t  g_lockTeamsAtStart;
extern  vmCvar_t  g_minCommandPeriod;
extern  vmCvar_t  g_minNameChangePeriod;
extern  vmCvar_t  g_maxNameChanges;
extern  vmCvar_t  g_newbieNumbering;
extern  vmCvar_t  g_newbieNamePrefix;

extern  vmCvar_t  g_timelimit;
extern  vmCvar_t  g_suddenDeathTime;
extern  vmCvar_t  g_suddenDeath;
extern  vmCvar_t  g_suddenDeathMode;
extern  vmCvar_t  g_friendlyFire;
extern  vmCvar_t  g_friendlyFireHumans;
extern  vmCvar_t  g_friendlyFireAliens;
extern  vmCvar_t  g_retribution;
extern  vmCvar_t  g_friendlyFireMovementAttacks;
extern  vmCvar_t  g_friendlyBuildableFire;
extern  vmCvar_t  g_password;
extern  vmCvar_t  g_needpass;
extern  vmCvar_t  g_gravity;
extern  vmCvar_t  g_speed;
extern  vmCvar_t  g_knockback;
extern  vmCvar_t  g_quadfactor;
extern  vmCvar_t  g_inactivity;
extern  vmCvar_t  g_debugMove;
extern  vmCvar_t  g_debugAlloc;
extern  vmCvar_t  g_debugDamage;
extern  vmCvar_t  g_weaponRespawn;
extern  vmCvar_t  g_weaponTeamRespawn;
extern  vmCvar_t  g_synchronousClients;
extern  vmCvar_t  g_motd;
extern  vmCvar_t  g_warmup;
extern  vmCvar_t  g_warmupMode;
extern  vmCvar_t  g_doWarmup;
extern  vmCvar_t  g_blood;
extern  vmCvar_t  g_allowVote;
extern  vmCvar_t  g_requireVoteReasons;
extern  vmCvar_t  g_voteLimit;
extern  vmCvar_t  g_suddenDeathVotePercent;
extern  vmCvar_t  g_suddenDeathVoteDelay;
extern  vmCvar_t  g_extendVotesPercent;
extern  vmCvar_t  g_extendVotesTime;
extern  vmCvar_t  g_extendVotesCount;
extern  vmCvar_t  g_kickVotesPercent;
extern  vmCvar_t  g_customVote1;
extern  vmCvar_t  g_customVote2;
extern  vmCvar_t  g_customVote3;
extern  vmCvar_t  g_customVote4;
extern  vmCvar_t  g_customVote5;
extern  vmCvar_t  g_customVote6;
extern  vmCvar_t  g_customVote7;
extern  vmCvar_t  g_customVote8;
#define CUSTOM_VOTE_COUNT 8
extern  vmCvar_t  g_customVotePercent;
extern  vmCvar_t  g_mapVotesPercent;
extern  vmCvar_t  g_mapRotationVote;
extern  vmCvar_t  g_extendVotesPercent;
extern  vmCvar_t  g_extendVotesTime;
extern  vmCvar_t  g_extendVotesCount;
extern  vmCvar_t  g_readyPercent;
extern  vmCvar_t  g_designateVotes;
extern  vmCvar_t  g_teamAutoJoin;
extern  vmCvar_t  g_teamForceBalance;
extern  vmCvar_t  g_banIPs;
extern  vmCvar_t  g_filterBan;
extern  vmCvar_t  g_smoothClients;
extern  vmCvar_t  g_outdatedClientMessage;
extern  vmCvar_t  pmove_fixed;
extern  vmCvar_t  pmove_msec;
extern  vmCvar_t  g_rankings;
extern  vmCvar_t  g_allowShare;
extern  vmCvar_t  g_creditOverflow;
extern  vmCvar_t  g_enableDust;
extern  vmCvar_t  g_enableBreath;
extern  vmCvar_t  g_singlePlayer;

extern  vmCvar_t  g_humanBuildPoints;
extern  vmCvar_t  g_alienBuildPoints;
extern  vmCvar_t  g_humanStage;
extern  vmCvar_t  g_humanKills;
extern  vmCvar_t  g_humanMaxStage;
extern  vmCvar_t  g_humanStage2Threshold;
extern  vmCvar_t  g_humanStage3Threshold;
extern  vmCvar_t  g_alienStage;
extern  vmCvar_t  g_alienKills;
extern  vmCvar_t  g_alienMaxStage;
extern  vmCvar_t  g_alienStage2Threshold;
extern  vmCvar_t  g_alienStage3Threshold;
extern  vmCvar_t  g_teamImbalanceWarnings;

extern  vmCvar_t  g_unlagged;

extern  vmCvar_t  g_disabledEquipment;
extern  vmCvar_t  g_disabledClasses;
extern  vmCvar_t  g_disabledBuildables;

extern  vmCvar_t  g_markDeconstruct;
extern  vmCvar_t  g_markDeconstructMode;
extern  vmCvar_t  g_deconDead;

extern  vmCvar_t  g_debugMapRotation;
extern  vmCvar_t  g_currentMapRotation;
extern  vmCvar_t  g_currentMap;
extern  vmCvar_t  g_nextMap;
extern  vmCvar_t  g_initialMapRotation;
extern  vmCvar_t  g_chatTeamPrefix;
extern  vmCvar_t  g_actionPrefix;
extern  vmCvar_t  g_floodMaxDemerits;
extern  vmCvar_t  g_floodMinTime;
extern  vmCvar_t  g_spamTime;

extern  vmCvar_t  g_shove;

extern  vmCvar_t  g_mapConfigs;

extern  vmCvar_t  g_layouts;
extern  vmCvar_t  g_layoutAuto;

extern  vmCvar_t  g_admin;
extern  vmCvar_t  g_adminLog;
extern  vmCvar_t  g_adminParseSay;
extern  vmCvar_t  g_adminSayFilter;
extern  vmCvar_t  g_adminNameProtect;
extern  vmCvar_t  g_adminTempMute;
extern  vmCvar_t  g_adminTempBan;
extern  vmCvar_t  g_adminMaxBan;
extern  vmCvar_t  g_adminTempSpec;
extern  vmCvar_t  g_adminMapLog;
extern  vmCvar_t  g_minLevelToJoinTeam;
extern  vmCvar_t  g_minDeconLevel;
extern  vmCvar_t  g_minDeconAffectsMark;
extern  vmCvar_t  g_forceAutoSelect;
extern  vmCvar_t  g_minLevelToSpecMM1;
extern  vmCvar_t  g_banNotice;

extern  vmCvar_t  g_devmapKillerHP;
extern  vmCvar_t  g_killerHP;

extern  vmCvar_t  g_privateMessages;
extern  vmCvar_t  g_fullIgnore;
extern  vmCvar_t  g_decolourLogfiles;
extern  vmCvar_t  g_publicSayadmins;
extern  vmCvar_t  g_myStats;
extern  vmCvar_t  g_teamStatus;
extern  vmCvar_t  g_antiSpawnBlock;

extern  vmCvar_t  g_dretchPunt;

extern  vmCvar_t  g_devmapNoGod;
extern  vmCvar_t  g_devmapNoStructDmg;

extern  vmCvar_t  g_slapKnockback;
extern  vmCvar_t  g_slapDamage;

extern  vmCvar_t  g_voteMinTime;
extern  vmCvar_t  g_mapvoteMaxTime;
extern  vmCvar_t  g_blacklistedMaps;

extern  vmCvar_t  g_msg;
extern  vmCvar_t  g_msgTime;
extern  vmCvar_t  g_welcomeMsg;
extern  vmCvar_t  g_welcomeMsgTime;
extern  vmCvar_t  g_deconBanTime;

extern  vmCvar_t  g_buildLogMaxLength;

extern vmCvar_t  g_AllStats;
extern vmCvar_t  g_AllStatsTime;

extern  vmCvar_t  mod_jetpackFuel;
extern  vmCvar_t  mod_jetpackConsume;
extern  vmCvar_t  mod_jetpackRegen;

extern  vmCvar_t  g_adminExpireTime;

extern  vmCvar_t  g_autoGhost;

extern  vmCvar_t  g_teamKillThreshold;

extern  vmCvar_t  g_aimbotAdvertBan;
extern  vmCvar_t  g_aimbotAdvertBanTime;
extern  vmCvar_t  g_aimbotAdvertBanReason;

extern  vmCvar_t  g_Bubbles;
extern  vmCvar_t  g_scrimMode;
extern  vmCvar_t  g_gradualFreeFunds;
extern  vmCvar_t  g_bleedingSpree;
extern  vmCvar_t  g_practise;
extern  vmCvar_t  g_specNoclip;

extern  vmCvar_t g_spawnPuck;
extern  vmCvar_t g_puckName;

void      trap_Printf( const char *fmt );
void      trap_Error( const char *fmt );
int       trap_Milliseconds( void );
int       trap_RealTime( qtime_t *qtime );
int       trap_Argc( void );
void      trap_Argv( int n, char *buffer, int bufferLength );
void      trap_Args( char *buffer, int bufferLength );
int       trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode );
void      trap_FS_Read( void *buffer, int len, fileHandle_t f );
void      trap_FS_Write( const void *buffer, int len, fileHandle_t f );
void      trap_FS_FCloseFile( fileHandle_t f );
int       trap_FS_GetFileList( const char *path, const char *extension, char *listbuf, int bufsize );
int       trap_FS_Seek( fileHandle_t f, long offset, int origin ); // fsOrigin_t
void      trap_SendConsoleCommand( int exec_when, const char *text );
void      trap_Cvar_Register( vmCvar_t *cvar, const char *var_name, const char *value, int flags );
void      trap_Cvar_Update( vmCvar_t *cvar );
void      trap_Cvar_Set( const char *var_name, const char *value );
int       trap_Cvar_VariableIntegerValue( const char *var_name );
float     trap_Cvar_VariableValue( const char *var_name );
void      trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );
void      trap_LocateGameData( gentity_t *gEnts, int numGEntities, int sizeofGEntity_t,
                               playerState_t *gameClients, int sizeofGameClient );
void      trap_DropClient( int clientNum, const char *reason );
void      trap_SendServerCommand( int clientNum, const char *text );
void      trap_SetConfigstring( int num, const char *string );
void      trap_GetConfigstring( int num, char *buffer, int bufferSize );
void      trap_GetUserinfo( int num, char *buffer, int bufferSize );
void      trap_SetUserinfo( int num, const char *buffer );
void      trap_GetServerinfo( char *buffer, int bufferSize );
void      trap_SetBrushModel( gentity_t *ent, const char *name );
void      trap_Trace( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs,
                      const vec3_t end, int passEntityNum, int contentmask );
int       trap_PointContents( const vec3_t point, int passEntityNum );
qboolean  trap_InPVS( const vec3_t p1, const vec3_t p2 );
qboolean  trap_InPVSIgnorePortals( const vec3_t p1, const vec3_t p2 );
void      trap_AdjustAreaPortalState( gentity_t *ent, qboolean open );
qboolean  trap_AreasConnected( int area1, int area2 );
void      trap_LinkEntity( gentity_t *ent );
void      trap_UnlinkEntity( gentity_t *ent );
int       trap_EntitiesInBox( const vec3_t mins, const vec3_t maxs, int *entityList, int maxcount );
qboolean  trap_EntityContact( const vec3_t mins, const vec3_t maxs, const gentity_t *ent );
int       trap_BotAllocateClient( void );
void      trap_BotFreeClient( int clientNum );
void      trap_GetUsercmd( int clientNum, usercmd_t *cmd );
qboolean  trap_GetEntityToken( char *buffer, int bufferSize );

void      trap_SnapVector( float *v );
void      trap_SendGameStat( const char *data );
