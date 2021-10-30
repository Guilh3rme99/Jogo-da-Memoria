#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif


#include "gamedata.hpp"
#include "abcg.hpp"
#include <imgui.h>
#include <map>
#include <iostream>
#include <random>
#include <algorithm> 
#include <chrono>


class OpenGLWindow : public abcg::OpenGLWindow {
 public:
  std::vector<std::tuple<float, float>> inicialPos;
  std::map<std::tuple<float, float>, int> nLados;
  std::map<int, glm::vec3> cores;
  void setupModel(int sides, const glm::vec3 color);
  std::tuple<float, float> flip1;
  std::tuple<float, float> flip2;
  bool click1;
  bool click2;
  int n;
  bool errou;
  bool rend;
  bool win;
  bool win2;

 protected:
  void handleEvent(SDL_Event& event) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;
  void iniciar();

  //std::vector<std::tuple<float, float>> inicialPos;



 private:
  GLuint m_vao{};
  GLuint m_vboPositions{};
  GLuint m_vboColors{};
  GLuint m_program{};

  GameData m_gameData;

  int m_viewportWidth{};
  int m_viewportHeight{};

  std::default_random_engine m_randomEngine;
  glm::ivec2 mousePosition;
  double x;
  double y;
  ImFont* m_font{};

  int m_delay{200};
  abcg::ElapsedTimer m_elapsedTimer;

};

#endif