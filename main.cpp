#include <iostream>
#include <cmath>
#include <thread>
#include <string>
#include <map>
#include <SFML/Graphics.hpp>
#include <chrono>

namespace {
	bool greenOrange{ false };

	constexpr auto wWIDTH{ 768 }, wHEIGHT{ 768 };

	constexpr auto PI{ 3.14159265359 }, dPI{ 6.28318530718 };
	int precision{ 5 }, alpha{ 0 }, beta{ 0 }, scale{ 35 };

	std::map<std::string, int*> setMap{
		{ "precision", &precision },
		{ "alpha", &alpha },
		{ "beta", &beta },
		{ "scale", &scale },
	};
}

auto makeCurve() {
	std::vector<sf::Vertex> curve;

	for (double i{ -PI }; i <= PI; i += PI / (50.0 * precision)) {
		curve.push_back(sf::Vertex(sf::Vector2f(
			(wWIDTH * (scale / 100.0)) * std::sin(i * alpha) + wWIDTH / 2, wHEIGHT / 2 + std::cos(i * beta) * (wHEIGHT * (scale / 100.0)) // x = sin(i * alpha) y = cos(i * beta)
		), (greenOrange) ? sf::Color::White : sf::Color::Black));

		// std::cout << "i: " << i << "\tx: " << curve[i].position.x << "\ty: " << curve[i].position.y << "\tsin(i): " << std::sin(i) << "\tcos(4i): " << std::cos(i * 4) << '\n';
	}

	return curve;
}

inline void drawAxes(sf::RenderWindow& w) {
	const static sf::Color col(192, 192, 192);

	static sf::Vertex horizontalLine[]{
		sf::Vertex(sf::Vector2f(wWIDTH / 2, 0), col),
		sf::Vertex(sf::Vector2f(wWIDTH / 2, wHEIGHT), col)
	};
	static sf::Vertex verticalLine[]{
		sf::Vertex(sf::Vector2f(0, wHEIGHT / 2), col),
		sf::Vertex(sf::Vector2f(wWIDTH, wHEIGHT / 2), col)
	};

	w.draw(horizontalLine, 2, sf::LinesStrip);
	w.draw(verticalLine, 2, sf::LinesStrip);
}

void drawGreenOrange(sf::RenderWindow& w) {
	static sf::Vertex box[]{
		sf::Vertex(sf::Vector2f(0, 0), sf::Color(255, 64, 0)),
		sf::Vertex(sf::Vector2f(wWIDTH, 0), sf::Color(128, 191, 0)),
		sf::Vertex(sf::Vector2f(wWIDTH, wHEIGHT), sf::Color(0, 255, 0)),
		sf::Vertex(sf::Vector2f(0, wHEIGHT), sf::Color(191, 128, 0)),
	};

	static bool goGreen[]{ true, true, false, false };

	w.draw(box, 4, sf::TriangleFan);

	for (char i{}; i < 4; ++i) {
		if (goGreen[i]) {
			box[i].color.r--;
			box[i].color.g++;
		} else {
			box[i].color.r++;
			box[i].color.g--;
		}

		if (box[i].color.g == 255 && goGreen[i])
			goGreen[i] = false;
		else if (box[i].color.g == 64 && !goGreen[i])
			goGreen[i] = true;
	}
}

inline auto consoleListener(std::vector<sf::Vertex>& c, sf::RenderWindow& rw) {
	return [&]() {
		std::string query;
		std::cout << std::fixed;
		while (true) {
			std::cout << ">>> ";
			std::cin >> query;

			if (query == "set") {
				std::cin >> query;
				if (setMap[query] != nullptr) std::cin >> *setMap[query];
				else std::cout << "Could not define: " << query;
			} else if (query == "get") {
				std::cin >> query;
				if (setMap[query] != nullptr) std::cout << query << " = " << *setMap[query];
				else std::cout << "Could not define: " << query;
			} else if (query == "exit") {
				rw.close();
				break;
			} else if (query == "help") {
				std::cout <<
					"set\t: sets value for a specified variable\n"
					"get\t: gets value for a specified variable\n"
					"exit\t: exits application\n"
					"help\t: shows this text\n"
					"compile\t: recalculate curve\n"
					"green-orange\t: a special render";
			} else if (query == "compile") {
				using clock = std::chrono::steady_clock;
				auto start = clock::now();
				c = makeCurve();
				auto elapsed = clock::now() - start;
				std::cout << "Compiled in " << elapsed.count();
			} else if (query == "green-orange") {
				greenOrange = !greenOrange;
				c = makeCurve();
			} else std::cout << "Could not get query: " << query;

			std::cout << std::endl;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cin.clear();
		}
	};
}

int main() {
	std::vector<sf::Vertex> c;
	sf::RenderWindow rw(sf::VideoMode(wWIDTH, wHEIGHT), L"Lissajous Curve", sf::Style::Close, sf::ContextSettings(0, 0, 8));

	std::thread t(consoleListener(c, rw));

	rw.setFramerateLimit(30);

	for (sf::Event ev; rw.isOpen();) {
		while (rw.pollEvent(ev))
			if (ev.type == sf::Event::Closed)
				rw.close();

		if (greenOrange) drawGreenOrange(rw); else rw.clear(sf::Color(240, 240, 240));

		drawAxes(rw);

		rw.draw(&c[0], c.size(), sf::LinesStrip);
		rw.display();
	}

	t.join();
}
