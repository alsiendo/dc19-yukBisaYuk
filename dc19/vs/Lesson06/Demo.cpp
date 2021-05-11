#include "Demo.h"
Demo::Demo()
{
}
Demo::~Demo()
{
	deInitAudio();
}


//MAIN
void Demo::Init()
{
	InitAudio();
	BuildBG();
	BuildStartBG();
	BuildEndBG();
	BuildMC();
	BuildTheVirus();
	BuildHealthPoint();
	buttonMenu();
	buttonGameOver();
}
void Demo::Update(float deltaTime)
{
	if (theGame == true) {
		//Play BGM
		Mix_PlayMusic(bgm, -1);

		//Check The Game
		if (CheckWin(EnemiesLeft, healthPoint)) {
			yposAtkVirus = 1000;
			xposAttack = 1000;
			yposAttack = 1000;
			endGame = true;
			menuStart = false;
			theGame = false;
		}
		changeHP();

		//Jack
		ControlJack(deltaTime);
		UpdatePlayerSpriteAnim(deltaTime);

		//Enemy
		UpdateEnemy(deltaTime);
		UpdateAttackfMC(deltaTime);

		//Atk from Enemy
		UpdateAtkVirus(deltaTime);

		SFX();

	}
	// Pilih Button di Awal Game
	if (IsKeyDown("ChooseTheButton")) {
		if (activeButtonIndex == 0 && tanda == 0) {
			theGame = true;
			menuStart = false;
			endGame = false;
			if (tanda == 0) tanda = 1;
			else tanda = 0;
		}
		else if (activeButtonGameOver == 0 && tanda == 1) {
			healthPoint = 3;
			EnemiesLeft = 1;
			xposEnemy = (GetScreenWidth() - widthEnemy);
			yposEnemy = (GetScreenHeight() - heightEnemy) / 2;
			xposAtkVirus = -1000;
			yposAtkVirus = yposEnemy;
			theGame = true;
			menuStart = false;
			endGame = false;
			DrawBG();
			DrawMC();
			DrawTheVirus();
			DrawHealthPoint();
			if (tanda == 0) tanda = 1;
			else tanda = 0;
		}else if (activeButtonIndex == 1 || activeButtonGameOver == 1) {
			SDL_Quit();
			exit(0);
		}
	}
	if (IsKeyUp("Move Down")) {
		if (activeButtonIndex < NUM_BUTTON - 1) {
			activeButtonIndex++;
			activeButtonGameOver++;
			SDL_Delay(150);
		}
	}
	if (IsKeyUp("Move Up")) {
		if (activeButtonIndex > 0) {
			activeButtonIndex--;
			activeButtonGameOver--;
			SDL_Delay(150);
		}
	}
}
void Demo::Render()
{
	glViewport(0, 0, GetScreenWidth(), GetScreenHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	
	if (theGame == true) {
		DrawBG();
		DrawMC();
		DrawTheVirus();
		DrawHealthPoint();
	}
	else if (menuStart == true) {
		mat4 projection;
		projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
		glUniformMatrix4fv(glGetUniformLocation(this->programOption, "projection"), 1, GL_FALSE, value_ptr(projection));
		DrawStartBG();
		renderButtonMenu();
	}
	else if (endGame == true) {
		mat4 projection;
		projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
		glUniformMatrix4fv(glGetUniformLocation(this->programGameOver, "projection"), 1, GL_FALSE, value_ptr(projection));
		DrawEndBG();
		renderGameOver();
	}
}


//JACK
void Demo::UpdatePlayerSpriteAnim(float deltaTime)
{
	frame_dur += deltaTime;

	// Mengatur kecepatan perpindahan antar frame
	if (frame_dur > FRAME_DUR) {
		frame_dur = 0;

		if (frame_idx == NUM_FRAMES - 1) frame_idx = 0;  else frame_idx++;

		UseShader(this->program);
		glUniform1i(glGetUniformLocation(this->program, "frameIndex"), frame_idx);

		UseShader(this->programUp);
		glUniform1i(glGetUniformLocation(this->programUp, "frameIndex"), frame_idx);

		UseShader(this->programDown);
		glUniform1i(glGetUniformLocation(this->programDown, "frameIndex"), frame_idx);

		UseShader(this->programAtk);
		glUniform1i(glGetUniformLocation(this->programAtk, "frameIndex"), frame_idx);

		UseShader(this->programHurt);
		glUniform1i(glGetUniformLocation(this->programHurt, "frameIndex"), frame_idx);
	}

	// Mengatur animasi yang dipakai (Switching)
	if (IsShoot(shoot)) {
		yposAttack = ypos;
		xposAttack = xpos;
		
		xposAtk = xCurPos;

		xpos = 1000;
		xposDown = 1000;
		xposUp = 1000;
		xposHurt = 1000;

		jackShoot = false;
		shoot = false;
	}
	if (jackUp) {
		xposUp = xCurPos;

		xpos = 1000;
		xposDown = 1000;
		xposAtk = 1000;
		xposHurt = 1000;
	}
	if (jackIdle) {
		xpos = xCurPos;

		xposUp = 1000;
		xposDown = 1000;
		xposAtk = 1000;
		xposHurt = 1000;
	}
	if (jackDown) {
		xposDown = xCurPos;

		xpos = 1000;
		xposUp = 1000;
		xposAtk = 1000;
		xposHurt = 1000;
	}


}
void Demo::UpdateAttackfMC(float deltaTime)
{
	frame_durAttack += deltaTime;

	if (frame_durAttack > FRAME_DUR) {
		frame_durAttack = 0;

		if (frame_idxAttack == NUM_FRAMES - 1) frame_idxAttack = 0;  else frame_idxAttack++;

		UseShader(this->programAttack);
		glUniform1i(glGetUniformLocation(this->programAttack, "frameIndex"), frame_idxAttack);
	}

	if (xposAttack < GetScreenWidth() + 100) {
		xposAttack += 3;
	}

	if (OnTarget(xposAttack, yposAttack)) {
		DeleteEnemy();
		EnemiesLeft -= 1;
	}

	if (Hurt(xposAtkVirus, yposAtkVirus, ypos)) {
		healthPoint -= 1;
	}
}

void Demo::ControlJack(float deltaTime)
{
	// State
	jackIdle = true; jackUp = false; jackDown = false; jackShoot = false; jackhurt = false;

	if (((IsKeyDown("Move Up")) && (ypos > -15)) ) {
		jackUp = true;
		ypos -= deltaTime * yVelocity;yposUp -= deltaTime * yVelocity; yposDown -= deltaTime * yVelocity; yposAtk -= deltaTime * yVelocity;yposHurt -= deltaTime * yVelocity;
		
		jackIdle = false;
		jackDown = false;
	}

	if (((IsKeyDown("Move Down")) && (ypos < 480))) {
		jackDown = true;
		ypos += deltaTime * yVelocity;yposUp += deltaTime * yVelocity; yposDown += deltaTime * yVelocity;yposAtk += deltaTime * yVelocity;yposHurt += deltaTime * yVelocity;
		
		jackIdle = false;
		jackUp = false;
	}

	if (IsKeyDown("Shoot") && (xposAttack > GetScreenWidth() + 100)) {
		// Hanya bisa ditekan ketika serangan di luar frame
		shoot = true;
		jackShoot = true;

		jackIdle = false;
		jackUp = false;
		jackDown = false;
	}

}

void Demo::BuildMC() {
	BuildJack();
	BuildJackUp();
	BuildJackDown();
	BuildJackAtk();
	BuildJackHurt();
	BuildAttackfMC();
}
void Demo::DrawMC() {
	DrawJack();
	DrawJackUp();
	DrawJackDown();
	DrawJackHurt();
	DrawJackAtk();
	DrawAttackfMC();
}

void Demo::DrawJack() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	UseShader(this->program);
	glUniform1i(glGetUniformLocation(this->program, "ourTexture"), 0);

	// set flip
	glUniform1i(glGetUniformLocation(this->program, "flip"), flip);
	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xpos, ypos, 0.0f));
	// Scale sprite 
	model = scale(model, vec3(widthJack, heightJack, 1));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAO);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	glDisable(GL_BLEND);
}
void Demo::BuildJack()
{
	this->program = BuildShader("playerSprite.vert", "playerSprite.frag");

	// Pass n to shader
	UseShader(this->program);
	glUniform1f(glGetUniformLocation(this->program, "n"), 1.0f / NUM_FRAMES);

	// Load and create a texture 
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("Assets/MC/idle.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	widthJack = ((float)width) / NUM_FRAMES;
	heightJack = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->program, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xpos = (GetScreenWidth() - widthJack) / GetScreenWidth();
	ypos = (GetScreenHeight() - heightJack) / 2;
	
	yVelocity = 0.2f;

	// Add input mapping
	// to offer input change flexibility you can save the input mapping configuration in a configuration file (non-hard code) !
	InputMapping("Quit", SDLK_ESCAPE);
	InputMapping("Move Up", SDLK_UP);
	InputMapping("Move Down", SDLK_DOWN);
	InputMapping("Shoot", SDLK_s);
}
void Demo::BuildJackUp()
{
	this->programUp = BuildShader("playerSprite.vert", "playerSprite.frag");

	// Pass n to shader
	UseShader(this->programUp);
	glUniform1f(glGetUniformLocation(this->programUp, "n"), 1.0f / NUM_FRAMES);

	// Load and create a texture 
	glGenTextures(1, &textureUp);
	glBindTexture(GL_TEXTURE_2D, textureUp); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("Assets/MC/fUp.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	widthJack = ((float)width) / NUM_FRAMES;
	heightJack = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAOUp);
	glGenBuffers(1, &VBOUp);
	glGenBuffers(1, &EBOUp);

	glBindVertexArray(VAOUp);

	glBindBuffer(GL_ARRAY_BUFFER, VBOUp);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOUp);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->programUp, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xposUp = 1000;
	yposUp = (GetScreenHeight() - heightJack) / 2;

	yVelocity = 0.2f;
}
void Demo::DrawJackUp() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureUp);
	UseShader(this->programUp);
	glUniform1i(glGetUniformLocation(this->programUp, "ourTexture"), 0);

	// set flip
	glUniform1i(glGetUniformLocation(this->programUp, "flip"), flip);
	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xposUp, yposUp, 0.0f));
	// Scale sprite 
	model = scale(model, vec3(widthJack, heightJack, 1));
	glUniformMatrix4fv(glGetUniformLocation(this->programUp, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAOUp);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	glDisable(GL_BLEND);
}
void Demo::BuildJackDown()
{
	this->programDown = BuildShader("playerSprite.vert", "playerSprite.frag");

	// Pass n to shader
	UseShader(this->programDown);
	glUniform1f(glGetUniformLocation(this->programDown, "n"), 1.0f / NUM_FRAMES);

	// Load and create a texture 
	glGenTextures(1, &textureDown);
	glBindTexture(GL_TEXTURE_2D, textureDown); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("Assets/MC/fDown.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	widthJack = ((float)width) / NUM_FRAMES;
	heightJack = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAODown);
	glGenBuffers(1, &VBODown);
	glGenBuffers(1, &EBODown);

	glBindVertexArray(VAODown);

	glBindBuffer(GL_ARRAY_BUFFER, VBODown);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBODown);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->programDown, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xposDown = 1000;
	yposDown = (GetScreenHeight() - heightJack) / 2;

	yVelocity = 0.2f;
}
void Demo::DrawJackDown() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureDown);
	UseShader(this->programDown);
	glUniform1i(glGetUniformLocation(this->programDown, "ourTexture"), 0);

	// set flip
	glUniform1i(glGetUniformLocation(this->programDown, "flip"), flip);
	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xposDown, yposDown, 0.0f));
	// Scale sprite 
	model = scale(model, vec3(widthJack, heightJack, 1));
	glUniformMatrix4fv(glGetUniformLocation(this->programDown, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAODown);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	glDisable(GL_BLEND);
}
void Demo::BuildJackAtk()
{
	this->programAtk = BuildShader("playerSprite.vert", "playerSprite.frag");

	// Pass n to shader
	UseShader(this->programAtk);
	glUniform1f(glGetUniformLocation(this->programAtk, "n"), 1.0f / NUM_FRAMES);

	// Load and create a texture 
	glGenTextures(1, &textureAtk);
	glBindTexture(GL_TEXTURE_2D, textureAtk); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("Assets/MC/atk.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	widthJack = ((float)width) / NUM_FRAMES;
	heightJack = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAOAtk);
	glGenBuffers(1, &VBOAtk);
	glGenBuffers(1, &EBOAtk);

	glBindVertexArray(VAOAtk);

	glBindBuffer(GL_ARRAY_BUFFER, VBOAtk);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOAtk);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->programAtk, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xposAtk = 1000;
	yposAtk = (GetScreenHeight() - heightJack) / 2;

	yVelocity = 0.2f;
}
void Demo::DrawJackAtk() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureAtk);
	UseShader(this->programAtk);
	glUniform1i(glGetUniformLocation(this->programAtk, "ourTexture"), 0);

	// set flip
	glUniform1i(glGetUniformLocation(this->programAtk, "flip"), flip);
	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xposAtk, yposAtk, 0.0f));
	// Scale sprite 
	model = scale(model, vec3(widthJack, heightJack, 1));
	glUniformMatrix4fv(glGetUniformLocation(this->programAtk, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAOAtk);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	glDisable(GL_BLEND);
}
void Demo::BuildJackHurt()
{
	this->programHurt = BuildShader("playerSprite.vert", "playerSprite.frag");

	// Pass n to shader
	UseShader(this->programHurt);
	glUniform1f(glGetUniformLocation(this->programHurt, "n"), 1.0f / NUM_FRAMES);

	// Load and create a texture 
	glGenTextures(1, &textureHurt);
	glBindTexture(GL_TEXTURE_2D, textureHurt); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("Assets/MC/hurt.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	widthJack = ((float)width) / NUM_FRAMES;
	heightJack = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAOHurt);
	glGenBuffers(1, &VBOHurt);
	glGenBuffers(1, &EBOHurt);

	glBindVertexArray(VAOHurt);

	glBindBuffer(GL_ARRAY_BUFFER, VBOHurt);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOHurt);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->programHurt, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xposHurt = 1000;
	yposHurt = (GetScreenHeight() - heightJack) / 2;

	yVelocity = 0.2f;
}
void Demo::DrawJackHurt() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureHurt);
	UseShader(this->programHurt);
	glUniform1i(glGetUniformLocation(this->programHurt, "ourTexture"), 0);

	// set flip
	glUniform1i(glGetUniformLocation(this->programHurt, "flip"), flip);
	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xposHurt, yposHurt, 0.0f));
	// Scale sprite 
	model = scale(model, vec3(widthJack, heightJack, 1));
	glUniformMatrix4fv(glGetUniformLocation(this->programHurt, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAOHurt);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	glDisable(GL_BLEND);
}
void Demo::BuildAttackfMC()
{
	this->programAttack = BuildShader("playerSprite.vert", "playerSprite.frag");

	// Pass n to shader
	UseShader(this->programAttack);
	glUniform1f(glGetUniformLocation(this->programAttack, "n"), 1.0f / NUM_FRAMES);

	// Load and create a texture 
	glGenTextures(1, &textureAttack);
	glBindTexture(GL_TEXTURE_2D, textureAttack); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("Assets/beam1.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	widthAttack = ((float)width) / NUM_FRAMES;
	heightAttack = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAOAttack);
	glGenBuffers(1, &VBOAttack);
	glGenBuffers(1, &EBOAttack);

	glBindVertexArray(VAOAttack);

	glBindBuffer(GL_ARRAY_BUFFER, VBOAttack);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOAttack);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->programAttack, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xposAttack = 1000;
	yposAttack = 1000;

	yVelocityAttack = 0.3f;
}
void Demo::DrawAttackfMC() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureAttack);
	UseShader(this->programAttack);
	glUniform1i(glGetUniformLocation(this->programAttack, "ourTexture"), 0);

	// set flip
	glUniform1i(glGetUniformLocation(this->programAttack, "flip"), flipAttack);
	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xposAttack, yposAttack, 0.0f));
	// Scale sprite 
	model = scale(model, vec3(widthAttack, heightAttack, 1));
	glUniformMatrix4fv(glGetUniformLocation(this->programAttack, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAOAttack);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	glDisable(GL_BLEND);
}


//ENEMY
void Demo::UpdateEnemy(float deltaTime)
{
	frame_durEnemy += deltaTime;

	if (frame_durEnemy > FRAME_DUR) {
		frame_durEnemy = 0;

		if (frame_idxEnemy == NUM_FRAMES - 1) frame_idxEnemy = 0;  else frame_idxEnemy++;

		UseShader(this->programEnemy);
		glUniform1i(glGetUniformLocation(this->programEnemy, "frameIndex"), frame_idxEnemy);
	}
}
void Demo::UpdateAtkVirus(float deltaTime)
{
	frame_durAtkVirus += deltaTime;

	if (frame_durAtkVirus > FRAME_DUR) {
		frame_durAtkVirus = 0;

		if (frame_idxAtkVirus == NUM_FRAMES - 1) frame_idxAtkVirus = 0;  else frame_idxAtkVirus++;

		UseShader(this->programAtkVirus);
		glUniform1i(glGetUniformLocation(this->programAtkVirus, "frameIndex"), frame_idxAtkVirus);

		countAtkVirus++;
	}

	if (xposAtkVirus >= -100) {
		xposAtkVirus -= 3;
	}

	if ((countAtkVirus % 100) == 0) {
		xposAtkVirus = xposEnemy - widthEnemy;
	}
}

void Demo::BuildTheVirus() {
	BuildEnemy();
	BuildAtkVirus();
}
void Demo::DrawTheVirus() {
	DrawEnemy();
	DrawAtkVirus();
}

void Demo::BuildEnemy()
{
	this->programEnemy = BuildShader("playerSprite.vert", "playerSprite.frag");

	// Pass n to shader
	UseShader(this->programEnemy);
	glUniform1f(glGetUniformLocation(this->programEnemy, "n"), 1.0f / NUM_FRAMES);

	// Load and create a texture 
	glGenTextures(1, &textureEnemy);
	glBindTexture(GL_TEXTURE_2D, textureEnemy); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("Assets/Virus/vIdle.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	widthEnemy = ((float)width) / NUM_FRAMES;
	heightEnemy = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAOEnemy);
	glGenBuffers(1, &VBOEnemy);
	glGenBuffers(1, &EBOEnemy);

	glBindVertexArray(VAOEnemy);

	glBindBuffer(GL_ARRAY_BUFFER, VBOEnemy);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOEnemy);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->programEnemy, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xposEnemy = (GetScreenWidth() - widthEnemy);
	yposEnemy = (GetScreenHeight() - heightEnemy)/2;
	gravityEnemy = 0.05f;
	xVelocityEnemy = 0.1f;
}
void Demo::DrawEnemy() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureEnemy);
	UseShader(this->programEnemy);
	glUniform1i(glGetUniformLocation(this->programEnemy, "ourTexture"), 0);

	// set flip
	glUniform1i(glGetUniformLocation(this->programEnemy, "flip"), flipEnemy);
	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xposEnemy, yposEnemy, 0.0f));
	// Scale sprite 
	model = scale(model, vec3(widthEnemy, heightEnemy, 1));
	glUniformMatrix4fv(glGetUniformLocation(this->programEnemy, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAOEnemy);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	glDisable(GL_BLEND);
}
void Demo::BuildAtkVirus()
{
	this->programAtkVirus = BuildShader("playerSprite.vert", "playerSprite.frag");

	// Pass n to shader
	UseShader(this->programAtkVirus);
	glUniform1f(glGetUniformLocation(this->programAtkVirus, "n"), 1.0f / NUM_FRAMES);

	// Load and create a texture 
	glGenTextures(1, &textureAtkVirus);
	glBindTexture(GL_TEXTURE_2D, textureAtkVirus); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("Assets/beam2.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	widthAttackVirus = ((float)width) / NUM_FRAMES;
	heightAttackVirus = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAOAtkVirus);
	glGenBuffers(1, &VBOAtkVirus);
	glGenBuffers(1, &EBOAtkVirus);

	glBindVertexArray(VAOAtkVirus);

	glBindBuffer(GL_ARRAY_BUFFER, VBOAtkVirus);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOAtkVirus);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->programAtkVirus, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xposAtkVirus = -1000;
	yposAtkVirus = yposEnemy;

	yVelocityAtkVirus = 0.3f;
}
void Demo::DrawAtkVirus() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureAtkVirus);
	UseShader(this->programAtkVirus);
	glUniform1i(glGetUniformLocation(this->programAtkVirus, "ourTexture"), 0);

	// set flip
	glUniform1i(glGetUniformLocation(this->programAtkVirus, "flip"), flipAtkVirus);
	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xposAtkVirus, yposAtkVirus, 0.0f));
	// Scale sprite 
	model = scale(model, vec3(widthAttackVirus, heightAttackVirus, 1));
	glUniformMatrix4fv(glGetUniformLocation(this->programAtkVirus, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAOAtkVirus);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	glDisable(GL_BLEND);
}

void Demo::DeleteEnemy() {
	xposEnemy = 1000;
	yposEnemy = 1000;
}




//HP
void Demo::BuildHealthPoint() {
	BuildHPFull();
	BuildHPMin1();
	BuildHPMin2();
	BuildHPEmpty();
}
void Demo::DrawHealthPoint() {
	DrawHPFull();
	DrawHPMin1();
	DrawHPMin2();
	DrawHPEmpty();
}

void Demo::BuildHPFull()
{
	this->programHPFull = BuildShader("crateSprite.vert", "crateSprite.frag");
	UseShader(this->programHPFull);

	// Load and create a texture 
	glGenTextures(1, &textureHPFull);
	glBindTexture(GL_TEXTURE_2D, textureHPFull); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("Assets/Health/full.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	widthHP = (float)width;
	heightHP = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAOHPFull);
	glGenBuffers(1, &VBOHPFull);
	glGenBuffers(1, &EBOHPFull);

	glBindVertexArray(VAOHPFull);

	glBindBuffer(GL_ARRAY_BUFFER, VBOHPFull);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOHPFull);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->programHPFull, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xposHPFull = GetScreenWidth()-widthHP+100;
	yposHPFull = GetScreenHeight()/heightHP;
}
void Demo::DrawHPFull() {
	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureHPFull);
	// Activate shader
	UseShader(this->programHPFull);
	glUniform1i(glGetUniformLocation(this->programHPFull, "ourTexture"), 1);

	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xposHPFull, yposHPFull, 0.0f));
	// Scale sprite 
	model = scale(model, vec3(widthHP * 0.5, heightHP * 0.5, 3));
	glUniformMatrix4fv(glGetUniformLocation(this->programHPFull, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAOHPFull);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
}
void Demo::BuildHPMin1()
{
	this->programHPMin1 = BuildShader("crateSprite.vert", "crateSprite.frag");
	UseShader(this->programHPMin1);

	// Load and create a texture 
	glGenTextures(1, &textureHPMin1);
	glBindTexture(GL_TEXTURE_2D, textureHPMin1); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("Assets/Health/minus1.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	widthHP = (float)width;
	heightHP = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAOHPMin1);
	glGenBuffers(1, &VBOHPMin1);
	glGenBuffers(1, &EBOHPMin1);

	glBindVertexArray(VAOHPMin1);

	glBindBuffer(GL_ARRAY_BUFFER, VBOHPMin1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOHPMin1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->programHPMin1, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xposHPMin1 = 1000;
	yposHPMin1 = 1000;
}
void Demo::DrawHPMin1() {
	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureHPMin1);
	// Activate shader
	UseShader(this->programHPMin1);
	glUniform1i(glGetUniformLocation(this->programHPMin1, "ourTexture"), 1);

	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xposHPMin1, yposHPMin1, 0.0f));
	// Scale sprite 
	model = scale(model, vec3(widthHP * 0.5, heightHP * 0.5, 3));
	glUniformMatrix4fv(glGetUniformLocation(this->programHPMin1, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAOHPMin1);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
}
void Demo::BuildHPMin2()
{
	this->programHPMin2 = BuildShader("crateSprite.vert", "crateSprite.frag");
	UseShader(this->programHPMin2);

	// Load and create a texture 
	glGenTextures(1, &textureHPMin2);
	glBindTexture(GL_TEXTURE_2D, textureHPMin2); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("Assets/Health/minus2.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	widthHP = (float)width;
	heightHP = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAOHPMin2);
	glGenBuffers(1, &VBOHPMin2);
	glGenBuffers(1, &EBOHPMin2);

	glBindVertexArray(VAOHPMin2);

	glBindBuffer(GL_ARRAY_BUFFER, VBOHPMin2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOHPMin2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->programHPMin2, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xposHPMin2 = 1000;
	yposHPMin2 = 1000;
}
void Demo::DrawHPMin2() {
	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureHPMin2);
	// Activate shader
	UseShader(this->programHPMin2);
	glUniform1i(glGetUniformLocation(this->programHPMin2, "ourTexture"), 1);

	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xposHPMin2, yposHPMin2, 0.0f));
	// Scale sprite 
	model = scale(model, vec3(widthHP * 0.5, heightHP * 0.5, 3));
	glUniformMatrix4fv(glGetUniformLocation(this->programHPMin2, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAOHPMin2);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
}
void Demo::BuildHPEmpty()
{
	this->programHPEmpty = BuildShader("crateSprite.vert", "crateSprite.frag");
	UseShader(this->programHPEmpty);

	// Load and create a texture 
	glGenTextures(1, &textureHPEmpty);
	glBindTexture(GL_TEXTURE_2D, textureHPEmpty); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("Assets/Health/empty.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	widthHP = (float)width;
	heightHP = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAOHPEmpty);
	glGenBuffers(1, &VBOHPEmpty);
	glGenBuffers(1, &EBOHPEmpty);

	glBindVertexArray(VAOHPEmpty);

	glBindBuffer(GL_ARRAY_BUFFER, VBOHPEmpty);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOHPEmpty);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->programHPMin2, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xposHPEmpty = 1000;
	yposHPEmpty  = 1000;
}
void Demo::DrawHPEmpty() {
	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureHPEmpty);
	// Activate shader
	UseShader(this->programHPEmpty);
	glUniform1i(glGetUniformLocation(this->programHPEmpty, "ourTexture"), 1);

	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xposHPEmpty, yposHPEmpty, 0.0f));
	// Scale sprite 
	model = scale(model, vec3(widthHP * 0.5, heightHP * 0.5, 3));
	glUniformMatrix4fv(glGetUniformLocation(this->programHPEmpty, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAOHPEmpty);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
}



//BG
void Demo::BuildBG()
{
	this->programBG = BuildShader("crateSprite.vert", "crateSprite.frag");
	UseShader(this->programBG);

	// Load and create a texture 
	glGenTextures(1, &textureBG);
	glBindTexture(GL_TEXTURE_2D, textureBG); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("Assets/BG/cropBG.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	widthBG = (float)width;
	heightBG = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAOBG);
	glGenBuffers(1, &VBOBG);
	glGenBuffers(1, &EBOBG);

	glBindVertexArray(VAOBG);

	glBindBuffer(GL_ARRAY_BUFFER, VBOBG);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOBG);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->programBG, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xposBG = 0;
	yposBG = -2;
}
void Demo::DrawBG() {
	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureBG);
	// Activate shader
	UseShader(this->programBG);
	glUniform1i(glGetUniformLocation(this->programBG, "ourTexture"), 1);

	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xposBG, yposBG, 0.0f));
	// Scale sprite 
	model = scale(model, vec3(widthBG, heightBG, 1));
	glUniformMatrix4fv(glGetUniformLocation(this->programBG, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAOBG);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
}
void Demo::BuildStartBG()
{
	this->programStartBG = BuildShader("crateSprite.vert", "crateSprite.frag");
	UseShader(this->programStartBG);

	// Load and create a texture 
	glGenTextures(1, &textureStartBG);
	glBindTexture(GL_TEXTURE_2D, textureStartBG); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("Assets/BG/startBG.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	widthStartBG = (float)width;
	heightStartBG = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAOStartBG);
	glGenBuffers(1, &VBOStartBG);
	glGenBuffers(1, &EBOStartBG);

	glBindVertexArray(VAOStartBG);

	glBindBuffer(GL_ARRAY_BUFFER, VBOStartBG);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOStartBG);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->programStartBG, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xposStartBG = 0;
	yposStartBG = -2;
}
void Demo::DrawStartBG() {
	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureStartBG);
	// Activate shader
	UseShader(this->programStartBG);
	glUniform1i(glGetUniformLocation(this->programStartBG, "ourTexture"), 1);

	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xposStartBG, yposStartBG, 0.0f));
	// Scale sprite 
	model = scale(model, vec3(widthStartBG, heightStartBG, 1));
	glUniformMatrix4fv(glGetUniformLocation(this->programStartBG, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAOStartBG);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
}
void Demo::BuildEndBG()
{
	this->programEndBG = BuildShader("crateSprite.vert", "crateSprite.frag");
	UseShader(this->programEndBG);

	// Load and create a texture 
	glGenTextures(1, &textureEndBG);
	glBindTexture(GL_TEXTURE_2D, textureEndBG); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("Assets/BG/endBG.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	widthEndBG = (float)width;
	heightEndBG = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAOEndBG);
	glGenBuffers(1, &VBOEndBG);
	glGenBuffers(1, &EBOEndBG);

	glBindVertexArray(VAOEndBG);

	glBindBuffer(GL_ARRAY_BUFFER, VBOEndBG);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOEndBG);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->programEndBG, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xposEndBG = 0;
	yposEndBG = -2;
}
void Demo::DrawEndBG() {
	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureEndBG);
	// Activate shader
	UseShader(this->programEndBG);
	glUniform1i(glGetUniformLocation(this->programEndBG, "ourTexture"), 1);

	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xposEndBG, yposEndBG, 0.0f));
	// Scale sprite 
	model = scale(model, vec3(widthEndBG, heightEndBG, 1));
	glUniformMatrix4fv(glGetUniformLocation(this->programEndBG, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAOEndBG);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
}


//Menu Option
void Demo::buttonMenu() {
	string buttons[NUM_BUTTON] = { "Opsigame/play1.png", "Opsigame/quit1.png" };
	string hover_buttons[NUM_BUTTON] = { "Opsigame/play2.png", "Opsigame/quit2.png" };

	glGenTextures(3, &textureOption[0]);
	glGenTextures(3, &hover_textureOption[0]);

	for (int i = 0; i < NUM_BUTTON; i++) {
		// Load and create a texture 
		glBindTexture(GL_TEXTURE_2D, textureOption[i]); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Load, create texture 
		int width, height;
		unsigned char* image = SOIL_load_image(buttons[i].c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

		// Set up vertex data (and buffer(s)) and attribute pointers
		button_widthOption[i] = (float)width;
		button_heightOption[i] = (float)height;

		// Load and create a texture 
		glBindTexture(GL_TEXTURE_2D, hover_textureOption[i]); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		image = SOIL_load_image(hover_buttons[i].c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

		 // Set up vertex data (and buffer(s)) and attribute pointers
		hover_button_widthOption[i] = (float)width;
		hover_button_heightOption[i] = (float)height;
	}

	GLfloat vertices[] = {
		// Positions	// Texture Coords
		1,  1,			1.0f, 1.0f, // Bottom Right
		1,  0,			1.0f, 0.0f, // Top Right
		0,  0,			0.0f, 0.0f, // Top Left
		0,  1,			0.0f, 1.0f  // Bottom Left 
	};


	glGenVertexArrays(1, &VAOquit);
	glGenBuffers(1, &VBOquit);
	glBindVertexArray(VAOquit);
	glBindBuffer(GL_ARRAY_BUFFER, VBOquit);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	this->programOption = BuildShader("shader.vert", "shader.frag");
	InputMapping("ChooseTheButton", SDLK_RETURN);
	InputMapping("NextButton", SDLK_DOWN);
	InputMapping("PrevButton", SDLK_UP);
}
void Demo::renderButtonMenu() {
	// Enable transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	int texture_id[NUM_BUTTON] = { GL_TEXTURE1, GL_TEXTURE2 };
	// Activate shader
	UseShader(this->programOption);
	glUniform1i(glGetUniformLocation(this->programOption, "text"), 0);

	glBindVertexArray(VAOquit);
	for (int i = 0; i < NUM_BUTTON; i++) {

		glActiveTexture(texture_id[i]);
		glBindTexture(GL_TEXTURE_2D, (activeButtonIndex == i) ? hover_textureOption[i] : textureOption[i]);
		glUniform1i(glGetUniformLocation(this->programOption, "ourTexture"), i + 1);

		mat4 model;
		model = translate(model, vec3((GetScreenWidth() - button_widthOption[i]) / 2, (i + 3) * 100, 0.0f));
		model = scale(model, vec3(button_widthOption[i], button_heightOption[i], 1));
		glUniformMatrix4fv(glGetUniformLocation(this->programOption, "model"), 1, GL_FALSE, value_ptr(model));

		glDrawArrays(GL_QUADS, 0, 4);
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}

void Demo::buttonGameOver() {
	string buttons[NUM_BUTTON] = { "Opsigame/restart1.png", "Opsigame/quit1.png" };
	string hover_buttons[NUM_BUTTON] = { "Opsigame/restart2.png", "Opsigame/quit2.png" };

	glGenTextures(3, &textureGameOver[0]);
	glGenTextures(3, &hover_GameOver[0]);

	for (int i = 0; i < NUM_BUTTON; i++) {
		// Load and create a texture 
		glBindTexture(GL_TEXTURE_2D, textureGameOver[i]); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Load, create texture 
		int width, height;
		unsigned char* image = SOIL_load_image(buttons[i].c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

		// Set up vertex data (and buffer(s)) and attribute pointers
		button_widthGameOver[i] = (float)width;
		button_heightGameOver[i] = (float)height;

		// Load and create a texture 
		glBindTexture(GL_TEXTURE_2D, hover_GameOver[i]); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		image = SOIL_load_image(hover_buttons[i].c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

		 // Set up vertex data (and buffer(s)) and attribute pointers
		hover_button_widthGameOver[i] = (float)width;
		hover_button_heightGameOver[i] = (float)height;
	}

	GLfloat vertices[] = {
		// Positions	// Texture Coords
		1,  1,			1.0f, 1.0f, // Bottom Right
		1,  0,			1.0f, 0.0f, // Top Right
		0,  0,			0.0f, 0.0f, // Top Left
		0,  1,			0.0f, 1.0f  // Bottom Left 
	};


	glGenVertexArrays(1, &VAOquit2);
	glGenBuffers(1, &VBOquit2);
	glBindVertexArray(VAOquit2);
	glBindBuffer(GL_ARRAY_BUFFER, VBOquit2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	this->programGameOver = BuildShader("shader.vert", "shader.frag");
	InputMapping("ChooseTheButton", SDLK_RETURN);
	InputMapping("NextButton", SDLK_DOWN);
	InputMapping("PrevButton", SDLK_UP);
}
void Demo::renderGameOver() {
	// Enable transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	int texture_id[NUM_BUTTON] = { GL_TEXTURE1, GL_TEXTURE2 };
	// Activate shader
	UseShader(this->programGameOver);
	glUniform1i(glGetUniformLocation(this->programGameOver, "text"), 0);

	glBindVertexArray(VAOquit2);
	for (int i = 0; i < NUM_BUTTON; i++) {

		glActiveTexture(texture_id[i]);
		glBindTexture(GL_TEXTURE_2D, (activeButtonGameOver == i) ? hover_GameOver[i] : textureGameOver[i]);
		glUniform1i(glGetUniformLocation(this->programGameOver, "ourTexture"), i + 1);

		mat4 model;
		model = translate(model, vec3((GetScreenWidth() - button_widthGameOver[i]) / 2, (i + 3) * 100, 0.0f));
		model = scale(model, vec3(button_widthGameOver[i], button_heightGameOver[i], 1));
		glUniformMatrix4fv(glGetUniformLocation(this->programGameOver, "model"), 1, GL_FALSE, value_ptr(model));

		glDrawArrays(GL_QUADS, 0, 4);
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}




// GAME
int main(int argc, char** argv) {

	Engine::Game& game = Demo();
	game.Start("Dunia Covid-19", 800, 600, false, WindowFlag::WINDOWED, 60, 1);

	return 0;
}
void Demo::changeHP() {
	if (healthPoint == 3) {
		xposHPFull = GetScreenWidth() - widthHP + 100;
		yposHPFull = GetScreenHeight() / heightHP;

		xposHPMin1 = 1000; yposHPMin1 = 1000;
		xposHPMin2 = 1000; yposHPMin2 = 1000;
		xposHPEmpty = 1000;yposHPEmpty= 1000;
	}
	else if (healthPoint == 2) {
		xposHPMin1 = GetScreenWidth() - widthHP + 100;
		yposHPMin1 = GetScreenHeight() / heightHP;

		xposHPFull = 1000; yposHPFull = 1000;
		xposHPMin2 = 1000; yposHPMin2 = 1000;
		xposHPEmpty = 1000; yposHPEmpty = 1000;
	}
	else if (healthPoint == 1) {
		xposHPMin2 = GetScreenWidth() - widthHP + 100;
		yposHPMin2 = GetScreenHeight() / heightHP;

		xposHPFull = 1000; yposHPFull = 1000;
		xposHPMin1 = 1000; yposHPMin1 = 1000;
		xposHPEmpty = 1000; yposHPEmpty = 1000;
	}
	else {
		xposHPEmpty = GetScreenWidth() - widthHP + 100;
		yposHPEmpty = GetScreenHeight() / heightHP;

		xposHPFull = 1000; yposHPFull = 1000;
		xposHPMin1 = 1000; yposHPMin1 = 1000;
		xposHPMin2 = 1000; yposHPMin2 = 1000;
	}
}
bool Demo::IsShoot(bool check) {
	return check;
}
bool Demo::OnTarget(float x, float y) {
	return (x == 673 && y >= 168 && y <= 278);
}
bool Demo::Hurt(float x1, float y1, float y2) {
	float atas = y2 + 70;
	float bawah = y2 - 70;
 	return (x1 == -34 && y1 >= bawah && y1 <= atas);
}
bool Demo::CheckWin(float EnemiesLeft, int healthPoint) {
	if (EnemiesLeft == 0 || healthPoint == 0) {
		return true;
	}else {
		return false;
	}
}

void Demo::InitAudio() {
	int flags = MIX_INIT_MP3 | MIX_INIT_FLAC | MIX_INIT_OGG;
	if (flags != Mix_Init(flags)) {
		cout << "Mixer error" << endl;
	}

	int audio_rate = 22050; Uint16 audio_format = AUDIO_S16SYS; int audio_channels = 2; int audio_buffers = 4096;
	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
		cout << "Audio error" << endl;
	}

	bgm = Mix_LoadMUS("Sounds/bgm.ogg");
	up = Mix_LoadWAV("Sounds/terbang.wav");
	down = Mix_LoadWAV("Sounds/turun.wav");
	Theshoot = Mix_LoadWAV("Sounds/mcAtk.wav");
	gameOver = Mix_LoadWAV("Sounds/gameOver.wav");
	hurts = Mix_LoadWAV("Sounds/hurt.wav");
	option = Mix_LoadWAV("Sounds/option.wav");
	virusAttack = Mix_LoadWAV("Sounds/virusAtk.wav");
	winGame = Mix_LoadWAV("Sounds/winGame.wav");
}
void Demo::SFX() {
	if (Mix_Playing(sfx_up) == 0 && IsKeyDown("Move Up")) {
		sfx_up = Mix_PlayChannel(-1, up, 0);
	}
	if (Mix_Playing(sfx_down) == 0 && IsKeyDown("Move Down")) {
		sfx_down = Mix_PlayChannel(-1, up, 0);
	}
	if (Mix_Playing(sfx_Theshoot) == 0 && IsKeyDown("Shoot")) {
		sfx_Theshoot = Mix_PlayChannel(-1, Theshoot, 0);
	}
	// Set Audio dari GameOver - WinGame belum
	
}
void Demo::deInitAudio() {
	if (Mix_Playing(sfx_up) == 0) {
		Mix_FreeChunk(up);
	}
	if (Mix_Playing(sfx_down) == 0) {
		Mix_FreeChunk(down);
	}
	if (Mix_Playing(sfx_Theshoot) == 0) {
		Mix_FreeChunk(Theshoot);
	}
	if (bgm != NULL) {
		Mix_FreeMusic(bgm);
	}
	Mix_CloseAudio();
}