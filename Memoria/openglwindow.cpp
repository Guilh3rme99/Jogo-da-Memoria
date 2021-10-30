#include "openglwindow.hpp"

#include <imgui.h>

#include <cppitertools/itertools.hpp>
#include <map>

#include "abcg.hpp"

void OpenGLWindow::handleEvent(SDL_Event &event) {
  // Mouse events
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
      x = ((mousePosition.x / (double)600) * 2) - 1;
      y = -(((mousePosition.y / (double)600) * 2) - 1);
      m_gameData.m_input.set(static_cast<size_t>(Input::Click));
    }
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_LEFT)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Click));
  }
}

void OpenGLWindow::initializeGL() {
  // Load a new font
  const ImGuiIO &io{ImGui::GetIO()};
  const auto filename{getAssetsPath() + "Inconsolata-Medium.ttf"};
  m_font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 30.0f);
  const auto *vertexShader{R"gl(
    #version 410

    layout(location = 0) in vec2 inPosition;
    layout(location = 1) in vec4 inColor;

    uniform vec2 translation;
    uniform float scale;

    out vec4 fragColor;

    void main() {
      vec2 newPosition = inPosition * scale + translation;
      gl_Position = vec4(newPosition, 0, 1);
      fragColor = inColor;
    }
  )gl"};

  const auto *fragmentShader{R"gl(
    #version 410

    in vec4 fragColor;

    out vec4 outColor;

    void main() { outColor = fragColor; }
  )gl"};

  // Create shader program
  m_program = createProgramFromString(vertexShader, fragmentShader);

  // Clear window
  abcg::glClearColor(0, 0, 0, 1);
  abcg::glClear(GL_COLOR_BUFFER_BIT);

  // Setar variaveis no estado inicial
  iniciar();
}

void OpenGLWindow::paintGL() {
  if(win){
    if(win2){
      Sleep(3000);
      iniciar();
    }else{
      win2 = true;
    }
  }
  
  std::vector<std::tuple<float, float>>::iterator pos;
  std::vector<std::tuple<float, float>>::iterator remove;
  for (pos = inicialPos.begin(); pos != inicialPos.end(); pos++) {
    const glm::vec3 azul{30.0f / 255, 144.0f / 255, 255.0f / 255};
    setupModel(4, azul);

    abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

    abcg::glUseProgram(m_program);

    const glm::vec2 translation{std::get<0>(*pos), std::get<1>(*pos)};
    const GLint translationLocation{
        abcg::glGetUniformLocation(m_program, "translation")};
    abcg::glUniform2fv(translationLocation, 1, &translation.x);

    std::uniform_real_distribution<float> rd2(0.01f, 0.25f);
    const auto scale{0.25f};
    const GLint scaleLocation{abcg::glGetUniformLocation(m_program, "scale")};
    abcg::glUniform1f(scaleLocation, scale);

    // Render
    abcg::glBindVertexArray(m_vao);
    abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, 4 + 2);
    abcg::glBindVertexArray(0);

    abcg::glUseProgram(0);
  }
  if (m_gameData.m_input[static_cast<size_t>(Input::Click)]) {
    m_gameData.m_input.reset(static_cast<size_t>(Input::Click));
    for (pos = inicialPos.begin(); pos != inicialPos.end(); pos++) {
      if (x >= std::get<0>(*pos) - 0.176776695f &&
          x <= std::get<0>(*pos) + 0.176776695f &&
          y >= std::get<1>(*pos) - 0.176776695f &&
          y <= std::get<1>(*pos) + 0.176776695f) {
        const glm::vec3 branco{1.0f, 1.0f, 1.0f};
        setupModel(4, branco);

        abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);
        abcg::glUseProgram(m_program);

        const glm::vec2 translation{std::get<0>(*pos), std::get<1>(*pos)};
        const GLint translationLocation{
            abcg::glGetUniformLocation(m_program, "translation")};
        abcg::glUniform2fv(translationLocation, 1, &translation.x);

        const auto scale{0.20f};
        const GLint scaleLocation{
            abcg::glGetUniformLocation(m_program, "scale")};
        abcg::glUniform1f(scaleLocation, scale);

        // Render
        abcg::glBindVertexArray(m_vao);
        abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, 4 + 2);
        abcg::glBindVertexArray(0);
        abcg::glUseProgram(0);

        std::tuple<float, float> atual{std::get<0>(*pos), std::get<1>(*pos)};

        if (!click1) {
          click1 = true;
          flip1 = atual;
        } else {
          click2 = true;
          flip2 = atual;
        }

        remove = pos;

        setupModel(nLados[atual], cores[nLados[atual]]);
        abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

        abcg::glUseProgram(m_program);

        const glm::vec2 translation2{std::get<0>(*pos), std::get<1>(*pos)};
        const GLint translationLocation2{
            abcg::glGetUniformLocation(m_program, "translation")};
        abcg::glUniform2fv(translationLocation2, 1, &translation2.x);

        const auto scale2{0.10f};
        const GLint scaleLocation2{
            abcg::glGetUniformLocation(m_program, "scale")};
        abcg::glUniform1f(scaleLocation2, scale2);

        // Render
        abcg::glBindVertexArray(m_vao);
        abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, nLados[atual] + 2);
        abcg::glBindVertexArray(0);
        abcg::glUseProgram(0);
      }
    }
    if (click1 && click2) {
      if (nLados[flip1] == nLados[flip2]) {
        inicialPos.erase(remove);
        n = 0;
        flip1 = std::tuple<float, float>(0, 0);
        flip2 = std::tuple<float, float>(0, 0);
      } else {
        ;
        inicialPos.erase(remove);
        errou = true;
        n = 0;
      }
      click1 = false;
      click2 = false;
    } else if (click1 && n == 0) {
      inicialPos.erase(remove);
      n = 1;
    }
  }
  if (errou) {
    if (rend) {
      Sleep(1000);
      inicialPos.push_back(flip2);
      inicialPos.push_back(flip1);
      errou = false;
      rend = false;
      flip1 = std::tuple<float, float>(0, 0);
      flip2 = std::tuple<float, float>(0, 0);
    } else {
      rend = true;
    }
  }
  if (inicialPos.empty()) {
    m_gameData.m_state = State::Win;
  }
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();
  {
    const auto size{ImVec2(600, 650)};
    const auto position{ImVec2((m_viewportWidth - size.x) / 2.0f,
                               ((m_viewportHeight - size.y) / 2.0f))};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                           ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoInputs};
    ImGui::Begin(" ", nullptr, flags);
    if (m_gameData.m_state == State::Win && !win2) {
      std::cout<< "Win\n";
      ImGui::PushFont(m_font);
      ImGui::SetCursorPos(ImVec2(200.0f, 550.0f));
      ImGui::SetCursorPosY( 75.0f);
      ImGui::Text("You Win!!!");
      ImGui::PopFont();
      win = true;
    }
    std::cout<< "Not!!!!!\n";
    ImGui::End();
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_vboPositions);
  abcg::glDeleteBuffers(1, &m_vboColors);
  abcg::glDeleteVertexArrays(1, &m_vao);
}

void OpenGLWindow::setupModel(int sides, const glm::vec3 color) {
  // Release previous resources, if any
  abcg::glDeleteBuffers(1, &m_vboPositions);
  abcg::glDeleteBuffers(1, &m_vboColors);
  abcg::glDeleteVertexArrays(1, &m_vao);

  const glm::vec3 color1 = color;
  const glm::vec3 color2 = color;

  std::vector<glm::vec2> positions(0);
  std::vector<glm::vec3> colors(0);

  // Polygon center
  positions.emplace_back(0, 0);
  colors.push_back(color1);

  // Border vertices
  const auto step{M_PI * 2.0 / sides};
  for (const auto angle : iter::range(M_PI * 0.25, M_PI * 2, step)) {
    positions.emplace_back(std::cos(angle), std::sin(angle));
    colors.push_back(color2);
  }

  // Duplicate second vertex
  positions.push_back(positions.at(1));
  colors.push_back(color2);

  // Generate VBO of positions
  abcg::glGenBuffers(1, &m_vboPositions);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate VBO of colors
  abcg::glGenBuffers(1, &m_vboColors);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  abcg::glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3),
                     colors.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  const auto positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};
  const auto colorAttribute{abcg::glGetAttribLocation(m_program, "inColor")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_vao);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboPositions);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glEnableVertexAttribArray(colorAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  abcg::glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void OpenGLWindow::iniciar() {
  abcg::glClearColor(0, 0, 0, 1);
  abcg::glClear(GL_COLOR_BUFFER_BIT);
  
  m_gameData.m_input.set(static_cast<size_t>(Input::NotClick));
  m_gameData.m_state = State::Playing;

  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  std::array<int, 12> shuf{4, 4, 3, 3, 5, 5, 6, 6, 7, 7, 8, 8};
  unsigned num = std::chrono::system_clock::now().time_since_epoch().count();
  std::shuffle(shuf.begin(), shuf.end(), std::default_random_engine(num));

  inicialPos.push_back(std::tuple<float, float>(-0.75f, -0.5f));
  nLados[std::tuple<float, float>(-0.75f, -0.5f)] = shuf[0];

  inicialPos.push_back(std::tuple<float, float>(-0.3f, -0.5f));
  nLados[std::tuple<float, float>(-0.3f, -0.5f)] = shuf[1];

  inicialPos.push_back(std::tuple<float, float>(0.15f, -0.5f));
  nLados[std::tuple<float, float>(0.15f, -0.5f)] = shuf[2];

  inicialPos.push_back(std::tuple<float, float>(0.6f, -0.5f));
  nLados[std::tuple<float, float>(0.6f, -0.5f)] = shuf[3];

  inicialPos.push_back(std::tuple<float, float>(-0.75f, 0.0f));
  nLados[std::tuple<float, float>(-0.75f, 0.0f)] = shuf[4];

  inicialPos.push_back(std::tuple<float, float>(-0.3f, 0.0f));
  nLados[std::tuple<float, float>(-0.3f, 0.0f)] = shuf[5];

  inicialPos.push_back(std::tuple<float, float>(0.15f, 0.0f));
  nLados[std::tuple<float, float>(0.15f, 0.0f)] = shuf[6];

  inicialPos.push_back(std::tuple<float, float>(0.6f, 0.0f));
  nLados[std::tuple<float, float>(0.6f, 0.f)] = shuf[7];

  inicialPos.push_back(std::tuple<float, float>(-0.75f, 0.5f));
  nLados[std::tuple<float, float>(-0.75f, 0.5f)] = shuf[8];

  inicialPos.push_back(std::tuple<float, float>(-0.3f, 0.5f));
  nLados[std::tuple<float, float>(-0.3f, 0.5f)] = shuf[9];

  inicialPos.push_back(std::tuple<float, float>(0.15f, 0.5f));
  nLados[std::tuple<float, float>(0.15f, 0.5f)] = shuf[10];

  inicialPos.push_back(std::tuple<float, float>(0.6f, 0.5f));
  nLados[std::tuple<float, float>(0.6f, 0.5f)] = shuf[11];

  cores[3] = glm::vec3(0.0f, 255.0f / 255, 127.0f / 255);
  cores[4] = glm::vec3(178.0f / 255, 34.0f / 255, 340.0f / 255);
  cores[5] = glm::vec3(148.0f / 255, 0.0f / 255, 211.0f / 255);
  cores[6] = glm::vec3(255.0f / 255, 255.0f / 255, 0.0f / 255);
  cores[7] = glm::vec3(255.0f / 255, 0.0f / 255, 0.0f / 255);
  cores[8] = glm::vec3(0.0f / 255, 0.0f / 255, 0.0f / 255);

  click1 = false;
  click2 = false;
  errou = false;
  rend = false;
  win = false;
  win2 = false;
  n = 0;
}