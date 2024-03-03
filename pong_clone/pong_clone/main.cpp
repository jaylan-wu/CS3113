/**
* Author: Jaylan Wu
* Assignment: Pong Clone
* Date due: 2024-03-02, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_GLEXT_PROTOTYPES 1
#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

// —— NEW STUFF —— //
#include <ctime>   //
#include "cmath"   //
// ——————————————— //

const int WINDOW_WIDTH  = 640 * 1.5,
          WINDOW_HEIGHT = 480 * 1.5;

const float BG_RED     = 1.0f,
            BG_BLUE    = 1.0f,
            BG_GREEN   = 1.0f,
            BG_OPACITY = 1.0f;

const int VIEWPORT_X      = 0,
          VIEWPORT_Y      = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char  V_SHADER_PATH[]          = "shaders/vertex_textured.glsl",
            F_SHADER_PATH[]          = "shaders/fragment_textured.glsl",
            LSRFM_BALL[]             = "assets/lsrfm_ball.png",
            LSRFM_BOARD[]            = "assets/lsrfm_board.png",
            BORDER[]                 = "assets/border.png",
            GAME_OVER[]              = "assets/game_over.png";

const float MILLISECONDS_IN_SECOND     = 1000.0;
const float MINIMUM_COLLISION_DISTANCE = 1.0f;

const int   NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL    = 0;
const GLint TEXTURE_BORDER     = 0;

SDL_Window* g_display_window;
bool  g_game_is_running = true;
bool  g_computer_player = false;
float g_previous_ticks  = 0.0f;

ShaderProgram g_shader_program;
glm::mat4     g_view_matrix,
              g_projection_matrix,
              g_ball_1_matrix,
              g_ball_2_matrix,
              g_ball_3_matrix,
              g_left_board_matrix,
              g_right_board_matrix,
              g_top_border_matrix,
              g_bottom_border_matrix,
              g_right_border_matrix,
              g_left_border_matrix,
              g_game_over_matrix;

GLuint g_ball_texture_id,
       g_board_texture_id,
       g_border_texture_id,
       g_game_over_texture_id;

// -------- BALL INFORMATION -------- //
glm::vec3 g_ball_1_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball_1_movement = glm::vec3(1.0f, 0.25f, 0.0f);

glm::vec3 g_ball_2_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball_2_movement = glm::vec3(0.75F, -0.10f, 0.0f);

glm::vec3 g_ball_3_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball_3_movement = glm::vec3(-0.5f, 0.2f, 0.0f);

const float BALL_WIDTH = 0.75f;
const float BALL_HEIGHT = 0.75f;
const float g_ball_speed = 3.0f;
float g_ball_rotation_angle = 0.0f;
const float g_ball_rotation_speed = -60.0f;
int g_number_of_balls = 1;

// -------- BOARD INFORMATION -------- //
// LEFT BOARD //
glm::vec3 g_left_board_position  = glm::vec3(-4.0f, 0.0f, 0.0f);
glm::vec3 g_left_board_movement  = glm::vec3(0.0f, 0.0f, 0.0f);
// RIGHT BOARD //
glm::vec3 g_right_board_position  = glm::vec3(4.0f, 0.0f, 0.0f);
glm::vec3 g_right_board_movement  = glm::vec3(0.0f, 0.0f, 0.0f);

const float BOARD_WIDTH = 0.5f;
const float BOARD_HEIGHT = 2.5f;
const float g_board_speed = 3.0f;

// -------- BORDER INFORMATION -------- //
// TOP BORDER //
glm::vec3 g_top_border_position     = glm::vec3(0.0f, 4.15f, 0.0f);
// BOTTOM BORDER //
glm::vec3 g_bottom_border_position  = glm::vec3(0.0f, -4.15f, 0.0f);
// LEFT BORDER //
glm::vec3 g_left_border_position    = glm::vec3(-5.4f, 0.0f, 0.0f);
// RIGHT BORDER //
glm::vec3 g_right_border_position   = glm::vec3(5.4f, 0.0f, 0.0f);

const float BORDER_WIDTH     = 10.0f;
const float BORDER_HEIGHT    = 8.0f;
const float BORDER_THICKNESS = 1.0f;

// -------- GAME OVER INFORMATION -------- //
glm::vec3 g_game_over_location = glm::vec3(0.0f, 0.0f, 0.0f);

bool  g_game_is_lost = false;


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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(image);
    
    return textureID;
}


void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("LE SSERAFIM PONG",
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
    
    g_ball_1_matrix = glm::mat4(1.0f);
    g_ball_2_matrix = glm::mat4(1.0f);
    g_ball_3_matrix = glm::mat4(1.0f);
    
    g_left_board_matrix  = glm::mat4(1.0f);
    g_right_board_matrix = glm::mat4(1.0f);
    
    g_top_border_matrix    = glm::mat4(1.0f);
    g_top_border_matrix    = glm::translate(g_top_border_matrix, g_top_border_position);
    g_top_border_matrix    = glm::scale(g_top_border_matrix, glm::vec3(BORDER_WIDTH, BORDER_THICKNESS, 1.0f));
    
    g_bottom_border_matrix = glm::mat4(1.0f);
    g_bottom_border_matrix = glm::translate(g_bottom_border_matrix, g_bottom_border_position);
    g_bottom_border_matrix    = glm::scale(g_bottom_border_matrix, glm::vec3(BORDER_WIDTH, BORDER_THICKNESS, 1.0f));

    g_left_border_matrix   = glm::mat4(1.0f);
    g_left_border_matrix   = glm::translate(g_left_border_matrix, g_left_border_position);
    g_left_border_matrix   = glm::scale(g_left_border_matrix, glm::vec3(BORDER_THICKNESS, BORDER_HEIGHT, 1.0f));

    g_right_border_matrix  = glm::mat4(1.0f);
    g_right_border_matrix  = glm::translate(g_right_border_matrix, g_right_border_position);
    g_right_border_matrix  = glm::scale(g_right_border_matrix, glm::vec3(BORDER_THICKNESS, BORDER_HEIGHT, 1.0f));
    
    g_game_over_matrix = glm::mat4(1.0f);
    g_game_over_matrix = glm::scale(g_game_over_matrix, glm::vec3(2.5f, 2.5f, 1.0f));

    g_view_matrix       = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_ball_texture_id      = load_texture(LSRFM_BALL);
    g_board_texture_id     = load_texture(LSRFM_BOARD);
    g_border_texture_id    = load_texture(BORDER);
    g_game_over_texture_id = load_texture(GAME_OVER);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_shader_program.get_program_id());
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_left_board_movement  = glm::vec3(0.0f);
    g_right_board_movement = glm::vec3(0.0f);
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_game_is_running = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_q:
                        g_game_is_running = false;
                        break;
                        
                    case SDLK_t:
                        g_computer_player = !g_computer_player;
                        break;
                        
                    case SDLK_1:
                        g_number_of_balls = 1;
                        break;
                        
                    case SDLK_2:
                        g_number_of_balls = 2;
                        break;
                        
                    case SDLK_3:
                        g_number_of_balls = 3;
                        break;
                        
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    
    if (key_state[SDL_SCANCODE_UP])
    {
        g_right_board_movement.y = 1.0f;
    }
    else if (key_state[SDL_SCANCODE_DOWN])
    {
        g_right_board_movement.y = -1.0f;
    }
    
    if (key_state[SDL_SCANCODE_W] && !g_computer_player)
    {
        g_left_board_movement.y = 1.0f;
    }
    else if (key_state[SDL_SCANCODE_S] && !g_computer_player)
    {
        g_left_board_movement.y = -1.0f;
    }
}

bool check_collision(glm::vec3 &position_a, glm::vec3 &position_b, float width_a, float width_b, float height_a, float height_b)
{
    float collision_factor = 0.9f;
    
    float x_distance = fabs(position_a.x - position_b.x) - ((width_a + width_b) * collision_factor / 2.0f);
    float y_distance = fabs(position_a.y - position_b.y) - ((height_a + height_b) * collision_factor / 2.0f);
    
    return ((x_distance <= 0.0f) && (y_distance <= 0.0f));
}

void update()
{
    // -------- COLLISION DETECTION -------- //
    // Boards and Ball //
    if (check_collision(g_ball_1_position, g_right_board_position, BALL_WIDTH, BOARD_WIDTH, BALL_HEIGHT, BOARD_HEIGHT) ||
        check_collision(g_ball_1_position, g_left_board_position, BALL_WIDTH, BOARD_WIDTH, BALL_HEIGHT, BOARD_HEIGHT)) {
        g_ball_1_movement.x *= -1;
        if (g_ball_1_position.x < 0) {
            g_ball_1_position.x += 0.05;
        } else {
            g_ball_1_position.x -= 0.05;
        }
    }
    if (check_collision(g_ball_2_position, g_right_board_position, BALL_WIDTH, BOARD_WIDTH, BALL_HEIGHT, BOARD_HEIGHT) ||
        check_collision(g_ball_2_position, g_left_board_position, BALL_WIDTH, BOARD_WIDTH, BALL_HEIGHT, BOARD_HEIGHT)) {
        g_ball_2_movement.x *= -1;
        if (g_ball_2_position.x < 0) {
            g_ball_2_position.x += 0.05;
        } else {
            g_ball_2_position.x -= 0.05;
        }
    }
    if (check_collision(g_ball_3_position, g_right_board_position, BALL_WIDTH, BOARD_WIDTH, BALL_HEIGHT, BOARD_HEIGHT) ||
        check_collision(g_ball_3_position, g_left_board_position, BALL_WIDTH, BOARD_WIDTH, BALL_HEIGHT, BOARD_HEIGHT)) {
        g_ball_3_movement.x *= -1;
        if (g_ball_3_position.x < 0) {
            g_ball_3_position.x += 0.05;
        } else {
            g_ball_3_position.x -= 0.05;
        }
    }
    // Right Board and Top Border Collision
    if (check_collision(g_right_board_position, g_top_border_position, BOARD_WIDTH, BORDER_WIDTH, BOARD_HEIGHT, BORDER_THICKNESS)) {
        g_right_board_movement.y += -1;
    }
    // Right Board and Top Border Collision
    if (check_collision(g_right_board_position, g_bottom_border_position, BOARD_WIDTH, BORDER_WIDTH, BOARD_HEIGHT, BORDER_THICKNESS)) {
        g_right_board_movement.y += 1;
    }
    // Left Board and Top Border Collision
    if (check_collision(g_left_board_position, g_top_border_position, BOARD_WIDTH, BORDER_WIDTH, BOARD_HEIGHT, BORDER_THICKNESS)) {
        g_left_board_movement.y += -1;
    }
    // Left Board and Top Border Collision
    if (check_collision(g_left_board_position, g_bottom_border_position, BOARD_WIDTH, BORDER_WIDTH, BOARD_HEIGHT, BORDER_THICKNESS)) {
        g_left_board_movement.y += 1;
    }
    
    // Top and Bottom border and Ball Collision//
    if (check_collision(g_ball_1_position, g_top_border_position, BALL_WIDTH, BORDER_WIDTH, BALL_HEIGHT, BORDER_THICKNESS) ||
        check_collision(g_ball_1_position, g_bottom_border_position, BALL_WIDTH, BORDER_WIDTH, BALL_HEIGHT, BORDER_THICKNESS)) {
        // std::cout << std::time(nullptr) << ": Collision with Left Board\n";
        g_ball_1_movement.y *= -1;
        if (g_ball_1_position.y < 0) {
            g_ball_1_position.y += 0.05;
        } else {
            g_ball_1_position.y -= 0.05;
        }
    }
    if (check_collision(g_ball_2_position, g_top_border_position, BALL_WIDTH, BORDER_WIDTH, BALL_HEIGHT, BORDER_THICKNESS) ||
        check_collision(g_ball_2_position, g_bottom_border_position, BALL_WIDTH, BORDER_WIDTH, BALL_HEIGHT, BORDER_THICKNESS)) {
        // std::cout << std::time(nullptr) << ": Collision with Left Board\n";
        g_ball_2_movement.y *= -1;
        if (g_ball_2_position.y < 0) {
            g_ball_2_position.y += 0.05;
        } else {
            g_ball_2_position.y -= 0.05;
        }
    }
    if (check_collision(g_ball_3_position, g_top_border_position, BALL_WIDTH, BORDER_WIDTH, BALL_HEIGHT, BORDER_THICKNESS) ||
        check_collision(g_ball_3_position, g_bottom_border_position, BALL_WIDTH, BORDER_WIDTH, BALL_HEIGHT, BORDER_THICKNESS)) {
        // std::cout << std::time(nullptr) << ": Collision with Left Board\n";
        g_ball_3_movement.y *= -1;
        if (g_ball_3_position.y < 0) {
            g_ball_3_position.y += 0.05;
        } else {
            g_ball_3_position.y -= 0.05;
        }
    }
    // Left and Right border and Ball Collision//
    if (check_collision(g_ball_1_position, g_right_border_position, BALL_WIDTH, BORDER_THICKNESS, BALL_HEIGHT, BORDER_HEIGHT) ||
        check_collision(g_ball_1_position, g_left_border_position, BALL_WIDTH, BORDER_THICKNESS, BALL_HEIGHT, BORDER_HEIGHT)  ||
        check_collision(g_ball_2_position, g_right_border_position, BALL_WIDTH, BORDER_THICKNESS, BALL_HEIGHT, BORDER_HEIGHT) ||
        check_collision(g_ball_2_position, g_left_border_position, BALL_WIDTH, BORDER_THICKNESS, BALL_HEIGHT, BORDER_HEIGHT)  ||
        check_collision(g_ball_3_position, g_right_border_position, BALL_WIDTH, BORDER_THICKNESS, BALL_HEIGHT, BORDER_HEIGHT) ||
        check_collision(g_ball_3_position, g_left_border_position, BALL_WIDTH, BORDER_THICKNESS, BALL_HEIGHT, BORDER_HEIGHT)) {
        // std::cout << std::time(nullptr) << ": Game is Lost\n";
        g_game_is_lost = true;
    }

    // -------- DELTA TIME -------- //
    if (!g_game_is_lost) {
        float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
        float delta_time = ticks - g_previous_ticks;
        g_previous_ticks = ticks;
        g_ball_rotation_angle += g_ball_rotation_speed * delta_time;
        
        // -------- BALL UPDATES -------- //
        if (g_number_of_balls >= 1) {
            g_ball_1_matrix    = glm::mat4(1.0f);
            if (glm::length(g_ball_1_movement) > 1.0f) {        // normalize the movement
                    g_ball_1_movement = glm::normalize(g_ball_1_movement);
            }
            g_ball_1_position += g_ball_1_movement * g_ball_speed * delta_time;
            g_ball_1_matrix    = glm::translate(g_ball_1_matrix, g_ball_1_position);
            g_ball_1_matrix    = glm::scale(g_ball_1_matrix, glm::vec3(BALL_WIDTH, BALL_HEIGHT, 1.0f));
            g_ball_1_matrix    = glm::rotate(g_ball_1_matrix, glm::radians(g_ball_rotation_angle), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        if (g_number_of_balls >= 2) {
            g_ball_2_matrix    = glm::mat4(1.0f);
            if (glm::length(g_ball_2_movement) > 1.0f) {        // normalize the movement
                    g_ball_2_movement = glm::normalize(g_ball_2_movement);
            }
            g_ball_2_position += g_ball_2_movement * g_ball_speed * delta_time;
            g_ball_2_matrix    = glm::translate(g_ball_2_matrix, g_ball_2_position);
            g_ball_2_matrix    = glm::scale(g_ball_2_matrix, glm::vec3(BALL_WIDTH, BALL_HEIGHT, 1.0f));
            g_ball_2_matrix    = glm::rotate(g_ball_2_matrix, glm::radians(g_ball_rotation_angle), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        if (g_number_of_balls >= 3) {
            g_ball_3_matrix    = glm::mat4(1.0f);
            if (glm::length(g_ball_3_movement) > 1.0f) {        // normalize the movement
                    g_ball_3_movement = glm::normalize(g_ball_3_movement);
            }
            g_ball_3_position += g_ball_3_movement * g_ball_speed * delta_time;
            g_ball_3_matrix    = glm::translate(g_ball_3_matrix, g_ball_3_position);
            g_ball_3_matrix    = glm::scale(g_ball_3_matrix, glm::vec3(BALL_WIDTH, BALL_HEIGHT, 1.0f));
            g_ball_3_matrix    = glm::rotate(g_ball_3_matrix, glm::radians(g_ball_rotation_angle), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        
        // -------- LEFT BOARD UPDATES -------- //
        g_left_board_matrix     = glm::mat4(1.0f);
        if (g_computer_player) {
            g_left_board_position.y = g_ball_1_position.y;
        } else {
            g_left_board_position  += g_left_board_movement * g_board_speed * delta_time;
        }
        g_left_board_matrix     = glm::translate(g_left_board_matrix, g_left_board_position);
        g_left_board_matrix     = glm::scale(g_left_board_matrix, glm::vec3(BOARD_WIDTH, BOARD_HEIGHT, 1.0f));
        
        // -------- RIGHT BOARD UPDATES -------- //
        g_right_board_matrix    = glm::mat4(1.0f);
        g_right_board_position += g_right_board_movement * g_board_speed * delta_time;
        g_right_board_matrix    = glm::translate(g_right_board_matrix, g_right_board_position);
        g_right_board_matrix    = glm::scale(g_right_board_matrix, glm::vec3(BOARD_WIDTH, BOARD_HEIGHT, 1.0f));
    }
    else {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }

}

void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
    };

    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    };
    
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    if (!g_game_is_lost){
        if (g_number_of_balls >= 1) {
            draw_object(g_ball_1_matrix, g_ball_texture_id);
        }
        if (g_number_of_balls >= 2) {
            draw_object(g_ball_2_matrix, g_ball_texture_id);
        }
        if (g_number_of_balls >= 3) {
            draw_object(g_ball_3_matrix, g_ball_texture_id);
        }
        draw_object(g_left_board_matrix, g_board_texture_id);
        draw_object(g_right_board_matrix, g_board_texture_id);
        
        draw_object(g_top_border_matrix, g_border_texture_id);
        draw_object(g_bottom_border_matrix, g_border_texture_id);
        draw_object(g_left_border_matrix, g_border_texture_id);
        draw_object(g_right_border_matrix, g_border_texture_id);
    } else {
        draw_object(g_game_over_matrix, g_game_over_texture_id);
    }
    
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    SDL_GL_SwapWindow(g_display_window);
}


void shutdown() { SDL_Quit(); }


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
