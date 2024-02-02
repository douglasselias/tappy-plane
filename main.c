#include "raylib.h"

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

  Texture2D background = LoadTexture("assets/background.png");
  Texture2D ceiling = LoadTexture("assets/ceiling.png");
  Texture2D ground = LoadTexture("assets/ground.png");
  Texture2D obstacle_ground = LoadTexture("assets/rock.png");
  Texture2D obstacle_ceiling = LoadTexture("assets/rockDown.png");
  Texture2D plane1 = LoadTexture("assets/planeYellow1.png");
  Texture2D plane2 = LoadTexture("assets/planeYellow2.png");
  Texture2D plane3 = LoadTexture("assets/planeYellow3.png");

  Texture2D number0 = LoadTexture("assets/number0.png");
  Texture2D number1 = LoadTexture("assets/number1.png");
  Texture2D number2 = LoadTexture("assets/number2.png");
  Texture2D number3 = LoadTexture("assets/number3.png");
  Texture2D number4 = LoadTexture("assets/number4.png");
  Texture2D number5 = LoadTexture("assets/number5.png");
  Texture2D number6 = LoadTexture("assets/number6.png");
  Texture2D number7 = LoadTexture("assets/number7.png");
  Texture2D number8 = LoadTexture("assets/number8.png");
  Texture2D number9 = LoadTexture("assets/number9.png");

  Texture2D tap = LoadTexture("assets/tap.png");
  Texture2D tapTick = LoadTexture("assets/tapTick.png");

  Texture2D gameOverText = LoadTexture("assets/textGameOver.png");
  Texture2D puff = LoadTexture("assets/puff.png");

  InitAudioDevice();
  Sound gameOverSound = LoadSound("assets/game_over.ogg");
  Sound jump_sound = LoadSound("assets/jump.wav");
  Sound score_sound = LoadSound("assets/score.wav");

  Music helicopterSound = LoadMusicStream("assets/helicopter.mp3");
  Music bgm = LoadMusicStream("assets/bgm.ogg");
  PlayMusicStream(bgm);
  PlayMusicStream(helicopterSound);

  bool played_game_over_sound = false;

  float puff_timeout = 0;

  Texture2D number_textures[10] = {
      number0, number1, number2, number3, number4,
      number5, number6, number7, number8, number9,
  };

  Texture2D score_texture = number0;
  Texture2D decimal_score_texture = number1;

  Texture2D plane = plane1;

  float on_screen_ground_x = 0;
  float offscreen_ground_x = ceiling.width;

  float obstacle_x = screen_width;
  float ceiling_obstacle_size = 0.5;
  float ground_obstacle_size = 2;

  Rectangle score_collider = {
      screen_width,
      0,
      1,
      screen_height,
  };
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
  Rectangle top_obstacle_collider = {
      obstacle_x,
      0,
      10,
      obstacle_ceiling.height,
  };
  Rectangle bottom_obstacle_collider = {
      obstacle_x,
      screen_height - (obstacle_ground.height * ground_obstacle_size),
      10,
      obstacle_ground.height,
  };

  int score = 0;
  Vector2 plane_position = {half_screen_width - 170,
                            half_screen_height - (plane.height / 2.0)};
  float plane_rotation = 0;
  float plane_velocity = 0;
  float gravity = 1000;
  float animation_frame = 0;

  Vector2 puff_position = {half_screen_width - 190, 0};

  float animation_frame_tap = 0;
  Texture2D hand = tap;

  SetRandomSeed(GetTime());

  bool scored_last_frame = false;
  bool game_over = false;
  bool show_colliders = false;
  bool is_started = false;

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();
    float impulse = 0;

    if ((IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(0))) {
      if (game_over) {
        played_game_over_sound = false;
        game_over = false;
        is_started = false;
        plane_rotation = 0;
        plane_velocity = 0;
        plane_position.y = half_screen_height - (plane.height / 2.0);
        score = 0;
        obstacle_x = screen_width;
        PlayMusicStream(bgm);
        PlayMusicStream(helicopterSound);
      } else {
        if (!is_started)
          is_started = true;
        puff_timeout = 30;
        impulse = -(gravity * 0.5);
        plane_velocity = impulse;
        PlaySound(jump_sound);

        if (puff_timeout > 0) {
          puff_position.x = half_screen_width - 190;
          puff_position.y = plane_position.y - 30;
        }
      }
    }

    UpdateMusicStream(bgm);
    UpdateMusicStream(helicopterSound);

    if (is_started) {
      plane_velocity += gravity * dt;
      plane_position.y += plane_velocity * dt;
    }

    if (plane_velocity < 0) {
      plane_rotation = -45;
    } else if (plane_velocity > 0) {
      plane_rotation = 45;
    }

    if (is_started)
      obstacle_x -= 3;
    if (obstacle_x < -obstacle_ground.width) {
      obstacle_x = screen_width;
      ceiling_obstacle_size = GetRandomValue(5, 20) / 10.0;
      ground_obstacle_size = (2 - ceiling_obstacle_size) + 0.5;
    }

    top_obstacle_collider.x = obstacle_x + (obstacle_ground.width / 2.0);
    top_obstacle_collider.height =
        obstacle_ceiling.height * ceiling_obstacle_size;

    bottom_obstacle_collider.x = obstacle_x + (obstacle_ground.width / 2.0);
    bottom_obstacle_collider.y =
        screen_height - (obstacle_ground.height * ground_obstacle_size),
    bottom_obstacle_collider.height =
        obstacle_ground.height * ground_obstacle_size;

    Rectangle plane_collider = {plane_position.x - (plane.width / 2.0),
                                plane_position.y - (plane.height / 2.0),
                                plane.width, plane.height};

    if (CheckCollisionRecs(plane_collider, top_obstacle_collider)) {
      game_over = true;
    }
    if (CheckCollisionRecs(plane_collider, bottom_obstacle_collider)) {
      game_over = true;
    }
    if (CheckCollisionRecs(plane_collider, ground_collider)) {
      game_over = true;
    }
    if (CheckCollisionRecs(plane_collider, ceiling_collider)) {
      game_over = true;
    }

    if (game_over && !played_game_over_sound) {
      StopMusicStream(bgm);
      StopMusicStream(helicopterSound);
      PlaySound(gameOverSound);
      played_game_over_sound = true;
    }

    if (CheckCollisionRecs(plane_collider, score_collider)) {
      if (!scored_last_frame && !game_over) {
        PlaySound(score_sound);
        score++;
        scored_last_frame = true;
      }
    } else {
      scored_last_frame = false;
    }

    if (score < 10) {
      score_texture = number_textures[score];
    } else if (score < 100) {
      int decimal = score / 10;
      int rest = score % 10;
      decimal_score_texture = number_textures[decimal];
      score_texture = number_textures[rest];
    }

    score_collider.x = obstacle_x + obstacle_ground.width;

    on_screen_ground_x -= 3;
    if (on_screen_ground_x < -ceiling.width) {
      on_screen_ground_x = ceiling.width;
    }
    offscreen_ground_x -= 3;
    if (offscreen_ground_x < -ceiling.width) {
      offscreen_ground_x = ceiling.width;
    }

    animation_frame++;
    int anim_frames = 2;
    if (animation_frame < anim_frames) {
      plane = plane1;
    } else if (animation_frame < anim_frames * 2) {
      plane = plane2;
    } else if (animation_frame < anim_frames * 3) {
      plane = plane3;
    } else if (animation_frame < anim_frames * 4) {
      plane = plane2;
    } else {
      animation_frame = 0;
    }

    animation_frame_tap++;
    int anim_tap_frames = 40;
    bool is_tap_tick = animation_frame_tap < anim_tap_frames;
    if (is_tap_tick) {
      hand = tapTick;
    } else if (animation_frame_tap < anim_tap_frames * 2) {
      hand = tap;
    } else {
      animation_frame_tap = 0;
    }

    if (puff_timeout > 0) {
      puff_timeout--;
      puff_position.x--;
      puff_position.y--;
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    draw_single_texture(background,
                        (Rectangle){0, 0, screen_width, screen_height});

    draw_single_texture(
        obstacle_ceiling,
        (Rectangle){obstacle_x, 1 * ceiling_obstacle_size,
                    obstacle_ceiling.width,
                    obstacle_ceiling.height * ceiling_obstacle_size});

    draw_single_texture(
        obstacle_ground,
        (Rectangle){obstacle_x,
                    screen_height -
                        obstacle_ground.height * ground_obstacle_size,
                    obstacle_ground.width,
                    obstacle_ground.height * ground_obstacle_size});

    draw_single_texture(ground, (Rectangle){on_screen_ground_x,
                                            screen_height - ground.height + 20,
                                            ground.width + 2, ground.height});
    draw_single_texture(ceiling, (Rectangle){on_screen_ground_x, -20,
                                             ground.width + 2, ground.height});

    draw_single_texture(ground, (Rectangle){offscreen_ground_x,
                                            screen_height - ground.height + 20,
                                            ground.width + 2, ground.height});
    draw_single_texture(ceiling, (Rectangle){offscreen_ground_x, -20,
                                             ground.width + 2, ground.height});

    if (score < 10) {
      DrawTexture(score_texture, half_screen_width, 50, WHITE);
    } else if (score < 100) {
      DrawTexture(decimal_score_texture,
                  half_screen_width - (decimal_score_texture.width / 2.0) + 6,
                  50, WHITE);
      DrawTexture(score_texture,
                  half_screen_width + (score_texture.width / 2.0) + 6, 50,
                  WHITE);
    }

    if (puff_timeout > 0) {
      DrawTexturePro(puff, (Rectangle){0, 0, puff.width, puff.height},
                     (Rectangle){puff_position.x, puff_position.y, puff.width,
                                 puff.height},
                     (Vector2){puff.width / 2.0, puff.height / 2.0}, -45,
                     WHITE);
    }

    DrawTexturePro(plane, (Rectangle){0, 0, plane.width, plane.height},
                   (Rectangle){plane_position.x, plane_position.y, plane.width,
                               plane.height},
                   (Vector2){(float)plane.width / 2, (float)plane.height / 2},
                   plane_rotation, WHITE);

    if (!is_started) {
      float pos_x = plane_position.x + (plane.width / 2.0) + 10;
      float pos_y = plane_position.y - (hand.height / 2.0) + 5;
      if (!is_tap_tick) {
        pos_x += 4;
        pos_y += 4;
      }
      DrawTexture(hand, pos_x, pos_y, WHITE);
    }

    if (show_colliders) {
      DrawRectangleLinesEx(plane_collider, 3, GOLD);
      DrawRectangleLinesEx(score_collider, 3, RED);
      DrawRectangleLinesEx(ceiling_collider, 3, LIME);
      DrawRectangleLinesEx(ground_collider, 3, MAGENTA);
      DrawRectangleLinesEx(top_obstacle_collider, 3, RED);
      DrawRectangleLinesEx(bottom_obstacle_collider, 3, ORANGE);
    }

    if (game_over) {
      DrawTexture(gameOverText,
                  (screen_width / 2.0) - (gameOverText.width / 2.0),
                  (screen_height / 2.0) - (gameOverText.height / 2.0), WHITE);
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}