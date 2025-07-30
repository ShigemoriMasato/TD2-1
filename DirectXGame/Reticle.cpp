#include "Reticle.h"
#include "Player.h"
#include "externals/imgui/imgui.h"

using namespace MyMath;

Reticle::Reticle(Player* player) {
	player_ = player;
}

void Reticle::GetTarget(std::list<Enemy*> enemies) {
	//Playerの位置を取得
	Vector3 ppos = player_->GetTransform().position;

	ImGui::Begin("Reticle");
	ImGui::Text("Player: %.2f", ppos.z);

	for (auto enemy : enemies) {
		Vector3 epos = enemy->GetTransform().position;

		ImGui::Text("%.2f", epos.z);

		if (epos.z >= ppos.z) {
			player_->SetBulletTargetPosition(enemy->GetPositionPtr());
		}

	}

	ImGui::End();
}
