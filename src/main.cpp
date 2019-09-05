#include <stdio.h>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

#include <Genode.hpp>
#include <Nx/IO/OpiArchive.hpp>

namespace Nx
{
    class SplashScreen : public Gx::Scene
    {
    private:
        sf::Texture* texture;
        Gx::Sprite* sprite = nullptr;
        Gx::Text* text = nullptr;
        sf::Font* font = nullptr;

    public:
        virtual void Stage()
        {
            auto resource = Gx::ResourceContainer::Instance();
 
            texture = resource->Load<sf::Texture>("D:/O2Jam/Assets/StateWaiting/Background.png");
            sprite = new Gx::Sprite(*texture); //resource->Cache<sf::Texture>("D:/O2Jam/Assets/StateWaiting/Background.png")
            AddChild(sprite);

            font = resource->Load<sf::Font>("D:/O2Jam/Assets/Global/Font.ttf");
            //font = new sf::Font();
            //font->loadFromFile("D:/O2Jam/Assets/Global/Font.ttf");
            text = new Gx::Text("Testing Abc", *font); //resource->Cache<sf::Font>("D:/O2Jam/Assets/Global/Font.ttf")
            AddChild(text);

            auto sequence = new Gx::Sequence({
                new Gx::Fade(sprite, 0, 5000),
                new Gx::Fade(sprite, 255, 5000)
            });

            Run(sequence);
            
            auto timeline = new Gx::Timeline();
            timeline->Add(
                new Gx::Timeline::KeyFrame(1500, new Gx::Fade(sprite, 0, 1000)), 
                new Gx::Timeline::KeyFrame(2000, new Gx::Fade(sprite, 255, 1000))
            );

            auto archive = resource->AddArchive<OpiArchive>("D:/O2Jam/Client/O2Jam Live/Image/Interface.opi");
            if (archive && archive->Contains("abusedata.ojs"))
                std::printf("exists");

            //Run(timeline);
        }

        virtual void OnKeyDown(sf::Event::KeyEvent ev) 
        {
            auto cache = Gx::ResourceContainer::Instance();
            if (ev.code == sf::Keyboard::Space)
                RemoveChild(sprite);
            else if (ev.code == sf::Keyboard::Enter)
            {
                sprite = new Gx::Sprite(
                    *cache->Cache<sf::Texture>("D:\\final-fantasy-xv.jpg")
                );
                
                AddChild(sprite);
            }
        }
    };

    class TestModule : public Gx::Module, public Gx::TaskContainer
    {
    public:
        Gx::Sequence* sequence;

        TestModule() 
            : Gx::Module(0)
        {
            auto callback = std::bind(&TestModule::Print, this);
            sequence = Gx::Sequence::Routine(
                new Gx::Action(callback), 50, 1000
            );

            //Run(sequence);
        }

        void Print()
        {
            static int sec = 0;
            //std::printf("%d\n", ++sec);
        }

        virtual void Update(double delta)
        {
            TaskContainer::Update(delta);
            //std::printf("FPS: %d \n", Gx::Application::Instance()->GetRenderFrequency());
        }
    };
}

int main() 
{
    sf::Texture hcursor;
    hcursor.loadFromFile("D:/O2Jam/Assets/Global/CursorSingle.png");
    auto image = hcursor.copyToImage();

    Gx::Cursor cursor = Gx::Cursor(hcursor);
    Gx::Application application(new Nx::SplashScreen(), sf::VideoMode(1024, 768), sf::VideoMode(800, 600));

    application.SetCursor(cursor);
    application.Install<Nx::TestModule>();

    int exitCode = application.Start();

    return exitCode;
}

