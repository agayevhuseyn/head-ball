#include "game.h"
#include "macros.h"
#include "config.h"

#define JMP_FORCE -950
#define SPEED 400

void init_game(Game* game)
{
  init_ball(&game->ball, vec2(WIDTH / 2.0, 0), 32);
  init_player(&game->players[0], PLAYER_LEFT_SIDE,  vec2(100, 300), vec2(0, 0), 64, SPEED, JMP_FORCE);
  init_player(&game->players[1], PLAYER_RIGHT_SIDE, vec2(1180, 300), vec2(0, 0), 64, SPEED, JMP_FORCE);
}

void draw_game(Game* game)
{
  draw_player(&game->players[0]);
  draw_player(&game->players[1]);
  draw_ball(&game->ball);
}

void update_game(Game* game, float dt)
{
  update_player(&game->players[0], dt);
  update_player(&game->players[1], dt);
  update_ball(&game->ball, dt, game->players);
}
