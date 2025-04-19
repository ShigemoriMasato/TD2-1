#pragma once

struct Matrix4x4 final {
	float m[4][4];
};

Matrix4x4 MakeIdentity4x4();
