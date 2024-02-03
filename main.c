#include "raylib.h"
#include <stdio.h>

typedef struct {
  Texture2D texture;
  Vector2 position;
  float velocity;
  float rotation;
  Music helix;
  Sound jump;
  int animation_frame;
} Plane;

typedef struct {
  Texture2D texture;
  float scale;
} Obstacle;

typedef struct {
  int value;
  Texture2D texture_ones;
  Texture2D texture_tens;
  Sound sound;
  Rectangle collider;
} Score;

typedef struct {
  Texture2D texture;
  Vector2 position;
  int animation_frame;
} Hand;

typedef struct {
  Texture2D texture;
  Sound sound;
} GameOver;

typedef struct {
  Texture2D texture;
  Vector2 position;
  int timeout;
} Puff;
void draw_single_texture(Texture2D texture, Rectangle dest) {
  DrawTexturePro(texture, (Rectangle){0, 0, texture.width, texture.height},
                 dest, (Vector2){0, 0}, 0, WHITE);
}

int main() {
  SetTraceLogLevel(LOG_WARNING);
  int screen_width = 470;
  int screen_height = 800;
  int half_screen_width = screen_width / 2;
  int half_screen_height = screen_height / 2;
  InitWindow(screen_width, screen_height, "Tappy Plane");
  SetTargetFPS(60);
  SetRandomSeed(GetTime());
  InitAudioDevice();

  Texture2D plane_textures[3] = {};
  for (int i = 0; i < 3; i++) {
    char filename[18];
    sprintf(filename, "assets/plane%d.png", i);
    plane_textures[i] = LoadTexture(filename);
  }

  Texture2D number_textures[10] = {};
  for (int i = 0; i < 10; i++) {
    char filename[19];
    sprintf(filename, "assets/number%d.png", i);
    number_textures[i] = LoadTexture(filename);
  }

  Texture2D background = LoadTexture("assets/background.png");
  Music bgm = LoadMusicStream("assets/bgm.ogg");
  SetMusicVolume(bgm, 0.5);
  PlayMusicStream(bgm);

  Texture2D ceiling = LoadTexture("assets/ceiling.png");
  Texture2D ground = LoadTexture("assets/ground.png");
  float on_screen_ground_x = 0;
  float off_screen_ground_x = ceiling.width;
  Rectangle ground_collider = {
      0,
      screen_height - ground.height,
      screen_width,
      ground.height,
  };
  Rectangle ceiling_collider = {
      0,
      0,
      screen_width,
      ground.height,
  };

  float gravity = 1000;
  bool show_colliders = false;
  /// @todo: ugly hack!
  bool scored_last_frame = false;
  /// @end_todo: ugly hack!

  /// @todo: add a button to click for restarting

  GameOver game_over = {
      .texture = LoadTexture("assets/textGameOver.png"),
      .sound = LoadSound("assets/game_over.ogg"),
  };

  bool is_game_over = false;
  bool is_started = false;

  Puff puff = {
      .texture = LoadTexture("assets/puff.png"),
      .position = {half_screen_width - 190, 0},
      .timeout = 0,
  };

  Texture2D hand_texture = LoadTexture("assets/tap.png");
  Texture2D hand_texture_action = LoadTexture("assets/tapTick.png");
  Hand hand = {
      .texture = hand_texture,
      .position = {0, 0},
      .animation_frame = 0,
  };

  Plane plane = {
      .texture = plane_textures[0],
      .position = {half_screen_width - 170,
                   half_screen_height - (plane_textures[0].height / 2.0)},
      .velocity = 0,
      .rotation = 0,
      .helix = LoadMusicStream("assets/helicopter.mp3"),
      .jump = LoadSound("assets/jump.wav"),
      .animation_frame = 0,
  };
  PlayMusicStream(plane.helix);

  Score score = {
      .value = 0,
      .texture_ones = number_textures[0],
      .texture_tens = number_textures[1],
      .sound = LoadSound("assets/score.wav"),
      .collider =
          {
              screen_width,
              0,
              1,
              screen_height,
          },
  };

  float obstacle_pos_x = screen_width;
  Obstacle top_obstacle = {
      .texture = LoadTexture("assets/rockDown.png"),
      .scale = GetRandomValue(5, 20) / 10.0,
  };
  Obstacle bottom_obstacle = {
      .texture = LoadTexture("assets/rock.png"),
      .scale = (2 - top_obstacle.scale) + 0.5,
  };

  Rectangle top_obstacle_collider = {
      obstacle_pos_x,
      0,
      10,
      top_obstacle.texture.height,
  };
  Rectangle bottom_obstacle_collider = {
      obstacle_pos_x,
      screen_height - (bottom_obstacle.texture.height * bottom_obstacle.scale),
      10,
      bottom_obstacle.texture.height,
  };

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    if (IsMouseButtonPressed(0)) {
      if (is_game_over) {
        is_game_over = false;
        is_started = false;
        plane.rotation = 0;
        plane.velocity = 0;
        plane.position.y = half_screen_height - (plane.texture.height / 2.0);
        score.value = 0;
        obstacle_pos_x = screen_width;
        // score.collider.x = screen_width;
        PlayMusicStream(bgm);
        PlayMusicStream(plane.helix);
      } else {
        if (!is_started)
          is_started = true;

        plane.velocity = -(gravity * 0.5);
        PlaySound(plane.jump);

        /// @todo: hmmm
        puff.timeout = 30;
        if (puff.timeout > 0) {
          puff.position.x = half_screen_width - 190;
          puff.position.y = plane.position.y - 30;
        }
      }
    }

    UpdateMusicStream(bgm);
    UpdateMusicStream(plane.helix);

    if (is_started) {
      plane.velocity += gravity * dt;
      plane.position.y += plane.velocity * dt;
    }

    if (plane.velocity < 0) {
      plane.rotation = -45;
    } else if (plane.velocity > 0) {
      plane.rotation = 45;
    }

    if (is_started)
      obstacle_pos_x -= 3;
    if (obstacle_pos_x < -bottom_obstacle.texture.width) {
      obstacle_pos_x = screen_width;
      top_obstacle.scale = GetRandomValue(5, 20) / 10.0;
      bottom_obstacle.scale = (2 - top_obstacle.scale) + 0.5;
    }

    top_obstacle_collider.x =
        obstacle_pos_x + (bottom_obstacle.texture.width / 2.0);
    top_obstacle_collider.height =
        top_obstacle.texture.height * top_obstacle.scale;

    bottom_obstacle_collider.x =
        obstacle_pos_x + (bottom_obstacle.texture.width / 2.0);
    bottom_obstacle_collider.y =
        screen_height -
        (bottom_obstacle.texture.height * bottom_obstacle.scale),
    bottom_obstacle_collider.height =
        bottom_obstacle.texture.height * bottom_obstacle.scale;

    Rectangle plane_collider = {plane.position.x - (plane.texture.width / 2.0),
                                plane.position.y - (plane.texture.height / 2.0),
                                plane.texture.width, plane.texture.height};

    if (!is_game_over) {
      if (CheckCollisionRecs(plane_collider, top_obstacle_collider) ||
          CheckCollisionRecs(plane_collider, bottom_obstacle_collider) ||
          CheckCollisionRecs(plane_collider, ground_collider) ||
          CheckCollisionRecs(plane_collider, ceiling_collider)) {
        is_game_over = true;
        StopMusicStream(bgm);
        StopMusicStream(plane.helix);
        PlaySound(game_over.sound);
      }
    }

    score.collider.x = obstacle_pos_x + bottom_obstacle.texture.width;

    if (CheckCollisionRecs(plane_collider, score.collider)) {
      if (!scored_last_frame && !is_game_over) {
        PlaySound(score.sound);
        score.value++;
        scored_last_frame = true;
      }
    } else {
      scored_last_frame = false;
    }

    if (score.value < 10) {
      score.texture_ones = number_textures[score.value];
    } else if (score.value < 100) {
      score.texture_tens = number_textures[score.value / 10];
      score.texture_ones = number_textures[score.value % 10];
    }

    on_screen_ground_x -= 3;
    if (on_screen_ground_x < -ceiling.width) {
      on_screen_ground_x = ceiling.width;
    }
    off_screen_ground_x -= 3;
    if (off_screen_ground_x < -ceiling.width) {
      off_screen_ground_x = ceiling.width;
    }

    plane.animation_frame++;
    int anim_frames = 2;
    if (plane.animation_frame < anim_frames) {
      plane.texture = plane_textures[0];
    } else if (plane.animation_frame < anim_frames * 2) {
      plane.texture = plane_textures[1];
    } else if (plane.animation_frame < anim_frames * 3) {
      plane.texture = plane_textures[2];
    } else if (plane.animation_frame < anim_frames * 4) {
      plane.texture = plane_textures[1];
    } else {
      plane.animation_frame = 0;
    }

    hand.animation_frame++;
    int anim_tap_frames = 40;
    bool is_hand_action = hand.animation_frame < anim_tap_frames;
    if (is_hand_action) {
      hand.texture = hand_texture_action;
    } else if (hand.animation_frame < anim_tap_frames * 2) {
      hand.texture = hand_texture;
    } else {
      hand.animation_frame = 0;
    }

    if (puff.timeout > 0) {
      puff.timeout--;
      puff.position.x--;
      puff.position.y--;
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_single_texture(background,
                        (Rectangle){0, 0, screen_width, screen_height});

    draw_single_texture(
        top_obstacle.texture,
        (Rectangle){obstacle_pos_x, 1 * top_obstacle.scale,
                    top_obstacle.texture.width,
                    top_obstacle.texture.height * top_obstacle.scale});

    draw_single_texture(
        bottom_obstacle.texture,
        (Rectangle){obstacle_pos_x,
                    screen_height -
                        bottom_obstacle.texture.height * bottom_obstacle.scale,
                    bottom_obstacle.texture.width,
                    bottom_obstacle.texture.height * bottom_obstacle.scale});

    draw_single_texture(ground, (Rectangle){on_screen_ground_x,
                                            screen_height - ground.height + 20,
                                            ground.width + 2, ground.height});
    draw_single_texture(ceiling, (Rectangle){on_screen_ground_x, -20,
                                             ground.width + 2, ground.height});

    draw_single_texture(ground, (Rectangle){off_screen_ground_x,
                                            screen_height - ground.height + 20,
                                            ground.width + 2, ground.height});
    draw_single_texture(ceiling, (Rectangle){off_screen_ground_x, -20,
                                             ground.width + 2, ground.height});

    if (score.value < 10) {
      DrawTexture(score.texture_ones, half_screen_width, 50, WHITE);
    } else if (score.value < 100) {
      DrawTexture(score.texture_tens,
                  half_screen_width - (score.texture_tens.width / 2.0) + 6, 50,
                  WHITE);
      DrawTexture(score.texture_ones,
                  half_screen_width + (score.texture_ones.width / 2.0) + 6, 50,
                  WHITE);
    }

    if (puff.timeout > 0) {
      DrawTexturePro(
          puff.texture,
          (Rectangle){0, 0, puff.texture.width, puff.texture.height},
          (Rectangle){puff.position.x, puff.position.y, puff.texture.width,
                      puff.texture.height},
          (Vector2){puff.texture.width / 2.0, puff.texture.height / 2.0}, -45,
          WHITE);
    }

    DrawTexturePro(
        plane.texture,
        (Rectangle){0, 0, plane.texture.width, plane.texture.height},
        (Rectangle){plane.position.x, plane.position.y, plane.texture.width,
                    plane.texture.height},
        (Vector2){plane.texture.width / 2.0, plane.texture.height / 2.0},
        plane.rotation, WHITE);

    if (!is_started) {
      float pos_x = plane.position.x + (plane.texture.width / 2.0) + 10;
      float pos_y = plane.position.y - (hand.texture.height / 2.0) + 5;
      if (!is_hand_action) {
        pos_x += 4;
        pos_y += 4;
      }
      DrawTexture(hand.texture, pos_x, pos_y, WHITE);
    }

    if (show_colliders) {
      int thickness = 3;
      Color color = MAGENTA;
      DrawRectangleLinesEx(plane_collider, thickness, color);
      DrawRectangleLinesEx(score.collider, thickness, color);
      DrawRectangleLinesEx(ceiling_collider, thickness, color);
      DrawRectangleLinesEx(ground_collider, thickness, color);
      DrawRectangleLinesEx(top_obstacle_collider, thickness, color);
      DrawRectangleLinesEx(bottom_obstacle_collider, thickness, color);
    }

    if (is_game_over) {
      DrawTexture(game_over.texture,
                  half_screen_width - (game_over.texture.width / 2.0),
                  half_screen_height - (game_over.texture.height / 2.0), WHITE);
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}