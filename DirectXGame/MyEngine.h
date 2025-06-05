#pragma once

class MyDirectX;
class Audio;
class Input;

class MyEngine {
public:

	void operator=(const MyEngine&) = delete;
	MyEngine(const MyEngine&) = delete;

	static void Initialize(const float kWindowWidth, const float kWindowHeight);

	static void BeginFrame();

	static void PreDraw();

	static void PostDraw();

	

private:

	MyEngine();

	static MyDirectX* myDirectX;
	static Audio* audio;
	static Input* input;

	static bool isColledPreDraw;
};
