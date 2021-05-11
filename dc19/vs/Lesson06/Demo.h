#ifndef DEMO_H
#define DEMO_H


#include <SOIL/SOIL.h>
#include <SDL/SDL_mixer.h>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <map>


#include "Game.h"

#define NUM_FRAMES 6
#define FRAME_DUR 80
#define NUM_BUTTON 2

using namespace glm;

class Demo :
	public Engine::Game
{
public:
	Demo();
	~Demo();
	virtual void Init();
	virtual void Update(float deltaTime);
	virtual void Render();
	void InitAudio();
	
	float widthJack = 0, heightJack = 0;
	float widthEnemy = 0, heightEnemy = 0;
	float widthAttack = 0, heightAttack = 0;
	float widthAttackVirus = 0, heightAttackVirus = 0;

	float healthPoint = 3;
	float EnemiesLeft = 1;
	int countAtkVirus = 1;
private:
	float xCurPos = (GetScreenWidth() - widthJack) / GetScreenWidth(); 
	float yCurPos = (GetScreenHeight() - heightJack) / 2;
	
	float frame_dur = 0, yVelocity = 0;
	bool jackIdle = true, jackUp = false, jackDown = false, jackShoot = false, jackhurt = false;
	float xpos = 0, ypos = 0;
	float xposUp = 0, yposUp = 0;
	float xposDown = 0, yposDown = 0;
	float xposAtk = 0, yposAtk = 0;
	float xposHurt = 0, yposHurt = 0;
	GLuint VBO, VAO, EBO, texture, program; 
	GLuint VBOUp, VAOUp, EBOUp, textureUp, programUp;
	GLuint VBODown, VAODown, EBODown, textureDown, programDown;
	GLuint VBOAtk, VAOAtk, EBOAtk, textureAtk, programAtk;
	GLuint VBOHurt, VAOHurt, EBOHurt, textureHurt, programHurt;
	unsigned int frame_idx = 0, flip = 0;
	void BuildJack();
	void BuildJackUp();
	void BuildJackDown();
	void BuildJackAtk();
	void BuildJackHurt();
	void DrawJack();
	void DrawJackUp();
	void DrawJackDown();	
	void DrawJackAtk();
	void DrawJackHurt();
	void ControlJack(float deltaTime);
	bool IsShoot(bool check);
	void UpdatePlayerSpriteAnim(float deltaTime);


	// ------------------------------------------------------------Character Virus
	bool shoot = false;
	float frame_durEnemy = 0, oldxposEnemy = 0, xposEnemy = 0, yposEnemy = 0, gravityEnemy = 0, xVelocityEnemy = 0, yVelocityEnemy = 0, yposGroundEnemy = 0;
	float frame_durAtkVirus = 0, oldxposAtkVirus = 0, xposAtkVirus = 0, yposAtkVirus = 0, gravityAtkVirus = 0, xVelocityAtkVirus = 0, yVelocityAtkVirus = 0, yposGroundAtkVirus = 0;
	GLuint VBOEnemy, VAOEnemy, EBOEnemy, textureEnemy, programEnemy;
	GLuint VBOAtkVirus, VAOAtkVirus, EBOAtkVirus, textureAtkVirus, programAtkVirus;
	bool onGroundEnemy = true;
	bool enemy_idle = true;
	unsigned int frame_idxEnemy = 0, flipEnemy = 0;
	unsigned int frame_idxAtkVirus = 0, flipAtkVirus = 0;
	
	void BuildEnemy();
	void DrawEnemy();
	void UpdateEnemy(float deltaTime);
	void BuildAtkVirus();
	void DrawAtkVirus();
	void UpdateAtkVirus(float deltaTime);


	// ------------------------------------------------------------Shoot
	float frame_durAttack = 0, oldxposAttack = 0, xposAttack = 0, yposAttack = 0, gravityAttack = 0, xVelocityAttack = 0, yVelocityAttack = 0, yposGroundAttack = 0;
	GLuint VBOAttack, VAOAttack, EBOAttack, textureAttack, programAttack;
	unsigned int frame_idxAttack = 0, flipAttack = 0;
	void BuildAttackfMC();
	void DrawAttackfMC();
	void UpdateAttackfMC(float deltaTime);
	void DeleteEnemy();


	// ------------------------------------------------------------HealthPoint
	float widthHP = 0, heightHP = 0, xposHPFull = 0, yposHPFull = 0;
	float xposHPMin1 = 0, yposHPMin1 = 0;
	float xposHPMin2 = 0, yposHPMin2 = 0;
	float xposHPEmpty = 0, yposHPEmpty = 0;
	GLuint VBOHPFull, VAOHPFull, EBOHPFull, textureHPFull, programHPFull;
	GLuint VBOHPMin1, VAOHPMin1, EBOHPMin1, textureHPMin1, programHPMin1;
	GLuint VBOHPMin2, VAOHPMin2, EBOHPMin2, textureHPMin2, programHPMin2;
	GLuint VBOHPEmpty, VAOHPEmpty, EBOHPEmpty, textureHPEmpty, programHPEmpty;
	void BuildHPFull();
	void BuildHPMin1();
	void BuildHPMin2();
	void BuildHPEmpty();
	void DrawHPFull();
	void DrawHPMin1();
	void DrawHPMin2();
	void DrawHPEmpty();


	// ------------------------------------------------------------BG
	float widthBG = 0, heightBG = 0, xposBG = 0, yposBG = 0;
	GLuint VBOBG, VAOBG, EBOBG, textureBG, programBG;
	void BuildBG();
	void DrawBG();
	float widthStartBG = 0, heightStartBG = 0, xposStartBG = 0, yposStartBG = 0;
	GLuint VBOStartBG, VAOStartBG, EBOStartBG, textureStartBG, programStartBG;
	void BuildStartBG();
	void DrawStartBG();
	float widthEndBG = 0, heightEndBG = 0, xposEndBG = 0, yposEndBG = 0;
	GLuint VBOEndBG, VAOEndBG, EBOEndBG, textureEndBG, programEndBG;
	void BuildEndBG();
	void DrawEndBG();

	
	// ------------------------------------------------------------Game
	bool menuStart = true;
	bool theGame = false;
	bool endGame = false;
	bool OnTarget(float x, float y);
	bool Hurt(float x1, float y1, float y2);
	bool CheckWin(float EnemiesLeft, int healthPpoint);
	int tanda = 0;


	// ------------------------------------------------------------Musics
	Mix_Music *bgm = NULL;
	Mix_Chunk* up = NULL;
	Mix_Chunk* down = NULL;
	Mix_Chunk* Theshoot = NULL;
	Mix_Chunk* gameOver = NULL;
	Mix_Chunk* hurts = NULL;
	Mix_Chunk* option = NULL;
	Mix_Chunk* virusAttack = NULL;
	Mix_Chunk* winGame = NULL;

	int sfx_up = -1;
	int sfx_down = -1;
	int sfx_Theshoot = -1;
	int sfx_gameOver = -1;
	int sfx_hurts = -1;
	int sfx_option = -1;
	int sfx_virusAttack = -1;
	int sfx_winGame = -1;


	// ------------------------------------------------------------ Game Option
	void buttonMenu();
	void renderButtonMenu();
	GLuint textureOption[NUM_BUTTON], hover_textureOption[NUM_BUTTON], VBOplay, VBOquit, VAOplay, VAOquit, programOption;
	float button_widthOption[NUM_BUTTON], button_heightOption[NUM_BUTTON], hover_button_widthOption[NUM_BUTTON], hover_button_heightOption[NUM_BUTTON];
	int activeButtonIndex = 0;

	void buttonGameOver();
	void renderGameOver();
	GLuint textureGameOver[NUM_BUTTON], hover_GameOver[NUM_BUTTON], VBOrestart, VBOquit2, VAOrestart, VAOquit2, programGameOver;
	float button_widthGameOver[NUM_BUTTON], button_heightGameOver[NUM_BUTTON], hover_button_widthGameOver[NUM_BUTTON], hover_button_heightGameOver[NUM_BUTTON];
	int activeButtonGameOver = 0;


	void BuildMC();
	void DrawMC();
	void BuildTheVirus();
	void DrawTheVirus();
	void BuildHealthPoint();
	void DrawHealthPoint();
	void SFX();
	void deInitAudio();
	void changeHP();
};
#endif

