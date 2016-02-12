// Robin Heinemann Copyright (C) 2015

#include <iostream>
#include <chrono>
#include <cstdio>
#include <cmath>
#include <csignal>
#include <fstream>

#include <GL/glew.h>

#include <Stopwatch.h>

#include "Window.h"
#include "config.h"
#include "ShaderProgram.h"
#include "SoundProcessor.h"
#include "Server.h"
#include "Client.h"
#include "PosClient.h"

Server * server;

constexpr int FREQUENCY_INCREMENT = 0;

typedef struct {
	unsigned int vbo;
	unsigned int vao;
} buffer;

typedef struct {
	unsigned int id;
} ObjectInfo;

typedef union {
    struct {
	    double x;
	    double y;
	    double z;
	};

	double pos[3];
} v3;

double listenerPos[3 * 4] = {
	1, 0, -0.707,
	-1, 0, -0.707,
	0, 1, 0.707,
	0, -1, 0.707,
};

int glew_init() {
	glewExperimental = true;

	int status = glewInit();

    if (status != GLEW_OK) {
        std::cout << "glewInit error: " << glewGetErrorString(status) << std::endl;
        exit(-1);
    }

	std::cout << "GL_VERSION : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GL_VENDOR : " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "GL_RENDERER : " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "GLEW_VERSION : " << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "GLSL VERSION : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    if (!GLEW_VERSION_3_3) {
        std::cout << "Atleast version 3.3 of OpenGL is needed for this program to work!" << std::endl;
		return -1;
    }

	return 0;
}

int init_listeners(std::vector<float> &points_buffer, SoundProcessor &sound_processor, float radius) {
	int count = 4;
	double line_length = 0.28;

	points_buffer.push_back(0);
	points_buffer.push_back(0);
	points_buffer.push_back(radius);
	points_buffer.push_back(1);
	points_buffer.push_back(0);
	points_buffer.push_back(0);

	points_buffer.push_back(0);
	points_buffer.push_back(1 * line_length);
	points_buffer.push_back(radius);
	points_buffer.push_back(1);
	points_buffer.push_back(0);
	points_buffer.push_back(0);

	points_buffer.push_back(sin(M_PI * (60.0 / 180.0)) * line_length);
	points_buffer.push_back(0.5 * line_length);
	points_buffer.push_back(radius);
	points_buffer.push_back(1);
	points_buffer.push_back(0);
	points_buffer.push_back(0);

	points_buffer.push_back(0.5 * line_length * tan(M_PI * (30.0 / 180.0)));
	points_buffer.push_back(0.5 * line_length);
	points_buffer.push_back(radius);
	points_buffer.push_back(1);
	points_buffer.push_back(0);
	points_buffer.push_back(0);

	return count;
}

void terminate(int signal) {
	server->close();

	exit(EXIT_SUCCESS);
}


int main(int argc, char ** argv) {
	Window * window = new Window(800, 600, APPLICATION_NAME);
	buffer points;
	buffer lines;
	int count = 1;

	std::vector<float> lines_buffer;
	std::vector<float> points_buffer;
	std::vector<ObjectInfo> obj_buffer;

	bool mouseDown = false;
	int mouseX = 0, mouseY = 0, mouseDX = 0, mouseDY = 0, screenSizeX = window->getSize().x, screenSizeY =  window->getSize().y;

	float scale[2] = {1.0, 1.0}, centerX = 0.0, centerY = 0.0, mouseDXScreen = 0.0, mouseDYScreen = 0.0;
	float radius = 50;
	unsigned int samplerate = 192000;

	std::vector<SoundProcessor::v3> listener;

	std::signal(SIGTERM, terminate);
	std::signal(SIGINT, terminate);
	std::signal(SIGABRT, terminate);


	double distBetween = 0.28;
	listener.push_back(SoundProcessor::v3(0, 0, 0));
	listener.push_back(SoundProcessor::v3(0, distBetween, 0));
	listener.push_back(SoundProcessor::v3(sin(M_PI* (60.0 / 180.0)) * distBetween, 0.5 * distBetween, 0));
	listener.push_back(SoundProcessor::v3(tan((30.0 / 180.0) * M_PI) * (distBetween / 2.0), distBetween / 2.0, distBetween * 0.333333 * sqrt(6)));

	SoundProcessor soundProcessor(samplerate, listener);

	glew_init();

	int listener_count = init_listeners(points_buffer, soundProcessor, radius);

	server = new Server(atoi(argv[1]), [listener](sf::TcpSocket * socket) {
			unsigned int size = listener.size();

			socket->send(&size, sizeof(int));
			std::cout << "client connected: " << socket->getRemoteAddress() << ":" << socket->getRemotePort() << std::endl;
		});

	Client client(argv[2], atoi(argv[3]));

	count = listener.size();

	glGenBuffers(1, &points.vbo);
	glGenVertexArrays(1, &points.vao);

	glBindBuffer(GL_ARRAY_BUFFER, points.vbo);
	glBufferData(GL_ARRAY_BUFFER, 6 * count * sizeof(float), points_buffer.data(), GL_STREAM_DRAW);
	glBindVertexArray(points.vao);

	ShaderProgram * shaderProgram = new ShaderProgram("#version 130\n"
													  "uniform vec2 center;\n"
													  "uniform vec2 scale;\n"
													  "in vec3 vp;\n"
													  "in vec3 color;\n"
													  "out vec3 Color;\n"
													  "void main() {\n"
													  "   gl_Position = vec4((vp.x + center.x) / scale.x, (vp.y - center.y) / scale.y, 0.0, 1.0);\n"
													  "   gl_PointSize = vp.z / scale.x;\n"
													  "   Color = color;\n"
													  "}\n"
													  , "#version 130\n"
													  "in vec3 Color;\n"
													  "out vec4 frag_colour;\n"
													  "void main () {\n"
													  "    gl_FragColor = vec4(Color, 1.0);\n"
 													  "}");

	shaderProgram->vertexAttribPointer("color", 3, GL_FLOAT, false, 24, (void *) 12);
	shaderProgram->vertexAttribPointer("vp", 3, GL_FLOAT, false, 24, 0);

	lines_buffer.push_back(-1);
	lines_buffer.push_back(0);
	lines_buffer.push_back(1);
	lines_buffer.push_back(0);
	lines_buffer.push_back(1);
	lines_buffer.push_back(0);

	lines_buffer.push_back(1);
	lines_buffer.push_back(0);
	lines_buffer.push_back(1);
	lines_buffer.push_back(0);
	lines_buffer.push_back(1);
	lines_buffer.push_back(0);

	lines_buffer.push_back(0);
	lines_buffer.push_back(-1);
	lines_buffer.push_back(1);
	lines_buffer.push_back(0);
	lines_buffer.push_back(1);
	lines_buffer.push_back(0);

	lines_buffer.push_back(0);
	lines_buffer.push_back(1);
	lines_buffer.push_back(1);
	lines_buffer.push_back(0);
	lines_buffer.push_back(1);
	lines_buffer.push_back(0);

	glGenBuffers(1, &lines.vbo);
	glGenVertexArrays(1, &lines.vao);

	glBindBuffer(GL_ARRAY_BUFFER, lines.vbo);
	glBufferData(GL_ARRAY_BUFFER, lines_buffer.size() * sizeof(float), lines_buffer.data(), GL_STREAM_DRAW);
	glBindVertexArray(lines.vao);

	shaderProgram->vertexAttribPointer("color", 3, GL_FLOAT, false, 24, (void *) 12);
	shaderProgram->vertexAttribPointer("vp", 3, GL_FLOAT, false, 24, 0);

    glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_PROGRAM_POINT_SIZE);

    glPointSize(20.0);
	glLineWidth(3.0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glClearColor(1, 1, 1, 1);

	screenSizeX = window->getSize().x;
	screenSizeY = window->getSize().y;

	float freq = 500;
	auto now = std::chrono::high_resolution_clock::now();
	auto lastTime = std::chrono::high_resolution_clock::now();

	Stopwatch::getInstance().setCustomSignature(32435);

	double aX = 0, aY = 0, sradius = 1.4;
	double lastX = 0, lastY = 0, lastZ = 0;
	int iterations;

	std::ofstream outfile;
	outfile.open ("pos_high_res.csv");

	//PosClient posclient(argv[4], std::atoi(argv[5]));

	while (window->open()) {
		TICK("simulation_total");
		TICK("simulation_process_events");

		auto events = window->pollEvents();

		for(auto event : events) {
			switch (event.type) {
			case sf::Event::Closed: {
				window->close();
				break;
			}
			case sf::Event::Resized: {
                glViewport(0, 0, event.size.width, event.size.height);
				screenSizeX = event.size.width;
				screenSizeY = event.size.height;
				break;
			}
			case sf::Event::MouseButtonPressed: {
				if(event.mouseButton.button == sf::Mouse::Left) {
					mouseDown = true;

					mouseX = sf::Mouse::getPosition().x;
					mouseY = sf::Mouse::getPosition().y;
				}
				else if(event.mouseButton.button == sf::Mouse::Right) {
					float x = event.mouseButton.x, y = event.mouseButton.y, dx, dy;
					bool add = true;

					x = (2.0 * (x / screenSizeX) - 1.0) * scale[0] - centerX;
					y = -(2.0 * (y / screenSizeY) - 1.0) * scale[1] + centerY;

					for(int i = (points_buffer.size() / 6) - 1; i > listener.size() - 1; i--) {
						dx = (points_buffer[6 * i] - x) * screenSizeX * scale[0];
						dy = (points_buffer[6 * i + 1] - y) * screenSizeY * scale[1];

						if(sqrt(dx * dx + dy * dy) < radius * scale[0]) {
							soundProcessor.remove(points_buffer[6 * i], points_buffer[6 * i + 1]);
							points_buffer.erase(points_buffer.begin() + 6 * i - 1, points_buffer.begin() + 6 * i + 5);
							obj_buffer.erase(obj_buffer.begin() + i - listener.size());

							count--;
							add = false;

							break;
						}
					}

					if(add) {
						points_buffer.push_back(x);
						points_buffer.push_back(y);
						points_buffer.push_back(radius);
						points_buffer.push_back(0);
						points_buffer.push_back(0);
						points_buffer.push_back(0);

						ObjectInfo oinfo;
						SoundProcessor::SoundObject * obj = new SoundProcessor::SoundObject(x, y, 0, freq);

						oinfo.id = soundProcessor.add(obj);

						obj_buffer.push_back(oinfo);

						std::cout << "adding with freq: " << freq << std::endl;

						//freq += 0;
						freq += FREQUENCY_INCREMENT;

						count++;
					}

					glBindBuffer(GL_ARRAY_BUFFER, points.vbo);
					glBufferData(GL_ARRAY_BUFFER, 6 * count * sizeof(float), points_buffer.data(), GL_STREAM_DRAW);
				}
				break;
			}
			case sf::Event::MouseButtonReleased: {
				if(event.mouseButton.button == sf::Mouse::Left) {
					mouseDown = false;
					centerX = centerX - mouseDXScreen;
					centerY = centerY - mouseDYScreen;
					mouseDXScreen = 0.0;
					mouseDYScreen = 0.0;
				}
				break;
			}
			case sf::Event::MouseWheelScrolled: {
				scale[0] *= (event.mouseWheelScroll.delta > 0 ? 0.9 : 1.11111111);
				scale[1] *= (event.mouseWheelScroll.delta > 0 ? 0.9 : 1.11111111);

				break;
			}
			default:
				break;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);

		if (mouseDown) {
			mouseDX = mouseX - sf::Mouse::getPosition().x;
			mouseDY = mouseY - sf::Mouse::getPosition().y;

			mouseDXScreen = ((((double) mouseDX * 2) / (double) screenSizeX)) * scale[0];
			mouseDYScreen = ((((double) mouseDY * 2) / (double) screenSizeY)) * scale[1];
		}

		TOCK("simulation_process_events");

		double duration = (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock().now() - lastTime).count()) / 1000000000.0;

		if(duration > 2) {
			if(client.buffer != nullptr) {
				outfile << lastX << ", "
						<< lastY << ", "
						<< lastZ << ", "
						<< client.buffer[0] << ", "
						<< client.buffer[1] << ", "
						<< client.buffer[2] << std::endl;
			}


			lastTime = std::chrono::high_resolution_clock().now();

			soundProcessor.remove(lastX, lastY);
			if(iterations > 0)
				points_buffer.erase(points_buffer.begin() + points_buffer.size() - 7, points_buffer.begin() + points_buffer.size() - 1);

			double x = sradius * sin(aX) * sin(aY);
			double y = sradius * cos(aX) * sin(aY);
			double z = sradius * cos(aY);

			x += 0.080829;
			y += 0.14;
			z += 0.0571548;

			points_buffer.push_back(x);
			points_buffer.push_back(y);
			points_buffer.push_back(radius);
			points_buffer.push_back(0);
			points_buffer.push_back(0);
			points_buffer.push_back(0);

			SoundProcessor::SoundObject * obj = new SoundProcessor::SoundObject(x, y, z, freq);
			soundProcessor.add(obj);
//			posclient.setPosition(x, y, z);

			lastX = x;
			lastY = y;
			lastZ = z;

			aX += ((2.0 * M_PI) / 360.0) * 2.0;
			if(iterations % 180 == 0) {
				aY += ((2.0 * M_PI) / 360.0) * 2.0;
			}
            if(iterations % (180 * 180) == 0) {
				sradius += 0.5;
			}

			iterations++;

			glBindBuffer(GL_ARRAY_BUFFER, points.vbo);
			glBufferData(GL_ARRAY_BUFFER, 6 * count * sizeof(float), points_buffer.data(), GL_STREAM_DRAW);
		}

		double time = (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock().now() - now).count()) / 1000000000.0;
		now = std::chrono::high_resolution_clock().now();
		unsigned int samples = (float) samplerate * time;

		//std::cout << __PRETTY_FUNCTION__ << ": samples = " << samples;

		TICK("simulation_generate_samples");

		double * current_samples = soundProcessor.sample(samples);

		TOCK("simulation_generate_samples");
/*
		for(int i = 0; i < samples * listener.size(); i++) {
			std::cout << current_samples[i] << std::endl;
		}
*/
		server->send(current_samples, samples * listener.size());

		free(current_samples);


		TICK("simulation_draw");

//		glBindBuffer(GL_ARRAY_BUFFER,  points.vbo);
//		shaderProgram->useProgram();
	    glBindVertexArray(points.vao);

		shaderProgram->uniform2f("center", centerX - mouseDXScreen, centerY - mouseDYScreen);
		shaderProgram->uniform2f("scale", scale[0], scale[1]);

		glBufferData(GL_ARRAY_BUFFER, 6 * count * sizeof(float), points_buffer.data(), GL_STREAM_DRAW);

		glDrawArrays(GL_POINTS, 0, count);

		glBindVertexArray(lines.vao);

		std::vector<float> data = client.getPoints();

		lines_buffer.clear();
		lines_buffer.insert(lines_buffer.begin(), data.begin(), data.end());

		glBindBuffer(GL_ARRAY_BUFFER, lines.vbo);
//		glBufferData(GL_ARRAY_BUFFER, lines_buffer.size() * sizeof(float), lines_buffer.data(), GL_STREAM_DRAW);

//		glDrawArrays(GL_LINES, 0, lines_buffer.size() / 6);

		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STREAM_DRAW);

//		if(count > listener.size())
//			glDrawArrays(GL_LINES, 0, data.size() / 6);

		if(count > listener.size())
			glDrawArrays(GL_POINTS, 0, data.size() / 6);

		window->display();
		TOCK("simulation_draw");

		TOCK("simulation_total");

        Stopwatch::getInstance().sendAll();
    }

	terminate(0);

	return 0;
}
