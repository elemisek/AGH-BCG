#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "font.h"

class Menu : public sf::Drawable
{
private:
	sf::Font font;
	sf::Text* text;
	sf::RectangleShape* rectangle;

	sf::Texture* colors_texture;
	sf::Sprite* colors_sprite;
	sf::Uint8* colors_pixels;
	const unsigned int colors_size_x = 765; // colour stripes width
	const unsigned int colors_size_y = 60; // colour stripes height
	// fill colour stripes
	inline void draw_to_color_pixels(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b)
	{
		colors_pixels[4 * (y * colors_size_x + x) + 0] = r;
		colors_pixels[4 * (y * colors_size_x + x) + 1] = g;
		colors_pixels[4 * (y * colors_size_x + x) + 2] = b;
		colors_pixels[4 * (y * colors_size_x + x) + 3] = 255;
	}

public:
	Menu()
	{
		font.loadFromMemory(font_data, font_data_size);
		text = new sf::Text;
		text->setFont(font);
		text->setCharacterSize(12);
		text->setFillColor(sf::Color::White);

		rectangle = new sf::RectangleShape(sf::Vector2f(796, 536)); // white frame around canvas
		rectangle->setFillColor(sf::Color::Transparent);
		rectangle->setOutlineColor(sf::Color::White);
		rectangle->setOutlineThickness(1.0f);
		rectangle->setPosition(2, 62);

		unsigned int x, y;
		colors_pixels = new sf::Uint8[colors_size_x * colors_size_y * 4];
		for (x = 0; x < 255; x++)
			for (y = 0; y < 30; y++)
			{
				draw_to_color_pixels(x, y, x, 255, 0);
				draw_to_color_pixels(x + 255, y, 255, 255 - x, 0);
				draw_to_color_pixels(x + 510, y, 255, 0, x);
				draw_to_color_pixels(254 - x, y + 30, 0, 255, 255 - x);
				draw_to_color_pixels(509 - x, y + 30, 0, x, 255);
				draw_to_color_pixels(764 - x, y + 30, 255 - x, 0, 255);
			}

		colors_texture = new sf::Texture();
		colors_texture->create(colors_size_x, colors_size_y); // set colour stripes size
		colors_texture->update(colors_pixels); // fill with colours

		colors_sprite = new sf::Sprite();
		colors_sprite->setTexture(*colors_texture);
		colors_sprite->setPosition(1, 1); // start from left upper corner
	}

	void outtextxy(sf::RenderTarget& target, float x, float y, const wchar_t* str) const
	{
		text->setPosition(x, y); //  postions inside text menu
		text->setString(str);
		target.draw(*text);
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		outtextxy(target, 5, 600, L"f - wybór koloru rysowania"); 
		outtextxy(target, 5, 615, L"b - wybór koloru wypelniania");
		outtextxy(target, 5, 630, L"l - rysowanie linii");

		outtextxy(target, 200, 600, L"r - rysowanie prostokata");
		outtextxy(target, 200, 615, L"a - rysowanie wypelnionego prostok?ta");
		outtextxy(target, 200, 630, L"c - rysowanie okr?gu");

		outtextxy(target, 470, 600, L"w - zapis do pliku");
		outtextxy(target, 470, 615, L"o - odczyt z pliku");
		outtextxy(target, 470, 630, L"esc - wyj?cie");

		outtextxy(target, 650, 615, L"Aktualne:");

		target.draw(*rectangle); // drawing the white frame around the canvas
		target.draw(*colors_sprite); // drawing colour stripes
	}
};

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 650), "GFK Lab 01", sf::Style::Titlebar | sf::Style::Close);
	sf::Event e;

	sf::Texture buffer_texture;
	sf::Texture drawing_texture;
	sf::Sprite buffer_sprite;
	sf::Image colour_palete; // colour stripes

	sf::Font font;
	sf::Text* text;

	char key_state = ' '; // which key was pressed
	bool do_draw = false;
	int mouse_x_start = 0, mouse_y_start = 0; // starting mouse position

	// shapes
	sf::VertexArray line(sf::LinesStrip, 2);
	sf::CircleShape circle;
	sf::RectangleShape rectangle;

	sf::Color contour_colour = sf::Color::Black; // black contour colour
	sf::Color fill_colour = sf::Color::Black; // black fill colour

	Menu menu;
	window.setFramerateLimit(60);

	buffer_texture.create(800, 650);
	drawing_texture.create(800, 650);
	colour_palete.create(765, 60); // colour stripes

	buffer_sprite.setTexture(buffer_texture);

	font.loadFromMemory(font_data, font_data_size);
	text = new sf::Text;
	text->setFont(font);
	text->setCharacterSize(12);
	text->setFillColor(sf::Color::White);

	window.clear(sf::Color::Black); // set background colour to black

	window.draw(menu);

	drawing_texture.update(window);
	colour_palete = drawing_texture.copyToImage();
	window.clear(sf::Color::Black);
	drawing_texture.update(window);

	while (window.isOpen())
	{
		window.clear(sf::Color::Black);

		while (window.pollEvent(e))
		{
			if (e.type == sf::Event::Closed)
			{
				window.close();
			}
			else if (e.type == sf::Event::KeyPressed)
			{
				switch (e.key.code)
				{
				case 0: // a
					key_state = 'a';
					break;
				case 1: // b
					key_state = 'b';
					break;
				case 2: // c
					key_state = 'c';
					break;
				case 5: // f
					key_state = 'f';
					break;
				case 11: // l
					key_state = 'l';
					break;
				case 14: // o
					key_state = 'o';
					buffer_texture.loadFromFile("image.png"); // loading canvas from .png file
					break;
				case 17: // r
					key_state = 'r';
					break;
				case 22: // w
					key_state = 'w';
					buffer_texture.copyToImage().saveToFile("image.png"); // saving canvas to .png file
					break;
				case 36: // esc
					window.close();
					break;
				}
			}
			else if (e.type == sf::Event::MouseButtonPressed) // mouse pressed, start drawing
			{
				do_draw = true;
				mouse_x_start = sf::Mouse::getPosition(window).x;
				mouse_y_start = sf::Mouse::getPosition(window).y;
			}
			else if (e.type == sf::Event::MouseButtonReleased) // mouse released, end drawing
			{
				do_draw = false;
				buffer_texture.update(drawing_texture.copyToImage()); // update frames

				switch (key_state)
				{
				case 'f':
					// get mouse position for contour colour if it's within the color stripes
					if (sf::Mouse::getPosition(window).y <= 60)
						contour_colour = colour_palete.getPixel(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
					key_state = ' ';
					break;
				case 'b':
					// get mouse position for fill colour if it's within the color stripes
					if (sf::Mouse::getPosition(window).y <= 60)
						fill_colour = colour_palete.getPixel(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
					key_state = ' ';
					break;
				}
			}
		}
		
		window.draw(buffer_sprite);
		if (do_draw)
		{
			switch (key_state)
			{
			case 'l': // draw line
				line[0].position = sf::Vector2f(static_cast<float> (mouse_x_start), static_cast<float> (mouse_y_start));
				line[0].color = contour_colour;
				line[1].position = sf::Vector2f(static_cast<float> (sf::Mouse::getPosition(window).x), static_cast<float> (sf::Mouse::getPosition(window).y));
				line[1].color = fill_colour;
				window.draw(line);
				break;
			case 'c': // draw circle
				circle.setRadius(sqrt(static_cast<float> (pow(sf::Mouse::getPosition(window).x - static_cast<float> (mouse_x_start), 2) + pow(sf::Mouse::getPosition(window).y - static_cast<float> (mouse_y_start), 2))) / 2);
				circle.setOutlineColor(contour_colour);
				circle.setFillColor(fill_colour);
				circle.setOutlineThickness(2.0f);
				circle.setOrigin(sf::Vector2f(0, circle.getRadius()));
				circle.setPosition(sf::Vector2f(static_cast<float> (mouse_x_start), static_cast<float> (mouse_y_start - circle.getRadius())));
				circle.move(0, circle.getRadius());
				circle.setRotation(0);
				circle.rotate(static_cast<float> (atan(static_cast<float> (sf::Mouse::getPosition(window).y - static_cast<float> (mouse_y_start)) / static_cast<float>(sf::Mouse::getPosition(window).x - static_cast<float> (mouse_x_start))) * 180.0 / 3.14159265));
				circle.setScale(1, 1);
				if (sf::Mouse::getPosition(window).x - mouse_x_start < 0)
				{
					circle.setScale(-1, 1);
				}
				window.draw(circle);
				break;
			case 'r': // draw empty rectangle
				rectangle.setSize(sf::Vector2f(static_cast<float> (sf::Mouse::getPosition(window).x) - mouse_x_start, static_cast<float> (sf::Mouse::getPosition(window).y) - mouse_y_start));
				rectangle.setFillColor(sf::Color::Transparent);
				rectangle.setOutlineColor(contour_colour);
				rectangle.setOutlineThickness(2.0f);
				rectangle.setPosition(static_cast<float> (mouse_x_start), static_cast<float> (mouse_y_start));
				window.draw(rectangle);
				break;
			case 'a': // draw filled rectangle
				rectangle.setSize(sf::Vector2f(static_cast<float> (sf::Mouse::getPosition(window).x - static_cast<float> (mouse_x_start)), (static_cast<float> (sf::Mouse::getPosition(window).y - static_cast<float> (mouse_y_start)))));
				rectangle.setFillColor(fill_colour);
				rectangle.setOutlineColor(contour_colour);
				rectangle.setOutlineThickness(2.0f);
				rectangle.setPosition(static_cast<float> (mouse_x_start), static_cast<float> (mouse_y_start));
				window.draw(rectangle);
				break;
			}
		}
		drawing_texture.update(window);
		rectangle.setSize(sf::Vector2f(34, 30));
		rectangle.setFillColor(contour_colour);

		rectangle.setOutlineColor(sf::Color::Transparent);
		rectangle.setOutlineThickness(0.0f);
		rectangle.setPosition(765, 1);
		window.draw(rectangle);
		rectangle.setFillColor(fill_colour);
		rectangle.setPosition(765, 30);
		window.draw(rectangle);

		window.draw(menu);

		text->setPosition(650, 630);
		text->setString(key_state);
		window.draw(*text);

		window.display();

	}
	return 0;
}