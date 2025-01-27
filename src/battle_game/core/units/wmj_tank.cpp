#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"
#include "wmj_tank.h"

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
int tank_number = 1;
}  // namespace

wmjTank::wmjTank(GameCore *game_core, uint32_t id, uint32_t player_id)
    :Tank(game_core, id, player_id) {
  if (!~tank_body_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      /* Tank Body */
      tank_body_model_index = mgr->RegisterModel(
          {{{-0.8f, 0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 0.0f, 1.0f}},
           {{-0.8f, -0.8f}, {0.0f, 0.0f}, {0.0f, 1.0f, 1.0f, 1.0f}},
           {{0.8f, 0.8f}, {0.0f, 0.0f}, {1.0f, 0.0f, 1.0f, 1.0f}},
           {{0.8f, -0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
           {{0.0f, 1.1f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}},
          {0, 1, 2, 1, 2, 3,0 , 2, 4});
    }

    {
      /* Tank Turret */
      std::vector<ObjectVertex> turret_vertices;
      std::vector<uint32_t> turret_indices;
      const int precision = 60;
      const float inv_precision = 1.0f / float(precision);
      for (int i = 0; i < precision; i++) {
        auto theta = (float(i) + 0.5f) * inv_precision;
        theta *= glm::pi<float>() * 2.0f;
        auto sin_theta = std::sin(theta);
        auto cos_theta = std::cos(theta);
        turret_vertices.push_back({{sin_theta * 0.5f, cos_theta * 0.5f},
                                   {0.0f, 0.0f},
                                   {0.7f, 0.7f, 0.7f, 1.0f}});
        turret_indices.push_back(i);
        turret_indices.push_back((i + 1) % precision);
        turret_indices.push_back(precision);
      }
      turret_vertices.push_back(
          {{0.0f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{-0.1f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{0.1f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{-0.1f, 1.2f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{0.1f, 1.2f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_indices.push_back(precision + 1 + 0);
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 3);
      tank_turret_model_index =
          mgr->RegisterModel(turret_vertices, turret_indices);
    }
  }
}

void wmjTank::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  //battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  if (tank_number == 1) {
    battle_game::SetColor(glm::vec4{1.0f, 0.0f, 0.0f, 0.5f});
  }
  if (tank_number == 2) {
    battle_game::SetColor(glm::vec4{1.0f, 0.8f, 0.0f, 0.5f});
  }
  if (tank_number == 3) {
    battle_game::SetColor(glm::vec4{0.5f, 0.0f, 0.5f, 0.5f});
  }
  if (tank_number == 4) {
    battle_game::SetColor(glm::vec4{0.0f, 0.5f, 0.3f, 0.5f});
  }
  battle_game::DrawModel(tank_body_model_index);
  battle_game::SetRotation(turret_rotation_);
  battle_game::DrawModel(tank_turret_model_index);
}

 void wmjTank::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
}
 

void wmjTank::Fire() {
  if (fire_count_down_) {
    fire_count_down_--;
  } else {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT] &&tank_number ==1) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({1.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({-1.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({-0.5f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        GenerateBullet<bullet::CannonBall>(
            position_ + Rotate({+0.5f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = kTickPerSecond;  // Fire interval 1 second
      }
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT] &&
          tank_number == 2) {
        auto velocity = Rotate(glm::vec2{0.0f, 10.0f}, turret_rotation_);
        GenerateBullet<bullet::GoldBullet>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = kTickPerSecond; 
      }
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT] &&
          tank_number == 4) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::Laser>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = kTickPerSecond/10;
      }
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT] &&
          tank_number == 3) {
        auto velocity = Rotate(glm::vec2{0.0f, 5.0f}, turret_rotation_);
        GenerateBullet<bullet::DFRocket>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity);
        fire_count_down_ = kTickPerSecond ;
      }
    }
  }
  auto wmj_player = game_core_->GetPlayer(player_id_);
  auto &input_data = wmj_player->GetInputData();
  if (input_data.key_down[GLFW_KEY_1]) {
    tank_number = 1;
  }
  if (input_data.key_down[GLFW_KEY_2]) {
    tank_number = 2;
  }
  if (input_data.key_down[GLFW_KEY_3]) {
    tank_number = 3;
  }
  if (input_data.key_down[GLFW_KEY_4]) {
    tank_number = 4;
  }
}


const char *wmjTank::UnitName() const {
  return "wmj_Tank";
}

const char *wmjTank::Author() const {
  return "wmj";
}
}  // namespace battle_game::unit
