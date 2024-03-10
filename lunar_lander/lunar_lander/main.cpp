/**
* Author: Jaylan Wu
* Assignment: Lunar Lander
* Date due: 2024-03-09, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define NUMBER_OF_ENEMIES 3
#define FIXED_TIMESTEP 0.0166666f
#define ACC_OF_GRAVITY -9.81f
#define PLATFORM_COUNT 4
#define BORDER_COUNT 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"

// ————— STRUCTS AND ENUMS —————//
struct GameState
{
    Entity* background;
    Entity* player;
    Entity* platforms;
    // Entity* border;
};

// ————— CONSTANTS ————— //
const int WINDOW_WIDTH = 640 * 1.5,
          WINDOW_HEIGHT = 480 * 1.5;

const int FONTBANK_SIZE = 16;

const float BG_RED = 0.0f,
            BG_BLUE = 0.0f,
            BG_GREEN = 0.0f,
            BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;
const char  BACKGROUND_FILEPATH[]  = "assets/background.jpg",
            PLATFORM_FILEPATH[]    = "assets/image4.png",
            HAERIN_FILEPATH[]      = "assets/haerin.png",
            FONT_FILEPATH[]        = "assets/font1.png";

const int NUMBER_OF_TEXTURES = 1;  // to be generated, that is
const GLint LEVEL_OF_DETAIL  = 0;  // base image level; Level n is the nth mipmap reduction image
const GLint TEXTURE_BORDER   = 0;  // this value MUST be zero

// ————— VARIABLES ————— //
GameState g_game_state;

SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_shader_program;
GLuint g_font_texture_id;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_time_accumulator = 0.0f;

// List of Platform Locations
glm::vec3 g_platform_locations[] = {glm::vec3(-3.2f, -3.0f, 0.0f),
                                    glm::vec3(0.5f, -4.0f, 0.0f),
                                    glm::vec3(4.0f, -3.4f, 0.0f)};

// List of Platform Scores
float     g_platform_scores[]   = {1.5f, 1.0f, 2.0f};

// ———— GENERAL FUNCTIONS ———— //
GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return textureID;
}

void draw_text(ShaderProgram *program, GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position)
{
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;

    std::vector<float> vertices;
    std::vector<float> texture_coordinates;

    for (int i = 0; i < text.size(); i++) {
        int spritesheet_index = (int) text[i];
        float offset = (screen_size + spacing) * i;
        
        float u_coordinate = (float) (spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
        float v_coordinate = (float) (spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

        vertices.insert(vertices.end(), {
            offset + (-0.5f * screen_size), 0.5f * screen_size,
            offset + (-0.5f * screen_size), -0.5f * screen_size,
            offset + (0.5f * screen_size), 0.5f * screen_size,
            offset + (0.5f * screen_size), -0.5f * screen_size,
            offset + (0.5f * screen_size), 0.5f * screen_size,
            offset + (-0.5f * screen_size), -0.5f * screen_size,
        });

        texture_coordinates.insert(texture_coordinates.end(), {
            u_coordinate, v_coordinate,
            u_coordinate, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate + width, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate, v_coordinate + height,
        });
    }

    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);
    
    program->set_model_matrix(model_matrix);
    glUseProgram(program->get_program_id());
    
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates.data());
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int) (text.size() * 6));
    
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("NewJeans Lunar Lander!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix       = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    // ————— FONT ————— //
    g_font_texture_id = load_texture(FONT_FILEPATH);
    
    // ————— BACKGROUND ————— //
    g_game_state.background = new Entity();
    g_game_state.background->set_position(glm::vec3(0.0f));
    g_game_state.background->set_size(glm::vec3(12.0f, 8.0f, 0.0f));
    g_game_state.background->m_texture_id = load_texture(BACKGROUND_FILEPATH);
    g_game_state.background->update(0.0f, NULL, 0);

    // ————— PLAYER ————— //
    g_game_state.player = new Entity();
    g_game_state.player->set_position(glm::vec3(0.0f, 3.5f, 0.0f));
    g_game_state.player->set_movement(glm::vec3(0.0f));
    g_game_state.player->set_acceleration(glm::vec3(0.0f, ACC_OF_GRAVITY * 0.01, 0.0f));
    g_game_state.player->set_speed(1.0f);
    g_game_state.player->set_score(100);
    g_game_state.player->m_texture_id = load_texture(HAERIN_FILEPATH);
    g_game_state.player->set_size(glm::vec3(0.5f, 0.5f, 0.0f));
    

    // ————— PLATFORM ————— //
    g_game_state.platforms = new Entity[PLATFORM_COUNT];

    for (int i = 0; i < PLATFORM_COUNT-1; i++)
    {
        g_game_state.platforms[i].set_position(g_platform_locations[i]);
        g_game_state.platforms[i].set_size(glm::vec3(2.0f, 2.0f, 0.0f));
        g_game_state.platforms[i].set_score(g_platform_scores[i]);
        g_game_state.platforms[i].set_type("platform");
        g_game_state.platforms[i].m_texture_id = load_texture(PLATFORM_FILEPATH);
        g_game_state.platforms[i].update(0.0f, NULL, 0);
    }
    
    // ————— BORDER ————— //
    g_game_state.platforms[3].set_position(glm::vec3(0.0f, -4.5f, 0.0f));
    g_game_state.platforms[3].set_size(glm::vec3(20.0f, 1.0f, 0.0f));
    g_game_state.platforms[3].set_type("border");
    g_game_state.platforms[3].update(0.0f, NULL, 0);

    // ————— GENERAL ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    // VERY IMPORTANT: If nothing is pressed, we don't want to go anywhere
    g_game_state.player->set_acceleration(glm::vec3(0.0f, ACC_OF_GRAVITY * 0.01, 0.0f));

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            // End game
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_game_is_running = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                // Quit the game with a keystroke
                g_game_is_running = false;
                break;

            default:
                break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (g_game_state.player->get_score() > 0){
        if (key_state[SDL_SCANCODE_LEFT])
        {
            g_game_state.player->move_left();
            g_game_state.player->m_animation_indices = g_game_state.player->m_walking[g_game_state.player->LEFT];
        }
        else if (key_state[SDL_SCANCODE_RIGHT])
        {
            g_game_state.player->move_right();
            g_game_state.player->m_animation_indices = g_game_state.player->m_walking[g_game_state.player->RIGHT];
        }
    }

    // This makes sure that the player can't move faster diagonally
    if (glm::length(g_game_state.player->get_movement()) > 1.0f)
    {
        g_game_state.player->set_movement(glm::normalize(g_game_state.player->get_movement()));
    }
}

void update()
{
    // ————— DELTA TIME ————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND; // get the current number of ticks
    float delta_time = ticks - g_previous_ticks; // the delta time is the difference from the last frame
    g_previous_ticks = ticks;

    // ————— FIXED TIMESTEP ————— //
    delta_time += g_time_accumulator;
    if (delta_time < FIXED_TIMESTEP)
    {
        g_time_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP)
    {
        if (g_game_state.player->get_collision() == "") {
            g_game_state.player->update(FIXED_TIMESTEP, g_game_state.platforms, PLATFORM_COUNT);
        }
        delta_time -= FIXED_TIMESTEP;
    }
    g_time_accumulator = delta_time;
}

void render()
{
    // ————— GENERAL ————— //
    glClear(GL_COLOR_BUFFER_BIT);
    
    // ————— BACKGROUND ————— //
    g_game_state.background->render(&g_shader_program);

    // If the player is not collided with anything
    if (g_game_state.player->get_collision() == "") {
        // ————— PLAYER ————— //
        g_game_state.player->render(&g_shader_program);

        // ————— PLATFORM ————— //
        for (int i = 0; i < PLATFORM_COUNT; i++) g_game_state.platforms[i].render(&g_shader_program);
        
        // ————— FONT ————— //
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << g_game_state.player->get_score();
        std::string character_score = ss.str();
        draw_text(&g_shader_program, g_font_texture_id, std::string("Power:") + character_score,
                  0.35f, -0.15f, glm::vec3(-4.75f, 3.5f, 0.0f));
    } 
    // if the player collides with the border
    else if (g_game_state.player->get_collision() == "border") {
        draw_text(&g_shader_program, g_font_texture_id, std::string("Game Over!"),
                  0.8f, -0.3f, glm::vec3(-1.85f, 0.0f, 0.0f));
    }
    // if the player lands on a platform
    else if (g_game_state.player->get_collision() == "platform") {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << g_game_state.player->get_score();
        std::string character_score = ss.str();
        draw_text(&g_shader_program, g_font_texture_id, std::string("Score:") + character_score,
                  0.8f, -0.3f, glm::vec3(-2.4f, -0.5f, 0.0f));
        draw_text(&g_shader_program, g_font_texture_id, std::string("Success!"),
                  0.8f, -0.3f, glm::vec3(-1.65f, 0.5f, 0.0f));
    }

    // ————— GENERAL ————— //
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() { SDL_Quit(); }

// ————— DRIVER GAME LOOP ————— /
int main(int argc, char* argv[])
{
    initialise();

    while (g_game_is_running)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}
