#include <algorithm>
#include <iostream>
#include <array>
#include <numeric>
#include <functional>

#include <imgui.h>
#include <imgui-SFML.h>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Bloom.hpp"
#include "Lib.hpp"
#include "Game.hpp"
#include "HotReloadShader.hpp"
#include "app.h"
#include "C.hpp"


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Player.hpp"
#include "SecondOrderDynamics.h"

extern "C" {
	// Force the use of the NVidia-graphics card on notebooks with both an IGP and a GPU
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	// AMD equivalent to the above
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

using namespace std;
using namespace sf;

static HotReloadShader * bloomShader = nullptr;
static HotReloadShader * blurShader = nullptr;

static std::array<double, 60> dts;
static int curDts = 0;

int main()
{
    sf::RenderWindow window(sf::VideoMode(C::RES_X, C::RES_Y,32), "SFML works!", sf::Style::Fullscreen);
	window.setVerticalSyncEnabled(false);
	Font font;

	// load font
    if (!font.loadFromFile("res/MAIAN.TTF")) {
        cout << "ERROR NO FONT" << '\n';
        return 1;
    }

	// check shader available
	if (!sf::Shader::isAvailable())
	{
		cout << "ERROR NO SHADER SYSTEM" << '\n';
		return 1;
	}

	// Manage sound
	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile("mainSound.wav")) return 1;
	
	sf::Sound sound;
	sound.setBuffer(buffer);
	sound.setVolume(20);
	sound.setLoop(true);
	//sound.play();

	// init ImGui SFML
	ImGui::SFML::Init(window);

	// init game
    Game g(&window);
	Player& player = g.getPlayer();
	
	// init Camera
	sf::Vector2f cameraPos ={ 0 , 0};
	SecondOrderDynamics cameraDynamic(g.f, g.z, g.r, cameraPos);
	View v = window.getDefaultView();
	Vector2f viewCenter = v.getCenter();
	float zoom = g.zoom;
	v.zoom(zoom);

	sf::Clock timer;
	
	sf::Text fpsCounter;
	fpsCounter.setFont(font);
	fpsCounter.setString("FPS:");
	
	double frameStart = 0.0;
	double frameEnd = 0.0;

	sf::Texture winTex;
	winTex.create(window.getSize().x, window.getSize().y);
	
	bloomShader = new HotReloadShader("res/simple.vert", "res/bloom.frag");
	blurShader = new HotReloadShader("res/simple.vert", "res/blur.frag");
	sf::RenderTexture* destX = new sf::RenderTexture();
	destX->create(window.getSize().x, window.getSize().y);
	destX->clear(sf::Color(0, 0, 0, 0));

	sf::RenderTexture* destFinal = new sf::RenderTexture();
	destFinal->create(window.getSize().x, window.getSize().y);
	destFinal->clear(sf::Color(0, 0, 0, 0));	

	float bloomWidth = 0 ;
	sf::Glsl::Vec4 bloomMul(1,1,1,0.8f);

	
    while (window.isOpen())
    {
		double dt = frameEnd - frameStart;
		frameStart = Lib::getTimeStamp();
		dt = std::max(dt, 0.00000001);
    	
		sf::Event event;
		while (window.pollEvent(event))//sort un evenement de la liste pour le traiter
		{
			ImGui::SFML::ProcessEvent(event);
			g.processInput(event);
			
			if (event.type == sf::Event::Resized) {
				auto nsz = window.getSize();
				winTex.create(window.getSize().x, window.getSize().y);

				destX->create(window.getSize().x, window.getSize().y);
				destX->clear(sf::Color(0, 0, 0, 0));

				destFinal->create(window.getSize().x, window.getSize().y);
				destFinal->clear(sf::Color(0, 0, 0, 0));

				v = sf::View(Vector2f(nsz.x * 0.5f, nsz.y * 0.5f), Vector2f((float)nsz.x, (float)nsz.y));
				viewCenter = v.getCenter();
			}
		}		
    	
    	//don't use imgui before this;
    	ImGui::SFML::Update(window, sf::seconds((float)dt));

		
    	
		if (ImGui::CollapsingHeader("View")) {
			auto sz = v.getSize();
			ImGui::Value("size x", sz.x);
			ImGui::Value("size y", sz.y);
		}
		if (ImGui::CollapsingHeader("App Stats", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen)) {
			//double df = (Lib::getTimeStamp() - frameStart);

			double mdt = std::accumulate(dts.begin(),dts.end(),0.0) / dts.size();
			ImGui::LabelText("Update Time", "%0.6f", dt);
			ImGui::LabelText("FPS", "%0.6f", 1.0 / dt);

			static double captureMdt = 0.0;
			if (curDts == 0) {
				captureMdt = mdt;
			}
			ImGui::LabelText("Avg Update Time", "%0.6f", captureMdt);
			ImGui::LabelText("Avg FPS", "%0.6f", 1.0 / captureMdt);
		}
		if (ImGui::CollapsingHeader("Bloom Control")) {
			ImGui::SliderFloat("bloomWidth", &bloomWidth, 0, 55);//55 is max acceptable kernel size for constants, otherwise we should use a texture
			ImGui::ColorEdit4("bloomMul", &bloomMul.x);
			ImGui::ColorEdit4("bloomMul2", &bloomMul.x);
		}    	
    	
    	g.update(dt);
    	g.cameraView = v;
		g.im();
    	g.draw(window);
		window.draw(fpsCounter);

    	// Bloom Management
    	if (blurShader) blurShader->update(dt);
    	if (bloomShader) bloomShader->update(dt);
    	if (bloomWidth)
    	{
    		window.setView(window.getDefaultView());
    		Bloom::render(window,winTex,destX,destFinal,&blurShader->sh,&bloomShader->sh, bloomWidth, bloomMul);
    	}
    	if (!g.isEditing)
    	{
    		float posXDiff = 100;
                	float posYDiff = player.crouching ? 20 : 0;
                	if (player.moveRight) posXDiff *= -1;
                	cameraDynamic.setParams(g.f, g.z, g.r);
                	cameraPos = cameraDynamic.Update(dt,{ (float)player.getPosPixel().x - posXDiff,(float) player.getPosPixel().y + posYDiff});
                	v.setCenter(cameraPos);
                	v.zoom(g.zoom / zoom);
                	zoom = g.zoom;
                	window.setView(v);//keep view up to date in case we want to do something with like... you know what.
    	}
	    else
	    {
		    window.setView(window.getDefaultView());
	    }
    	
    	
		ImGui::SFML::Render(window);
        window.display();
		frameEnd = Lib::getTimeStamp();
		fpsCounter.setString("FPS: "+std::to_string(1.0 / dt));
		ImGui::EndFrame();
    	
		curDts++;
		if (curDts >= dts.size()) {
			curDts = 0;
		}
		dts[curDts] = dt;

    	window.clear();
    }

	ImGui::SFML::Shutdown();

    return 0;
}

// Exécuter le programme : Ctrl+F5 ou menu Déboguer > Exécuter sans débogage
// Déboguer le programme : F5 ou menu Déboguer > Démarrer le débogage

// Astuces pour bien démarrer : 
//   1. Utilisez la fenêtre Explorateur de solutions pour ajouter des fichiers et les gérer.
//   2. Utilisez la fenêtre Team Explorer pour vous connecter au contrôle de code source.
//   3. Utilisez la fenêtre Sortie pour voir la sortie de la génération et d'autres messages.
//   4. Utilisez la fenêtre Liste d'erreurs pour voir les erreurs.
//   5. Accédez à Projet > Ajouter un nouvel élément pour créer des fichiers de code, ou à Projet > Ajouter un élément existant pour ajouter des fichiers de code existants au projet.
//   6. Pour rouvrir ce projet plus tard, accédez à Fichier > Ouvrir > Projet et sélectionnez le fichier .sln.
