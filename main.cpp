#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <deque>
#include <string>
#include <fstream>

using std::cout, std::deque, std::to_string, std::string, std::stoi, std::ofstream, std::ifstream, std::getline, std::ws, std::cin;

/*
* written by chapel1337
* started 12/21/2022, finished on 12/25/2022
* had family come over for christmas, so this was an on-off project
* probably the largest project i've ever done(?)
* i experimented with a quick credits screen on startup; it might be irritating after prolonged exposure
* and, of course, due to the lack of high definition rendering it's inevitable that some moron will notice it and think this is garbage 
* i didn't work on this during the 23rd, as i was working on a seperate project 
* while working on this, i figured out how to make an effective button, so i will update the snow falling and finally add the snow pushing feature
* release seems to be broken for some unknwon reason, so i'll be releasing this as debug
* merry christmas
* 
* (12/26/2022 - 12/27/2022) - added front resizing threshold, decreased credits length, centered rectangle x and y, attempted to add save and load feature but failed, added background changing, added color picker, added window maximizing/minimizing, added window fullscreen
* can't think of how to design buttons for background changer and color picker
* 
* 12/28/2022 - enabled hardware acceleration, fixed unreliability with color picker, added icon for background changer button, added controller support (besides joystick), made pressing pressing any key or button during credits will now skip it, included background change for undo, made exiting fullscreen return to previous window dimensions
*/

SDL_Window* window{ SDL_CreateWindow("paint", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1050, 750, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE) };
SDL_Renderer* renderer{ SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED) };

SDL_Surface* windowIcon{ SDL_LoadBMP("paint_brush.bmp") };

SDL_GameController* controller{ NULL };
SDL_Joystick* joystick{ NULL };

TTF_Font* arialFont{};

SDL_Rect toolbarBackground{ 0, 0, 1000, 50 };

SDL_Rect colorRect{ 10, 10, 25, 25 };
SDL_Rect selectionRect{ 10, 10, 26, 26 }; 

SDL_Color currentColor{ 255, 0, 0, 255 };
SDL_Color customColor{ 255, 255, 255, 255 };
string customColorString{ "225, 255, 255" };

SDL_Rect changeBackgroundRect{ 860, 10, 25, 25 };
SDL_FRect changeBackgroundInnerRect{ 863.8, 14.5, 17, 17 };
deque<SDL_Color> backgroundColors{ SDL_Color(20, 20, 20, 255), };

SDL_Rect colorPickerRect{ 910, 10, 25, 25 };

SDL_Rect rgbRect{ 680, 10, 150, 25 };
SDL_Rect increaseSizeRect{ 960, -2, 30, 50 };
SDL_Rect decreaseSizeRect{ 1000, -6, 25, 50 };
SDL_Rect underlineRect{ 682, 35, 10, 2 };

SDL_Surface* rgbSurface{};
SDL_Texture* rgbTexture{};

SDL_Surface* increaseSizeSurface{};
SDL_Texture* increaseSizeTexture{};

SDL_Surface* decreaseSizeSurface{};
SDL_Texture* decreaseSizeTexture{};

SDL_Event event{};

deque<SDL_Rect> paint{};
deque<SDL_Color> paintColors{};

deque<int> paintPlaced{};
int currentPaintPlaced{};

bool button1Down{};
bool button1Up{};

bool fullscreen{};
bool maximized{};

bool backgroundChanged{};
bool pickingColor{};

bool settingCustom{};
int customPosition{};

int windowW{};
int windowH{};

int mouseX{};
int mouseY{};

// none = 0, increase = 1, decrease = 2
int paintSizeChanged{};
int paintSize{ 15 };

void loop();
void refresh();
void credits();

int main()
{
	// i wish this was toggleable
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	controller = SDL_GameControllerOpen(0);
	joystick = SDL_JoystickOpen(0);

	SDL_SetWindowIcon(window, windowIcon);

	arialFont = TTF_OpenFont("arial.ttf", 15);

	rgbSurface = TTF_RenderText_Solid(arialFont, customColorString.c_str(), SDL_Color(75, 75, 75, 255));
	rgbTexture = SDL_CreateTextureFromSurface(renderer, rgbSurface);

	credits();
	refresh();

	return 0;
}

void credits()
{
	SDL_Surface* creditsSurface{};
	SDL_Texture* creditsTexture{};
	SDL_Rect creditsRect{ 325, 250, 400, 250 };

	for (int i{ 1 }; i <= 254; ++i)
	{
		SDL_PollEvent(&event);

		if (event.type == SDL_QUIT)
		{
			SDL_DestroyWindow(window);
			SDL_DestroyRenderer(renderer);
			SDL_FreeSurface(creditsSurface);

			SDL_Quit();
			exit(1);
		}
		else if (event.type == SDL_KEYDOWN || event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_MOUSEBUTTONDOWN)
		{
			return;
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		creditsSurface = TTF_RenderText_Solid(arialFont, "chapel1337", SDL_Color(225, 140, 0, i));
		creditsTexture = SDL_CreateTextureFromSurface(renderer, creditsSurface);

		SDL_RenderCopy(renderer, creditsTexture, NULL, &creditsRect);
		SDL_RenderPresent(renderer);

		SDL_Delay(5);
	}

	for (int i{}; i <= 175; ++i)
	{
		SDL_PollEvent(&event);

		if (event.type == SDL_QUIT)
		{
			SDL_DestroyWindow(window);
			SDL_DestroyRenderer(renderer);
			SDL_FreeSurface(creditsSurface);

			SDL_Quit();
			exit(1);
		}
		else if (event.type == SDL_KEYDOWN || event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_MOUSEBUTTONDOWN)
		{
			return;
		}

		SDL_Delay(10);
	}

	for (int i{ 254 }; i >= 1; --i)
	{
		SDL_PollEvent(&event);

		if (event.type == SDL_QUIT)
		{
			SDL_DestroyWindow(window);
			SDL_DestroyRenderer(renderer);
			SDL_FreeSurface(creditsSurface);

			SDL_Quit();
			exit(1);
		}
		else if (event.type == SDL_KEYDOWN || event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_MOUSEBUTTONDOWN)
		{
			return;
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		creditsSurface = TTF_RenderText_Solid(arialFont, "chapel1337", SDL_Color(225, 140, 0, i));
		creditsTexture = SDL_CreateTextureFromSurface(renderer, creditsSurface);

		SDL_RenderCopy(renderer, creditsTexture, NULL, &creditsRect);
		SDL_RenderPresent(renderer);

		SDL_Delay(5);
	}

	SDL_FreeSurface(creditsSurface);
}

void refresh()
{
	SDL_GetWindowSize(window, &toolbarBackground.w, NULL);

	SDL_SetRenderDrawColor(renderer, backgroundColors[backgroundColors.size() - 1].r, backgroundColors[backgroundColors.size() - 1].g, backgroundColors[backgroundColors.size() - 1].b, backgroundColors[backgroundColors.size() - 1].a);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 105, 105, 105, 255);
	SDL_RenderFillRect(renderer, &toolbarBackground);

	colorRect.x = 10;

	for (int i{ 1 }; i <= 21; ++i)
	{
		if (i == 6 || i == 11 || i == 16 || i == 21)
		{
			colorRect.x += 35;
		}
		if (i != 1)
		{
			colorRect.x += 25;
		}

		switch (i)
		{
		case 1:
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			break;

		case 2:
			SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
			break;

		case 3:
			SDL_SetRenderDrawColor(renderer, 150, 0, 0, 255);
			break;

		case 4:
			SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);
			break;

		case 5:
			SDL_SetRenderDrawColor(renderer, 50, 0, 0, 255);
			break;


		case 6:
			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
			break;

		case 7:
			SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
			break;

		case 8:
			SDL_SetRenderDrawColor(renderer, 0, 150, 0, 255);
			break;

		case 9:
			SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255);
			break;

		case 10:
			SDL_SetRenderDrawColor(renderer, 0, 50, 0, 255);
			break;


		case 11:
			SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
			break;

		case 12:
			SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255);
			break;

		case 13:
			SDL_SetRenderDrawColor(renderer, 0, 0, 150, 255);
			break;

		case 14:
			SDL_SetRenderDrawColor(renderer, 0, 0, 100, 255);
			break;

		case 15:
			SDL_SetRenderDrawColor(renderer, 0, 0, 50, 255);
			break;


		case 16:
			SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
			break;

		case 17:
			SDL_SetRenderDrawColor(renderer, 200, 200, 0, 255);
			break;

		case 18:
			SDL_SetRenderDrawColor(renderer, 150, 150, 0, 255);
			break;

		case 19:
			SDL_SetRenderDrawColor(renderer, 100, 100, 0, 255);
			break;

		case 20:
			SDL_SetRenderDrawColor(renderer, 50, 50, 0, 255);
			break;


		case 21:
			SDL_SetRenderDrawColor(renderer, customColor.r, customColor.g, customColor.b, customColor.a);
			break;
		}

		SDL_RenderFillRect(renderer, &colorRect);
	}

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderDrawRect(renderer, &selectionRect);

	SDL_SetRenderDrawColor(renderer, 75, 75, 75, 255);
	if (settingCustom)
	{
		SDL_RenderFillRect(renderer, &underlineRect);
	}

	SDL_RenderCopy(renderer, rgbTexture, NULL, &rgbRect);
	
	SDL_SetRenderDrawColor(renderer, 75, 75, 75, 255);
	SDL_RenderFillRect(renderer, &changeBackgroundRect);

	if (backgroundChanged)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderDrawRect(renderer, &changeBackgroundRect);
	}

	SDL_SetRenderDrawColor(renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);
	SDL_RenderFillRectF(renderer, &changeBackgroundInnerRect);

	SDL_SetRenderDrawColor(renderer, 75, 75, 75, 255);
	SDL_RenderFillRect(renderer, &colorPickerRect);

	if (pickingColor)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderDrawRect(renderer, &colorPickerRect);
	}

	increaseSizeSurface = TTF_RenderText_Solid(arialFont, "+", SDL_Color(75, 75, 75, 255));
	increaseSizeTexture = SDL_CreateTextureFromSurface(renderer, increaseSizeSurface);

	decreaseSizeSurface = TTF_RenderText_Solid(arialFont, "-", SDL_Color(75, 75, 75, 255));
	decreaseSizeTexture = SDL_CreateTextureFromSurface(renderer, decreaseSizeSurface);

	if (paintSizeChanged == 1)
	{
		increaseSizeSurface = TTF_RenderText_Solid(arialFont, "+", SDL_Color(45, 45, 45, 255));
		increaseSizeTexture = SDL_CreateTextureFromSurface(renderer, increaseSizeSurface);
	}
	else if (paintSizeChanged == 2)
	{
		decreaseSizeSurface = TTF_RenderText_Solid(arialFont, "-", SDL_Color(45, 45, 45, 255));
		decreaseSizeTexture = SDL_CreateTextureFromSurface(renderer, decreaseSizeSurface);
	}

	SDL_RenderCopy(renderer, increaseSizeTexture, NULL, &increaseSizeRect);
	SDL_RenderCopy(renderer, decreaseSizeTexture, NULL, &decreaseSizeRect);

	for (int i{}; i < paint.size(); ++i)
	{
		SDL_SetRenderDrawColor(renderer, paintColors[i].r, paintColors[i].g, paintColors[i].b, paintColors[i].a);
		SDL_RenderFillRect(renderer, &paint[i]);
	}

	SDL_RenderPresent(renderer);

	loop();
}

void loop()
{
	while (true)
	{
		SDL_PollEvent(&event);
		SDL_GetMouseState(&mouseX , &mouseY);

		if (event.type == SDL_QUIT)
		{
			SDL_DestroyWindow(window);
			SDL_DestroyRenderer(renderer);

			SDL_FreeSurface(increaseSizeSurface);
			SDL_FreeSurface(decreaseSizeSurface);
			SDL_FreeSurface(rgbSurface);
			SDL_FreeSurface(windowIcon);

			SDL_Quit();
			exit(1);
		}
		else if (!paintPlaced.empty() && !backgroundColors.empty() && ((event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_z) || (event.type == SDL_CONTROLLERBUTTONDOWN && event.cbutton.button == SDL_CONTROLLER_BUTTON_B)))
		{
			if (paintPlaced[paintPlaced.size() - 1] == 0)
			{
				backgroundColors.pop_back();
				currentColor = backgroundColors[backgroundColors.size() - 1];
			}
			else
			{
				for (int i{}; i < paintPlaced[paintPlaced.size() - 1]; ++i)
				{
					paint.pop_back();
					paintColors.pop_back();
				}
			}

			paintPlaced.pop_back();
			refresh();
		}
		else if ((event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_f) || (event.type == SDL_CONTROLLERBUTTONDOWN && event.cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER))
		{
			if (fullscreen)
			{
				SDL_SetWindowFullscreen(window, 0);

				SDL_SetWindowSize(window, windowW, windowH);
				SDL_RestoreWindow(window);

				fullscreen = false;
			}
			else
			{
				SDL_GetWindowSize(window, &windowW, &windowH);

				SDL_MaximizeWindow(window);
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

				fullscreen = true;
			}

			refresh();
		}
		else if (!fullscreen && ((event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_m) || (event.type == SDL_CONTROLLERBUTTONDOWN && event.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)))
		{
			if (maximized)
			{
				SDL_RestoreWindow(window);

				maximized = false;
			}
			else
			{
				SDL_MaximizeWindow(window);

				maximized = true;
			}

			refresh();
		}
		else if ((event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) || (event.type == SDL_CONTROLLERBUTTONDOWN && event.cbutton.button == SDL_CONTROLLER_BUTTON_A))
		{
			SDL_GetMouseState(&mouseX, &mouseY);

			settingCustom = false;
			backgroundChanged = false;
			paintSizeChanged = 0;

			// this took way too long to implement
			if (mouseY <= 50)
			{
				if (mouseX <= 35)
				{
					selectionRect.x = 10;
					currentColor = SDL_Color(255, 0, 0, 255);
				}
				else if (mouseX >= 35 && mouseX <= 60)
				{
					selectionRect.x = 35;
					currentColor = SDL_Color(200, 0, 0, 255);
				}
				else if (mouseX >= 60 && mouseX <= 85)
				{
					selectionRect.x = 60;
					currentColor = SDL_Color(150, 0, 0, 255);
				}
				else if (mouseX >= 85 && mouseX <= 110)
				{
					selectionRect.x = 85;
					currentColor = SDL_Color(100, 0, 0, 255);
				}
				else if (mouseX >= 110 && mouseX <= 135)
				{
					selectionRect.x = 110;
					currentColor = SDL_Color(50, 0, 0, 255);
				}

				else if (mouseX >= 170 && mouseX <= 195)
				{
					selectionRect.x = 170;
					currentColor = SDL_Color(0, 255, 0, 255);
				}
				else if (mouseX >= 195 && mouseX <= 220)
				{
					selectionRect.x = 195;
					currentColor = SDL_Color(0, 200, 0, 255);
				}
				else if (mouseX >= 220 && mouseX <= 245)
				{
					selectionRect.x = 220;
					currentColor = SDL_Color(0, 150, 0, 255);
				}
				else if (mouseX >= 245 && mouseX <= 270)
				{
					selectionRect.x = 245;
					currentColor = SDL_Color(0, 100, 0, 255);
				}
				else if (mouseX >= 270 && mouseX <= 295)
				{
					selectionRect.x = 270;
					currentColor = SDL_Color(0, 50, 0, 255);
				}

				else if (mouseX >= 330 && mouseX <= 355)
				{
					selectionRect.x = 330;
					currentColor = SDL_Color(0, 0, 255, 255);
				}
				else if (mouseX >= 355 && mouseX <= 380)
				{
					selectionRect.x = 355;
					currentColor = SDL_Color(0, 0, 200, 255);
				}
				else if (mouseX >= 380 && mouseX <= 405)
				{
					selectionRect.x = 380;
					currentColor = SDL_Color(0, 0, 150, 255);
				}
				else if (mouseX >= 405 && mouseX <= 430)
				{
					selectionRect.x = 405;
					currentColor = SDL_Color(0, 0, 100, 255);
				}
				else if (mouseX >= 430 && mouseX <= 455)
				{
					selectionRect.x = 430;
					currentColor = SDL_Color(0, 0, 50, 255);
				}

				else if (mouseX >= 490 && mouseX <= 515)
				{
					selectionRect.x = 490;
					currentColor = SDL_Color(255, 255, 0, 255);
				}
				else if (mouseX >= 515 && mouseX <= 540)
				{
					selectionRect.x = 515;
					currentColor = SDL_Color(200, 200, 0, 255);
				}
				else if (mouseX >= 540 && mouseX <= 565)
				{
					selectionRect.x = 540;
					currentColor = SDL_Color(150, 150, 0, 255);
				}
				else if (mouseX >= 565 && mouseX <= 590)
				{
					selectionRect.x = 565;
					currentColor = SDL_Color(100, 100, 0, 255);
				}
				else if (mouseX >= 590 && mouseX <= 615)
				{
					selectionRect.x = 590;
					currentColor = SDL_Color(50, 50, 0, 255);
				}

				else if (mouseX >= 650 && mouseX <= 675)
				{
					selectionRect.x = 650;
					currentColor = customColor;
				}

				else if (mouseX >= 682 && mouseX <= 696)
				{
					customPosition = 0;
					underlineRect.x = 682;

					settingCustom = true;
				}
				else if (mouseX >= 696 && mouseX <= 710)
				{
					customPosition = 1;
					underlineRect.x = 696;

					settingCustom = true;
				}
				else if (mouseX >= 710 && mouseX <= 724)
				{
					customPosition = 2;
					underlineRect.x = 710;

					settingCustom = true;
				}

				else if (mouseX >= 738 && mouseX <= 752)
				{
					customPosition = 5;
					underlineRect.x = 738;

					settingCustom = true;
				}
				else if (mouseX >= 752 && mouseX <= 766)
				{
					customPosition = 6;
					underlineRect.x = 752;

					settingCustom = true;
				}
				else if (mouseX >= 766 && mouseX <= 780)
				{
					customPosition = 7;
					underlineRect.x = 766;

					settingCustom = true;
				}

				else if (mouseX >= 794 && mouseX <= 808)
				{
					customPosition = 10;
					underlineRect.x = 794;

					settingCustom = true;
				}
				else if (mouseX >= 808 && mouseX <= 822)
				{
					customPosition = 11;
					underlineRect.x = 808;

					settingCustom = true;
				}
				else if (mouseX >= 822 && mouseX <= 836)
				{
					customPosition = 12;
					underlineRect.x = 822;

					settingCustom = true;
				}

				else if (mouseX >= increaseSizeRect.x && mouseX <= increaseSizeRect.x + increaseSizeRect.w && paintSize != 35)
				{
					++paintSize;

					paintSizeChanged = 1;
				}
				else if (mouseX >= decreaseSizeRect.x && mouseX <= decreaseSizeRect.x + decreaseSizeRect.w && paintSize != 2)
				{
					--paintSize;

					paintSizeChanged = 2;
				}

				else if (mouseX >= changeBackgroundRect.x && mouseX <= changeBackgroundRect.x + changeBackgroundRect.w)
				{
					backgroundChanged = true;

					backgroundColors.push_back(currentColor);

					paintPlaced.push_back(0);
					currentPaintPlaced = 0;
				}

				else if (mouseX >= colorPickerRect.x && mouseX <= colorPickerRect.x + colorPickerRect.w)
				{
					pickingColor = true;
				}

				refresh();
			}
			else
			{
				button1Down = true;
			}
		}
		else if ((event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) || (event.type == SDL_CONTROLLERBUTTONUP && event.cbutton.button == SDL_CONTROLLER_BUTTON_A))
		{
			button1Up = true;
			button1Down = false;
		}
		else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
		{
			SDL_GetWindowSize(window, &windowW, &windowH);

			if (windowW < 1000)
			{
				SDL_SetWindowSize(window, 1075, windowH);
			}
			if (windowH < 750)
			{
				SDL_SetWindowSize(window, windowW, 750);
			}

			refresh();
		}
		else if (settingCustom && event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym == SDLK_0)
			{
				customColorString[customPosition] = '0';
			}
			else if (event.key.keysym.sym == SDLK_1)
			{
				customColorString[customPosition] = '1';
			}
			else if (event.key.keysym.sym == SDLK_2)
			{
				customColorString[customPosition] = '2';
			}

			if (customPosition != 0 && customPosition != 5 && customPosition != 10)
			{
				if (event.key.keysym.sym == SDLK_3)
				{
					customColorString[customPosition] = '3';
				}
				else if (event.key.keysym.sym == SDLK_4)
				{
					customColorString[customPosition] = '4';
				}
				else if (event.key.keysym.sym == SDLK_5)
				{
					customColorString[customPosition] = '5';
				}
			}

			if ((customPosition != 0 && customPosition != 5 && customPosition != 10) && (customPosition != 1 && customPosition != 6 && customPosition != 11))
			{
				if (event.key.keysym.sym == SDLK_6)
				{
					customColorString[customPosition] = '6';
				}
				else if (event.key.keysym.sym == SDLK_7)
				{
					customColorString[customPosition] = '7';
				}
				else if (event.key.keysym.sym == SDLK_8)
				{
					customColorString[customPosition] = '8';
				}
				else if (event.key.keysym.sym == SDLK_9)
				{
					customColorString[customPosition] = '9';
				}
			}

			rgbSurface = TTF_RenderText_Solid(arialFont, customColorString.c_str(), SDL_Color(75, 75, 75, 255));
			rgbTexture = SDL_CreateTextureFromSurface(renderer, rgbSurface);

			// hell
			customColor = SDL_Color(stoi(customColorString.substr(0, 3)), stoi(customColorString.substr(5, 7)), stoi(customColorString.substr(10, 12)), 255);

			if (selectionRect.x == 650)
			{
				currentColor = customColor;
			}

			refresh();
		}

		if (pickingColor && button1Down)
		{
			for (int i{}; i < paint.size(); ++i)
			{
				if (mouseX >= paint[i].x && mouseX <= paint[i].x + paint[i].w && mouseY >= paint[i].y && mouseY <= paint[i].y + paint[i].h) // && (paintColors[i].r != backgroundColor.r && paintColors[i].g != backgroundColor.g && paintColors[i].b != backgroundColor.b))
				{
					currentColor = paintColors[i];
				}
			}

			pickingColor = false; 
			button1Down = false;

			refresh();
		}
		else if (button1Down)
		{
			SDL_Rect paintDot{ 0, 0, paintSize, paintSize };

			SDL_GetMouseState(&paintDot.x, &paintDot.y);
			// SDL_GameControllerRumble(controller, 0xFFFF, 0xFFFF, 10);

			paintDot.x -= paintSize / 2;
			paintDot.y -= paintSize / 2;

			if (paintDot.y >= 50)
			{
				++currentPaintPlaced;

				paint.push_back(paintDot);
				paintColors.push_back(currentColor);

				SDL_SetRenderDrawColor(renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);
				SDL_RenderFillRects(renderer, &paintDot, 5);

				SDL_RenderPresent(renderer);
			}                                          
		}
		else if (button1Up && currentPaintPlaced != 0)                                                                                                                                       
		{
			paintPlaced.push_back(currentPaintPlaced);
			currentPaintPlaced = 0;
		}
	}
}
