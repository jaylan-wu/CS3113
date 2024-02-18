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

const int WINDOW_WIDTH  = 640*1.75,
          WINDOW_HEIGHT = 480*1.75;

int season = 0;
int season_frame_counter = 0;
const int MAX_SEASON_FRAME = 200;

const float BG_SPRING_RED      = 0.024f,
            BG_SPRING_BLUE     = 0.839f,
            BG_SPRING_GREEN    = 0.627f,
            BG_SPRING_OPACITY  = 1.0f;

const float BG_SUMMER_RED      = 0.898f,
            BG_SUMMER_BLUE     = 0.827f,
            BG_SUMMER_GREEN    = 0.321f,
            BG_SUMMER_OPACITY  = 1.0f;

const float BG_FALL_RED        = 0.675f,
            BG_FALL_BLUE       = 0.224f,
            BG_FALL_GREEN      = 0.192f,
            BG_FALL_OPACITY    = 1.0f;

const float BG_WINTER_RED      = 0.247f,
            BG_WINTER_BLUE     = 0.518f,
            BG_WINTER_GREEN    = 0.898f,
            BG_WINTER_OPACITY  = 1.0f;

const int VIEWPORT_X      = 0,
          VIEWPORT_Y      = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char  V_SHADER_PATH[]          = "shaders/vertex_textured.glsl",
            F_SHADER_PATH[]          = "shaders/fragment_textured.glsl",
            CHICKEN_FILEPATH[]       = "assets/chicken.png",
            PURPLE_JUNIMO_FILEPATH[] = "assets/junimo_purple.png";

const float MILLISECONDS_IN_SECOND     = 1000.0;

const int   NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL    = 0;
const GLint TEXTURE_BORDER     = 0;

SDL_Window* g_display_window;
bool  g_game_is_running = true;
float g_previous_ticks  = 0.0f;

ShaderProgram g_shader_program;
glm::mat4     g_view_matrix,
              g_junimo_model_matrix,
              g_projection_matrix,
              g_chicken_model_matrix;

GLuint g_junimo_texture_id,
       g_chicken_texture_id;

// translation of the sprites
const float RADIUS = 2.7f;
const float SPEED = 1.0;

// heartbeat for the junimo
const float GROWTH_FACTOR = 0.7f;
const float SHRINK_FACTOR = -0.7f;
const int MAX_JUNIMO_SCALING_FRAME = 50;
float g_junimo_scaling = 1.0f;
int g_junimo_frame_counter = 0;
bool g_junimo_is_growing = true;

// rotation for the chicken
const float ROTATION_SPEED = 2.5f;
float g_chicken_z_rotate = 0.05f;

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
    g_display_window = SDL_CreateWindow("Simple 2D Scene",
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
    
    g_junimo_model_matrix = glm::mat4(1.0f);
//    g_junimo_model_matrix = glm::scale(g_junimo_model_matrix, glm::vec3(0.3f, 0.3f, 1.0f));
    
    g_chicken_model_matrix = glm::mat4(1.0f);
    g_chicken_model_matrix = glm::translate(g_chicken_model_matrix, glm::vec3(1.0f, 1.0f, 0.0f));
    
    g_view_matrix       = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_chicken_texture_id = load_texture(CHICKEN_FILEPATH);
    g_junimo_texture_id  = load_texture(PURPLE_JUNIMO_FILEPATH);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_shader_program.get_program_id());
    glClearColor(BG_SPRING_RED, BG_SPRING_BLUE, BG_SPRING_GREEN, BG_SPRING_OPACITY);
//    glClearColor(BG_SUMMER_RED, BG_SUMMER_BLUE, BG_SUMMER_GREEN, BG_SUMMER_OPACITY);
//    glClearColor(BG_FALL_RED, BG_FALL_BLUE, BG_FALL_GREEN, BG_FALL_OPACITY);
//    glClearColor(BG_WINTER_RED, BG_WINTER_BLUE, BG_WINTER_GREEN, BG_WINTER_OPACITY);

    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void process_input()
{
    SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
            {
                g_game_is_running = false;
            }
        }
}


void update()
{
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    // pulsing jumino
    g_junimo_frame_counter += 1;
    glm::vec3 scale_vector;
    
    if (g_junimo_frame_counter >= MAX_JUNIMO_SCALING_FRAME)
    {
        g_junimo_is_growing = !g_junimo_is_growing;
        g_junimo_frame_counter = 0;
    }

    g_junimo_scaling = g_junimo_scaling + (g_junimo_is_growing ? GROWTH_FACTOR : SHRINK_FACTOR) * delta_time;
    
    scale_vector = glm::vec3(g_junimo_scaling, g_junimo_scaling, 1.0f);
    
    // movement of the chicken and jumino
    float rotation_angle = SPEED * g_previous_ticks;
    float junimo_x = RADIUS * cos(rotation_angle);
    float junimo_y = RADIUS * sin(rotation_angle);
    
    // chicken rotation
    g_chicken_z_rotate += ROTATION_SPEED * delta_time;
        
    g_junimo_model_matrix = glm::mat4(1.0f);
    g_junimo_model_matrix = glm::translate(g_junimo_model_matrix, glm::vec3(junimo_x, junimo_y, 0.0f));
    g_junimo_model_matrix = glm::scale(g_junimo_model_matrix, scale_vector);
    
    g_chicken_model_matrix = glm::mat4(1.0f);
    g_chicken_model_matrix = glm::translate(g_chicken_model_matrix, glm::vec3(-junimo_x/3, junimo_y/3, 0.0f));
    g_chicken_model_matrix = glm::rotate(g_chicken_model_matrix, g_chicken_z_rotate, glm::vec3(0.0f, 0.0f, 1.0f));
    
    // changing the seasons
    season_frame_counter += 1;
    
    if (season_frame_counter >= MAX_SEASON_FRAME)
    {
        if (season == 0) {
            glClearColor(BG_SPRING_RED, BG_SPRING_BLUE, BG_SPRING_GREEN, BG_SPRING_OPACITY);
            season++;
        } else if (season == 1) {
            glClearColor(BG_SUMMER_RED, BG_SUMMER_BLUE, BG_SUMMER_GREEN, BG_SUMMER_OPACITY);
            season++;
        } else if (season == 2) {
            glClearColor(BG_FALL_RED, BG_FALL_BLUE, BG_FALL_GREEN, BG_FALL_OPACITY);
            season++;
        }
        else if (season == 3) {
            glClearColor(BG_WINTER_RED, BG_WINTER_BLUE, BG_WINTER_GREEN, BG_WINTER_OPACITY);
            season = 0;
        }
        season_frame_counter = 0;
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
    
    draw_object(g_junimo_model_matrix, g_junimo_texture_id);
    draw_object(g_chicken_model_matrix, g_chicken_texture_id);
    
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
