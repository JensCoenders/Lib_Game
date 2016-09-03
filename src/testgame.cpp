#include <iostream>
#include <sstream>
#include "testgame.h"

using namespace std;

Game_TextObject* g_textDisplay;
int g_lastTurn = 1;
int* g_tttBoard = NULL;
Game_Object** g_tttObjects = NULL;

int ttf_checkForWinner()
{
	// Check for each player (2 players)
	for (int i = 1; i <= 2; i++)
	{
		// Check each tile on the board
		for (int j = 0; j < 3; j++)
		{
			bool win = true;
			for (int k = 0; k < 3; k++)
			{
				if (g_tttBoard[j * 3 + k] != i)
				{
					win = false;
					break;
				}
			}

			if (win)
				return i;

			if (j == 0)
			{
				for (int k = 0; k < 3; k++)
				{
					win = true;

					for (int l = 0; l < 3; l++)
					{
						if (g_tttBoard[l * 3 + k] != i)
						{
							win = false;
							break;
						}
					}

					if (win)
						return i;
				}

				win = true;

				for (int k = 0; k < 3; k++)
				{

					if (g_tttBoard[k * 3 + k] != i)
					{
						win = false;
						break;
					}
				}

				if (win)
					return i;
			}

			if (j == 2)
			{
				win = true;

				for (int k = 0; k < 3; k++)
				{

					if (g_tttBoard[(2 - k) * 3 + k] != i)
					{
						win = false;
						break;
					}
				}

				if (win)
					return i;
			}
		}
	}

	bool fullyFilled = true;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (g_tttBoard[j * 3 + i] == 0)
			{
				fullyFilled = false;
				break;
			}
		}

		if (!fullyFilled)
			break;
	}

	if (fullyFilled)
		return 3;

	return 0;
}

void tttBackgroundClicked(Game_AdvancedObject& object, Game_ObjectEvent& event)
{
	Game_MouseClickedEvent& mouseClickedEvent = (Game_MouseClickedEvent&) event;
	if (!mouseClickedEvent.pressed)
		return;

	unsigned int clickedContainer = 0;
	if (mouseClickedEvent.relX >= 0 && mouseClickedEvent.relX < 47)
		clickedContainer |= 0x1;
	else if (mouseClickedEvent.relX >= 50 && mouseClickedEvent.relX < 97)
		clickedContainer |= 0x2;
	else if (mouseClickedEvent.relX >= 100 && mouseClickedEvent.relX < 147)
		clickedContainer |= 0x4;

	if (mouseClickedEvent.relY >= 0 && mouseClickedEvent.relY < 47)
		clickedContainer |= 0x8;
	else if (mouseClickedEvent.relY >= 50 && mouseClickedEvent.relY < 97)
		clickedContainer |= 0x10;
	else if (mouseClickedEvent.relY >= 100 && mouseClickedEvent.relY < 147)
		clickedContainer |= 0x20;

	int boardRow = 0, boardColumn = 0;
	switch (clickedContainer)
	{
		case 0x9:
			break;
		case 0xA:
			boardColumn = 1;
			break;
		case 0xC:
			boardColumn = 2;
			break;
		case 0x11:
			boardRow = 1;
			break;
		case 0x12:
			boardRow = 1;
			boardColumn = 1;
			break;
		case 0x14:
			boardRow = 1;
			boardColumn = 2;
			break;
		case 0x21:
			boardRow = 2;
			break;
		case 0x22:
			boardRow = 2;
			boardColumn = 1;
			break;
		case 0x24:
			boardRow = 2;
			boardColumn = 2;
			break;
	}

	if (g_tttBoard[boardRow * 3 + boardColumn] == 0)
	{
		string textureName = (g_lastTurn == 1) ? "cross" : "circle";
		textureName += ".png";
		g_tttObjects[boardRow * 3 + boardColumn]->setRenderPars(
		        new Game_RP_ImageTexture(game_assetGetPath(textureName, "textures")));

		g_tttBoard[boardRow * 3 + boardColumn] = g_lastTurn;
		g_lastTurn = (g_lastTurn == 1 ? 2 : 1);
	}

	int winner = 0;
	if ((winner = ttf_checkForWinner()))
	{
		string resultText;
		if (winner == 3)
			resultText = "Draw!";
		else
		{
			resultText = "Player ";
			resultText += (char) (winner | 0x30);
			resultText += " wins!";
		}

		g_textDisplay->setText(resultText);
		g_textDisplay->setFrameUpdate(NULL);
		object.mouseClickedFunc = NULL;
	}
}

void textDisplayFU(Game_Object& object)
{
	static int prevLastTurn = 0;
	if (prevLastTurn != g_lastTurn)
	{
		Game_TextObject& textObject = (Game_TextObject&) object;
		switch (g_lastTurn)
		{
			case 1:
				textObject.setText("Player 1's turn");
				break;
			case 2:
				textObject.setText("Player 2's turn");
				break;
		}

		prevLastTurn = g_lastTurn;
	}
}

void speedLabFU(Game_Object& object)
{
	Game_TextObject& advancedObject = (Game_TextObject&) object;

	static int lastMovementSpeed = 0;
	if (lastMovementSpeed != game_shmGet(SHM_WORLD_MAIN_CAMERA).movementSpeed)
	{
		lastMovementSpeed = game_shmGet(SHM_WORLD_MAIN_CAMERA).movementSpeed;

		ostringstream stream;
		stream << "Camera movement speed: " << lastMovementSpeed;
		advancedObject.setText(stream.str());
	}
}

void zoomScaleLabFU(Game_Object& object)
{
	static double lastZoomScale = 0;
	if (lastZoomScale != game_shmGet(SHM_WORLD_ZOOM_SCALE))
	{
		lastZoomScale = game_shmGet(SHM_WORLD_ZOOM_SCALE);

		ostringstream stream;
		stream << "Zoom scale: " << lastZoomScale;
		((Game_TextObject&) object).setText(stream.str());
	}
}

void runTestGame()
{
	// Initialize game
	game_shmPut(SHM_WORLD_KEYBOARD_MOVES_CAMERA, false);

	g_tttBoard = new int[9];
	g_tttObjects = new Game_Object*[9];
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			g_tttBoard[i * 3 + j] = 0;

			g_tttObjects[i * 3 + j] = new Game_Object(j * 51 + 5, i * 51 + 32, 47, 47, true);
			g_tttObjects[i * 3 + j]->setTextureUpdate(imageTextureObjectTU);
			game_renderAddObject(g_tttObjects[i * 3 + j], GAME_LAYER_LEVEL_MID_1);
		}
	}

	// Debug objects
	Game_TextObject fpsObject(5, -51, 0, 0, true);
	fpsObject.setText("FPS: 0");
	game_shmPut(SHM_MISC_FPS_OBJECT, &fpsObject);

	Game_TextObject speedLab(5, -26, 0, 0, true);
	speedLab.setFrameUpdate(speedLabFU);

	Game_TextObject zoomScaleLab(5, -1, 0, 0, true);
	zoomScaleLab.setFrameUpdate(zoomScaleLabFU);

	game_renderAddObject(&fpsObject, GAME_LAYER_GUI_BACKGROUND);
	game_renderAddObject(&speedLab, GAME_LAYER_GUI_BACKGROUND);
	game_renderAddObject(&zoomScaleLab, GAME_LAYER_GUI_BACKGROUND);

	// Game objects
	Game_Object background(0, 0, -1, -1, true);
	background.setTextureUpdate(imageTextureObjectTU);
	background.setRenderPars(new Game_RP_ImageTexture(game_assetGetPath("background.png", "textures")));

	Game_AdvancedObject tttBackground(5, 32, 150, 150, true);
	tttBackground.setTextureUpdate(imageTextureObjectTU);
	tttBackground.setRenderPars(new Game_RP_ImageTexture(game_assetGetPath("tic_tac_toe_background.png", "textures")));
	tttBackground.mouseClickedFunc = tttBackgroundClicked;

	Game_TextObject textDisplay(5, 0, 0, 0, true);
	textDisplay.setFrameUpdate(textDisplayFU);
	g_textDisplay = &textDisplay;

	game_renderAddObject(&background, GAME_LAYER_LEVEL_BACKGROUND);
	game_renderAddObject(&tttBackground, GAME_LAYER_LEVEL_MID_2);
	game_renderAddObject(&textDisplay, GAME_LAYER_GUI_FOREGROUND);

	// Main loop
	game_runMainLoop();

	// Clean up
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
			delete g_tttObjects[i * 3 + j];
	}

	delete[] g_tttBoard;
	delete[] g_tttObjects;
}
